#include "common/util/Util.h"
#include "PathExtraction2D.h"

USING_Q_NAMESPACE
using namespace std;

int main(int _argc, char **_argv){
	//q::qPrint("# main\n");
	//q::qPrint("_argv[1] %s\n", _argv[1]);

	if(_argc > 1){
		qString typeStr = _argv[1];
		if(typeStr == "PathExtraction2D"){
			PathExtraction2D(_argc, _argv);
		}
	}
	else{
		qPrint("# not enough arguments\n");
	}

	//system("Pause");

	return EXIT_SUCCESS;
}
