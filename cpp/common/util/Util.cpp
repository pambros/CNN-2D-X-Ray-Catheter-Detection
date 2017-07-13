#include "common/util/Util.h"
#ifdef USE_MULTITHREADING
	#include "common/util/Thread.h"
#endif

#include <stdio.h>
#include <stdarg.h>

//#define Q_UNIT_TEST
#ifdef Q_UNIT_TEST
	#undef qAssert
	#define qAssert(_test, ...) if((_test) == Q_FALSE){qPrint("ASSERT %s %d " #_test "\n", __FILE__, __LINE__);exit(1);};

	#include "common/maths/Vector.h"
	#include "common/maths/Matrix.h"
	#include "common/util/UtilTime.h"
#endif

#define MAX_BUFFER 256

#ifdef USE_PRINT
	#define FILE_PATH WORKING_PATH"log.txt"
	//#define FILE_PATH "log.txt"
	//#define LOG_IN_FILE 1
#endif

#ifdef Q_SSE_AVX
void* operator new(size_t _size)
{
#ifdef _MSC_VER
	return _aligned_malloc(_size, 32);
#elif defined(__GNUC__)
	// return memalign(_size, 32);
	return aligned_alloc(_size, 32);
#endif
}

void* operator new[](size_t _size)
{
#ifdef _MSC_VER
	return _aligned_malloc(_size, 32);
#elif defined(__GNUC__)
	// return memalign(_size, 32);
	return aligned_alloc(_size, 32);
#endif
}

void operator delete(void *_p)
{
#ifdef _MSC_VER
	_aligned_free(_p);
#elif defined(__GNUC__)
	free(_p);
#endif
}

void operator delete[](void *_p)
{
#ifdef _MSC_VER
	_aligned_free(_p);
#elif defined(__GNUC__)
	free(_p);
#endif
}
#endif

BEGIN_Q_NAMESPACE
	qDefaultException gDefaultException;

