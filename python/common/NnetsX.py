import numpy as np

import logging
logging.getLogger("tensorflow").setLevel(logging.ERROR) # NOTSET DEBUG INFO WARNING ERROR CRITICAL

import tensorflow as tf
print(tf.__version__)
from tensorflow.keras.datasets import mnist
from tensorflow.keras.models import Model, Sequential, model_from_json, load_model
from tensorflow.keras.layers import Input, Dense, Dropout, BatchNormalization, Activation, Flatten, Reshape, Permute
from tensorflow.keras.layers import GaussianNoise, Conv2D, MaxPooling2D, UpSampling2D, Cropping1D
from tensorflow.keras.optimizers import SGD
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras import backend as K
print(K.backend())
print(tf.keras.backend.image_data_format())

if K.image_data_format() == 'channels_first':
	IS_CHANNELS_FIRST = True
	FEATURE_AXIS = 1
	Y_AXIS = 2
	X_AXIS = 3
else:
	IS_CHANNELS_FIRST = False
	Y_AXIS = -3
	X_AXIS = -2
	FEATURE_AXIS = -1

BATCH_NORMALIZATION_NO = -1
BATCH_NORMALIZATION_YES = 0

DOWNSAMPLING_MAXPOOL = 0
DOWNSAMPLING_STRIDED_CONV = 1

UPSAMPLING_UPSAMPLE = 0
UPSAMPLING_TRANSPOSED_CONV = 1

# F1 score: harmonic mean of precision and sensitivity DICE = 2*TP/(2*TP + FN + FP)
def DiceCoef(y_true, y_pred):
	y_true_f = K.flatten(y_true)
	y_pred_f = K.flatten(y_pred)
	intersection = K.sum(y_true_f*y_pred_f)
	return (2.*intersection)/(K.sum(y_true_f) + K.sum(y_pred_f) + 0.00001)

def DiceCoefLoss(y_true, y_pred):
	return -DiceCoef(y_true, y_pred)

def MyReLU(_x):
	return Activation('relu')(_x)
	
