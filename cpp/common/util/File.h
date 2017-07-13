#ifndef __FILE_HEADER_
#define __FILE_HEADER_

#include <stdio.h>
#include <vector>

#ifdef __GNUC__
	#include <stdarg.h> // va_start...
	#include <cstdlib> // atof()
	#include <cstring> // strcmp()
	#include <sys/stat.h>
#endif

#include "common/util/Util.h"
#include "common/maths/Vector.h"
#ifdef NEED_MATRIX
	#include "common/maths/Matrix.h"
#endif

#define Q_EOF (EOF)

BEGIN_Q_NAMESPACE

#define SAFE_CLOSE_FILE(_f) \
	{ \
		if((_f) != NULL) \
		{ \
			fclose(_f); \
			_f = NULL; \
		} \
	}

class qFile{
public:
	virtual ~qFile(){}
	
	virtual qsize_t Ftell() = 0;
	virtual qsize_t Fread(void *_buffer, qsize_t _size, qsize_t _count) = 0;
	virtual int Getc(void) = 0;
	virtual qs32 Vfprintf(const qchar8 *_str, va_list _varg) = 0;

	inline qs32 Fprintf(const qchar8 *_str, ...){
		va_list varg;
		va_start(varg, _str);
		qs32 ret = Vfprintf(_str, varg);
		va_end(varg);
		return ret;
	}

	virtual qsize_t Fwrite(const void *_ptr, qsize_t _size, qsize_t _count) = 0;

	virtual int Fseek(long int _offset, int _origin) = 0;

	qu32 m_Err;
};

class qStdFile : public qFile{
public:
	qStdFile(const char *_filename, const char *_mode){
		m_Err = 1;
#ifdef __GNUC__
		m_File = fopen(_filename, _mode);
		if(m_File == NULL){
			m_Err = 0;
		}
#elif defined(_MSC_VER)
		errno_t errNo = fopen_s(&m_File, _filename, _mode);
		if(errNo != 0){
			qPrint("errNo %d\n", errNo);
			m_Err = 0;
		}
#else
	#error "not supported yet"
#endif
	}

	~qStdFile(){
		SAFE_CLOSE_FILE(m_File);
	}
	
	inline qsize_t Ftell(){
		return ftell(m_File);
	}

	inline qsize_t Fread(void *_buffer, qsize_t _size, qsize_t _count){
		return fread(_buffer, _size, _count, m_File);
	}

	inline int Getc(void){
		return fgetc(m_File);
	}
	
	inline qs32 Vfprintf(const qchar8 *_str, va_list _varg){
		return vfprintf(m_File, _str, _varg);
	}

	inline qsize_t Fwrite(const void *_ptr, qsize_t _size, qsize_t _count){
		return fwrite(_ptr, _size, _count, m_File);
	}

	// TODO use a 64 bits version with the _offset?
	inline int Fseek(long int _offset, int _origin){
		return fseek(m_File, _offset, _origin);
	}

private:
	FILE *m_File;
};

class qRamFile : public qFile{
public:
	~qRamFile(){
		SAFE_DELETE_UNIQUE(m_MemFile);
	}
	
	inline qsize_t Ftell(){
		return m_Mem - m_MemFile;
	}

	inline qsize_t Fread(void *_buffer, qsize_t _size, qsize_t _count){
		qsize_t sizeLeft = m_Size - (m_Mem - m_MemFile);
		qsize_t realCount = _count;
		if(sizeLeft < _size*_count){
			realCount = sizeLeft/_size;
			m_Eof = Q_TRUE;
		}
		
		if(realCount != 0){
			memcpy(_buffer, m_Mem, realCount*_count);
			m_Mem = m_Mem + realCount*_count;
		}

		return realCount;
	}

