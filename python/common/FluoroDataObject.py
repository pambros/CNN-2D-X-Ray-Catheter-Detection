import numpy as np
import time
import random

from File import *
from UtilImage import *
from DataObject import *

DILATION_STRUCTURE = (5,5)
EQUALIZATION = NORMALIZE_CONTRAST_STRETCHING

# fluoro info
INFO_SIZE_X = 0
INFO_SIZE_Y = 1
INFO_PAD_X = 2
INFO_PAD_Y = 3
INFO_DIAPHRAGM_X1 = 4
INFO_DIAPHRAGM_X2 = 5
INFO_DIAPHRAGM_Y1 = 6
INFO_DIAPHRAGM_Y2 = 7
INFO_COUNT = 8

SET_PROCEDURE = 0
SET_FLUORO_FILE = 1
SET_FLUORO_FRAME = 2
SET_FLUORO_CENTERLINE = 3
SET_FLUORO_INFO = 4

if True:
# if False:
	def GetFloat32NormalizedFrameWithoutBorders(_image, _bitDepth, _normalize, _imgInfo):
		image = _image[_imgInfo[INFO_DIAPHRAGM_Y1]:_imgInfo[INFO_DIAPHRAGM_Y2] + 1, _imgInfo[INFO_DIAPHRAGM_X1]:_imgInfo[INFO_DIAPHRAGM_X2] + 1]
		image = GetFloat32NormalizedFrame(image, _bitDepth, _normalize)
		image = ski.util.pad(image, ((_imgInfo[INFO_DIAPHRAGM_Y1], _imgInfo[INFO_SIZE_Y] - 1 - _imgInfo[INFO_DIAPHRAGM_Y2]),(_imgInfo[INFO_DIAPHRAGM_X1], _imgInfo[INFO_SIZE_X] - 1 - _imgInfo[INFO_DIAPHRAGM_X2])), 'constant', constant_values=0)
		return image
else:
	# debug with no removing of the border
	def GetFloat32NormalizedFrameWithoutBorders(_image, _bitDepth, _normalize, _imgInfo):
		return GetFloat32NormalizedFrame(_image, _bitDepth, _normalize)

def GetIdFromSet(_set, _fluoroFile, _frame):
	for i in range(len(_set)):
		if _set[i][SET_FLUORO_FILE] == _fluoroFile and _set[i][SET_FLUORO_FRAME] == _frame:
			return i
	return -1

