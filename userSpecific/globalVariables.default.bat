@set _UTIL_DIR=C:/Programs

@set _MINGW_BIN_DIR=%_UTIL_DIR%/mingw64/bin

@set _MSVC_DIR=%_UTIL_DIR%/Microsoft Visual Studio 14.0
@set _MSVC_BIN_DIR=%_MSVC_DIR%/VC/bin
@set _MSVC_BIN_X64_DIR=%_MSVC_BIN%/amd64
@set _MSVC_BIN_X86_AMD64_DIR=%_MSVC_BIN_DIR%/x86_amd64

@set _PYTHON_VERSION=3.6
@set _PYTHON_DIR=%_UTIL_DIR%/Python/
@set _PYTHON_INCLUDE_DIR=%_PYTHON_DIR%/include
@set _PYTHON_LIB=%_PYTHON_DIR%/libs/python36.lib
@set _PYTHON="%_PYTHON_DIR%/python.exe"

@set _CUDA_DIR=%_UTIL_DIR%/CUDA
@set OPENBLAS_HOME=%_UTIL_DIR%/OpenBLAS

@set _CMAKE="%_UTIL_DIR%/CMake/bin/cmake.exe"

@rem name of the compiler using cmake syntax
@set _COMPILER_NAME="Visual Studio 14 2015 Win64"

@set _CPU=x64

@set _LIBS_DIR=C:/Libs

@set _BOOST_SRC_DIR=%_LIBS_DIR%/boost
@set _BOOST_LIB_DIR=%_LIBS_DIR%/boost_build/lib

@set _LIB_GPU_ARRAY_DIR=%_LIBS_DIR%/libGpuArray
@set _LIB_GPU_ARRAY_BIN_DIR=%_LIB_GPU_ARRAY_DIR%/bin
@set _LIB_GPU_ARRAY_LIB_DIR=%_LIB_GPU_ARRAY_DIR%/lib
@set _LIB_GPU_ARRAY_INCLUDE_DIR=%_LIB_GPU_ARRAY_DIR%/include

@set PATH= 
@set PATH=%PATH%;C:/Windows/system32
@set PATH=%PATH%;%_MINGW_BIN_DIR%
@set PATH=%PATH%;%_CUDA_DIR%/bin
@set PATH=%PATH%;%_CUDA_DIR%/libnwp
@set PATH=%PATH%;%_MSVC_BIN_X64_DIR%
@set PATH=%PATH%;%_MSVC_BIN_X86_AMD64_DIR%
@set PATH=%PATH%;%_MSVC_BIN_DIR%
@set PATH=%PATH%;%OPENBLAS_HOME%/bin
@set VS150COMNTOOLS= 
@set VS140COMNTOOLS=%_MSVC_DIR%/Common7/Tools/
@set VS120COMNTOOLS= 
@set VS110COMNTOOLS= 
@set VS100COMNTOOLS= 

@rem ************************************** for Theano
@set PATH=%PATH%;%_LIB_GPU_ARRAY_BIN_DIR%

@set LIBRARY_PATH= 
@set LIBRARY_PATH=%LIBRARY_PATH%;%_CUDA_DIR%/lib/x64
@set LIBRARY_PATH=%LIBRARY_PATH%;%_LIB_GPU_ARRAY_LIB_DIR%

@set CPLUS_INCLUDE_PATH= 
@set CPLUS_INCLUDE_PATH=%CPLUS_INCLUDE_PATH%;%_CUDA_DIR%/include
@set CPLUS_INCLUDE_PATH=%CPLUS_INCLUDE_PATH%;%_LIB_GPU_ARRAY_INCLUDE_DIR%

@set MKL_NUM_THREADS=8
@set OMP_NUM_THREADS=8
@set GOTO_NUM_THREADS=8
@rem @set THEANO_FLAGS=mode=FAST_RUN,floatX=float32,device=cpu,openmp=True
@rem @set THEANO_FLAGS=mode=FAST_RUN,floatX=float32,device=cpu,openmp=True,blas.ldflags=-L%OPENBLAS_HOME%/bin -lopenblas

@set THEANO_FLAGS=mode=FAST_RUN,floatX=float32,device=cuda,gpuarray.preallocate=0.80,openmp=True,blas.ldflags=-L%OPENBLAS_HOME%/bin -lopenblas,dnn.conv.algo_bwd_filter=deterministic,dnn.conv.algo_bwd_data=deterministic
@rem @set THEANO_FLAGS=mode=FAST_RUN,floatX=float32,device=cuda,gpuarray.preallocate=0.80,openmp=True,blas.ldflags=-L%OPENBLAS_HOME%/bin -lopenblas

@rem @set KERAS_BACKEND=tensorflow
@set KERAS_BACKEND=theano

@rem ************************************** for TACE and PyTACELib
@set _TACE_BIN_DIR=%_PROJECT_PATH%/cpp/generated/%_USER_SPECIFIC%/Release

@set PYTHONPATH=%PYTHONPATH%;%_PROJECT_PATH%/python/common;%_TACE_BIN_DIR%