#if defined(DEBUG) || defined(Q_UNIT_TEST)
	class UnitTestsUtil
	{
	public:
		inline UnitTestsUtil(void)
		{
			qAssert(sizeof(qu8) == 1);
			qAssert(sizeof(qs8) == 1);
			qAssert(sizeof(qchar8) == 1);
			qAssert(sizeof(qchar16) == 2);
			qAssert(sizeof(qu16) == 2);
			qAssert(sizeof(qs16) == 2);
			qAssert(sizeof(qu32) == 4);
			qAssert(sizeof(qs32) == 4);
			qAssert(sizeof(qu64) == 8);
			qAssert(sizeof(qs64) == 8);
			qAssert(sizeof(qf32) == 4);
			qAssert(sizeof(qf64) == 8);

			// check SolveLinearSystem
			/*Matrix44 mat;
			mat[0][0] = 0.0; mat[0][1] = 2.0; mat[0][2] = 0.0; mat[0][3] = 1.0;
			mat[1][0] = 2.0; mat[1][1] = 2.0; mat[1][2] = 3.0; mat[1][3] = 2.0;
			mat[2][0] = 4.0; mat[2][1] = -3.0; mat[2][2] = 0.0; mat[2][3] = 1.0;
			mat[3][0] = 6.0; mat[3][1] = 1.0; mat[3][2] = -6.0; mat[3][3] = -5.0;
			Vector4 b;
			b[0] = 0.0; b[1] = -2.0; b[2] = -7.0; b[3] = 6.0;
			Vector4 c;
			SolveLinearSystem(mat, b, c);
			qAssert(APPROX_EQUAL(c[0], -0.5, UTIL_EPSILON));
			qAssert(APPROX_EQUAL(c[1], 1.0, UTIL_EPSILON));
			qAssert(APPROX_EQUAL(c[2], 0.3333333333333, UTIL_EPSILON));
			qAssert(APPROX_EQUAL(c[3], -2.0, UTIL_EPSILON));

			// round
			qAssert(ROUND(0.) == 0.);
			qAssert(ROUND(0.1) == 0.);
			qAssert(ROUND(0.5) == 1.);
			qAssert(ROUND(0.6) == 1.);
			qAssert(ROUND(-0.1) == 0.);
			qAssert(ROUND(-0.5) == 0.);
			qAssert(ROUND(-0.6) == -1.);*/

#ifdef Q_UNIT_TEST
			{
				q::Vector4 a(1., 2., 3., 4.);
				q::Vector4 b(2., 4., 6., 8.);

				q::Vector4 c = a + b;
				qPrint("c[0] == %f && c[1] == %f && c[2] == %f && c[3] == %f\n", c[0], c[1], c[2], c[3]);
				qAssert(c[0] == 3. && c[1] == 6. && c[2] == 9. && c[3] == 12.);

				c = a - b;
				qPrint("c[0] == %f && c[1] == %f && c[2] == %f && c[3] == %f\n", c[0], c[1], c[2], c[3]);
				qAssert(c[0] == -1. && c[1] == -2. && c[2] == -3. && c[3] == -4.);

				c = a*b;
				qPrint("c[0] == %f && c[1] == %f && c[2] == %f && c[3] == %f\n", c[0], c[1], c[2], c[3]);
				qAssert(c[0] == 2. && c[1] == 8. && c[2] == 18. && c[3] == 32.);

				c = a/b;
				qPrint("c[0] == %f && c[1] == %f && c[2] == %f && c[3] == %f\n", c[0], c[1], c[2], c[3]);
				qAssert(c[0] == 0.5 && c[1] == 0.5 && c[2] == 0.5 && c[3] == 0.5);

				q::Vector4 d(5.);
				qPrint("d[0] = %f d[1] = %f d[2] = %f d[3] = %f\n", d[0], d[1], d[2], d[3]);

				c = a*4;
				qPrint("c[0] == %f && c[1] == %f && c[2] == %f && c[3] == %f\n", c[0], c[1], c[2], c[3]);
				qAssert(c[0] == 4 && c[1] == 8 && c[2] == 12 && c[3] == 16);

				for(qu32 j = 0; j < 2; ++j){
					START_CHRONO(vectorChrono);
					for(qu32 i = 0; i < 100000000; ++i){
						c = a + b;
						a = c + b;
					}
					END_CHRONO(vectorChrono);
					PRINT_CHRONO(vectorChrono);
				}
			}

			// thread test with AVX instructions
	#ifdef Q_USE_THREAD
		#define VECTOR_NB_USED_PROCS NB_PROCS
			{
				class VectorThread : public qThread{
				public:
					inline VectorThread(void){
					}

					inline ~VectorThread(void){
					}

				private:
					inline void run(void){
						q::Vector4 a(1., 2., 3., 4.);
						q::Vector4 b(2., 4., 6., 8.);
						q::Vector4 c = a + b;
						for(qu32 i = 0; i < 100000000; ++i){
							c = a + b;
							a = c + b;
						}
						m_Var = a[0];
					}

					qf64 m_Var;
				};
				
				START_CHRONO(vectorChrono2);
				VectorThread **thread = Q_NEW VectorThread*[VECTOR_NB_USED_PROCS];
				for(qu32 i = 0; i < VECTOR_NB_USED_PROCS; ++i){
					thread[i] = Q_NEW VectorThread();
					thread[i]->start();
				}
			
				for(qu32 i = 0; i < VECTOR_NB_USED_PROCS; ++i){
					thread[i]->wait();
				}

				for(qu32 i = 0; i < VECTOR_NB_USED_PROCS; ++i){
					SAFE_DELETE_UNIQUE(thread[i]);
				}
				SAFE_DELETE(thread);
			
				END_CHRONO(vectorChrono2);
				PRINT_CHRONO(vectorChrono2);
			}
	#endif

			{
				Matrix44 mat1 = Matrix44();
				mat1[0][0] =  1.;mat1[0][1] =  2.;mat1[0][2] =  3.;mat1[0][3] =  4.;
				mat1[1][0] =  5.;mat1[1][1] =  6.;mat1[1][2] =  7.;mat1[1][3] =  8.;
				mat1[2][0] =  9.;mat1[2][1] = 10.;mat1[2][2] = 11.;mat1[2][3] = 12.;
				mat1[3][0] = 13.;mat1[3][1] = 14.;mat1[3][2] = 15.;mat1[3][3] = 16.;
				Vector4 vec = Vector4(1., 2., 3., 4.);
				vec = mat1*vec;
				PrintVector4(vec);
				qAssert(vec[0] == 30 && vec[1] == 70 && vec[2] == 110 && vec[3] == 150);
			
				for(qu32 j = 0; j < 2; ++j){
					START_CHRONO(matrixChrono);
					for(qu32 i = 0; i < 100000000; ++i){
						vec = mat1*vec;
					}
					END_CHRONO(matrixChrono);
					PRINT_CHRONO(matrixChrono);
				}

				Matrix44 mat2 = Matrix44();
				mat2[0][0] =  1.5;mat2[0][1] =  2.5;mat2[0][2] =  3.5;mat2[0][3] =  4.5;
				mat2[1][0] =  5.5;mat2[1][1] =  6.5;mat2[1][2] =  7.5;mat2[1][3] =  8.5;
				mat2[2][0] =  9.5;mat2[2][1] = 10.5;mat2[2][2] = 11.5;mat2[2][3] = 12.5;
				mat2[3][0] = 13.5;mat2[3][1] = 14.5;mat2[3][2] = 15.5;mat2[3][3] = 16.5;
				mat2 = mat1*mat2;
				PrintMatrix(mat2);
				qAssert(mat2[0][0] == 95 && mat2[0][1] == 105 && mat2[0][2] == 115 && mat2[0][3] == 125);
				qAssert(mat2[1][0] == 215 && mat2[1][1] == 241 && mat2[1][2] == 267 && mat2[1][3] == 293);
				qAssert(mat2[2][0] == 335 && mat2[2][1] == 377 && mat2[2][2] == 419 && mat2[2][3] == 461);
				qAssert(mat2[3][0] == 455 && mat2[3][1] == 513 && mat2[3][2] == 571 && mat2[3][3] == 629);

				for(qu32 j = 0; j < 2; ++j){
					START_CHRONO(matrixChrono);
					for(qu32 i = 0; i < 100000000; ++i){
						mat2 = mat1*mat2;
					}
					END_CHRONO(matrixChrono);
					PRINT_CHRONO(matrixChrono);
				}
			}

			// thread test with AVX instructions
	#ifdef Q_USE_THREAD
		#define MATRIX_NB_USED_PROCS NB_PROCS
			{
				class MatrixThread : public qThread{
				public:
					inline MatrixThread(void){
					}

					inline ~MatrixThread(void){
					}

					inline void run(void){
						Matrix44 mat1 = Matrix44();
						mat1[0][0] =  1.;mat1[0][1] =  2.;mat1[0][2] =  3.;mat1[0][3] =  4.;
						mat1[1][0] =  5.;mat1[1][1] =  6.;mat1[1][2] =  7.;mat1[1][3] =  8.;
						mat1[2][0] =  9.;mat1[2][1] = 10.;mat1[2][2] = 11.;mat1[2][3] = 12.;
						mat1[3][0] = 13.;mat1[3][1] = 14.;mat1[3][2] = 15.;mat1[3][3] = 16.;
						Matrix44 mat2 = Matrix44();
						mat2[0][0] =  1.5;mat2[0][1] =  2.5;mat2[0][2] =  3.5;mat2[0][3] =  4.5;
						mat2[1][0] =  5.5;mat2[1][1] =  6.5;mat2[1][2] =  7.5;mat2[1][3] =  8.5;
						mat2[2][0] =  9.5;mat2[2][1] = 10.5;mat2[2][2] = 11.5;mat2[2][3] = 12.5;
						mat2[3][0] = 13.5;mat2[3][1] = 14.5;mat2[3][2] = 15.5;mat2[3][3] = 16.5;
						for(qu32 i = 0; i < 100000000; ++i){
							mat2 = mat1*mat2;
						}
						m_Var = mat2[0][0];
					}

					qf64 m_Var;
				};
				
				START_CHRONO(matrixChrono);
				MatrixThread **thread = Q_NEW MatrixThread*[MATRIX_NB_USED_PROCS];
				for(qu32 i = 0; i < MATRIX_NB_USED_PROCS; ++i){
					thread[i] = Q_NEW MatrixThread();
					thread[i]->start();
				}
			
				qf64 sum = 0.;
				for(qu32 i = 0; i < MATRIX_NB_USED_PROCS; ++i){
					thread[i]->wait();
					sum = sum + thread[i]->m_Var;
				}

				for(qu32 i = 0; i < MATRIX_NB_USED_PROCS; ++i){
					SAFE_DELETE_UNIQUE(thread[i]);
				}
				SAFE_DELETE(thread);
			
				END_CHRONO(matrixChrono);
				PRINT_CHRONO(matrixChrono);

				qPrint("sum %f\n", sum);
			}
	#endif
#endif
		}
	};
	static UnitTestsUtil gUnitTestsUtil;