class FluoroDataObject(DataObject):
	def __init__(_self, _dataList, _sizeImageX, _sizeImageY, _nbChannel, _savePath, _pctTrainingSet = 0.7):
		# print("FluoroDataObject::__init__")

		# prepare the data list
		_self.m_ValidSetList = []
		for data in _dataList:
			if IsFileExist(data[SET_FLUORO_FILE]) == False:
				print("warning " + str(data[SET_FLUORO_FILE]) + " was not found")
				continue
				
			dcmInfo = ReadOnlyDicomInfo(data[SET_FLUORO_FILE])
			if dcmInfo.Columns > _sizeImageX or dcmInfo.Rows > _sizeImageY:
				print("warning " + str(data[SET_FLUORO_FILE]) + " fluoro bigger than (" + str(_sizeImageX) + "," + str(_sizeImageY) + ") are not used")
				continue

			if IsFileExist(data[SET_FLUORO_CENTERLINE]) == False:
				print("warning " + str(data[SET_FLUORO_CENTERLINE]) + " was not found")
				continue
				
			if data[SET_FLUORO_FRAME] < _nbChannel - 1:
				print("warning " + str(data[SET_FLUORO_FILE]) + " frames before " + str(_nbChannel - 1) + " are not used")
				continue

			_self.m_ValidSetList.append(data)

		_self.m_NeedSetList = []
		for data in _self.m_ValidSetList:
			for i in range(_nbChannel):
				id = (_nbChannel - 1) - i
				if GetIdFromSet(_self.m_NeedSetList, data[SET_FLUORO_FILE], data[SET_FLUORO_FRAME] - id) == -1:
					_self.m_NeedSetList.append([data[SET_PROCEDURE], data[SET_FLUORO_FILE], data[SET_FLUORO_FRAME] - id, data[SET_FLUORO_CENTERLINE]])
		
		# load the data
		if IsFileExist(_savePath + "X.h5") == True:
			_self.m_X = LoadH5Set(_savePath + "X.h5")
		else:
			t0 = time.time()
			_self.m_X = np.empty([len(_self.m_NeedSetList), 1, _sizeImageY, _sizeImageX], dtype=np.float32)
			for frameId in range(len(_self.m_NeedSetList)):
				frame = _self.m_NeedSetList[frameId]
				print("frame[SET_FLUORO_FILE], frame[SET_FLUORO_FRAME] " + str(frame[SET_FLUORO_FILE]) + " " + str(frame[SET_FLUORO_FRAME]))
				tmpFrame = GetFloat32DicomFrame(frame[SET_FLUORO_FILE], frame[SET_FLUORO_FRAME], _normalize=EQUALIZATION)
				if tmpFrame.shape[1] != _sizeImageX or tmpFrame.shape[0] != _sizeImageY:
					tmpFrame, _, _ = PadImage(tmpFrame, _sizeImageX, _sizeImageY)
				_self.m_X[frameId][0] = tmpFrame
			print("_self.m_X.shape" + str(_self.m_X.shape))
			print("time " + str(time.time() - t0) + " s")
			SaveH5Set(_savePath + "X.h5", _self.m_X)
		
		if IsFileExist(_savePath + "Y.h5") == True:
			_self.m_Y = LoadH5Set(_savePath + "Y.h5")
		else:
			t0 = time.time()
			_self.m_Y = np.empty([len(_self.m_ValidSetList), 1, _sizeImageY, _sizeImageX], dtype=np.float32)
			for frameId in range(len(_self.m_ValidSetList)):
				frame = _self.m_ValidSetList[frameId]
				dcmInfo = ReadOnlyDicomInfo(frame[SET_FLUORO_FILE])
				ptsList = PtsListFromFile(frame[SET_FLUORO_CENTERLINE])
				tmpFrame = PtsListToMask(dcmInfo.Columns, dcmInfo.Rows, ptsList, DILATION_STRUCTURE).astype(np.float32)
				if tmpFrame.shape[1] != _sizeImageX or tmpFrame.shape[0] != _sizeImageY:
					tmpFrame, _, _ = PadImage(tmpFrame, _sizeImageX, _sizeImageY)
				_self.m_Y[frameId][0] = tmpFrame
			print("_self.m_Y.shape" + str(_self.m_Y.shape))
			print("time " + str(time.time() - t0) + " s")
			SaveH5Set(_savePath + "Y.h5", _self.m_Y)
		
		# divide the data in training and testing set
		procedureIdList = []
		for frame in _self.m_NeedSetList:
			if frame[SET_PROCEDURE] not in procedureIdList:
				procedureIdList.append(frame[SET_PROCEDURE])
		print("len(procedureIdList) " + str(len(procedureIdList)))
		# print(procedureIdList)

		shuffleProcedures = np.array(procedureIdList)
		# random.shuffle(shuffleProcedures)
		# print(shuffleProcedures)
		
		_self.m_TrainSetList = []
		_self.m_TestSetList = []
		for frame in _self.m_ValidSetList:
			id = np.where(shuffleProcedures == frame[SET_PROCEDURE])[0][0]
			if id + 1 <= _pctTrainingSet*len(shuffleProcedures):
				_self.m_TrainSetList.append(frame)
				# print("train " + str(frame[SET_PROCEDURE_ID]))
			else:
				_self.m_TestSetList.append(frame)
				# print("test " + str(frame[SET_PROCEDURE_ID]) + " " + str(frame[SET_FLUORO_ID]) + " " + str(frame[SET_FLUORO_TIME]))
		
		print("len(_self.m_TrainSetList) " + str(len(_self.m_TrainSetList)))
		print("len(_self.m_TestSetList) " + str(len(_self.m_TestSetList)))
	
	@staticmethod
	def CreateImageX(_xx, _i, _X, _previousImages, _output, _previousOutput, _setFiles, _needSetX, _needSetY, _blackImage):
		# print("FluoroDataObject::CreateImageX")
		fluoroFile = _setFiles[_i][SET_FLUORO_FILE]
		frame = _setFiles[_i][SET_FLUORO_FRAME]
		
		id = GetIdFromSet(_needSetX, fluoroFile, frame)
		_xx[0][...] = _X[id,0,...]
		for t in range(_previousImages):
			id = GetIdFromSet(_needSetX, fluoroFile, frame - (t + 1))
			_xx[t + 1][...] = _X[id,0,...]
		for t in range(_previousOutput):
			id = GetIdFromSet(_needSetY, fluoroFile, frame - (t + 1))
			# id = GetIdFromSet(_needSetY, fluoroFile, frame - (t + 0))
			# if debug <= 4: # debug
				# print(str(t) + " " + str(fluoroFile) + " " + str(int(frame - (t + 1))) + " " + str(id))
			if id == -1:
				_xx[_previousImages + t + 1][...] = _blackImage[...]
			else:
				_xx[_previousImages + t + 1][...] = _output[id,...]

	@staticmethod
	def GetIdFromNeed(_i, _setFiles, _needSetY):
		# print("FluoroDataObject::GetIdFromNeed")
		fluoroFile = _setFiles[_i][SET_FLUORO_FILE]
		frame = _setFiles[_i][SET_FLUORO_FRAME]
		return GetIdFromSet(_needSetY, fluoroFile, frame)