from __future__ import division
import sys
import numpy as np
import random
import time

print("sys.getrecursionlimit() " + str(sys.getrecursionlimit()))
sys.setrecursionlimit(2000)
np.random.seed(987654)
random.seed(1234569)

from File import *
from FluoroDataObject import *
from DataAugmentation import *

if True:
# if False:
	from NnetsX import *

GENERATED_PATH = "generated/"
DATA_PATH = "../generateTrainTestDataset/generated/"

WEIGHT_FILE = GENERATED_PATH + "bestTrainWeight" + ".h5"

SIZE_X = 1024
SIZE_Y = 1024

NB_CHANNEL = 4

FORMAT_CONFIG = "{0:04d}"

# load training data [SET_PROCEDURE, SET_FLUORO_FILE, SET_FLUORO_FRAME, SET_FLUORO_CENTERLINE, SET_FLUORO_INFO] and save it in X.h5 and Y.h5
dataList = []
NB_GENERATED_SEQUENCES = 50
for i in range(NB_GENERATED_SEQUENCES):
	dataList.append([i, DATA_PATH + "sequence" + str(FORMAT_CONFIG).format(i) + ".dcm", 3, DATA_PATH + "sequence" + str(FORMAT_CONFIG).format(i) + "centerline" + str(FORMAT_CONFIG).format(3) + ".txt"])

# warning if you change dataList, remove the files X.h5 and Y.h5. If not, the previous X.h5 and Y.h5 files will be loaded.
dataObject = FluoroDataObject(dataList, SIZE_X, SIZE_Y, NB_CHANNEL, _savePath = GENERATED_PATH, _pctTrainingSet = 0.5)

if True:
# if False:
	optimizer = SGD(lr=0.01, decay=5e-4, momentum=0.99)
	nnets = NNets()
	nnets.m_SamePartActivation = MyReLU
	nnets.m_RegularizerL1L2 = False
	nnets.m_Dropout = 0.5
	nnets.m_Residual = True
	nnets.m_BatchNormalization = BATCH_NORMALIZATION_YES
	nnets.m_BorderMode = "same"
	nnets.m_Initialization = "glorot_uniform"
	nnets.m_DownSampling = DOWNSAMPLING_STRIDED_CONV
	nnets.m_UpSampling = UPSAMPLING_UPSAMPLE
	nbUsedChannel = NB_CHANNEL
	nbStartFilter = 8
	kernelSize = 3
	nbConvPerLayer = [2, 2, 2, 2, 2, 2, 2]
	nbDeconvPerLayer = [2, 2, 2, 2, 2, 2]
	nbEpoch = 150
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
	
	model = nnets.DefineDeepUVNet((nbUsedChannel, SIZE_Y, SIZE_X), _nbFilters=nbStartFilter, _kernelSize=kernelSize \
		, _convPerLevel=nbConvPerLayer, _upConvPerLevel=nbDeconvPerLayer, _optimizer=optimizer)
	print("input " + str(model.get_input_shape_at(0)) + " output " + str(model.get_output_shape_at(0)))
	print(len(model.layers))
	# model.summary()

	modelCheckpoint = ModelCheckpoint(GENERATED_PATH + "bestTrainWeight" + ".h5", monitor='loss', save_best_only=True, save_weights_only=True)
	modelCheckpointTest = ModelCheckpoint(GENERATED_PATH + "bestTestWeight" + ".h5", monitor='val_loss', save_best_only=True, save_weights_only=True)
	
	def ImgGenerator():
		for image in GenerateImageOnTheFly(dataObject.CreateImageX, dataObject.GetIdFromNeed, dataObject.m_X, nbUsedChannel - 1, dataObject.m_Y, None, 0, dataObject.m_TrainSetList, dataObject.m_NeedSetList, dataObject.m_ValidSetList, _batchSize=batchsize, _epochSize=EPOCH_SIZE, _nbData=nbData, _keepPctOriginal=keepPctOriginal, _trans=trans, _rot=rot, _zoom=zoom, _shear=shear, _elastix=elastix, _intensity=intensity, _hflip=hflip, _vflip=vflip, _3Dshape=is3Dshape):
			yield image

	def ValidationGenerator():
		for image in GenerateValidationOnTheFly(dataObject.CreateImageX, dataObject.GetIdFromNeed, dataObject.m_X, nbUsedChannel - 1, dataObject.m_Y, None, 0, dataObject.m_TestSetList, dataObject.m_NeedSetList, dataObject.m_ValidSetList, _batchSize=batchsize, _3Dshape=is3Dshape):
			yield image
	
	# if True:
	if False:
		model.load_weights(WEIGHT_FILE)
	
	if True:
	# if False:
		stepsPerEpoch = math.ceil(EPOCH_SIZE/batchsize)
		validationSteps = math.ceil(len(dataObject.m_TestSetList)/batchsize)
		history = model.fit_generator(ImgGenerator(), verbose=2, workers=1, validation_data=ValidationGenerator(), steps_per_epoch=stepsPerEpoch, epochs=nbEpoch, validation_steps=validationSteps, callbacks=[modelCheckpoint, modelCheckpointTest])
		model.save_weights(GENERATED_PATH + "lastweight" + ".h5")
		SavePickle(history.history, GENERATED_PATH + "history" + ".pickle")