@set _UTIL_DIR=C:/Programs

@set _MINGW_BIN_DIR=%_UTIL_DIR%/mingw64/bin

@set _MSVC_DIR=%_UTIL_DIR%/Microsoft Visual Studio 14.0
@set _MSVC_BIN_DIR=%_MSVC_DIR%/VC/bin
@set _MSVC_BIN_X64_DIR=%_MSVC_BIN%/amd64
@set _MSVC_BIN_X86_AMD64_DIR=%_MSVC_BIN_DIR%/x86_amd64

@set _PYTHON_VERSION=3.7
@set _PYTHON_DIR=%_UTIL_DIR%/Python/
@set _PYTHON_INCLUDE_DIR=%_PYTHON_DIR%/include
@set _PYTHON_LIB=%_PYTHON_DIR%/libs/python37.lib
@rem @set _PYTHON_LIB=%_PYTHON_DIR%/libs/libpython37.a
@set _PYTHON="%_PYTHON_DIR%/python.exe"

@set _CUDA_DIR=%_UTIL_DIR%/CUDA
@set OPENBLAS_HOME=%_UTIL_DIR%/OpenBLAS

@set _CMAKE="%_UTIL_DIR%/CMake/bin/cmake.exe"

@rem name of the compiler using cmake syntax
@set _COMPILER_NAME="Visual Studio 14 2015 Win64"
@rem @set _COMPILER_NAME="MinGW Makefiles"

@set _CPU=x64

@set _LIBS_DIR=C:/Libs

@set _BOOST_SRC_DIR=%_LIBS_DIR%/boost
@set _BOOST_LIB_DIR=%_LIBS_DIR%/boost_build/lib

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

@set LIBRARY_PATH= 
@set LIBRARY_PATH=%LIBRARY_PATH%;%_CUDA_DIR%/lib/x64

@set CPLUS_INCLUDE_PATH= 
@set CPLUS_INCLUDE_PATH=%CPLUS_INCLUDE_PATH%;%_CUDA_DIR%/include

@rem for PyTACELib
@set _TACE_DLL_DIR=%_PROJECT_PATH%/cpp/generated/%_USER_SPECIFIC%/release
rem @set _TACE_DLL_DIR=%_PROJECT_PATH%/cpp/generated/%_USER_SPECIFIC%/release/Release
rem @set PATH=%PATH%;%_TACE_DLL_DIR%

@set PYTHONPATH=%PYTHONPATH%;%_PROJECT_PATH%/python/common;%_TACE_DLL_DIR%