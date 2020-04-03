cmake_minimum_required(VERSION 3.13)

project(TACE)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DQ_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQ_DEBUG")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BUILD_TYPE})

set(TACELIB_PATH "")
include_directories("./")

option(USE_AVX "Use AVX instructions" OFF)
if(MSVC)
	if(USE_AVX)
		add_definitions(-DUSE_AVX)
		ADD_DEFINITIONS(/arch:AVX)
	endif(USE_AVX)
	# ADD_DEFINITIONS(/Qvec-report:1)
	# ADD_DEFINITIONS(/Qvec-report:2)
	# ADD_DEFINITIONS(/GL)
	# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
endif(MSVC)

option(USE_ITK "Use ITK library" ON)
if(USE_ITK)
	find_package(ITK REQUIRED)
	include(${ITK_USE_FILE})
	add_definitions(-DUSE_ITK)
endif(USE_ITK)

set(TACE_SPECIFIC_SOURCES)
if(USE_ITK)
	add_definitions(-DNEED_FILE)
	list(APPEND TACE_SPECIFIC_SOURCES
	${TACELIB_PATH}common/util/File.cpp
	${TACELIB_PATH}common/util/File.h)
endif(USE_ITK)

add_library(
	# TACELib STATIC
	TACELib SHARED
	
	${TACELIB_PATH}common/maths/Maths.h
	${TACELIB_PATH}common/maths/Vector.h
	
	${TACELIB_PATH}common/segmentation/ExtractCenterline.cpp
	${TACELIB_PATH}common/segmentation/ExtractCenterline.h
	
	${TACELIB_PATH}common/structure/Branch.cpp
	${TACELIB_PATH}common/structure/Branch.h
	${TACELIB_PATH}common/structure/PtList.cpp
	${TACELIB_PATH}common/structure/PtList.h
	${TACELIB_PATH}common/structure/Tree.cpp
	${TACELIB_PATH}common/structure/Tree.h
	
	${TACELIB_PATH}common/util/Util.cpp
	${TACELIB_PATH}common/util/Util.h
	
	${TACE_SPECIFIC_SOURCES}
)

# set_target_properties(TACELib PROPERTIES COMPILE_FLAGS "-DQ_STATIC=1")
set_target_properties(TACELib PROPERTIES COMPILE_FLAGS "-DQ_SHARED=1 -DQ_DLL_EXPORTS=1")

target_link_libraries(
	TACELib
	${EXTRA_LIBS}
)

if(USE_ITK)
add_executable(
	TACE
	src/TACE/main.cpp
	src/TACE/PathExtraction2D.cpp
	src/TACE/PathExtraction2D.h
)

target_link_libraries(
	TACE
	TACELib
	${ITK_LIBRARIES}
)
endif(USE_ITK)

option(MAKE_PYTHON_WRAPPER "Use python wrapper" ON)
if(MAKE_PYTHON_WRAPPER)
	find_package (Python COMPONENTS Development)
	# message(${Python_LIBRARIES})
	# message(${Python_INCLUDE_DIRS})
	
	include_directories(${Python_INCLUDE_DIRS})

	set(Boost_USE_STATIC_LIBS ON)
	# set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_MULTITHREADED ON)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS python37 numpy37 REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
	link_directories(${Boost_LIBRARY_DIRS})
	# message(${Boost_INCLUDE_DIRS})
	# message(${Boost_LIBRARY_DIRS})

	if(PYTHON_DEBUG_OUTPUT)
		add_definitions(-DPYTHON_DEBUG_OUTPUT)
	endif(PYTHON_DEBUG_OUTPUT)

	add_library(
		PyTACELib SHARED
		src/PyTACELib/PythonWrapper.cpp
	)
	
	target_link_libraries(
		PyTACELib
		${Boost_LIBRARIES}
		${Python_LIBRARIES}
		TACELib
	)
	# message(${Boost_LIBRARIES})
	# message(${Python_LIBRARIES})
	
	if(ADDITIONAL_LIBRARIES_DEBUG)
		# message(${ADDITIONAL_LIBRARIES_DEBUG}) # print with no delimiter
		# message("${ADDITIONAL_LIBRARIES_DEBUG}") # print whith ";" delimiter
		# list(LENGTH ADDITIONAL_LIBRARIES_DEBUG ADDITIONAL_LIBRARIES_DEBUG_LEN)
		# message(${ADDITIONAL_LIBRARIES_DEBUG_LEN})
		target_link_libraries(PyTACELib debug ${ADDITIONAL_LIBRARIES_DEBUG} ${ADDITIONAL_LIBRARIES})
	endif()
	if(ADDITIONAL_LIBRARIES_RELEASE)
		target_link_libraries(PyTACELib optimized ${ADDITIONAL_LIBRARIES_RELEASE} ${ADDITIONAL_LIBRARIES})
	endif()
	
	# don't prepend wrapper library name with lib
	if (WIN32)
		set_target_properties(PyTACELib PROPERTIES SUFFIX ".pyd" IMPORT_PREFIX "" PREFIX "")
	else()
		set_target_properties(PyTACELib PROPERTIES SUFFIX ".so" IMPORT_PREFIX "" PREFIX "")
	endif()
endif(MAKE_PYTHON_WRAPPER)