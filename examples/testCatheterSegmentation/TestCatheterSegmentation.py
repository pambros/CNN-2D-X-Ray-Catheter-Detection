from __future__ import division
import sys
import numpy as np
import random

print("sys.getrecursionlimit() " + str(sys.getrecursionlimit()))
sys.setrecursionlimit(2000)
np.random.seed(987654)
random.seed(1234569)

from UtilImage import *
from FluoroExtraction import *

GENERATED_PATH = "generated/"
DATA_PATH = "../generateTrainTestDataset/generated/"
WEIGHT_FILE = "../trainCatheterSegmentation/generated/bestTrainWeight.h5"

EQUALIZATION = NORMALIZE_CONTRAST_STRETCHING

DATA_DCM_FILE = 0
DATA_FRAME_ID = 1

dataList = [
	[DATA_PATH + "sequence0045.dcm", 3],
	[DATA_PATH + "sequence0046.dcm", 3],
	[DATA_PATH + "sequence0047.dcm", 3],
	[DATA_PATH + "sequence0048.dcm", 3],
	[DATA_PATH + "sequence0049.dcm", 3]
]

X = np.empty([len(dataList), NB_CHANNEL, SIZE_Y, SIZE_X], dtype=np.float32)
for i in range(len(dataList)):
	assert(IsFileExist(dataList[i][DATA_DCM_FILE]) == True)
	dcmInfo = ReadOnlyDicomInfo(dataList[i][DATA_DCM_FILE])
	print("dcmInfo.Columns " + str(dcmInfo.Columns) + " dcmInfo.Rows " + str(dcmInfo.Rows) + " dcmInfo.NumberOfFrames " + str(dcmInfo.NumberOfFrames))
	assert(dataList[i][DATA_FRAME_ID] >= NB_CHANNEL - 1)

	for j in range(NB_CHANNEL):
		currentFrameId = dataList[i][DATA_FRAME_ID] - j
		frame = GetFloat32DicomFrame(dataList[i][DATA_DCM_FILE], currentFrameId, _normalize=EQUALIZATION)
		X[i, j][...] = frame

fluoroExtraction = FluoroExtraction(WEIGHT_FILE)

for i in range(len(dataList)):
	centerline, Y = fluoroExtraction.ExtractCenterline(X[i])
	
	# for j in range(NB_CHANNEL):
	for j in range(1):
		SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_X" + str(j) + ".png", (X[i, j]*255).astype(np.uint8))
	SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_Y0.png", (Y[0, 0]*255).astype(np.uint8))
	
	rgbImage = GrayToRGB(X[i, 0])
	rgbImage = np.moveaxis(rgbImage, 2, 0)
	DrawCenterline(rgbImage, centerline, [0., 0.5, 1.], [1., 0., 0.], _size=4., _hls=True)
	rgbImage = np.moveaxis(rgbImage, 0, -1)
	SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_X0centerline.png", (rgbImage*255).astype(np.uint8))