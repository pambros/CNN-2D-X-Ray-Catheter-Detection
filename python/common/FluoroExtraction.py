from __future__ import division
import scipy as sp
import time
from skimage.morphology import skeletonize

from Util import *
from FluoroDataObject import *

from NnetsX import *

import PyTACELib

# PYTACELIB_DEBUG_OUTPUT = True
PYTACELIB_DEBUG_OUTPUT = False

THRESHOLD_BINARY = 0.01
# THRESHOLD_BINARY = 0.5

SIZE_X = 1024
SIZE_Y = 1024

NB_CHANNEL = 4

def GetCenterline(_image, _imgInfo = None):
	image = _image
	image[image > THRESHOLD_BINARY] = 1
	image[image <= THRESHOLD_BINARY] = 0
	if _imgInfo != None:
		image = image[_imgInfo[INFO_PAD_Y] + _imgInfo[INFO_DIAPHRAGM_Y1]:_imgInfo[INFO_PAD_Y] + _imgInfo[INFO_DIAPHRAGM_Y2] + 1,_imgInfo[INFO_PAD_X] + _imgInfo[INFO_DIAPHRAGM_X1]:_imgInfo[INFO_PAD_X] + _imgInfo[INFO_DIAPHRAGM_X2] + 1]
	image = skeletonize(image)
	# print("image.dtype " + str(image.dtype))
	image = (image*255).astype(np.uint8)
	# SaveImage("generated/debug.png", image)
	if PYTACELIB_DEBUG_OUTPUT == True:
		centerline, debugStepList = PyTACELib.ExtractCenterline(image)
	else:
		centerline = PyTACELib.ExtractCenterline(image)
	
	centerline = np.array(centerline)
	if centerline.shape[0] > 3:
		centerline = np.swapaxes(centerline, 0, 1)
		
		tck,u = sp.interpolate.splprep([centerline[0], centerline[1]], k=3)
		x, y = sp.interpolate.splev(np.linspace(0, 1, len(centerline[0])), tck)
		# print("x.shape " + str(x.shape))
		# print("y.shape " + str(y.shape))
		centerline = np.stack((x, y))
		centerline = np.swapaxes(centerline, 0, 1)
		# print("centerline.shape " + str(centerline.shape))
		# print(centerline[:5])
	if _imgInfo != None:
		centerline = centerline + (_imgInfo[INFO_PAD_X] + _imgInfo[INFO_DIAPHRAGM_X1], _imgInfo[INFO_PAD_Y] + _imgInfo[INFO_DIAPHRAGM_Y1])
	centerline = centerline.clip((0, 0), (image.shape[1] - SMALL_POSITIVE_NUMBER, image.shape[0] - SMALL_POSITIVE_NUMBER))
	
	if PYTACELIB_DEBUG_OUTPUT == True:
		return centerline, debugStepList
		
	return centerline

class FluoroExtraction(object):
	def __init__(_self, _weightsFile=None):
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
		
		_self.m_Model = nnets.DefineDeepUVNet((nbUsedChannel, SIZE_Y, SIZE_X), _nbFilters=nbStartFilter, _kernelSize=kernelSize \
			, _convPerLevel=nbConvPerLayer, _upConvPerLevel=nbDeconvPerLayer, _optimizer=optimizer)
		print("input " + str(_self.m_Model.get_input_shape_at(0)) + " output " + str(_self.m_Model.get_output_shape_at(0)))
		print(len(_self.m_Model.layers))
		# _self.m_Model.summary()
		
		if _weightsFile is not None:
			_self.m_Model.load_weights(_weightsFile)

	def ExtractCenterline(_self, _images, _imgInfo = None):
		t0 = time.time()
		Y = _self.m_Model.predict(_images, batch_size=1)
		print("_self.m_Model.predict " + str(time.time() - t0) + " s")
		
		if PYTACELIB_DEBUG_OUTPUT == True:
			centerline, debugStepList = GetCenterline(Y[0, 0], _imgInfo)
		else:
			centerline = GetCenterline(Y[0, 0], _imgInfo)
		print("ExtractCenterline " + str(time.time() - t0) + " s")
		print("Y.shape " + str(Y.shape) + " Y.dtype " + str(Y.dtype) + " min(Y) " + str(np.min(Y)) + " max(Y) " + str(np.max(Y)))
		print("len(centerline) " + str(len(centerline)))

		return centerline, Y