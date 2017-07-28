import numpy as np
import os
import sys
import shutil
if sys.version_info < (3,0):
	# does not exist anymore with python 3.5, cPickle is already implemented in pickle
	import cPickle
else:
	import pickle as cPickle
try:
	import h5py
except:
	print("cannot import h5py")

try:
	from fs.zipfs import ZipFS
	gZipLoaded = True
except:
	gZipLoaded = False

from System import *

FORMAT_FLOAT = "{:0.6f}"

def GetPathWithoutExtension(_path):
    return os.path.splitext(_path)[0]
	
def GetFileNameWithoutExtension(_path):
	return os.path.splitext(os.path.basename(_path))[0]
	
def GetFileNameExtension(_path):
	return os.path.splitext(os.path.basename(_path))[1]
	
def RemoveQuotationMark(_path):
    if _path[0] == "\"" and _path[-1] == "\"":
        return _path[1:-1]
    return _path

_7_ZIP = os.getenv('_7_ZIP')
if _7_ZIP is not None:
    _7_ZIP = GetPathWithoutExtension(RemoveQuotationMark(_7_ZIP))

if gZipLoaded == True:
	class ZipFileManager(object):
		def __init__(_self):
			# print("ZipFileManager::__init__")
			_self.m_ZipFileList = {}
			_self.m_pythonVersion = sys.version_info
			return
			
		def __enter__(_self):
			# print("ZipFileManager::__enter__")
			return
			
		def __exit__(_self):
			# print("ZipFileManager::__exit__")
			# if sys.version_info < (3,0): # does not work all the time because sys may have been deleted before
			if _self.m_pythonVersion < (3,0):
				for key, value in _self.m_ZipFileList.iteritems():
					if value != None:
						value.close()
			else:
				for key, value in _self.m_ZipFileList.items():
					if value != None:
						value.close()
			_self.m_ZipFileList.clear()
			return
			
		def __del__(_self):
			# print("ZipFileManager::__del__")
			_self.__exit__()
			return
			
		def GetZipFile(_self, _zipfile):
			# print("ZipFileManager::GetZipFile")
			projectsFS = _self.m_ZipFileList.get(_zipfile)
			if projectsFS == None:
				print("ZipFileManager::GetZipFile " + str(_zipfile))
				projectsFS = ZipFS(_zipfile, mode = 'r')
				_self.m_ZipFileList[_zipfile] = projectsFS
			return projectsFS
			
		def CloseZipfFile(_self, _zipfile):
			# print("ZipFileManager::CloseZipfFile")
			projectsFS = _self.m_ZipFileList.get(_zipfile)
			if projectsFS != None:
				print("ZipFileManager::CloseZipfFile " + str(_zipfile))
				projectsFS.close()
				del _self.m_ZipFileList[_zipfile]
			return
		
	gZipFileManager = ZipFileManager()

def ZipAndDeleteFolder(_folder):
	if os.path.exists(_folder):
		global gZipFileManager
		gZipFileManager.CloseZipfFile(_folder + ".zip")
		# stdout = ">> " + "zipStdout.txt"
		stdout = "> nul"
		# stderr = "2>> " + "zipStderr.txt"
		stderr = "2> nul"
		CallExecutable(_7_ZIP, "a \"" + _folder + ".zip\" \"" + _folder + "\\*\" -y " + stdout + " " + stderr)
		# CallExecutable(_7_ZIP, "a -mx0 \"" + _folder + ".zip\" \"" + _folder + "\\*\" -y " + stdout + " " + stderr)
		shutil.rmtree(_folder)
	return

def OpenFile(_fileName, _flags):
	if gZipLoaded == True:
		# if the file is inside a zip file "//C:/blabla.zip//insideZip/lala.txt"
		if _fileName[0] == '/' and _fileName[1] == '/':
			pos = _fileName.find("//", 2)
			zipFile = _fileName[2:pos]
			filename = _fileName[pos + 2:]
			realFilename = zipFile[:-4] + '/' + filename
			
			# check first if the real file exist before looking in the zip file
			if _flags == 'w' or os.path.isfile(realFilename) == True:
				return open(realFilename, _flags)
			
			global gZipFileManager
			projectsFS = gZipFileManager.GetZipFile(zipFile)
			return projectsFS.open(filename)
		
	return open(_fileName, _flags)

def CloseFile(_f):
	_f.close()

def ReadLine(_f):
	line = ""
	while True:
		line = _f.readline().rstrip()
		if len(line) == 0 or line[0] != '#':
			break
	return line

def IsFileExist(_fileName):
	if gZipLoaded == True:
		# if the file is inside a zip file "//C:/blabla.zip//insideZip/lala.txt"
		if _fileName[0] == '/' and _fileName[1] == '/':
			pos = _fileName.find("//", 2)
			zipFile = _fileName[2:pos]
			filename = _fileName[pos + 2:]
			realFilename = zipFile[:-4] + '/' + filename
			# check first if the real file exist before looking in the zip file
			if os.path.isfile(realFilename) == True:
				return True
			
			if os.path.isfile(zipFile) == True:
				global gZipFileManager
				projectsFS = gZipFileManager.GetZipFile(zipFile)
				return projectsFS.isfile(filename)
			
			return False
	
	return os.path.isfile(_fileName)

