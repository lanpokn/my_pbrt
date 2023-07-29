import numpy as np
import sys
sys.path.append("../")
import dsi
import time
import cv2
sys.path.append("../src")
from dvs_sensor import *
import matplotlib.pyplot as plt

dsi.initSimu(337, 450)
dsi.initLatency(100.0, 30.0, 100.0, 1000.0)
dsi.initContrast(0.3, 0.6, 0.035)
init_bgn_hist_cpp("../data/noise_pos_3klux.npy", "../data/noise_pos_3klux.npy")
path_img = "/home/lanpokn/Documents/2023/IEBCS-main/data/img/ball.mp4"
capture = cv2.VideoCapture(path_img) 
ret,img  = capture.read()
#img = cv2.imread(path_img + 'ball.mp4'.format(1606209988861033))
img = cv2.cvtColor(img, cv2.COLOR_RGB2Lab)[:, :, 0]
dsi.initImg(img)
#img = cv2.imread(path_img + 'ball.mp4'.format(1606209988909912))
#img = cv2.cvtColor(img, cv2.COLOR_RGB2Lab)[:, :, 0]
ret,img  = capture.read()
img = cv2.cvtColor(img, cv2.COLOR_RGB2Lab)[:, :, 0]
s = dsi.updateImg(img, 46000)
print(s)
s = dsi.getShape()
print(s)
s = dsi.getCurv()
print(s)
print("Test completed")
