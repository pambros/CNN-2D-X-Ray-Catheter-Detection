from __future__ import division
import sys
import numpy as np
import random
import time

# print("sys.getrecursionlimit() " + str(sys.getrecursionlimit()))
sys.setrecursionlimit(2000)
np.random.seed(987654)
random.seed(1234569)

from File import *
from FluoroDataObject import *
from DataAugmentation import *
from NnetsX import *
from FluoroExtraction import *

GENERATED_PATH = "generated/"
DATA_PATH = "../generateTrainTestDataset/generated/"

FORMAT_CONFIG = "{0:04d}"

# load training data [SET_PROCEDURE, SET_FLUORO_FILE, SET_FLUORO_FRAME, SET_FLUORO_CENTERLINE, SET_FLUORO_INFO] and save it in X.h5 and Y.h5
NB_GENERATED_SEQUENCES = 20
dataList = []
for i in range(NB_GENERATED_SEQUENCES):
	dataList.append([i, DATA_PATH + "sequence" + str(FORMAT_CONFIG).format(i) + ".dcm", 3, DATA_PATH + "sequence" + str(FORMAT_CONFIG).format(i) + "centerline" + str(FORMAT_CONFIG).format(3) + ".txt"])

# warning if you change dataList, remove the files X.h5 and Y.h5. If not, the previous X.h5 and Y.h5 files will be loaded.
dataObject = FluoroDataObject(dataList, SIZE_X, SIZE_Y, NB_CHANNEL, _savePath = GENERATED_PATH, _pctTrainingSet = 0.5)

fluoroExtraction = FluoroExtraction()
# fluoroExtraction = FluoroExtraction(GENERATED_PATH + "bestTrainWeight" + ".h5")

nbUsedChannel = NB_CHANNEL
nbEpoch = 300
batchsize = 2
EPOCH_SIZE = len(dataObject.m_TrainSetList)
is3Dshape = False
# data augmentation
nbData = -1
keepPctOriginal = 0.5
trans = 0.16 # +/- proportion of the image size
rot = 9 # +/- degree
zoom = 0.12 # +/- factor
shear = 0 # +/- in radian x*np.pi/180
elastix = 0 # in pixel
intensity = 0.07 # +/- factor
hflip = True
vflip = True

modelCheckpoint = ModelCheckpoint(GENERATED_PATH + "bestTrainWeight" + ".h5", monitor='loss', save_best_only=True, save_weights_only=True)
modelCheckpointTest = ModelCheckpoint(GENERATED_PATH + "bestTestWeight" + ".h5", monitor='val_loss', save_best_only=True, save_weights_only=True)

def ImgGenerator():
	for image in GenerateImageOnTheFly(dataObject.CreateImageX, dataObject.GetIdFromNeed, dataObject.m_X, nbUsedChannel - 1, dataObject.m_Y, None, 0, dataObject.m_TrainSetList, dataObject.m_NeedSetList, dataObject.m_ValidSetList, _batchSize=batchsize, _epochSize=EPOCH_SIZE, _nbData=nbData, _keepPctOriginal=keepPctOriginal, _trans=trans, _rot=rot, _zoom=zoom, _shear=shear, _elastix=elastix, _intensity=intensity, _hflip=hflip, _vflip=vflip, _3Dshape=is3Dshape):
		yield image

def ValidationGenerator():
	for image in GenerateValidationOnTheFly(dataObject.CreateImageX, dataObject.GetIdFromNeed, dataObject.m_X, nbUsedChannel - 1, dataObject.m_Y, None, 0, dataObject.m_TestSetList, dataObject.m_NeedSetList, dataObject.m_ValidSetList, _batchSize=batchsize, _3Dshape=is3Dshape):
		yield image

stepsPerEpoch = math.ceil(EPOCH_SIZE/batchsize)
validationSteps = math.ceil(len(dataObject.m_TestSetList)/batchsize)
history = fluoroExtraction.m_Model.fit_generator(ImgGenerator(), verbose=2, workers=1, validation_data=ValidationGenerator(), steps_per_epoch=stepsPerEpoch, epochs=nbEpoch, validation_steps=validationSteps, callbacks=[modelCheckpoint, modelCheckpointTest])
fluoroExtraction.m_Model.save_weights(GENERATED_PATH + "lastweight" + ".h5")
SavePickle(history.history, GENERATED_PATH + "history" + ".pickle")