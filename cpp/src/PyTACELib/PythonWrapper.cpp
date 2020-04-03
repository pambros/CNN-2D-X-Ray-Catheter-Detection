#include "common/segmentation/ExtractCenterline.h"

#ifdef _MSC_VER
	#pragma warning(push)
	// suppresses warning C4244: conversion from 'Py_ssize_t' to 'unsigned int', possible loss of data
	#pragma warning (disable : 4244)
#endif

// TODO because of "bug"? in boost 1.63 -> should be removed with more recent boost version
// #define BOOST_LIB_NAME "boost_numpy"
// #include <boost/config/auto_link.hpp>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

USING_Q_NAMESPACE

// warning _data is modified inside the function. _data should be unsigned and 1 byte size (q::qu8)
#ifdef PYTHON_DEBUG_OUTPUT
boost::python::tuple ExtractCenterlinePython(boost::python::numpy::ndarray _data){
#else
boost::python::list ExtractCenterlinePython(boost::python::numpy::ndarray _data){
#endif
	qAssert(_data.get_dtype().get_itemsize() == sizeof(qu8));
	if(_data.get_dtype().get_itemsize() != sizeof(qu8)){
		qPrint("ExtractCenterline dtype of the array should be numpy.uint8\n");
	}

	qAssert(_data.get_nd() == 2);
	if(_data.get_nd() != 2){
		qPrint("ExtractCenterline dimension of the array should be 2\n");
	}

	qAssert(_data.strides(1) == 1 && _data.strides(0) == _data.shape(1));
	if(_data.strides(1) != 1 || _data.strides(0) != _data.shape(1)){
		qPrint("ExtractCenterline TODO be careful here the numpy array needs to have contiguous data. If not, we need to implement code to take care of _data.strides()\n");
	}

	qu8* imageBuffer = reinterpret_cast<qu8*>(_data.get_data());
	qsize_t imageSizeX = _data.shape(1);
	qsize_t imageSizeY = _data.shape(0);
	PtList centerline;
#ifdef PYTHON_DEBUG_OUTPUT
	DebugOutput debugOutput;
	ExtractCenterline(imageBuffer, imageSizeX, imageSizeY, centerline, debugOutput);
#else
	ExtractCenterline(imageBuffer, imageSizeX, imageSizeY, centerline);
#endif

	//boost::python::object tu = boost::python::make_tuple('a', 'b', 'c');
	//boost::python::numpy::ndarray example_tuple = boost::python::numpy::array(tu);

	boost::python::list pyList;
	for(qs32 ptId = 0; ptId < static_cast<qs32>(centerline.size()); ++ptId){
		pyList.append(boost::python::make_tuple(centerline[ptId].x(), centerline[ptId].y()));
	}

#ifdef PYTHON_DEBUG_OUTPUT
	boost::python::list debugStepList;
	for(std::vector<DebugStep>::iterator it = debugOutput._debugSteps.begin(); it != debugOutput._debugSteps.end(); ++it){
		DebugStep &debugStep = *it;
		boost::python::list centerlinesList;
		for(std::vector<PtList>::iterator it2 = debugStep.centerlinesList.begin(); it2 != debugStep.centerlinesList.end(); ++it2){
			PtList &ptl = *it2;
			boost::python::list tmpList;
			for(qsize_t i = 0; i < ptl.size(); ++i){
				tmpList.append(boost::python::make_tuple(ptl[i].x(), ptl[i].y()));
			}
			centerlinesList.append(tmpList);
		}
		boost::python::list connectionsList;
		for(qsize_t i = 0; i < debugStep.connectionsList.size(); ++i){
			connectionsList.append(boost::python::make_tuple(debugStep.connectionsList[i].x(), debugStep.connectionsList[i].y()));
		}
		debugStepList.append(boost::python::make_tuple(centerlinesList, connectionsList));
	}

	return boost::python::make_tuple(pyList, debugStepList);
#else
	return pyList;
#endif
}

const qchar8* Test(void){
	return "Test successful!";
}

BOOST_PYTHON_MODULE(PyTACELib){
	Py_Initialize();
	boost::python::numpy::initialize();

	boost::python::def("ExtractCenterline", ExtractCenterlinePython);

	boost::python::def("Test", Test);
}
