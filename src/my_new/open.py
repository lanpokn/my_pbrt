# import imageio
# image = imageio.imread('src/tests/killeroo-simple.exr', 'exr')
# imageio.imwrite('save_imageio.png', image)

import os
os.environ["OPENCV_IO_ENABLE_OPENEXR"]="1"
import cv2
image = cv2.imread("src/tests/killeroo-simple.exr",cv2.IMREAD_UNCHANGED)
cv2.imshow("show",image)
cv2.waitKey()