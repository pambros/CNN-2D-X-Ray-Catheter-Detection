import os
import sys
import numpy as np

# _TACELIB_PATH = os.getenv('_TACELIB_PATH')
# sys.path.append(_TACELIB_PATH)
import PyTACELib

# PYSEGLIB_DEBUG_OUTPUT = True
PYTACELIB_DEBUG_OUTPUT = False

# test if the module is loaded properly
assert(PyTACELib.Test() == "Test successful!")
print(PyTACELib.Test())

# draw two lines in an empty image
image = np.zeros((512, 512), dtype=np.uint8)
for i in range(10,60):
	image[128, i] = 255
	
for i in range(132,150):
	image[i, 60] = 255

# check if the C++ function can be called and if it can extract the centerline
print("execute PyTACELib.ExtractCenterline")

if PYTACELIB_DEBUG_OUTPUT == True:
	centerline, debugStepList = PyTACELib.ExtractCenterline(image)
	assert(len(debugStepList) == 3)
	print(len(debugStepList))
	print(debugStepList)
else:
	centerline = PyTACELib.ExtractCenterline(image)

assert(len(centerline) == 68)
print(len(centerline))
print(centerline)

print("The library has been successfully compiled and imported!")