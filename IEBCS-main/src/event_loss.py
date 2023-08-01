from event_file_io import EventsData
import numpy as np
from scipy.spatial import KDTree



# def chamferDistance(evs1, evs2):
#     # Create new arrays with float64 data type
#     evs1_float = np.zeros((evs1.shape[0], 4), dtype=np.float64)
#     evs2_float = np.zeros((evs2.shape[0], 4), dtype=np.float64)

#     # Assign values from evs1 and evs2 to the new arrays
#     evs1_float[:, 0] = evs1['x']
#     evs1_float[:, 1] = evs1['y']
#     evs1_float[:, 2] = evs1['p']
#     evs1_float[:, 3] = evs1['t']

#     evs2_float[:, 0] = evs2['x']
#     evs2_float[:, 1] = evs2['y']
#     evs2_float[:, 2] = evs2['p']
#     evs2_float[:, 3] = evs2['t']

#     # Create KDTree using evs2_float as points
#     tree1 = KDTree(evs1_float)
#     tree2 = KDTree(evs2_float)

#     # Query the tree with evs1_float as points
#     dists1, _ = tree2.query(evs1_float)
#     dists2, _ = tree1.query(evs2_float)

#     # Return the sum of distances
#     return np.sum(dists1)+np.sum(dists2)

def normalize_evs(evs):
    evs_normalized = evs.copy()
    epsilon = 1e-8  # Small epsilon value to avoid division by zero
    #不能归一化，两个点云的时间尺度很可能不同，归一化之后强行抹平了时间上的区别。因此t不能归一化，只能通过调整，让xypt尽量别差太多。对齐之后让t除以一个常数即可
    #t真不好搞，怎样才是对的？怎样对t进行数据处理才是合理的？总之就是如何设计那个协方差矩阵，有没有数学上的指导？暂时trick一下吧
    #因为目的是比较仿真与实际，t肯定对的上，大胆归一化
    evs_normalized['x'] = ((evs['x'] - evs['x'].min()) * 100 / (evs['x'].max() - evs['x'].min() + epsilon)).astype(float)
    evs_normalized['y'] = ((evs['y'] - evs['y'].min()) * 100 / (evs['y'].max() - evs['y'].min() + epsilon)).astype(float)
    evs_normalized['p'] = ((evs['p'] - evs['p'].min()) * 100 / (evs['p'].max() - evs['p'].min() + epsilon)).astype(float)
    evs_normalized['t'] = ((evs['t'] - evs['t'].min()) * 1000 / (evs['t'].max() - evs['t'].min() + epsilon)).astype(float)
    # evs_normalized['t'] = ((evs['t'] - evs['t'].min()))
    return evs_normalized

def chamferDistance(evs1, evs2):
    #默认输入的两个点云是同时开始的
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
    dists1, _ = tree2.query(evs1_float)
    dists2, _ = tree1.query(evs2_float)

    # Return the sum of distances
    return np.mean(dists1)+np.mean(dists2)

def main():
    events_data = EventsData()
    #make sure the video is long enough, or it can't disolay normally
    events_data.read_real_events("C:/Users/admin/Documents/metavision/recordings/output1.hdf5", 1000000)

    ev_data0 = events_data.events[0]
    ev_data1 = events_data.events[1]
    
    loss_same = chamferDistance(ev_data0, ev_data0)
    print(loss_same)
    loss_different = chamferDistance(ev_data0, ev_data1)
    print(loss_different)
    # events_data.display_events(ev_data0,3000)
    # events_data.display_events(ev_data1,3000)
    
    events_data_simi = EventsData()
    events_data_simi.read_real_events("C:/Users/admin/Documents/metavision/recordings/output2.hdf5", 1000000)
    ev_data0_simi = events_data_simi.events[0]
    loss_simi = chamferDistance(ev_data0, ev_data0_simi)
    print(loss_simi)
    events_data.display_events(ev_data0_simi,3000)

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