#endif

#ifdef LOG_IN_FILE
	class File
	{
	public:
		inline File(void)
		{
			qu32 err = qFOpen(m_File, FILE_PATH , "wb"); // "ab");
			qAssert(err != 0);
		}

		inline ~File(void)
		{
			FClose(m_File);
		}

		qFile *m_File;
	};
	static File gFilePrint;
#endif

#ifdef USE_PRINT
	void qPrint(const qchar8 * _format, ...)
	{
		va_list args;
		va_start(args, _format);

#ifdef Q_USE_THREAD
		BEGIN_MUTEX(mutexPrint)
#endif
#ifdef LOG_IN_FILE
			vfprintf(gFilePrint.m_File, _format, args);
			fflush(gFilePrint.m_File);
#endif
		
			// use cout
			qchar8 chrs[MAX_BUFFER];
			qVsnprintf(chrs, MAX_BUFFER, _format, args);
			qString str(chrs);
			std::cout << str << std::flush;

			// use the good old printf on stdout (or stderr)
			//vprintf(_format, args);
#ifdef Q_USE_THREAD
		END_MUTEX(mutexPrint)
#endif

		va_end(args);
	}

	void qPrintStdErr(const qchar8 * _format, ...)
	{	
		va_list args;
		va_start(args, _format);

#ifdef Q_USE_THREAD
		BEGIN_MUTEX(mutexPrint)
#endif	
		// use cout
		qchar8 chrs[MAX_BUFFER];
		qVsnprintf(chrs, MAX_BUFFER, _format, args);
		qString str(chrs);
		std::cerr << str << std::flush;

		// use the good old printf on stdout (or stderr)
		//vfprintf(stderr, _format, args);
#ifdef Q_USE_THREAD
		END_MUTEX(mutexPrint)
#endif

		va_end(args);
	}
