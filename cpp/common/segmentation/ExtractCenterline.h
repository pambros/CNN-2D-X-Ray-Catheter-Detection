#ifndef __EXTRACT_CENTERLINE_2D_HEADER_
#define __EXTRACT_CENTERLINE_2D_HEADER_
#include "common/util/Util.h"
#include "common/structure/PtList.h"

//#define PYTHON_DEBUG_OUTPUT

// warning _imageBuffer is modified inside the function
#ifdef PYTHON_DEBUG_OUTPUT
class DebugStep{
public:
	std::vector<q::PtList> centerlinesList;
	q::PtList connectionsList;
};

class DebugOutput{
public:
	std::vector<DebugStep> _debugSteps;
};

Q_DLL void ExtractCenterline(q::qu8 *_imageBuffer, q::qsize_t _imageSizeX, q::qsize_t _imageSizeY, q::PtList &_outCenterline, DebugOutput &_debugOutput);
#else
Q_DLL void ExtractCenterline(q::qu8 *_imageBuffer, q::qsize_t _imageSizeX, q::qsize_t _imageSizeY, q::PtList &_outCenterline);
#endif

#endif
