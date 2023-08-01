from metavision_core.event_io import EventsIterator

#We don’t provide any encoder for EVT3 format as its compressed nature makes it difficult to be re-created from an uncompressed CSV file
# class EventData:
    
# class EventDataset:
    

import argparse
from typing import List

class EventsData:
    def __init__(self):
        self.events = []
        self.global_counter = 0
        self.global_max_t = 0
        self.global_min_t = -1
        self.delta_t = 0

    def read_real_events(self, input_path: str,delta_t_input:int):
        """Process events and update EventsData object"""
        self.delta_t = delta_t_input
        mv_iterator = EventsIterator(input_path=input_path, delta_t = delta_t_input)
        for evs in mv_iterator:
            if evs.size == 0:
                continue
            if self.global_min_t ==-1:
                self.global_min_t = evs['t'][0]
            self.global_max_t = evs['t'][-1]
            counter = evs.size
            
            self.events.append(evs)
            self.global_counter += counter




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