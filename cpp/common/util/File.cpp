#include "File.h"
#ifdef USE_ZLIB
//extern "C" {
	#include "thirdParties/minizip/unzip.h"
//}
#endif

BEGIN_Q_NAMESPACE

#ifdef USE_ZLIB

//#define _USE_ZIP_FILE_MANAGER
#ifdef _USE_ZIP_FILE_MANAGER
class ZipFileManager{
public:
	inline unzFile getZipFile(const qString &_fileName){
		std::map<qString, unzFile>::iterator it = m_ZipFileList.find(_fileName);
		if(it != m_ZipFileList.end()){
			return it->second;
		}
		
		unzFile file = unzOpen(_fileName.c_str());
		m_ZipFileList.insert(std::pair<qString, unzFile>(_fileName, file));
		return file;
	}

	~ZipFileManager(){
		for(std::map<qString, unzFile>::iterator it = m_ZipFileList.begin(); it != m_ZipFileList.end(); ++it){
			// close zip file
			if(it->second != NULL){
		#ifdef USE_ASSERT
					qs32 ret =
		#endif
				unzClose(it->second);
				qAssert(ret == UNZ_OK);
			}
		}
	}
private:
	std::map<qString, unzFile> m_ZipFileList;
};

ZipFileManager gZipFileManager;
#endif

qZipFile::qZipFile(const char *_zipfile, const char *_filename, const char * /*_mode*/) : qRamFile() {
	m_Err = 0;
#ifdef _USE_ZIP_FILE_MANAGER
	unzFile file = gZipFileManager.getZipFile(_zipfile);
#else
	unzFile file = unzOpen(_zipfile);
#endif
	if(file == NULL){
		return;
	}

	try{
		qs32 ret = unzLocateFile(file, _filename, 1);
		if(ret != UNZ_OK){
			throw gDefaultException;
		}

		unz_file_info info;
		ret = unzGetCurrentFileInfo(file, &info, NULL, 0, NULL, 0, NULL, 0);
		if(ret != UNZ_OK){
			throw gDefaultException;
		}

		ret = unzOpenCurrentFile(file);
		if(ret != UNZ_OK){
			throw gDefaultException;
		}

		try{
			m_Size = info.uncompressed_size;
			m_MemFile = Q_NEW qu8[info.uncompressed_size];
			if(m_MemFile == NULL){
				throw gDefaultException;
			}

			m_Mem = m_MemFile;
			qu32 totalRead = 0;
			while(totalRead < info.uncompressed_size){
				ret = unzReadCurrentFile(file, m_Mem, info.uncompressed_size - totalRead);
				if(ret <= 0){
					throw gDefaultException;
				}
				totalRead += ret;
				m_Mem = m_Mem + ret;
			}
			m_Mem = m_MemFile;
			m_Err = 1;
		}
		catch(q::qDefaultException){
			ret = unzCloseCurrentFile(file);
			throw gDefaultException;
		}

		ret = unzCloseCurrentFile(file);
		if(ret == UNZ_CRCERROR){
			throw gDefaultException;
		}
	}
	catch(q::qDefaultException){
		q::qPrint("qZipFile::qZipFile Error during loading zip %s %s\n", _zipfile, _filename);
	}

#ifndef _USE_ZIP_FILE_MANAGER
	// close zip file
	if(file != NULL){
#ifdef USE_ASSERT
			qs32 ret =
#endif
		unzClose(file);
		qAssert(ret == UNZ_OK);
	}
#endif
}

#endif

qu32 qFOpen(qFile *&_file, const char *_filename, const char *_mode){
#ifdef USE_ZLIB
	qsize_t len = strlen(_filename);
	// if the file is inside a zip file "//C:/blabla.zip//insideZip/lala.txt"
	if(len >= 3 && _filename[0] == '/' && _filename[1] == '/'){
		qsize_t i = 2;
		for(; i < len; ++i){
			if(_filename[i] == '/' && _filename[i + 1] == '/'){
				break;
			}
		}
		qString zipFile = qString(_filename).substr(2, i - 2);
		qString fileName = qString(_filename + i + 2);
		// check first if the real file exist before looking in the zip file
		qString realFileName = zipFile.substr(0, zipFile.size() - 4) + qString("/") + fileName;
		if(_mode[0] == 'w' || qFileExists(realFileName.c_str()) == Q_TRUE){
			_file = Q_NEW qStdFile(realFileName.c_str(), _mode);
		}
		else{
			_file = Q_NEW qZipFile(zipFile.c_str(), fileName.c_str(), _mode);
		}
	}
	else
#endif
	{
		_file = Q_NEW qStdFile(_filename, _mode);
	}

	if(_file != NULL){
		return _file->m_Err;
	}
	return 0;
}

END_Q_NAMESPACE
