from __future__ import division
import scipy as sp
import numpy as np
import random

np.random.seed(987654)
random.seed(1234569)

from File import *
from Util import *
from UtilImage import *

NB_GENERATED_SEQUENCES = 50

FORMAT_CONFIG = "{0:04d}"

SIZE_X = 1024
SIZE_Y = 1024

NB_FRAMES = 4

NB_PTS = 500

def CreateFakeFluoroscopy(_x, _y):
	SMALL_POSITIVE_NUMBER = 0.000001
	
	# create the centerline from the control points
	tck, u = sp.interpolate.splprep([_x, _y])
	x, y = sp.interpolate.splev(np.linspace(0, 1, NB_PTS), tck)
	centerline = np.stack((x, y))
	centerline = np.swapaxes(centerline, 0, 1)
	centerline = np.array(centerline)
	centerline = centerline.clip((0, 0), (SIZE_X - SMALL_POSITIVE_NUMBER, SIZE_Y - SMALL_POSITIVE_NUMBER))

	# create the X-ray image with the centerline
	centerlineImage = np.zeros((SIZE_Y,SIZE_X))
	centerlineImage = GrayToRGB(centerlineImage)
	centerlineImage = np.moveaxis(centerlineImage, 2, 0)
	DrawCenterline(centerlineImage, centerline, _deltaColor=[0,0,0], _size=5)

	noise = np.random.rand((SIZE_X*SIZE_Y))
	NOISE_AMPLITUDE = 6
	noise = noise*NOISE_AMPLITUDE - NOISE_AMPLITUDE/2 # -NOISE_AMPLITUDE/2 <= i <= NOISE_AMPLITUDE/2
	noise = noise.reshape(SIZE_Y,SIZE_X)
	centerlineImage = centerlineImage[1] + noise
	centerlineImage = (centerlineImage + NOISE_AMPLITUDE/2)/(NOISE_AMPLITUDE + 1)
	# print(str(np.min(centerlineImage)) + " " + str(np.max(centerlineImage)))
	
	# create the segmented image with the centerline
	# segmentedImage = PtsListToMask(SIZE_X, SIZE_Y, centerline, DILATION_STRUCTURE)
	
	return centerline, centerlineImage #, segmentedImage
	
def CreateFakeSet(_nbSeq, _path):
	for seqId in range(_nbSeq):
	# for seqId in range(2):
		sequences = np.empty([NB_FRAMES, SIZE_Y, SIZE_X], dtype=np.float32)
		
		# nb control points is 4 or 5
		NB_CONTROL_PTS = int(np.random.rand(1)*2 + 4)
		#print("NB_CONTROL_PTS " + str(NB_CONTROL_PTS))
		
		OFFSET = 250
		x = np.random.rand(NB_CONTROL_PTS)*(SIZE_X - OFFSET*2) + OFFSET
		y = np.random.rand(NB_CONTROL_PTS)*(SIZE_Y - OFFSET*2) + OFFSET
		# print("control pts " + str(x) + " " + str(y))
		minFirstPt = min(x[0], SIZE_X - 1 - x[0], y[0], SIZE_Y - y[0])
		minLastPt = min(x[-1], SIZE_X - 1 - x[-1], y[-1], SIZE_Y - 1 - y[-1])
		if minFirstPt < minLastPt:
			x = np.flip(x, 0)
			y = np.flip(y, 0)

		for frameId in range(NB_FRAMES):
			x = x + np.random.rand(NB_CONTROL_PTS)*50
			y = y + np.random.rand(NB_CONTROL_PTS)*50
			centerline, centerlineImage = CreateFakeFluoroscopy(x, y)
			
			sequences[frameId][...] = centerlineImage[...]
			PtsListToFile(centerline, _path + "/sequence" + str(FORMAT_CONFIG).format(seqId) + "centerline" + str(FORMAT_CONFIG).format(frameId) + ".txt")
			# plb.figure()
			# _ = plb.imshow(centerlineImage, cmap='gray')
			
		sequences = sequences*65536
		sequences = sequences.astype(np.uint16)
		SaveDicomSequence(_path + "/sequence" + str(FORMAT_CONFIG).format(seqId) + ".dcm", sequences)
		print("sequence " + str(int(seqId + 1)) + "/" + str(_nbSeq) + " generated in directory " + _path)

path = "generated"
# MakeDirThreadSafe(path)
CreateFakeSet(NB_GENERATED_SEQUENCES, path)