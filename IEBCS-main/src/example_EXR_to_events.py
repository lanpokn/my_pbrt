# Haiqian Han
"""
TODO

"""
import cv2
import sys
sys.path.append("../../src")
from event_buffer import EventBuffer
from dvs_sensor import init_bgn_hist_cpp, DvsSensor
from event_display import EventDisplay
import dsi
import numpy as np
from ExrRead import read_exr_channel

from event_file_io import EventsData
import numpy as np
from scipy.spatial import KDTree
import math
import time
import cv2
from event_loss import *
import open3d as o3d
def Rotate_360_high():
    #TODO,add formal ICNS here, deal v2e in other place
    # filename = "C:/Users/hhq/Documents/blender/PBES_small/rotate_360/0000-0060.mkv"
    #lower lat will get more active pixel(remain event)
    lat = 10
    # higher jit will get blurer pixel and more active pixel , very obvious
    jit = 10
    #lower ref, higher active
    ref = 100
    #it's the trans func, higher tau , lowwer the event gen
    #only 2 chamfer in 300, best!
    #use loss to calibrate , not img, img is fucking
    tau = 300
    #just threshould, lower it can let it more realistic(not straight event)
    #very obvious!
    th = 0.3
    #higher it, more active is
    th_noise = 0.01
    # cap = cv2.VideoCapture(filename)
    #only first is hw, other are wh

    #rotate_360_high, 100-10=100-3-0.3-0.01
    width = 1280
    height = 720
    dsi.initSimu(int(height), int(width))
    dsi.initLatency(lat, jit, ref, tau)
    dsi.initContrast(th, th, th_noise)
    #the noise can also be better!
    init_bgn_hist_cpp("D:/2023/computional imaging/my_pbrt/IEBCS-main/data/noise_pos_161lux.npy", "D:/2023/computional imaging/my_pbrt/IEBCS-main/data/noise_pos_161lux.npy")
    isInit = False
    # dt = 1000  # FPS must be 1 kHz,this means fps = dt, and it shows that it can capture high fpx, like bird wings.
            #however, I do not need so high frequency, if I can't output somany pitures
    # if dt is too high, result is wrong? 
    # if bigger than 2857*2, result is wrong, I don't know why
    # if try to look better, may be small dt in 360...
    #if you change dt, t total is longer, that's right!
    dt = 2857
    ev_full = EventBuffer(1)
    ed = EventDisplay("Events", width, height, dt)
    time = 0
    out = cv2.VideoWriter('rotate_360_highlight_{}_{}_{}_{}_{}_{}_nonoise.avi'.format(lat, jit, ref, tau, th, th_noise),
        cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'), 20.0, (int(width), int(height)))
    i = 0
    while i<20:
        filename = "D:/2023/computional imaging/my_pbrt/build/Rotate_360_pbrt001"+str(i+20)+"exr"
        im = read_exr_channel(filename,"intensity",100)
        cv2.imshow("t", im)
        cv2.waitKey(1)
        im = im*750
        #value must near 127,otherwise it will get wrong answner
        #becaose backend suppose it as 0-255, you should not use 0-1
        out.write(im)
        i=i+1
        if im is None:
            break
        # im = cv2.cvtColor(im, cv2.COLOR_RGB2LUV)[:, :, 0]
        if not isInit:
            dsi.initImg(im)
            isInit = True
        else:
            buf = dsi.updateImg(im, dt)
            ev = EventBuffer(1)
            ev.add_array(np.array(buf["ts"], dtype=np.uint64),
                            np.array(buf["x"], dtype=np.uint16),
                            np.array(buf["y"], dtype=np.uint16),
                            np.array(buf["p"], dtype=np.uint64),
                            1000000)
            ed.update(ev, dt)
            ev_full.increase_ev(ev)
            time += dt
            if time > 0.1e7:
                break
    out.release()
    ev_full.write('D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/R_360_H_PBES.dat'.format(lat, jit, ref, tau, th, th_noise))
def Rotate_360_high_ICNS():
    filename = "D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/20_eevee.mkv"
    lat = 100
    jit = 10
    ref = 100
    tau = 300
    th = 0.3
    th_noise = 0.01
    cap = cv2.VideoCapture(filename)
    dsi.initSimu(int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)), int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)))
    dsi.initLatency(lat, jit, ref, tau)
    dsi.initContrast(th, th, th_noise)
    print(int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
    print(int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)))     
    init_bgn_hist_cpp("D:/2023/computional imaging/my_pbrt/IEBCS-main/data/noise_neg_161lux.npy", "D:/2023/computional imaging/my_pbrt/IEBCS-main/data/noise_neg_161lux.npy")
    isInit = False
    # dt = 1000  # FPS must be 1 kHz,this means fps = dt, and it shows that it can capture high fpx, like bird wings.
            #however, I do not need so high frequency, if I can't output somany pitures 
    dt = 2857
    ev_full = EventBuffer(1)
    ed = EventDisplay("Events", cap.get(cv2.CAP_PROP_FRAME_WIDTH), cap.get(cv2.CAP_PROP_FRAME_HEIGHT), dt*2)
    time = 0
    out = cv2.VideoWriter('video_{}_{}_{}_{}_{}_{}_nonoise.avi'.format(lat, jit, ref, tau, th, th_noise),
        cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'), 20.0, (int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)), int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))))
    while cap.isOpened():
        ret, im = cap.read()
        out.write(im)
        if im is None:
            break
        im = cv2.cvtColor(im, cv2.COLOR_RGB2LUV)[:, :, 0]
        cv2.imshow("t", im)
        cv2.waitKey(1)
        if not isInit:
            dsi.initImg(im)
            isInit = True
        else:
            buf = dsi.updateImg(im, dt)
            ev = EventBuffer(1)
            ev.add_array(np.array(buf["ts"], dtype=np.uint64),
                            np.array(buf["x"], dtype=np.uint16),
                            np.array(buf["y"], dtype=np.uint16),
                            np.array(buf["p"], dtype=np.uint64),
                            10000000)
            ed.update(ev, dt)
            ev_full.increase_ev(ev)
            time += dt
            if time > 0.1e7:
                break
    out.release()
    cap.release()
    ev_full.write('D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/R_360_H_ICNS.dat'.format(lat, jit, ref, tau, th, th_noise))

def View_3D(point_cloud):
    # Create a visualizer object
    vis = o3d.visualization.Visualizer()
    # Add the point cloud to the visualizer
    vis.create_window()
    vis.add_geometry(point_cloud)
    # Get the view control object
    view_control = vis.get_view_control()

    # Set the viewpoint from the left
    view_control.set_lookat([1, 0, 0])  # Look towards positive X-axis
    view_control.set_front([-1, 0, 0])  # Set negative X-axis as the front direction

    # Run the visualizer
    vis.run()
    # Destroy the visualizer window
    vis.destroy_window()
def Compare_Real_and_PBES(Realpath, simPath):
    """use it to test functions
    """    
    events_data = EventsData()
    events_data_IEBCS = EventsData()
    #make sure the video is long enough, or it can't disolay normally
    events_data.read_real_events(Realpath, 100000)
    events_data_IEBCS.read_IEBCS_events(simPath, 100000)
    #3D output is the best way to calibrate
    ev_data0 = events_data.events[0]
    ev_data1 = events_data_IEBCS.events[0]
    point_cloud = events_data.display_events_3D(ev_data0,1500,2000)
    View_3D(point_cloud)
    point_cloud = events_data.display_events_3D(ev_data1,1500,3000)
    View_3D(point_cloud)
    img1 = events_data.display_events(ev_data0,1500,3500)
    cv2.imshow("real",img1)
    cv2.waitKey()
    img2 = events_data.display_events(ev_data1,1500,3500)
    cv2.imshow("sim",img2)
    cv2.waitKey()
    #chamfer
    start_time = time.time()
    loss_same = chamfer_distance_loss(ev_data0, ev_data0)
    print(loss_same)
    loss_different = chamfer_distance_loss(ev_data0, ev_data1)
    print(loss_different)
    end_time = time.time()
    total_time = end_time - start_time
    print("Total time of chamfer method", total_time)
    
    #kernel
    start_time = time.time()
    loss_different = kernel_method_spike_cubes_loss(ev_data0, ev_data1,events_data.width,events_data.height)
    print(loss_different)
    
    end_time = time.time()
    total_time = end_time - start_time
    print("Total time of kernel method", total_time)

#Rotate_360_high()
#Compare_Real_and_PBES("D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/High_360_120deg.hdf5","D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/R_360_H_PBES.dat")
#Rotate_360_high_ICNS()
#Compare_Real_and_PBES("D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/High_360_120deg.hdf5",'D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/R_360_H_ICNS.dat')
Compare_Real_and_PBES("D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/High_360_120deg.hdf5",'D:/2023/computional imaging/my_pbrt/output/Rotate_360_high/R_360_H_ESIM.dat')