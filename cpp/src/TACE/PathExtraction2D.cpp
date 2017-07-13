#include "common/segmentation/ExtractCenterline.h"

#ifdef WIN32
	#pragma warning(push)
	#pragma warning (disable : 4996)
#endif
#include "itkImage.h"
#include "itkImageFileReader.h"
#ifdef WIN32
	#pragma warning(pop)
#endif

USING_Q_NAMESPACE

void PathExtraction2D(int _argc, char **_argv){
	qPrint("# ExtractCenterline\n");

	qString inImageFileName = qString(_argv[2]);
	qString outCenterlineFileName = qString(_argv[3]);

	typedef itk::Image<qu8, 2> ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(inImageFileName);
	reader->Update();

	ImageType::Pointer itkImg = reader->GetOutput();
	ImageType::RegionType region = itkImg->GetLargestPossibleRegion();
	ImageType::SizeType size = region.GetSize();
	//qPrint("# size %dx%d\n", size.m_Size[0], size.m_Size[1]);
	qu8* imageBuffer = itkImg->GetBufferPointer();
	//qPrint("# imageBuffer %p\n", imageBuffer);

	//q::Image<qu8> inImage = q::Image<qu8>(size.m_Size[0], size.m_Size[1], 1, 1, 1, 1, reinterpret_cast<qu8*>(imageBuffer));

	qsize_t imageSizeX = size.m_Size[0];
	qsize_t imageSizeY = size.m_Size[1];
	PtList centerline;

#ifdef PYTHON_DEBUG_OUTPUT
	DebugOutput debugOutput;
	ExtractCenterline(imageBuffer, imageSizeX, imageSizeY, centerline, debugOutput);

	qPrint("centerlinesList = []\n");
	qPrint("connectionsList = []\n");
	for(std::vector<DebugStep>::iterator it = debugOutput._debugSteps.begin(); it != debugOutput._debugSteps.end(); ++it){
		qPrint("centerlinesList.append([])\n");
		for(std::vector<q::PtList>::iterator it2 = it->centerlinesList.begin(); it2 != it->centerlinesList.end(); ++it2){
			qPrint("centerlinesList[-1].append([");
			PtList &centerline = *it2;
			for(qsize_t i = 0; i < centerline.size(); ++i){
				if(i != 0){
					qPrint(",");
				}
				qPrint("[%f, %f]", centerline[i].pos[PT_X], centerline[i].pos[PT_Y]);
			}
			qPrint("])\n");
		}

		qPrint("connectionsList.append([");
		PtList &centerline = it->connectionsList;
		for(qsize_t i = 0; i < centerline.size(); ++i){
			if(i != 0){
				qPrint(",");
			}
			qPrint("[%f, %f]", centerline[i].pos[PT_X], centerline[i].pos[PT_Y]);
		}
		qPrint("])\n");
	}
#else
	ExtractCenterline(imageBuffer, imageSizeX, imageSizeY, centerline);
#endif

	if(outCenterlineFileName != std::string("0")){
		centerline.SaveFile(outCenterlineFileName, PtList::FLAG_POS_X | PtList::FLAG_POS_Y);
	}
	else
	{
		qPrint("centerline = [");
		for(qsize_t i = 0; i < centerline.size(); ++i){
			if(i != 0){
				qPrint(",");
			}
			qPrint("[%f, %f]", centerline[i].pos[PT_X], centerline[i].pos[PT_Y]);
		}
		qPrint("]\n");
	}
}
