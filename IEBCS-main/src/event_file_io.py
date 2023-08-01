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
    events_data.read_real_events("C:/Users/admin/Documents/metavision/recordings/output1.hdf5",1000)

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