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
# filename = "C:/Users/hhq/Documents/blender/PBES_small/rotate_360/0000-0060.mkv"

#lower lat will get more active pixel(remain event)
lat = 100
# higher jit will get blurer pixel and more active pixel , very obvious
jit = 10
#lower ref, higher active
ref = 100
#it's the trans func, higher tau , lowwer the event gen
tau = 3
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
ev_full.write('evrotate_60_highlight_{}_{}_{}_{}_{}_{}.dat'.format(lat, jit, ref, tau, th, th_noise))