	inline int Getc(void){
		/*qs32 chr = 0;
		qsize_t uret = Fread(&chr, 1, 1);
		if(uret == 1){
			return chr;
		}*/
		if(m_Mem < m_MemFile + m_Size){
			qu8 chr = *m_Mem;
			m_Mem++;
			return chr;
		}
		m_Eof = Q_TRUE;
		return Q_EOF;
	}
	
	inline qs32 Vfprintf(const qchar8 * /*_str*/, va_list /*_varg*/){
		/*qchar8 buf[MAX_STR_BUFFER];
		qs32 ret = vsnprintf(buf, MAX_STR_BUFFER, _str, _varg);
		if(ret >= 0){
			qsize_t uret = Write(buf, ret, 1);
			if(uret == 1){
				return ret;
			}
		}*/
		return -1;
	}

	inline qsize_t Fwrite(const void * /*_ptr*/, qsize_t /*_size*/, qsize_t /*_count*/){	
		return 0;
	}

	inline int Fseek(long int _offset, int _origin){
		qs32 newPos = 0;
		if(_origin == SEEK_SET){
			newPos = _offset;
		}
		else if(_origin == SEEK_CUR){
			newPos = static_cast<qs32>(m_Mem - m_MemFile) + _offset;
		}
		else{
			// it's not size - 1 because we put on the EOF
			newPos = static_cast<qs32>(m_Size) + newPos;
		}

		if(newPos >= 0 && newPos <= (qs32)m_Size){
			m_Mem = m_MemFile + newPos;
			return 0;
		}

		return -1;
	}

protected:
	qRamFile() : m_MemFile(NULL)
				, m_Mem(NULL)
				, m_Size(0)
				, m_Eof(Q_TRUE){
	}

	qu8 *m_MemFile;
	qu8 *m_Mem;
	qu64 m_Size;
	qbool m_Eof;
};

#ifdef USE_ZLIB
class qZipFile : public qRamFile{
public:
	qZipFile(const char *_zipfile, const char *_filename, const char *_mode);

	~qZipFile(){
	}
};
#endif

inline qbool qFileExists(const char *_file)
{
#ifdef __GNUC__
	struct stat buffer;   
	return (stat(_file, &buffer) == 0); 
#elif defined(_MSC_VER)
	WIN32_FIND_DATA FindFileData;
#ifdef UNICODE
	qWstring unicodePath = MultiByteToWideCharString(qString(_file));
	HANDLE handle = FindFirstFile(unicodePath.c_str(), &FindFileData);
#else
	HANDLE handle = FindFirstFile(_file, &FindFileData);
#endif

	qbool found = Q_TRUE;
	if(handle == INVALID_HANDLE_VALUE){
		found = Q_FALSE;
	}
	if(found)
	{
		FindClose(handle);
	}
   return found;
#else
	#error "not supported yet"
#endif
}

Q_DLL qu32 qFOpen(qFile *&_file, const char *_filename, const char *_mode);

inline qs32 qFClose(qFile *&_file){
	SAFE_DELETE_UNIQUE(_file);
	return 0;
}

inline qu32 FileReadLineInternal(qFile *_file, qString &_buffer){
	_buffer.clear();
	_buffer.reserve(MAX_STR_BUFFER);

	int chr = _file->Getc();
	if(chr == EOF){
		return 0;
	}
	while(chr != EOF && chr != '\n'){
		if(chr != '\r'){
			_buffer += static_cast<qchar8>(chr);
		}
		chr = _file->Getc();
	}
	return 1;
}

inline qu32 FileReadLine(qFile *_file, qString &_buffer){
	qu32 ret = 0;
	do{
		ret = FileReadLineInternal(_file, _buffer);
	}while(ret == 1 && _buffer[0] == '#');
	return ret;
}

inline qu32 FileReachLine(qFile *_file, qString _line){
	qString buffer;
	qu32 ret = 0;
	do{
		ret = FileReadLineInternal(_file, buffer);
	}while(ret == 1 && buffer != _line);
	return ret;
}

