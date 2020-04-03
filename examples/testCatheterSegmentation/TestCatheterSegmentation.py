import numpy as np
import random

np.random.seed(987654)
random.seed(1234569)

from UtilImage import *
from FluoroExtraction import *

GENERATED_PATH = "generated/"
DATA_PATH = "../generateTrainTestDataset/generated/"
WEIGHT_FILE = "../trainCatheterSegmentation/generated/bestTrainWeight.h5"

EQUALIZATION = NORMALIZE_CONTRAST_STRETCHING

FORMAT_CONFIG = "{0:04d}"

DATA_DCM_FILE = 0
DATA_FRAME_ID = 1

NB_GENERATED_SEQUENCES = 20
dataList = []
for i in range(0,NB_GENERATED_SEQUENCES):
	dataList.append([DATA_PATH + "sequence" + str(FORMAT_CONFIG).format(i) + ".dcm", 3])

X = np.empty([len(dataList), 1, NB_CHANNEL, SIZE_Y, SIZE_X], dtype=np.float32)
for i in range(len(dataList)):
	assert(dataList[i][DATA_FRAME_ID] >= NB_CHANNEL - 1)
	assert(IsFileExist(dataList[i][DATA_DCM_FILE]) == True)
	
	dcmInfo = ReadOnlyDicomInfo(dataList[i][DATA_DCM_FILE])
	print("dcmInfo.Columns " + str(dcmInfo.Columns) + " dcmInfo.Rows " + str(dcmInfo.Rows) + " dcmInfo.NumberOfFrames " + str(dcmInfo.NumberOfFrames))

	for j in range(NB_CHANNEL):
		currentFrameId = dataList[i][DATA_FRAME_ID] - j
		frame = GetFloat32DicomFrame(dataList[i][DATA_DCM_FILE], currentFrameId, _normalize=EQUALIZATION)
		X[i, 0, j][...] = frame

fluoroExtraction = FluoroExtraction(WEIGHT_FILE)

for i in range(len(dataList)):
	centerline, Y = fluoroExtraction.ExtractCenterline(X[i])
	
	# for j in range(NB_CHANNEL):
	for j in range(1):
		SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_X" + str(j) + ".png", (X[i, 0, j]*255).astype(np.uint8))
	SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_Y0.png", (Y[0, 0]*255).astype(np.uint8))
	
	rgbImage = GrayToRGB(X[i, 0, 0])
	rgbImage = np.moveaxis(rgbImage, 2, 0)
	DrawCenterline(rgbImage, centerline, [0., 0.5, 1.], [1., 0., 0.], _size=4., _hls=True)
	rgbImage = np.moveaxis(rgbImage, 0, -1)
	SaveImage(GENERATED_PATH + "dcm_" + str(i) + "_X0centerline.png", (rgbImage*255).astype(np.uint8))