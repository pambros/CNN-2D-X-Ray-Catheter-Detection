from __future__ import division
import sys
import numpy as np
import random

from UtilImage import *

GENERATED_PATH = "generated/"
DATA_PATH = "../../examples/testCatheterSegmentation/generated/"

imageList = [19]

FACTOR = 0.25
SIZE = 1024
fullImage = np.zeros((len(imageList)*3, (int)(SIZE*FACTOR), (int)(SIZE*FACTOR), 3), dtype = np.float32)
inc = 0

for i in imageList:
	fullImage[inc][...] = GrayToRGB(ResizeImage(LoadImage(DATA_PATH + "dcm_" + str(i) + "_X0.png")/255., FACTOR))
	fullImage[inc + 1][...] = GrayToRGB(ResizeImage(LoadImage(DATA_PATH + "dcm_" + str(i) + "_Y0.png")/255., FACTOR))
	fullImage[inc + 2][...] = ResizeImageMultiChan(LoadImage(DATA_PATH + "dcm_" + str(i) + "_X0centerline.png")/255., FACTOR)
	inc = inc + 3

fullImage = np.rollaxis(fullImage, 3, 1)
SaveSetImagesMultiChan(GENERATED_PATH + "catheterExtractionExample.png", fullImage, 3, len(imageList))