def ListFilesInDir(_dirName):
	return os.listdir(_dirName)
	
def IsDirectoryExist(_dirName):
	return os.path.isdir(_dirName)

def MakeDirThreadSafe(_path):
	# if the file is inside a zip file "//C:/blabla.zip//insideZip/lala.txt"
	if _path[0] == '/' and _path[1] == '/':
		pos = _path.find("//", 2)
		zipFile = _path[2:pos]
		filename = _path[pos + 2:]
		_path = zipFile[:-4] + '/' + filename
		
	# these two lines are buggy if we are using multithread (race condition problem)
	# http://stackoverflow.com/questions/273192/in-python-check-if-a-directory-exists-and-create-it-if-necessary
	# if not os.path.exists(path):
		# os.makedirs(path)
	# still not the correct solution but better than the other one
	try:
		os.makedirs(_path)
	except OSError:
		if os.path.exists(_path):
			# we are nearly safe
			pass
		else:
			# there was an error on creation, so make sure we know about it
			raise
	return
	
# _set can be a numpy object or a python list of numpy object
# numpy object with float (32bits)
def SaveH5SetF32(_path, _set):
	f = h5py.File(_path, 'w')
	if type(_set).__module__ == 'numpy':
		f.create_dataset('set', np.shape(_set), dtype='f4', compression="lzf")
		f['set'][...] = _set
	else:
		for i in range(len(_set)):
			f.create_dataset('set' + str(i), np.shape(_set[i]), dtype='f4', compression="lzf")
			f['set' + str(i)][...] = _set[i]
	f.close()

# return a numpy object or a list a numpy object
def LoadH5Set(_path):
	f = h5py.File(_path, 'r')
	tmpSet = []
	for name in f:
		set = f[name][...]
		print(set.shape)
		tmpSet.append(set)
	f.close()
	if len(tmpSet) == 1:
		return tmpSet[0]
	return tmpSet

def SavePickle(_var, _path):
	with OpenFile(_path, 'wb') as f:
		cPickle.dump(_var, f, -1)

def LoadPickle(_path):
	with OpenFile(_path, 'rb') as f:
		var = cPickle.load(f)
	return var
	
# csv reader
def VectorListFromFile(_fileName, _split = ";"):
	vecList = []
	try:
		f = OpenFile(_fileName, 'r')

		while(True):
			line = f.readline()
			if len(line) == 0:
				break
			vec = []
			line = line.rstrip()
			list = line.split(_split)
			vecList.append(list)
	except IOError as err:
		errno, strerror = err.args
		
		print("VectorListFromFile() I/O error({0}): {1} ".format(errno, strerror) + str(_fileName))
	else:
		CloseFile(f)

	return vecList
	
def VectorToFileI(_f, _vec, _split = " "):
	for i in range(len(_vec)):
		_f.write(str(_vec[i]) + _split)
	_f.write("\n")
	return

def VectorToFileF(_f, _vec, _split = " "):
	for i in range(len(_vec)):
		_f.write(str(FORMAT_FLOAT).format(_vec[i]) + _split)
	_f.write("\n")
	return

def FileToMatrix(_f):
	mat = IdentityMatrix44()
	for i in range(0, 4):
		line = ReadLine(_f)
		list = line.split()
		for j in range(0, 4):
			mat[i][j] = float(list[j])
	return mat

def MatrixToFile(_f, _mat):
	for i in range(0, 4):
		for j in range(0, 4):
			_f.write(str(_mat[i][j]) + " ")
		_f.write("\n")
	return
	
def PtsListToFile(_ptsList, _fileName):
	try:
		f = OpenFile(_fileName, 'w')
		for i in range(0, len(_ptsList)):
			for j in range(0, len(_ptsList[i])):
				f.write(str(_ptsList[i][j]) + " ")
			f.write("\n")
	# except IOError as (errno, strerror):
	except IOError as err:
		errno, strerror = err.args
		print("PtsListToFile(): write I/O error({0}): {1}".format(errno, strerror))
	else:
		CloseFile(f)

def PtsListFromFile(_fileName):
	ptsList = []
	if IsFileExist(_fileName) == True:
		try:
			f = OpenFile(_fileName, 'r')
			while(True):
				line = f.readline()
				if len(line) == 0:
					break
				vec = []
				line = line.rstrip()
				list = line.split()
				for i in range(0, len(list)):
					vec.append(float(list[i]))
				ptsList.append(vec)
		# except IOError as (errno, strerror):
		except IOError as err:
			errno, strerror = err.args
			print("PtsListFromFile(): load I/O error({0}): {1}".format(errno, strerror))
		else:
			CloseFile(f)
	else:
		print("PtsListFromFile() Warning " + str(_fileName) + " doesn't exist")
	return ptsList