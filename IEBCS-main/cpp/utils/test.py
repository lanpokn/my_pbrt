import numpy as np
import cv2
import sgrb2lum

# Load an SRGB image
srgb = cv2.imread("image.png", cv2.IMREAD_COLOR)
srgb = srgb.astype(np.float32)

# Convert SRGB to luminance
luminance = sgrb2lum.SGRB2Luminance(srgb)

print(luminance)