inline qu32 FileSkipLine(qFile *_file){
	int chr = _file->Getc();
	while(chr != EOF && chr != '\n'){
		chr = _file->Getc();
	}
	return 1;
}

inline qu32 FileReadString(qFile *_file, qString &_str){
	qu32 ret = FileReadLine(_file, _str);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}
	return 1;
}

inline qu32 FileSaveString(qFile *_file, const char *_str, ...){
	va_list args;
	va_start(args, _str);

	int iErr = _file->Vfprintf(_str, args);
	if(iErr < 0){
		va_end(args);
		throw gDefaultException;
	}
	va_end(args);
	return 1;
}

#ifdef NEED_MATRIX
inline qu32 FileReadMatrix44(qFile *_file, Matrix44 &_mat){
	qString buffer;
	for(qu32 i = 0; i < 4; ++i){
		qu32 ret = FileReadLine(_file, buffer);
		if(ret != 1){
			throw gDefaultException;
			//return 0;
		}

		std::vector<qString> vecList;
		SplitString(buffer, " ", vecList);
		for(qu32 j = 0; j < 4; ++j){
			_mat[i][j] = atof(vecList[j].c_str());
		}
	}
	return 1;
}

inline qu32 FileSaveMatrix44(qFile *_file, const Matrix44 &_mat){
	for(qu32 i = 0; i < 4; ++i){
		for(qu32 j = 0; j < 4; ++j){
			int iErr = _file->Fprintf("%f ", _mat[i][j]);
			if(iErr < 0){
				throw gDefaultException;
			}
		}
		int iErr = _file->Fprintf("\n");
		if(iErr < 0){
			throw gDefaultException;
		}
	}
	return 1;
}

inline Matrix44 LoadMatrix44(const qString &_fileName){
	Matrix44 mat;

	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "rb");
		if(err == 0){
			throw gDefaultException;
		}

		/*qu32 iErr =*/ FileReachLine(file, qString("#\tMatrix44"));
		/*qu32 iErr =*/ FileReadMatrix44(file, mat);
	}
	catch(q::qDefaultException){
		q::qPrintStdErr("LoadMatrix44 Error during loading %s\n", _fileName);
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);

	return mat;
}

inline void SaveMatrix44(const Matrix44 &_mat, const qString &_fileName){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "wb");
		if(err == 0){
			throw gDefaultException;
		}

		/*qu32 iErr =*/ FileSaveString(file, "#\tMatrix44\n");
		/*iErr =*/ FileSaveMatrix44(file, _mat);
	}
	catch(q::qDefaultException){
		q::qPrintStdErr("SaveMatrix44 Error during saving %s\n", _fileName);
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
}
#endif

inline qu32 FileReadVector2Set(qFile *_file, std::vector<Vector2> &_vec){
	qString buffer;
	while(FileReadLine(_file, buffer) == 1){
		std::vector<qString> vecList;
		SplitString(buffer, " ", vecList);
		Vector2 v;
		for(qu32 i = 0; i < vecList.size(); ++i){
			v[i] = atof(vecList[i].c_str());
		}
		_vec.push_back(v);
	}
	return 1;
}

