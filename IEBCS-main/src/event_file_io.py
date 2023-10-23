#P 1 positive, 0 or -1 negative, it may different in different simulator
#thus it's important to use 0.5 or =1 to deal with 
#white positive
#in real camera: 1280 in first dimen, 720 in second dim, height is 720(second dim)
#in sim camera also 1280 first, 720 second
from metavision_core.event_io import EventsIterator
from metavision_sdk_core import PeriodicFrameGenerationAlgorithm
from metavision_sdk_ui import EventLoop, BaseWindow, Window, UIAction, UIKeyEvent
import cv2
import numpy as np
#We don’t provide any encoder for EVT3 format as its compressed nature makes it difficult to be re-created from an uncompressed CSV file
# class EventData:
    
# class EventDataset:
    

import argparse
from typing import List

import struct

def load_dat_event(filename, start=0, stop=-1, display=False):
    """ Load .dat event.
        Warning: only tested on the VGA sensor on V2 Prophesee event
        Args:
            filename: Path of the .dat file
            start: starting timestamp (us)
            stop: if different than -1, last timestamp
            display: display file infos
        Returns:
             ts, x, y, pol numpy arrays of timestamps, position and polarities
     """
    f = open(filename, 'rb')
    if f == -1:
        print("The file does not exist")
        return
    else:
        if display: print("Load DAT Events: " + filename)
    l = f.readline()
    all_lines = l
    while l[0] == 37:
        p = f.tell()
        if display: print(l)
        l = f.readline()
        all_lines = all_lines + l
    f.close()
    all_lines = str(all_lines)
    f = open(filename, 'rb')
    f.seek(p, 0)
    evType = np.uint8(f.read(1)[0])
    evSize = np.uint8(f.read(1)[0])
    p = f.tell()
    l_last = f.tell()
    if start > 0:
        t = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
        dat = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
        while t < start:
            p = f.tell()
            t = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
            dat = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])

    if stop > 0:
        t = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
        dat = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
        while t < stop:
            l_last = f.tell()
            t = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
            dat = np.uint32(struct.unpack("<I", bytearray(f.read(4)))[0])
    else:
        l_last = f.seek(0, 2)

    num_b = (l_last - p) // evSize * 2
    f.close()
    data = np.fromfile(filename, dtype=np.uint32, count=num_b, offset=p)
    ts = data[::2]
    v = 0
    ind = all_lines.find("Version")
    if ind > 0:
        v = int(all_lines[ind+8])
    if v >= 2:
        x_mask = np.uint32(0x00007FF)
        y_mask = np.uint32(0x0FFFC000)
        pol_mask = np.uint32(0x10000000)
        x_shift = 0
        y_shift = 14
        pol_shift = 28
    else:
        x_mask = np.uint32(0x00001FF)
        y_mask = np.uint32(0x0001FE00)
        pol_mask = np.uint32(0x00020000)
        x_shift = 0
        y_shift = 9
        pol_shift = 17
    x = data[1::2] & x_mask
    x = x >> x_shift
    y = data[1::2] & y_mask
    y = y >> y_shift
    pol = data[1::2] & pol_mask
    pol = pol >> pol_shift
    if len(ts) > 0:
        if display: 
            print("First Event: ", ts[0], " us")
            print("Last Event: ", ts[-1], " us")
            print("Number of Events: ", ts.shape[0])
    return ts, x, y, pol


