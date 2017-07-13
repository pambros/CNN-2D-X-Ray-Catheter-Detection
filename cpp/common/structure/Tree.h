#ifndef __TREE_HEADER_
#define __TREE_HEADER_
#include "common/util/Util.h"
#include "common/structure/Branch.h"
#ifdef NEED_FILE
	#include "common/util/File.h"
#endif

BEGIN_Q_NAMESPACE

#define CLOSE_DISTANCE_MAX (5) // in pixel
#define CLOSE_PT_DISTANCE_MAX (0) // in number of points, not really used anymore
#define LOOP_MIN_SIZE (30) // in pixel
#define NB_POINTS_TO_COMPUTE_DIRECTION (10) // in points

#define BIG_CLOSE_DISTANCE_MAX (20) // in pixel

//#define DEBUG_TREE

class Tree{
public:
	struct Parameters {
		Parameters(void){
			m_CloseDistanceMax = CLOSE_DISTANCE_MAX;
			m_ClosePtDistanceMax = CLOSE_PT_DISTANCE_MAX;
			m_LoopSizeMin = LOOP_MIN_SIZE;
			m_NbPointsToComputeDirection = NB_POINTS_TO_COMPUTE_DIRECTION;
			m_BigCloseDistanceMax = BIG_CLOSE_DISTANCE_MAX;
		}

#ifdef NEED_FILE
		Parameters(const qString &_paramFileName){
			qFile *file = NULL;
			try{
				qu32 err = qFOpen(file, _paramFileName.c_str(), "rb");
				if(err == 0){
					throw gDefaultException;
				}
				/*qu32 iErr =*/ FileReachLine(file, qString("#\tTreeParameters"));
				/*iErr =*/ FileReadU32(file, m_CloseDistanceMax);
				/*iErr =*/ FileReadS16(file, m_ClosePtDistanceMax);
				/*iErr =*/ FileReadU32(file, m_LoopSizeMin);
				/*iErr =*/ FileReadS16(file, m_NbPointsToComputeDirection);
				/*iErr =*/ FileReadU32(file, m_BigCloseDistanceMax);
			}
			catch(q::qDefaultException){
				q::qPrint("Tree::Parameters::Parameters Error during loading\n");
				qFClose(file);
				throw gDefaultException;
			}

			qFClose(file);
		}
#endif

		qu32 m_CloseDistanceMax; // in pixel
		Branch::IdType m_ClosePtDistanceMax; // in number of points, not really used anymore
		qu32 m_LoopSizeMin; // in pixel
		Branch::IdType m_NbPointsToComputeDirection; // in number of points
		qu32 m_BigCloseDistanceMax; // in pixel
	};

#ifdef NEED_FILE
	Tree(std::vector<PtList> &_branches, const qString &_paramFileName){
		m_Parameters = Q_NEW Parameters(_paramFileName);
		AddBranches(_branches);
	}
#endif

	Tree(std::vector<PtList> &_branches){
		m_Parameters = Q_NEW Parameters();
		AddBranches(_branches);
	}

	~Tree(){
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Q_DELETE_UNIQUE(*it);
		}
		SAFE_DELETE_UNIQUE(m_Parameters);
	}

	void AddBranch(PtList _ptl);

	void AddBranches(std::vector<PtList> &_branches){
		for(std::vector<PtList>::iterator it = _branches.begin(); it != _branches.end(); ++it){
			AddBranch(*it);
		}
	}

	// _branch2 has too loop (starting and ending point have to be closed)
	void MergeLoop(Branch *_branch1, Branch::IdType _connectedPt1, Branch *_branch2, Branch::IdType _connectedPt2, Branch *_branch3, Branch::IdType _connectedPt3, qbool _incompleteLoop);

	void MergeLongBranches(void);
	void MergeLoops(void);
	void MergeAndRemoveGaps(void);;

	void ClearFlag(qsize_t _flagId){
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			(*it)->m_Flag[_flagId] = Q_FALSE;
		}
	}

	typedef std::vector<Branch*> BranchList;
	BranchList m_BranchesList;
	Parameters *m_Parameters;

#ifdef DEBUG_TREE
	void CheckUp(void){
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			for(Branch::Neighbors::iterator nb = branch->m_Neighbors.begin(); nb != branch->m_Neighbors.end(); ++nb){
				// check that the branch is not linked with itself
				if(nb->second == branch){
					qPrintStdErrAndStop("a branch is linked with itself");
				}

				// check that the neighbor branch nb->second has also a link with branch
				Branch::Neighbors::iterator itNeighbor2 = nb->second->FindNeighborBranch(branch, nb->first, Q_TRUE);

				// check that connections from branch to nb->second are not connected to the same connection two or more times
				{
					qu8 count = 0;
					for(Branch::Neighbors::iterator nb2 = branch->m_Neighbors.begin(); nb2 != branch->m_Neighbors.end(); ++nb2){
						if(nb2->second == nb->second){
							Branch::Neighbors::iterator itNeighbor3 = nb2->second->FindNeighborBranch(branch, nb2->first, Q_TRUE);
							if(itNeighbor2->first == itNeighbor3->first){
								count = count + 1;
							}
						}
					}
					if(count != 1){
						qPrintStdErrAndStop("connections from branch are connected to the same connection two or more times");
					}
				}

				// check if there is not a double connection with the same branch and the same connected point
				{
					qu8 count = 0;
					std::pair<Branch::Neighbors::iterator, Branch::Neighbors::iterator> itPair = branch->m_Neighbors.equal_range(nb->first);
					for(Branch::Neighbors::iterator nb2 = itPair.first; nb2 != itPair.second; ++nb2){
						if(nb2->second == nb->second){
							count = count + 1;
						}
					}
					if(count != 1){
						qPrintStdErrAndStop("there is a double connection with the same branch and the same connected point");
					}
				}

				// TODO check if neighbor of neighbor are also connected together. ?NOT A TODO ANYMORE? assertion not valid all the time since only one connection between two branches?
			}
		}
	}
#endif
};

END_Q_NAMESPACE

#endif
