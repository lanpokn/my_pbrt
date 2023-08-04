"""loss: higher the loss, more similar the data 

"""
from event_file_io import EventsData
import numpy as np
from scipy.spatial import KDTree
import math
import time



##loss 1
def normalize_evs(evs):
    """_summary_

    Args:
        evs (_type_): _description_

    Returns:
        _type_: _description_
    """    
    evs_normalized = evs.copy()
    epsilon = 1e-8  # Small epsilon value to avoid division by zero
    #不能归一化，两个点云的时间尺度很可能不同，归一化之后强行抹平了时间上的区别。因此t不能归一化，只能通过调整，让xypt尽量别差太多。对齐之后让t除以一个常数即可
    #t真不好搞，怎样才是对的？怎样对t进行数据处理才是合理的？总之就是如何设计那个协方差矩阵，有没有数学上的指导？暂时trick一下吧
    #因为目的是比较仿真与实际，t肯定对的上，大胆归一化
    #先随机加噪声，是否噪声越多，loss越大。建议可以选几个指标，Event里边眼睛比loss好使。。。
    # 找一个数值上合理的指标，证明自己的我的仿真比别人的更好。 度量水很深 在附录里边验证。
    # 第一个维度可以只是证明自己的指标比别人的更好就行。第二个维度是我提出的指标需要证明，如正交，相加等，即证明自己可以作为度量
    # 11月15号，三个半月时间，用chamfer loss，用小实验，证明噪声增加强度（参考师兄之前的实验），选几个有利的指标，要验证指标本身有用，再证明我的仿真器比别人的好
    # 定下来之后就先不管了
    # 原本的实验场景也要搭建，做场景和实物一一对应的（要五到六个序列左右），出了数据用loss度量（70%）。冲cvpr不太够，还需要再找下游任务体现这个东西更好（重构或者深度估计）（80%）
    # 还要说这个未来可以做大规模数据集（20%），UE的3D模型是否能转成pbrt也要调研，这个直接问师兄
    # 1. 魏老师能否给我3D模型 （小2. 找一个对自己有用的matrix） 3. 原始的五个真实序列的下游度量 4 3D模型可以自动生成深度图，能否兼容产生事件， 写作半个月到一个月
    # 不可控性：魏老师那边能不能实现， 五个的下游验证（只用信号度量不靠谱）， 大规模如果能做，故事就很漂亮（不兼容就有风险）
    # 这周把matrix（loss)定下来，下周就搞3D模型，低速高速低光高光等
    # 1. 原始加噪声 2. 
    evs_normalized['x'] = ((evs['x'] - evs['x'].min()) * 100 / (evs['x'].max() - evs['x'].min() + epsilon)).astype(float)
    evs_normalized['y'] = ((evs['y'] - evs['y'].min()) * 100 / (evs['y'].max() - evs['y'].min() + epsilon)).astype(float)
    evs_normalized['p'] = ((evs['p'] - evs['p'].min()) * 100 / (evs['p'].max() - evs['p'].min() + epsilon)).astype(float)
    evs_normalized['t'] = ((evs['t'] - evs['t'].min()) * 1000 / (evs['t'].max() - evs['t'].min() + epsilon)).astype(float)
    # evs_normalized['x'] = ((evs['x'] - evs['x'].min()) )/ (1.00)
    # evs_normalized['y'] = ((evs['y'] - evs['y'].min()) )/ (1.00)
    # evs_normalized['p'] = ((evs['p'] - evs['p'].min()) )* 200.00
    # evs_normalized['t'] = ((evs['t'] - evs['t'].min()) ) / (1000.00)#t: ms
    # evs_normalized['t'] = ((evs['t'] - evs['t'].min()))
    return evs_normalized

def chamfer_distance_loss(evs1, evs2):
    """_summary_

    Args:
        evs1 (_type_): _description_
        evs2 (_type_): _description_

    Returns:
        losss from 0 to 1
    """    #默认输入的两个点云是同时开始的
    evs1_norm = normalize_evs(evs1)
    evs2_norm = normalize_evs(evs2)
    
    evs1_float = np.zeros((evs1_norm.shape[0], 4), dtype=np.float64)
    evs2_float = np.zeros((evs2_norm.shape[0], 4), dtype=np.float64)
    
    evs1_float[:, 0] = evs1_norm['x']
    evs1_float[:, 1] = evs1_norm['y']
    evs1_float[:, 2] = evs1_norm['p']
    evs1_float[:, 3] = evs1_norm['t']
    
    evs2_float[:, 0] = evs2_norm['x']
    evs2_float[:, 1] = evs2_norm['y']
    evs2_float[:, 2] = evs2_norm['p']
    evs2_float[:, 3] = evs2_norm['t']
    
    # Create KDTree using evs2_float as points
    tree1 = KDTree(evs1_float)
    tree2 = KDTree(evs2_float)

    # Query the tree with evs1_float as points
    # use log to avoid some point too far away
    dists1, _ = tree2.query(evs1_float)
    # dists1 = np.log(10*dists1+1)
    dists2, _ = tree1.query(evs2_float)
    # dists2 = np.exp(10*dists2+1)

    # Return the sum of distances
    return (np.mean(dists1)+np.mean(dists2))


