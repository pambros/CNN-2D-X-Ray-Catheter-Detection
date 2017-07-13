#include "ExtractCenterline.h"

#include "common/structure/PtList.h"
#include "common/structure/Tree.h"

#define STOP_AT_BRANCHES
#define REMOVE_BLOB

#ifndef VOXEL2D_TO_INDEX
	#define VOXEL2D_TO_INDEX(_x, _y, _sizeX) ((_y)*(_sizeX) + (_x))
#endif

USING_Q_NAMESPACE

#ifdef PYTHON_DEBUG_OUTPUT
void PrintCenterlines(Tree &_tree, DebugOutput &_debugOutput){
	_debugOutput._debugSteps.push_back(DebugStep());
	DebugStep &debugStep = _debugOutput._debugSteps.back();
	for(Tree::BranchList::iterator it = _tree.m_BranchesList.begin(); it != _tree.m_BranchesList.end(); ++it){
		debugStep.centerlinesList.push_back((*it)->m_Ptl);
	}

	for(Tree::BranchList::iterator it = _tree.m_BranchesList.begin(); it != _tree.m_BranchesList.end(); ++it){
		Branch::Neighbors &neighbors = (*it)->m_Neighbors;
		for(Branch::Neighbors::iterator it2 = neighbors.begin(); it2 != neighbors.end(); ++it2){
			debugStep.connectionsList.push_back((*it)->m_Ptl[it2->first]);
		}
	}
}
#else
	#define PrintCenterlines(...)
#endif

#if defined(PYTHON_DEBUG_OUTPUT) && defined(DEBUG_TREE)
	#define CHECK_UP(...) tree.CheckUp();
#else
	#define CHECK_UP(...)
#endif

void GetBranchPart(PtList &_ptl, qu8 _id, qu8 *_currentPixel, const qu8 * const _startInPixel, const qu8 * const _endInPixel, qu32 _x, qu32 _y, const qu32 _NB_GROW, qs32 *_growOffset, qs32 *_growX, qs32 *_growY){
	_ptl.push_back(Pt(Vector2(_x, _y)));
	*_currentPixel = static_cast<qu8>(_id);
	//qPrint("[%f,%f] ", ptl.back().pos[0], ptl.back().pos[1]);

	qbool findOne;
	do{
		findOne = Q_FALSE;
		qsize_t selectedGrow = 0;
		for(qsize_t grow = 0; grow < _NB_GROW; ++grow){
			qu8 *tmpPixel = _currentPixel + _growOffset[grow];
			if(tmpPixel >= _startInPixel && tmpPixel < _endInPixel && *tmpPixel == 255){
				if(findOne == Q_FALSE){
					findOne = Q_TRUE;
					selectedGrow = grow;
				}
				else{
					findOne = Q_FALSE;
					break;
				}
			}
#ifdef STOP_AT_BRANCHES
			else if(tmpPixel >= _startInPixel && tmpPixel < _endInPixel && *tmpPixel != 0 && *tmpPixel != static_cast<qu8>(_id)){
				findOne = Q_FALSE;
				break;
			}
#endif
		}

		if(findOne == Q_TRUE){
			qf64 tmpX = _ptl.back().pos[PT_X];
			qf64 tmpY = _ptl.back().pos[PT_Y];
			_ptl.push_back(Pt(Vector2(tmpX + _growX[selectedGrow], tmpY + _growY[selectedGrow])));
			//qPrint("[%f,%f] ", ptl.back().pos[0], ptl.back().pos[1]);
			_currentPixel = _currentPixel + _growOffset[selectedGrow];
			*_currentPixel = static_cast<qu8>(_id);
		}
	} while(findOne == Q_TRUE);
	//qPrint("\n");
}