class NNets(object):
	def __init__(_self):
		m_SamePartActivation = MyReLU
		m_RegularizerL1L2 = False # TODO not implemented
		m_Dropout = 0.5 # if -1: no dropout else: the percentage of dropped out neurons
		m_Residual = True
		m_BatchNormalization = BATCH_NORMALIZATION_YES
		m_BorderMode = "same"
		m_Initialization = "glorot_uniform" # TODO not implemented
		m_DownSampling = DOWNSAMPLING_STRIDED_CONV
		m_UpSampling = UPSAMPLING_UPSAMPLE # TODO deconv not tested

	def ResidualBlock(_self, _inputs, _nbConv, _nbFilters, _nbRow, _nbCol):
		x = _inputs
		# print("_inputs.shape " + str(_inputs.shape))
		for i in range(_nbConv - 1):
			x = Conv2D(_nbFilters, (_nbRow, _nbCol), padding=_self.m_BorderMode)(x)
			if _self.m_BatchNormalization != BATCH_NORMALIZATION_NO:
				x = BatchNormalization(axis=FEATURE_AXIS)(x)
			x = _self.m_SamePartActivation(x)
		x = Conv2D(_nbFilters, (_nbRow, _nbCol), padding=_self.m_BorderMode)(x)
		if _self.m_BatchNormalization != BATCH_NORMALIZATION_NO:
			x = BatchNormalization(axis=FEATURE_AXIS)(x)
		# print("x.shape " + str(x.shape))
		if _self.m_Residual == True:
			x = tf.keras.layers.add([x, _inputs])
		x = _self.m_SamePartActivation(x)
		return x
	
	def DownsampleBlock(_self, _inputs, _nbFilters):
		if _self.m_DownSampling == DOWNSAMPLING_MAXPOOL:
			x = MaxPooling2D(pool_size=(2, 2))(_inputs)
			# TODO max pool does not work with residual, make it work
			if _self.m_Residual == True:
				assert False
		else:
			x = Conv2D(_nbFilters, (2, 2), padding=_self.m_BorderMode, strides=(2,2))(_inputs)
			if _self.m_BatchNormalization != BATCH_NORMALIZATION_NO:
				x = BatchNormalization(axis=FEATURE_AXIS)(x)
			x = _self.m_SamePartActivation(x)
		return x
		
	def DownsamplingPart(_self, _inputs, _nbFilters, _kernelSize, _convPerLevel):
		levels = []
		downBlock = _inputs
		for i in range(len(_convPerLevel)):
			residualBlock = _self.ResidualBlock(downBlock, _convPerLevel[i], _nbFilters, _kernelSize, _kernelSize)
			levels.append(residualBlock)
			# we add a dropout only at the last two layers
			if _self.m_Dropout != -1 and i >= len(_convPerLevel) - 2: 
				residualBlock = Dropout(_self.m_Dropout)(residualBlock)
			# no pooling for the last layer
			if i != len(_convPerLevel) - 1:
				_nbFilters = _nbFilters*2
				downBlock = _self.DownsampleBlock(residualBlock, _nbFilters)
		# we add the dropout in the last layer
		levels[-1] = residualBlock
		return levels, _nbFilters
	
	def UpsampleBlock(_self, _inputs, _nbFilters):
		if _self.m_UpSampling == UPSAMPLING_UPSAMPLE:
			x = UpSampling2D(size=(2,2))(_inputs)
			x = Conv2D(_nbFilters, (2, 2), padding=_self.m_BorderMode)(x)	
		else:
			assert False
			# x = Deconvolution2D(_nbFilters, 2, 2, output_shape=_outputShape, padding=_self.m_BorderMode, strides=(2,2))(_inputs)
		if _self.m_BatchNormalization != BATCH_NORMALIZATION_NO:
			x = BatchNormalization(axis=FEATURE_AXIS)(x)	
		x = _self.m_SamePartActivation(x)
		return x

	def UpsamplingConcatPart(_self, _inputs, _inputLevels, _nbFilters, _kernelSize, _convPerLevel):
		levels = []
		residualUpBlock = _inputs
		for i in range(len(_convPerLevel)):
			_nbFilters = _nbFilters//2
			upBlock = _self.UpsampleBlock(residualUpBlock, _nbFilters)
			# print("upBlock.shape " + str(upBlock.shape))
			# upBlock = Dropout(0.5)(upBlock)
			mergeBlock = tf.keras.layers.concatenate([upBlock, _inputLevels[i]], axis=FEATURE_AXIS)
			residualUpBlock = _self.ResidualBlock(mergeBlock, _convPerLevel[i], _nbFilters*2, _kernelSize, _kernelSize)
			levels.append(residualUpBlock)
		return levels

	def DefineDeepUVNet(_self, _inputShape, _nbFilters, _kernelSize, _convPerLevel, _upConvPerLevel, _optimizer):
		inputs = Input(shape=_inputShape)
		# print("inputs.shape " + str(inputs.shape))
		x = GaussianNoise(0.03)(inputs)
		shortcut = Conv2D(_nbFilters, (1, 1), padding=_self.m_BorderMode)(x)
		# print("shortcut.shape " + str(shortcut.shape))
		
		levels, nbFilters = _self.DownsamplingPart(shortcut, _nbFilters, _kernelSize, _convPerLevel)
		
		inputLevels = []
		for i in range(len(_convPerLevel) - 1):
			inputLevels.append(levels[len(_convPerLevel) - 1 - 1 - i])
		
		upLevels = _self.UpsamplingConcatPart(levels[-1], inputLevels, nbFilters, _kernelSize, _upConvPerLevel)
		
		# sigmoid
		if True:
		# if False:
			outputs = Conv2D(1, (1, 1), activation='sigmoid')(upLevels[-1])

		# 2-class softmax
		# elif True:
		elif False:
			nbOutputs = 2
			outputs = Conv2D(nbOutputs, (1, 1))(upLevels[-1])
			outputs = Reshape((nbOutputs, _inputShape[1]*_inputShape[2]))(outputs)
			outputs = Permute((2,1))(outputs)
			outputs = Activation("softmax")(outputs)
			outputs = Permute((2,1))(outputs)
			outputs = Cropping1D(cropping=((0, 1)))(outputs)
			outputs = Reshape((nbOutputs - 1, _inputShape[1], _inputShape[2]))(outputs)
		
		model = Model(inputs=inputs, outputs=outputs)
		model.compile(optimizer=_optimizer, loss=DiceCoefLoss, metrics=[DiceCoef])

		return model