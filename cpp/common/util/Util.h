#ifndef __Q_UTIL_HEADER_
#define __Q_UTIL_HEADER_
#ifdef _MSC_VER
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(__GNUC__)
	#include <stdio.h> // fwrite() ...
	#include <cstdlib> // atof() atoi() ...
	#include <stdlib.h>
	#include <cstdio>
	#include <cstring> // memcpy()
#endif
#include <assert.h>
#include <string>
#include <exception>
#include <iostream>
#include <map>
#include <vector>

//#define WORKING_PATH "C:\\Disque\\"
#define WORKING_PATH ""

#ifdef Q_STATIC
	#define Q_DLL
#else
	#ifdef _MSC_VER
		#ifdef Q_DLL_EXPORTS
			#define Q_DLL __declspec(dllexport)
		#else
			#define Q_DLL __declspec(dllimport)
		#endif
	#else
		#ifdef Q_DLL_EXPORTS
			#define Q_DLL
		#else
			#define Q_DLL
		#endif
	#endif
#endif

#ifndef DEBUG
	#if defined(_DEBUG) || defined(Q_DEBUG)
		#define DEBUG
	#endif
#endif

#ifdef DEBUG
	#define USE_PRINT 1
	#define USE_ASSERT 1
#else
	#define USE_PRINT 1
#endif

#ifdef USE_ASSERT
	#define qAssert(...) assert(__VA_ARGS__)
#else
	#define qAssert(...) 
#endif

#ifdef USE_AVX
	#ifdef __AVX__
		#define Q_SSE_AVX
	#endif
#endif

#define Q_TRUE (true)
#define Q_FALSE (false)

#ifdef Q_SSE_AVX
	#ifdef _MSC_VER
		// Suppresses warning C4316: object allocated on the heap may not be aligned 32
		#pragma warning (disable : 4316)
	#endif

	#define Q_PAD_ALIGN_32 __declspec(align(32))

void* operator new(size_t _size);
void* operator new[](size_t _size);
void operator delete(void *_p);
void operator delete[](void *_p);
#else
	#define Q_PAD_ALIGN_32
#endif

#define Q_NEW new
#define Q_DELETE delete[]
#define Q_DELETE_UNIQUE delete

#define SAFE_DELETE(_a) \
	{ \
		if((_a) != NULL) \
		{ \
			Q_DELETE (_a); \
			_a = NULL; \
		} \
	}

#define SAFE_DELETE_UNIQUE(_a) \
	{ \
		if((_a) != NULL) \
		{ \
			Q_DELETE_UNIQUE (_a); \
			_a = NULL; \
		} \
	}

	
#define qMemcpy(_dst, _src, _size) memcpy(_dst, _src, _size)
#define qMemset(_dst, _val, _size) memset(_dst, _val, _size)
	
#ifdef _MSC_VER
	#define qSprintf(_buffer, _sizeBuffer, _format, ...) sprintf_s(_buffer, _sizeBuffer, _format, __VA_ARGS__)
	#define qVsnprintf(_buffer, _sizeBuffer, _format, ...) vsnprintf_s(_buffer, _sizeBuffer, _sizeBuffer - 1, _format, __VA_ARGS__)
	#define qSnprintf(_buffer, _sizeBuffer, _format, ...) _snprintf_s(_buffer, _sizeBuffer, _format, __VA_ARGS__)
	#define qStrcpy(_dest, _sizeBuffer, _source) strcpy_s(_dest, _sizeBuffer, _source)
	#define qStrncpy(_dest, _sizeDest, _source, _num) strncpy_s(_dest, _sizeDest, _source, _num)
	
	#define FMT_32 "I32"
	#define FMT_64 "I64"
#elif defined(__GNUC__)
	#define qSprintf(_buffer, _sizeBuffer, _format, ...) sprintf(_buffer, _format, __VA_ARGS__)
	#define qVsnprintf(_buffer, _sizeBuffer, _format, ...) vsnprintf(_buffer, _sizeBuffer, _format, __VA_ARGS__)
	#define qSnprintf(_buffer, _sizeBuffer, _format, ...) snprintf(_buffer, _sizeBuffer, _format, __VA_ARGS__)
	#define qStrcpy(_dest, _sizeBuffer, _source) strcpy(_dest, _source)
	#define qStrncpy(_dest, _sizeDest, _source, _num) strncpy(_dest, _source, _num)
	
	#define FMT_32 ""
	#define FMT_64 "ll"
#else
	#error "not supported yet"
#endif

#define BEGIN_Q_NAMESPACE namespace q {
#define END_Q_NAMESPACE }
#define USING_Q_NAMESPACE using namespace q;

BEGIN_Q_NAMESPACE
	typedef unsigned char qu8;
	typedef signed char qs8;
	typedef char qchar8;
	typedef short qchar16;
	typedef unsigned short qu16;
	typedef signed short qs16;
	typedef unsigned int qu32;
	typedef signed int qs32;
	typedef unsigned long long qu64;
	typedef signed long long qs64;
	typedef float qf32;
	typedef double qf64;
	typedef bool qbool;
	typedef size_t qsize_t;

	typedef std::string qString;
	typedef std::wstring qWstring;

	enum
	{
		DWORD_LOW = 0,
		DWORD_HIGH = 1,
		DWORD_COUNT
	};

	union Union64bits {
		qu32 asU32[DWORD_COUNT];
		qu64 asU64;
		qs64 asS64;
		qf64 asF64;
	};

	enum
	{
		UTIL_NO_ERROR = 0,
		UTIL_ERROR = -1,
		UTIL_IDLE = -2,

		MAX_STR_BUFFER = 1024,
		//MAX_LONG_STR_BUFFER = 30720 // 30*1024
	};

#ifdef USE_PRINT
	//#define qPrint(...) __qPrint(__VA_ARGS__)
	Q_DLL void qPrint(const qchar8 *_format, ...);
	Q_DLL void qPrintStdErr(const qchar8 * _format, ...);
#else
	//#define qPrint(...)
	//#define qPrintStdErr(...)
	inline void qPrint(const qchar8 * /*_format*/, ...){}
	inline void qPrintStdErr(const qchar8 * /*_format*/, ...){}
#endif

#define qPrintStdErrAndStop(_str) {qPrintStdErr("ERROR %s %d " _str "\n", __FILE__, __LINE__);exit(1);};

	// exception management
	class qDefaultException: public std::exception
	{
		virtual const qchar8* what() const throw()
		{
			qPrint("An exception happened.");
			//std::cout << "An exception happened." << std::endl;
			return "An exception happened.";
		}
	};
	Q_DLL extern qDefaultException gDefaultException;

	Q_DLL void SplitString(q::qString _str, q::qString _delimiter, std::vector<qString> &_vec);

	template <class _TYPE>
	void Swap(_TYPE &_var1, _TYPE &_var2){
		_TYPE tmp = _var1;
		_var1 = _var2;
		_var2 = tmp;
	}

#ifdef _MSC_VER
	qWstring MultiByteToWideCharString(qString _str);
	qString WideCharToMultiByteString(qWstring _wstr);
#elif defined(__GNUC__)
	// not used now
#else
	#error "not supported yet"
#endif
END_Q_NAMESPACE

#endif