#ifdef PYTHON_DEBUG_OUTPUT
void ExtractCenterline(qu8 *_imageBuffer, qsize_t _imageSizeX, qsize_t _imageSizeY, PtList &_outCenterline, DebugOutput &_debugOutput){
#else
void ExtractCenterline(qu8 *_imageBuffer, qsize_t _imageSizeX, qsize_t _imageSizeY, PtList &_outCenterline){
#endif
	qu8* imageBuffer = _imageBuffer;
	qsize_t imageSizeX = _imageSizeX;
	qsize_t imageSizeY = _imageSizeY;

	qsize_t imagePixelNumber = imageSizeX*imageSizeY;

	// get branches
	//////////////////////////////////////////////////////////////////////////////////////////
	// TODO this method does not stop at the border, it continues at the opposite side. Should correct that
	// precompute grow offset
	const qu32 NB_GROW = 8;
	const qs32 GROW_MIN = -1;
	const qs32 GROW_MAX = 1;
	qs32 growOffset[NB_GROW];
	qs32 growX[NB_GROW];
	qs32 growY[NB_GROW];
	qu32 id = 0;
	for(qs32 x = GROW_MIN; x <= GROW_MAX; ++x){
		for(qs32 y = GROW_MIN; y <= GROW_MAX; ++y){
			if(x != 0 || y != 0){
				growOffset[id] = static_cast<qs32>(VOXEL2D_TO_INDEX(x, y, imageSizeX));
				growX[id] = x;
				growY[id] = y;
				++id;
			}
		}
	}

#ifdef REMOVE_BLOB
	{
		qu8 *inPixel = imageBuffer;
		const qu8* const startInPixel = inPixel;
		const qu8* const endInPixel = startInPixel + imagePixelNumber;
		for(; inPixel < endInPixel; ++inPixel){
			qu32 nbFirstNeighbors = 0;
			for(qsize_t grow = 0; grow < NB_GROW; ++grow){
				qu8 *tmpPixel = inPixel + growOffset[grow];
				if(tmpPixel >= startInPixel && tmpPixel < endInPixel && *tmpPixel == 255){
					nbFirstNeighbors++;
				}
			}
			if(nbFirstNeighbors >= 3){
				*inPixel = 0;
			}
		}
	}
#endif

	std::vector<PtList> branchesSet;
	qu8 *inPixel = imageBuffer;
	const qu8* const startInPixel = inPixel;
	const qu8* const endInPixel = startInPixel + imagePixelNumber;
	for(qu32 y = 0; y < imageSizeY; ++y){
		for(qu32 x = 0; x < imageSizeX; ++x){
			if(*inPixel == 255){
				branchesSet.push_back(PtList());
				if(branchesSet.size() >= 255){
					qPrintStdErrAndStop("error there are more than 254 branches. Unexpected behaviour\n");
				}
				*inPixel = static_cast<qu8>(branchesSet.size());

				// get the left path and the right path if it exists
				qu32 nbFirstNeighbors = 0;
				const qu32 MAX_NB_FIRST_NEIGHBORS = 2;
				PtList ptlNeighbors[MAX_NB_FIRST_NEIGHBORS];
				for(qsize_t grow = 0; grow < NB_GROW; ++grow){
					qu8 *tmpPixel = inPixel + growOffset[grow];
					if(tmpPixel >= startInPixel && tmpPixel < endInPixel && *tmpPixel == 255){
						if(nbFirstNeighbors >= MAX_NB_FIRST_NEIGHBORS){
							qPrintStdErrAndStop("error nbFirstNeighbors >= 2 should not happen\n");
						}
						GetBranchPart(ptlNeighbors[nbFirstNeighbors], static_cast<qu8>(branchesSet.size()), tmpPixel, startInPixel, endInPixel, x + growX[grow], y + growY[grow], NB_GROW, growOffset, growX, growY);
						nbFirstNeighbors++;
					}
				}

				PtList &ptl = branchesSet.back();
				if(nbFirstNeighbors == 2){
					for(qs32 ptId = static_cast<qs32>(ptlNeighbors[1].size()) - 1; ptId >= 0; --ptId){
						ptl.push_back(Pt(Vector2(ptlNeighbors[1][ptId].x(), ptlNeighbors[1][ptId].y())));
					}
				}
				ptl.push_back(Pt(Vector2(x, y)));
				if(nbFirstNeighbors >= 1){
					for(qsize_t ptId = 0; ptId < ptlNeighbors[0].size(); ++ptId){
						ptl.push_back(Pt(Vector2(ptlNeighbors[0][ptId].x(), ptlNeighbors[0][ptId].y())));
					}
				}
			}
			inPixel++;
		}
	}

	//qPrint("# branchesSet.size() %d\n", branchesSet.size());
	if(branchesSet.size() == 0){
		// return fake centerline
		qf64 posX = static_cast<qf64>(imageSizeX)/2;
		qf64 posY = static_cast<qf64>(imageSizeY)/2;
		_outCenterline.appendItem(Pt(Vector2(posX, posY)));
		_outCenterline.appendItem(Pt(Vector2(posX + 1, posY)));
		_outCenterline.appendItem(Pt(Vector2(posX + 2, posY)));
		_outCenterline.appendItem(Pt(Vector2(posX + 3, posY)));
		_outCenterline.appendItem(Pt(Vector2(posX + 4, posY)));
		return;
	}

	Tree tree(branchesSet);
	CHECK_UP();
	PrintCenterlines(tree, _debugOutput);

	tree.MergeLongBranches();
	CHECK_UP();
	tree.MergeLoops();
	CHECK_UP();
	PrintCenterlines(tree, _debugOutput);

	tree.MergeAndRemoveGaps();
	CHECK_UP();
	PrintCenterlines(tree, _debugOutput);

	typedef std::map<qsize_t, PtList*> BranchesMap;
	BranchesMap sortedBranches;
	for(Tree::BranchList::iterator it = tree.m_BranchesList.begin(); it != tree.m_BranchesList.end(); ++it){
		sortedBranches.insert(std::pair<qsize_t, PtList*>((*it)->m_Ptl.size(), &((*it)->m_Ptl)));
	}

	// try to find the tip (heuristic: it should be closer to the center than the other endpoint)
	// try to find the tip (heuristic: farther than all borders)
	PtList *longestBranch = sortedBranches.rbegin()->second;
	PtList &centerline = _outCenterline;

	qf64 minDistFirst = (*longestBranch)[0].pos[PT_X];
	minDistFirst = MIN(minDistFirst, (imageSizeX - 1) - (*longestBranch)[0].pos[PT_X]);
	minDistFirst = MIN(minDistFirst, (*longestBranch)[0].pos[PT_Y]);
	minDistFirst = MIN(minDistFirst, (imageSizeY - 1) - (*longestBranch)[0].pos[PT_Y]);
	qf64 minDistLast = (*longestBranch)[(*longestBranch).size() - 1].pos[PT_X];
	minDistLast = MIN(minDistLast, (imageSizeX - 1) - (*longestBranch)[(*longestBranch).size() - 1].pos[PT_X]);
	minDistLast = MIN(minDistLast, (*longestBranch)[(*longestBranch).size() - 1].pos[PT_Y]);
	minDistLast = MIN(minDistLast, (imageSizeY - 1) - (*longestBranch)[(*longestBranch).size() - 1].pos[PT_Y]);
	//Vector6 center(static_cast<qf64>(imageSizeX]/2 - 1), static_cast<qf64>(imageSizeY/2 - 1), 0., 0., 0., 0.);
	//if(GetSquareDistance((*longestBranch)[0].pos, center) < GetSquareDistance((*longestBranch)[(*longestBranch).size() - 1].pos, center)){
	if(minDistFirst > minDistLast){
		centerline = PtList(*longestBranch);
	}
	else{
		for(qs32 ptId = static_cast<qs32>((*longestBranch).size()) - 1; ptId >= 0; --ptId){
			centerline.push_back(Pt(Vector2((*longestBranch)[ptId].x(), (*longestBranch)[ptId].y())));
		}
	}
}