#endif

	void SplitString(qString _str, qString _delimiter, std::vector<qString> &_vec){
		qsize_t size;
		while((size = _str.find(_delimiter)) != qString::npos){
			_vec.push_back(_str.substr(0, size));
			_str = _str.substr(size + 1);
		};
		if(_str.empty() == Q_FALSE){
		//if(strcmp(_str.c_str(), "") != 0){
			_vec.push_back(_str);
		}
	}

#ifdef _MSC_VER
	qWstring MultiByteToWideCharString(qString _str)
	{
		int wchars_num = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, NULL, 0);
		wchar_t* wchar = Q_NEW wchar_t[wchars_num];
		MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, wchar, wchars_num);
		qWstring wstr(wchar);
		Q_DELETE wchar;
		return wstr;
	}

	qString WideCharToMultiByteString(qWstring _wstr)
	{
		// test CP_ACP or CP_UTF8
		UINT codePage = CP_ACP;
		int chars_num = WideCharToMultiByte(codePage, 0, _wstr.c_str(), -1, NULL, 0, NULL, NULL);
		char* chrs = Q_NEW char[chars_num];
		const char DEFAULT_CHR = '@';
		BOOL flag = FALSE;
		int ret = WideCharToMultiByte(codePage, 0, _wstr.c_str(), -1, chrs, chars_num, &DEFAULT_CHR, &flag);
		if(ret != chars_num)
		{
			qPrint("Error during WideCharToMultiByte().\n");
			DWORD err = GetLastError();
			switch(err)
			{
				case ERROR_INSUFFICIENT_BUFFER:
					qPrint("A supplied buffer size was not large enough, or it was incorrectly set to NULL.\n");
				break;

				case ERROR_INVALID_FLAGS:
					qPrint("The values supplied for flags were not valid.\n");
				break;

				case ERROR_INVALID_PARAMETER:
					qPrint("Any of the parameter values was invalid.\n");
				break;

				case ERROR_NO_UNICODE_TRANSLATION:
					qPrint("Invalid Unicode was found in a string.\n");
				break;

				default:
					qPrint("Error unknown.\n");
			}
		}
		qAssert(ret == chars_num);
		qString str(chrs);
		Q_DELETE chrs;
		return str;
	}
#elif defined(__GNUC__)
	// not used now
#else
	#error "not supported yet"
#endif
END_Q_NAMESPACE