# evs is a single unit, which can be the input of loss, display and so on
class EventsData:
    def __init__(self):
        #x,y,t,p,type is ndarray
        self.events = []
        self.global_counter = 0
        self.global_max_t = -1
        self.global_min_t = -1
        self.delta_t = 0
        self.width = 0
        self.height = 0
    #delta_t is used to split original data
    def read_real_events(self, input_path: str,delta_t_input:int):
        """Process events and update EventsData object"""
        self.delta_t = delta_t_input
        mv_iterator = EventsIterator(input_path=input_path, delta_t = delta_t_input)
        self.height, self.width = mv_iterator.get_size()  # Camera Geometry
        for evs in mv_iterator:
            if evs.size == 0:
                continue
            if self.global_min_t ==-1:
                self.global_min_t = evs['t'][0]
            self.global_max_t = evs['t'][-1]
            counter = evs.size
            
            self.events.append(evs)
            self.global_counter += counter
        evs['t'] = evs['t']-self.global_min_t
        self.global_max_t = self.global_max_t-self.global_min_t
        self.global_min_t = 0

    def read_IEBCS_events(self, input_path: str,delta_t_input:int):
        # input path should be xxx .dat
        """Process events and update EventsData object"""
        ts, x, y, p = load_dat_event(input_path)
        self.delta_t = delta_t_input
        #x max is 
        self.height = np.max(y) + 1  # Set self.height as the maximum value of x plus 1
        self.width = np.max(x) + 1   # Set self.width as the maximum value of y plus 1

        start_time = ts[0]
        end_time = ts[-1]
        time_duration = end_time - start_time

        num_buffers = time_duration // delta_t_input

        for i in range(num_buffers):
            start_idx = np.searchsorted(ts, start_time + i * delta_t_input)
            end_idx = np.searchsorted(ts, start_time + (i + 1) * delta_t_input, side='right')
            #evs = np.zeros(end_idx - start_idx, dtype=[('x', 'int32'), ('y', 'int32'), ('t', 'int32'), ('p', 'int32')])
            evs = np.zeros(end_idx - start_idx, dtype=np.dtype({'names': ['x', 'y', 'p', 't'], 'formats': ['<u2', '<u2', '<i2', '<i8'], 'offsets': [0, 2, 4, 8], 'itemsize': 16}))            
            evs['x'] = x[start_idx:end_idx]
            evs['y'] = y[start_idx:end_idx]
            evs['t'] = ts[start_idx:end_idx]
            evs['p'] = p[start_idx:end_idx]
            self.events.append(evs)
            self.global_counter += evs.size

        remaining_events = ts[np.searchsorted(ts, start_time + num_buffers * delta_t_input):]
        if remaining_events.size > 0:
            #evs = np.zeros(remaining_events.size, dtype=[('x', 'int32'), ('y', 'int32'), ('t', 'int32'), ('p', 'int32')])
            evs = np.zeros(remaining_events.size, dtype=np.dtype({'names': ['x', 'y', 'p', 't'], 'formats': ['<u2', '<u2', '<i2', '<i8'], 'offsets': [0, 2, 4, 8], 'itemsize': 16}))            
            evs['x'] = x[np.searchsorted(ts, start_time + num_buffers * delta_t_input):]
            evs['y'] = y[np.searchsorted(ts, start_time + num_buffers * delta_t_input):]
            evs['t'] = ts[np.searchsorted(ts, start_time + num_buffers * delta_t_input):]
            evs['p'] = p[np.searchsorted(ts, start_time + num_buffers * delta_t_input):]
            self.events.append(evs)
            self.global_counter += evs.size

        if self.global_min_t == -1:
            self.global_min_t = ts[0]
        self.global_max_t = ts[-1]
        evs['t'] = evs['t']  -self.global_min_t
        self.global_max_t = self.global_max_t-self.global_min_t
        self.global_min_t = 0
        

    def display_events(self,evs,accumulation_time_us):
        # Window - Graphical User Interface
        # evs must one of the EventsData, or h and w are incorrect
        with Window(title="Metavision SDK Get Started", width=self.width, height=self.height, mode=BaseWindow.RenderMode.BGR) as window:
            def keyboard_cb(key, scancode, action, mods):
                if action != UIAction.RELEASE:
                    return
                if key == UIKeyEvent.KEY_ESCAPE or key == UIKeyEvent.KEY_Q:
                    window.set_close_flag()

            window.set_keyboard_callback(keyboard_cb)
            event_frame_gen = PeriodicFrameGenerationAlgorithm(sensor_width=self.width, sensor_height=self.height,
                                                    accumulation_time_us=accumulation_time_us)

            def on_cd_frame_cb(ts, cd_frame):
                window.show(cd_frame)

            event_frame_gen.set_output_callback(on_cd_frame_cb)
            
            EventLoop.poll_and_dispatch()
            window_cv = cv2.namedWindow("click to show", cv2.WINDOW_NORMAL)
            cv2.resizeWindow("click to show",self.width,self.height)
            cv2.imshow("click to show", 255)

            # Wait for a key press
            cv2.waitKey(0)

            # Close the window
            cv2.destroyAllWindows()
            event_frame_gen.process_events(evs)
            
            # if window.should_close():
            #     break





def main():
    events_data = EventsData()
    # events_data.read_real_events("C:/Users/admin/Documents/metavision/recordings/output1.hdf5",1000)
    events_data.read_IEBCS_events("D:/2023/计算成像与仿真/my_pbrt/IEBCS-main/ev_100_10_100_300_0.3_0.01.dat",10000)
    for ev_data in events_data.events:
        print("----- New event buffer! -----")
        counter = ev_data.size
        min_t = ev_data['t'][0]
        max_t = ev_data['t'][-1]
        print(f"There were {counter} events in this event buffer.")
        print(f"There were {events_data.global_counter} total events up to now.")
        print(f"The current event buffer included events from {min_t} to {max_t} microseconds.")
        print("----- End of the event buffer! -----")

    duration_seconds = events_data.global_max_t / 1.0e6
    print(f"There were {events_data.global_counter} events in total.")
    print(f"The total duration was {duration_seconds:.2f} seconds.")
    if duration_seconds >= 1:
        print(f"There were {events_data.global_counter / duration_seconds:.2f} events per second on average.")


if __name__ == "__main__":
    main()