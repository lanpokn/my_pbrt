import OpenEXR
import Imath
import numpy as np
import cv2

#BrightScale 1 not change ,1000 muti 1000, because scene is easy, and exr has a wide range 
# we can simply muti a number to get high img based on low img, so we only need to render low img
# which can barely not seen
def read_exr_channel(exr_file,channel_name,BrightScale = 1):
    # Read the EXR file
    exr = OpenEXR.InputFile(exr_file)

    # Get the image header and channel information
    header = exr.header()
    channel_info = header['channels']

    # Get the size of the image
    width = header['dataWindow'].max.x + 1
    height = header['dataWindow'].max.y + 1

    # Read the specified channel
    channel_data = exr.channel(channel_name, Imath.PixelType(Imath.PixelType.FLOAT))

    # Convert the channel data to a NumPy array
    channel_np = np.frombuffer(channel_data, dtype=np.float32)
    channel_np = np.reshape(channel_np, (height, width))

    # Create a CV data type with the same size as the channel data
    cv_image = np.zeros((height, width), dtype=np.float32)

    # Assign the channel data to the CV image
    cv_image[:, :] = channel_np * BrightScale

    # Display the CV image
    cv2.imshow("Channel Image", cv_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

# Example usage
exr_file = "C:/Users/hhq/Desktop/exr_test/Rotate_360_pbrt00121exr"
channel_name = "intensity"
read_exr_channel(exr_file, channel_name)