#TODO 此处可以再加上师兄的度量方法，与之前的完全不一样，可以考虑抄过来然后改编一下，只能说大开眼界，kd树不要了，数量一定对齐吗？
# 学长没用kdtree，而是先将两个点云数据分成一个个cube，然后一个个cube之间进行对比，距离累加起来。多余的部分不要了
#loss2
def cubes_3d_kernel_method(events, new_events, x_sigma, y_sigma, t_sigma):
    """
    Computing inner product between spike cubes using 3d gaussian kernel method.

    Inputs:
    -------
        events    - events include polarity, timestamp, x and y.
        new_events    - events after changing operation.
        x_sigma, y_sigma, t_sigma  - the parameters of 3d gaussian kernel.

    Outputs:
    -------
        inner_product    - the inner product between events and new_events.,has no upper limits

    """
    #print('events number={}'.format(len(events[0,:])))
    #print('ON number={}'.format(np.sum(events[0, :]==1)))
    ON_scale = np.sum(events[0, :]==1)/(len(events[0, :])) # ON events in history
    # new_OFF_scale = np.sum(new_events[0, :]==-1)/len(events[0, :]) # ON new events in history
    new_ON_scale = np.sum(new_events[0, :] == 1) / (len(new_events[0, :]))  # ON new events in history

    # print('events_numbers={}, new_events_numbers={}'.format(len(events[0, :]), len(new_events[0, :])))

    polarity_scale = ON_scale*new_ON_scale + (1-ON_scale)*(1-new_ON_scale)
    # polarity_scale = 1 + abs(ON_scale-new_OFF_scale) # simply polarity for integrated formulation.

    x_index = events[2, :][:, None] - new_events[2, :][None, :]
    y_index = events[3, :][:, None] - new_events[3, :][None, :]
    t_index = events[1, :][:, None] - new_events[1, :][None, :]

    dist_matrix = np.exp(- x_index**2 / (2*x_sigma**2) - y_index**2 / (2*y_sigma**2) - t_index**2 / (2*t_sigma**2))

    inner_product = polarity_scale * np.sum(dist_matrix)

    return inner_product


def cubes_3d_kernel_distance(events, new_events, x_sigma, y_sigma, t_sigma):
    """
    Computing distance between spike cubes using inner product in RKHS.

    Inputs:
    -------
        events    - events include polarity, timestamp, x and y.
        new_events    - events after changing operation.
        x_sigma, y_sigma, t_sigma  - the parameters of 3d gaussian kernel.

    Outputs:
    -------
        distance    - the distance between events and new_events.

    """

    if len(np.transpose(events)) <= 5 or len(np.transpose(new_events)) <= 5:
        distance = 0
    else:

        distance = cubes_3d_kernel_method(events, events, x_sigma, y_sigma, t_sigma)
        distance += cubes_3d_kernel_method(new_events, new_events, x_sigma, y_sigma, t_sigma)
        distance -= 2 * cubes_3d_kernel_method(events, new_events, x_sigma, y_sigma, t_sigma)

    return distance

def events_to_spike_cubes(events, width, height, x_cube_size, y_cube_size, t_cube_size):
    """
    events are split into spike cubes.

    Inputs:
    -------
        events    - the dataset of AER sensor including polarity(t), timestamp(ts), x coordinate(X) and y coordinate(Y).
        width, height    - the width and height resolutions of dynamic vision sensor.
        x_cube_size, y_cube_size, t_cube_size    - the width, height and temporal size of spike cubes.
   Outputs:
    -------
        events_cubes    - the cubes of events.

    """

    num = int((width/x_cube_size)*(height/y_cube_size)*(math.ceil(max(events[1, :] / t_cube_size))))
    events_cube = [[] for _ in range(num)]
    #print('num={}'.format(num))

    for i in range(events.shape[1]):

        k = math.floor(events[2, i]/x_cube_size) + math.floor(events[3, i]/y_cube_size)*int(width/x_cube_size) + math.floor(events[1, i]/t_cube_size)*int(width/x_cube_size)*int(height/y_cube_size)
        # if k == 102:
        #     print(k)
        #print('t_cube_size={}, k={}, i={}, event={}, feature={}'.format(t_cube_size, k, i, events[:, i], math.floor(events[1, i]/t_cube_size)))
        events_cube[k].append(events[:, i])


    return events_cube

