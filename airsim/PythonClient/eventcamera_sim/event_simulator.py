from numba.np.ufunc import parallel
import numpy as np
from types import SimpleNamespace
from numba import njit, prange, set_num_threads

EVENT_TYPE = np.dtype(
    [("timestamp", "f8"), ("x", "u2"), ("y", "u2"), ("polarity", "b")], align=True
)

TOL = 0.5
MINIMUM_CONTRAST_THRESHOLD = 0.01

CONFIG = SimpleNamespace(
    **{
        "contrast_thresholds": (0.01, 0.01),
        "sigma_contrast_thresholds": (0.0, 0.0),
        "refractory_period_ns": 1000,
        "max_events_per_frame": 200000,
    }
)

##esim is the main simulator, we can change it to ICNS, PBES and so on
# 10. The  esim  function iterates over each pixel of the image and performs the following steps: 
 
#    a. Calculates the difference in log intensity between the current and previous images. 
 
#    b. Checks if the absolute difference is below a predefined tolerance ( TOL ). If so, it continues to the next pixel. 
 
#    c. Determines the polarity (positive or negative) based on the sign of the intensity difference. 
 
#    d. Updates the crossing value based on polarity and tolerance. 
 
#    e. Calculates the lower and upper bounds for detecting spikes. 
 
#    f. Checks if the conditions for generating a spike are met (based on polarity and bounds). 
 
#    g. Updates the crossings and spikes arrays accordingly. 
 
#    h. Calculates the number of spikes based on the intensity difference and the check condition. 
 
#    i. Generates  spike_nums  number of events by assigning values to the corresponding fields of the  output_events  array. 
 
#    j. Updates the current time for each event. 
 
#    k. Checks if the maximum number of events per frame has been reached. If so, it returns the count. 
@njit(parallel=True)
def esim(
    x_end,
    current_image,
    previous_image,
    delta_time,
    crossings,
    last_time,
    output_events,
    spikes,
    refractory_period_ns,
    max_events_per_frame,
    n_pix_row,
):
    count = 0
    max_spikes = int(delta_time / (refractory_period_ns * 1e-3))
    for x in prange(x_end):
        itdt = np.log(current_image[x])
        it = np.log(previous_image[x])
        deltaL = itdt - it

        if np.abs(deltaL) < TOL:
            continue

        pol = np.sign(deltaL)

        cross_update = pol * TOL
        crossings[x] = np.log(crossings[x]) + cross_update

        lb = crossings[x] - it
        ub = crossings[x] - itdt

        pos_check = lb > 0 and (pol == 1) and ub < 0
        neg_check = lb < 0 and (pol == -1) and ub > 0

        spike_nums = (itdt - crossings[x]) / TOL
        cross_check = pos_check + neg_check
        spike_nums = np.abs(int(spike_nums * cross_check))

        crossings[x] = itdt - cross_update
        if spike_nums > 0:
            spikes[x] = pol

        spike_nums = max_spikes if spike_nums > max_spikes else spike_nums

        current_time = last_time
        for i in range(spike_nums):
            output_events[count].x = x % n_pix_row
            output_events[count].y = x // n_pix_row
            output_events[count].timestamp = np.round(current_time * 1e-6, 6)
            output_events[count].polarity = 1 if pol > 0 else -1

            count += 1
            current_time += (delta_time) / spike_nums

            if count == max_events_per_frame:
                return count

    return count


class EventSimulator:
    def __init__(self, W, H, first_image=None, first_time=None, config=CONFIG):
        self.H = H
        self.W = W
        self.config = config
        self.last_image = None
        if first_image is not None:
            assert first_time is not None
            self.init(first_image, first_time)

        self.npix = H * W

    def init(self, first_image, first_time):
        print("Initialized event camera simulator with sensor size:", first_image.shape)

        self.resolution = first_image.shape  # The resolution of the image

        # We ignore the 2D nature of the problem as it is not relevant here
        # It makes multi-core processing more straightforward
        first_image = first_image.reshape(-1)

        # Allocations
        self.last_image = first_image.copy()
        self.current_image = first_image.copy()

        self.last_time = first_time

        self.output_events = np.zeros(
            (self.config.max_events_per_frame), dtype=EVENT_TYPE
        )
        self.event_count = 0
        self.spikes = np.zeros((self.npix))

    def image_callback(self, new_image, new_time):
        if self.last_image is None:
            self.init(new_image, new_time)
            return None, None

        assert new_time > 0
        assert new_image.shape == self.resolution
        new_image = new_image.reshape(-1)  # Free operation

        np.copyto(self.current_image, new_image)

        delta_time = new_time - self.last_time

        config = self.config
        self.output_events = np.zeros(
            (self.config.max_events_per_frame), dtype=EVENT_TYPE
        )
        self.spikes = np.zeros((self.npix))

        self.crossings = self.last_image.copy()
        self.event_count = esim(
            self.current_image.size,
            self.current_image,
            self.last_image,
            delta_time,
            self.crossings,
            self.last_time,
            self.output_events,
            self.spikes,
            config.refractory_period_ns,
            config.max_events_per_frame,
            self.W,
        )

        np.copyto(self.last_image, self.current_image)
        self.last_time = new_time

        result = self.output_events[: self.event_count]
        result.sort(order=["timestamp"], axis=0)

        return self.spikes, result

class EventSimulatorNew:
    def __init__(self, W, H, first_image=None, first_time=None, config=CONFIG):
        self.H = H
        self.W = W
        self.config = config
        self.last_image = None
        if first_image is not None:
            assert first_time is not None
            self.init(first_image, first_time)

        self.npix = H * W

    def init(self, first_image, first_time):
        print("Initialized event camera simulator with sensor size:", first_image.shape)

        self.resolution = first_image.shape  # The resolution of the image

        # We ignore the 2D nature of the problem as it is not relevant here
        # It makes multi-core processing more straightforward
        first_image = first_image.reshape(-1)

        # Allocations
        self.last_image = first_image.copy()
        self.current_image = first_image.copy()

        self.last_time = first_time

        self.output_events = np.zeros(
            (self.config.max_events_per_frame), dtype=EVENT_TYPE
        )
        self.event_count = 0
        self.spikes = np.zeros((self.npix))

    def image_callback(self, new_image, new_time):
        if self.last_image is None:
            self.init(new_image, new_time)
            return None, None

        assert new_time > 0
        assert new_image.shape == self.resolution
        new_image = new_image.reshape(-1)  # Free operation

        np.copyto(self.current_image, new_image)

        delta_time = new_time - self.last_time

        config = self.config
        self.output_events = np.zeros(
            (self.config.max_events_per_frame), dtype=EVENT_TYPE
        )
        self.spikes = np.zeros((self.npix))

        self.crossings = self.last_image.copy()
        self.event_count = esim(
            self.current_image.size,
            self.current_image,
            self.last_image,
            delta_time,
            self.crossings,
            self.last_time,
            self.output_events,
            self.spikes,
            config.refractory_period_ns,
            config.max_events_per_frame,
            self.W,
        )

        np.copyto(self.last_image, self.current_image)
        self.last_time = new_time

        result = self.output_events[: self.event_count]
        result.sort(order=["timestamp"], axis=0)

        #spikes 是每一个位置触发的次数，result是具体的每一个事件的信息。这里有一个假设，即Event被线性触发，因此缺少了很多考虑。换成更高级的仿真，用二次过渡，不应期等细化模型，只要也能给出spikes和results即可，接下来研究这个问题
        #大概率只要引入dsi库，把输入的image给它即可，1x10000还是100x100应该问题不大
        return self.spikes, result