inline qu32 FileReadVector2Set(qString _fileName, std::vector<Vector2> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "rb");
		if(err == 0){
			throw gDefaultException;
		}

		FileReadVector2Set(file, _vec);
	}
	catch(q::qDefaultException){
		q::qPrint("FileReadVector2Set Error during loading %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileSaveVector2Set(qFile *_file, std::vector<Vector2> &_vec){
	for(std::vector<Vector2>::iterator it = _vec.begin(); it != _vec.end(); ++it){
		int iErr = _file->Fprintf("%f %f\n", (*it)[0], (*it)[1]);
		if(iErr < 0){
			throw gDefaultException;
		}
	}
	return 1;
}

inline qu32 FileSaveVector2Set(qString _fileName, std::vector<Vector2> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "wb");
		if(err == 0){
			throw gDefaultException;
		}

		FileSaveVector2Set(file, _vec);
	}
	catch (q::qDefaultException){
		q::qPrint("FileSaveVector2Set Error during saving %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileReadVector3(qFile *_file, Vector3 &_vec){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	std::vector<qString> vecList;
	SplitString(buffer, " ", vecList);
	for(qu32 i = 0; i < vecList.size(); ++i){
		_vec[i] = atof(vecList[i].c_str());
	}
	return 1;
}

inline qu32 FileSaveVector3(qFile *_file, const Vector3 &_vec){
	int iErr = _file->Fprintf("%f %f %f\n", _vec[0], _vec[1], _vec[2]);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadVector3Set(qFile *_file, std::vector<Vector3> &_vec){
	qString buffer;
	while(FileReadLine(_file, buffer) == 1){
		std::vector<qString> vecList;
		SplitString(buffer, " ", vecList);
		Vector3 v;
		for(qu32 i = 0; i < vecList.size(); ++i){
			v[i] = atof(vecList[i].c_str());
		}
		_vec.push_back(v);
	}
	return 1;
}

inline qu32 FileReadVector3Set(qString _fileName, std::vector<Vector3> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "rb");
		if(err == 0){
			throw gDefaultException;
		}

		FileReadVector3Set(file, _vec);
	}
	catch(q::qDefaultException){
		q::qPrint("FileReadVector3Set Error during loading %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileReadVector6Set(qFile *_file, std::vector<Vector6> &_vec){
	qString buffer;
	while(FileReadLine(_file, buffer) == 1){
		std::vector<qString> vecList;
		SplitString(buffer, " ", vecList);
		Vector6 v;
		for(qu32 i = 0; i < vecList.size(); ++i){
			v[i] = atof(vecList[i].c_str());
		}
		_vec.push_back(v);
	}
	return 1;
}

inline qu32 FileReadVector6Set(qString _fileName, std::vector<Vector6> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "rb");
		if(err == 0){
			throw gDefaultException;
		}

		FileReadVector6Set(file, _vec);
	}
	catch(q::qDefaultException){
		q::qPrint("FileReadVector6Set Error during loading %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileSaveVector6Set(qFile *_file, std::vector<Vector6> &_vec){
	for(std::vector<Vector6>::iterator it = _vec.begin(); it != _vec.end(); ++it){
		int iErr = _file->Fprintf("%f %f %f %f %f %f\n", (*it)[0], (*it)[1], (*it)[2], (*it)[3], (*it)[4], (*it)[5]);
		if(iErr < 0){
			throw gDefaultException;
		}
	}
	return 1;
}

inline qu32 FileSaveVector6Set(qString _fileName, std::vector<Vector6> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "wb");
		if(err == 0){
			throw gDefaultException;
		}

		FileSaveVector6Set(file, _vec);
	}
	catch (q::qDefaultException){
		q::qPrint("FileSaveVector6Set Error during saving %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileReadF32(qFile *_file, qf32 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = static_cast<qf32>(atof(buffer.c_str()));

	return 1;
}

inline qu32 FileSaveF32(qFile *_file, qf32 _val){
	int iErr = _file->Fprintf("%f\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadF64(qFile *_file, qf64 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atof(buffer.c_str());

	return 1;
}

inline qu32 FileReadU64(qFile *_file, qu64 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atoi(buffer.c_str());
	
	return 1;
}

inline qu32 FileSaveU64(qFile *_file, qu64 _val){
	int iErr = _file->Fprintf("%d\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileSaveF64(qFile *_file, qf64 _val){
	int iErr = _file->Fprintf("%f\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadF64List(qFile *_file, std::vector<qf64> &_vec){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	std::vector<qString> vecList;
	SplitString(buffer, " ", vecList);
	for(std::vector<qString>::iterator it = vecList.begin(); it != vecList.end(); ++it){
		_vec.push_back(atof((*it).c_str()));
	}

	return 1;
}

inline qu32 FileSaveF64List(qFile *_file, std::vector<qf64> &_vec){
	char buffer[MAX_STR_BUFFER];
	qString str;
	for(std::vector<qf64>::iterator it = _vec.begin(); it != _vec.end(); ++it){
		//qSprintf(buffer, MAX_STR_BUFFER, "%f ", *it);
		//qSprintf(buffer, MAX_STR_BUFFER, "%.12f ", *it);
		//qSprintf(buffer, MAX_STR_BUFFER, "%e ", *it);
		qSprintf(buffer, MAX_STR_BUFFER, "%.12e ", *it);
		str += buffer;
	}
	int iErr = _file->Fprintf("%s\n", str.c_str());
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadU32(qFile *_file, qu32 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atoi(buffer.c_str());
	
	return 1;
}

inline qu32 FileSaveU32(qFile *_file, qu32 _val){
	int iErr = _file->Fprintf("%d\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadU32List(qFile *_file, std::vector<qu32> &_vec){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	std::vector<qString> vecList;
	SplitString(buffer, " ", vecList);
	for(std::vector<qString>::iterator it = vecList.begin(); it != vecList.end(); ++it){
		_vec.push_back(atoi((*it).c_str()));
	}

	return 1;
}

inline qu32 FileSaveU32List(qFile *_file, std::vector<qu32> &_vec){
	char buffer[MAX_STR_BUFFER];
	qString str;
	for(std::vector<qu32>::iterator it = _vec.begin(); it != _vec.end(); ++it){
		qSprintf(buffer, MAX_STR_BUFFER, "%d ", *it);
		str += buffer;
	}
	int iErr = _file->Fprintf("%s\n", str.c_str());
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu16 FileReadU16(qFile *_file, qu16 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atoi(buffer.c_str());

	return 1;
}

inline qu16 FileSaveU16(qFile *_file, qu16 _val){
	int iErr = _file->Fprintf("%d\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu16 FileReadS16(qFile *_file, qs16 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atoi(buffer.c_str());

	return 1;
}

inline qu16 FileSaveS16(qFile *_file, qs16 _val){
	int iErr = _file->Fprintf("%d\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadStringList(qFile *_file, std::vector<qString> &_vec){
	qString buffer;
	while(FileReadLine(_file, buffer) == 1){
		_vec.push_back(buffer);
	}
	return 1;
}

inline qu32 FileReadStringList(qString _fileName, std::vector<qString> &_vec){
	qFile *file = NULL;
	try{
		qu32 err = qFOpen(file, _fileName.c_str(), "rb");
		if(err == 0){
			throw gDefaultException;
		}

		FileReadStringList(file, _vec);
	}
	catch(q::qDefaultException){
		q::qPrint("FileReadStringList Error during loading %s\n", _fileName.c_str());
		qFClose(file);
		throw gDefaultException;
	}

	qFClose(file);
	return 1;
}

inline qu32 FileReadS32(qFile *_file, qs32 &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	_val = atoi(buffer.c_str());
	
	return 1;
}

inline qu32 FileSaveS32(qFile *_file, qs32 _val){
	int iErr = _file->Fprintf("%d\n", _val);
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

inline qu32 FileReadBool(qFile *_file, qbool &_val){
	qString buffer;
	qu32 ret = FileReadLine(_file, buffer);
	if(ret != 1){
		throw gDefaultException;
		//return 0;
	}

	if(buffer[0] == 'T'){
		_val = Q_TRUE;
	}
	else{
		_val = Q_FALSE;
	}
	
	return 1;
}

inline qu32 FileSaveBool(qFile *_file, qbool _val){
	int iErr = 0;
	if(_val == Q_TRUE){
		iErr = _file->Fprintf("T\n");
	}
	else{
		iErr = _file->Fprintf("F\n");
	}
	if(iErr < 0){
		throw gDefaultException;
	}
	return 1;
}

END_Q_NAMESPACE

#endif