## this is the new loss!
def kernel_method_spike_cubes_loss(events, new_events, width=128, height=128, x_cube_size=32, y_cube_size=32, t_cube_size=5000, x_sigma=5, y_sigma=5, t_sigma=5000):
    """
        3d gaussian kernel method  for spike cubes, such as polarity independent and polarity interference.

        Inputs:
        -------
            events    - events include polarity, timestamp, x and y.
            new_events    - events after changing operation.
            width, height  - the width and height of dynamic vision sensor.
            x_cube_size, y_cube_size, t_cube_size  - the size of spike cube.
            x_sigma, y_sigma, t_sigma  - the 3d gaussian kernel parameters.

        Outputs:
        -------
            distance    - the distance between events and new_events.

    """
    ##ADD IT TO fix dimension bug
    evs1_float = np.zeros((4,events.shape[0]), dtype=np.float64)
    evs2_float = np.zeros((4,new_events.shape[0]), dtype=np.float64)
    
    evs1_float[0, :] = events['p']
    evs1_float[1, :] = events['t']
    evs1_float[2, :] = events['x']
    evs1_float[3, :] = events['y']
    
    evs2_float[0, :] = new_events['p']
    evs2_float[1, :] = new_events['t']
    evs2_float[2, :] = new_events['x']
    evs2_float[3, :] = new_events['y']
    
    # evs1_float = np.transpose(evs1_float)
    # evs2_float = np.transpose(evs2_float)

    events_cube = events_to_spike_cubes(evs1_float, width, height, x_cube_size, y_cube_size, t_cube_size)
    new_events_cubes = events_to_spike_cubes(evs2_float, width, height, x_cube_size, y_cube_size, t_cube_size)

    distance = 0
    for k in range(0, min(len(events_cube), len(new_events_cubes))):

        events_data = np.transpose(np.array(events_cube[k]))
        new_events_data = np.transpose(np.array(new_events_cubes[k]))

        if len(events_data)==0 and len(new_events_data)==0:
            distance += 0

        else:
            distance += cubes_3d_kernel_distance(events_data, new_events_data, x_sigma, y_sigma, t_sigma)

    return distance
    # change to [0  - 1]

def main():
    events_data = EventsData()
    #make sure the video is long enough, or it can't disolay normally
    events_data.read_real_events("C:/Users/admin/Documents/metavision/recordings/output1.hdf5", 1000000)

    ev_data0 = events_data.events[0]
    ev_data1 = events_data.events[1]
    
    start_time = time.time()
    loss_same = kernel_method_spike_cubes_loss(ev_data0, ev_data0,events_data.width,events_data.height)
    print(loss_same)
    loss_different = kernel_method_spike_cubes_loss(ev_data0, ev_data1,events_data.width,events_data.height)
    print(loss_different)
    # events_data.display_events(ev_data0,3000)
    # events_data.display_events(ev_data1,3000)
    
    events_data_simi = EventsData()
    events_data_simi.read_real_events("C:/Users/admin/Documents/metavision/recordings/output2.hdf5", 1000000)
    ev_data0_simi = events_data_simi.events[0]
    loss_simi = kernel_method_spike_cubes_loss(ev_data0, ev_data0_simi,events_data.width,events_data.height)
    print(loss_simi)
    # events_data.display_events(ev_data0_simi,3000)
    
    end_time = time.time()
    total_time = end_time - start_time
    print("Total time of kernel method without kdtree: ", total_time)
    
    start_time = time.time()
    loss_same = chamfer_distance_loss(ev_data0, ev_data0)
    print(loss_same)
    loss_different = chamfer_distance_loss(ev_data0, ev_data1)
    print(loss_different)
    # events_data.display_events(ev_data0,3000)
    # events_data.display_events(ev_data1,3000)
    
    events_data_simi = EventsData()
    events_data_simi.read_real_events("C:/Users/admin/Documents/metavision/recordings/output2.hdf5", 1000000)
    ev_data0_simi = events_data_simi.events[0]
    loss_simi = chamfer_distance_loss(ev_data0, ev_data0_simi)
    print(loss_simi)
    # events_data.display_events(ev_data0_simi,3000)
    
    end_time = time.time()
    total_time = end_time - start_time
    print("Total time of chamfer distance method: ", total_time)

    # for ev_data in events_data.events:
    #     print("----- New event buffer! -----")
    #     counter = ev_data.size
    #     min_t = ev_data['t'][0]
    #     max_t = ev_data['t'][-1]
    #     print(f"There were {counter} events in this event buffer.")
    #     print(f"There were {events_data.global_counter} total events up to now.")
    #     print(f"The current event buffer included events from {min_t} to {max_t} microseconds.")
    #     print("----- End of the event buffer! -----")

    # duration_seconds = events_data.global_max_t / 1.0e6
    # print(f"There were {events_data.global_counter} events in total.")
    # print(f"The total duration was {duration_seconds:.2f} seconds.")
    # if duration_seconds >= 1:
    #     print(f"There were {events_data.global_counter / duration_seconds:.2f} events per second on average.")


if __name__ == "__main__":
    main()