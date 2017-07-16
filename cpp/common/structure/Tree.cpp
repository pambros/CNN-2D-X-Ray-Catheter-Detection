#include "Tree.h"

#include <algorithm>

//#define DOT_PRODUCT_MARGIN (0.005) // give some margin when compare two dot products
#define DOT_PRODUCT_MARGIN (0.01) // give some margin when compare two dot products

BEGIN_Q_NAMESPACE

void Tree::AddBranch(PtList _ptl){
	Branch *branch = Q_NEW Branch(_ptl);

	if(m_BranchesList.empty() != Q_TRUE){
		qf64 sqrCloseDistanceMax = m_Parameters->m_CloseDistanceMax*m_Parameters->m_CloseDistanceMax;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch2 = *it;

			qf64 sqrDistMin = UTIL_BIG_POSITIVE_NUMBER;
			Branch::IdType connectedPt1 = 0;
			Branch::IdType connectedPt2 = 0;
			for(qsize_t i = 0; i < branch->m_Ptl.size(); ++i){
				for(qsize_t j = 0; j < branch2->m_Ptl.size(); ++j){
					qf64 sqrDist = GetSquareDistance(branch->m_Ptl[i].pos, branch2->m_Ptl[j].pos);
					if(sqrDist < sqrDistMin){
						sqrDistMin = sqrDist;
						connectedPt1 = static_cast<Branch::IdType>(i);
						connectedPt2 = static_cast<Branch::IdType>(j);
					}
				}
			}

			if(sqrDistMin <= sqrCloseDistanceMax){
				connectedPt1 = branch->GetRelatedConnectedPt(connectedPt1, m_Parameters->m_ClosePtDistanceMax);
				connectedPt2 = branch2->GetRelatedConnectedPt(connectedPt2, m_Parameters->m_ClosePtDistanceMax);

				// skip it, if we already did the connection before
				// or if the connection from branch2 is already connected to one from branch
				/*if(branch->IsConnectionAlreadyExist(connectedPt1, branch2, connectedPt2) == Q_TRUE){
					continue;
				}*/

				//branch->ConnectNeighborsToNewNeighbor(connectedPt1, branch2, connectedPt2);
				//branch2->ConnectNeighborsToNewNeighbor(connectedPt2, branch, connectedPt1);
				branch->InsertConnectionWithNeighbors(connectedPt1, branch2, connectedPt2);
			}
		}
	}

	m_BranchesList.push_back(branch);
}

// _branch2 has too loop (starting and ending point have to be close)
void Tree::MergeLoop(Branch *_branch1, Branch::IdType _connectedPt1, Branch *_branch2, Branch::IdType _connectedPt2, Branch *_branch3, Branch::IdType _connectedPt3, qbool _incompleteLoop = Q_FALSE){
	if(_branch2->IsEndingPoints(_connectedPt2) == Q_FALSE){
		Branch *tmpBranch = _branch2->Divide(_connectedPt2 + 1, static_cast<Branch::IdType>(_branch2->m_Ptl.size() - 1));

		_branch2->m_Neighbors.insert(Branch::NeighborsPair(0, tmpBranch));
		tmpBranch->m_Neighbors.insert(Branch::NeighborsPair(static_cast<Branch::IdType>(tmpBranch->m_Ptl.size() - 1), _branch2));
		_branch2->Merge(tmpBranch, 0, static_cast<Branch::IdType>(tmpBranch->m_Ptl.size() - 1));

		SAFE_DELETE_UNIQUE(tmpBranch);
		_connectedPt2 = 0;
	}

	Branch::IdType otherSideConnectedPt = _branch1->GetOtherSide(_connectedPt1);
	Branch::IdType otherSideConnectedPt2 = _branch2->GetOtherSide(_connectedPt2);
	Branch::IdType otherSideConnectedPt3 = _branch3->GetOtherSide(_connectedPt3);

	Vector2 dir2 = _branch2->GetDirection(otherSideConnectedPt2, _connectedPt2, m_Parameters->m_NbPointsToComputeDirection);
	Vector2 otherDir2;
	if(_incompleteLoop == Q_FALSE){
		otherDir2 = _branch2->GetDirection(_connectedPt2, otherSideConnectedPt2, m_Parameters->m_NbPointsToComputeDirection);
	}
	else{
		otherDir2 = Vector2(_branch2->m_Ptl[_connectedPt2].pos[PT_X], _branch2->m_Ptl[_connectedPt2].pos[PT_Y]) - Vector2(_branch2->m_Ptl[otherSideConnectedPt2].pos[PT_X], _branch2->m_Ptl[otherSideConnectedPt2].pos[PT_Y]);
		otherDir2.normalize();
	}

	qf64 measure1 = _branch1->GetSimilarityAngleMeasure(otherSideConnectedPt, _connectedPt1, -dir2, m_Parameters->m_NbPointsToComputeDirection);
	qf64 otherMeasure1 = _branch1->GetSimilarityAngleMeasure(otherSideConnectedPt, _connectedPt1, -otherDir2, m_Parameters->m_NbPointsToComputeDirection);
	qf64 measure3 = _branch3->GetSimilarityAngleMeasure(otherSideConnectedPt3, _connectedPt3, -dir2, m_Parameters->m_NbPointsToComputeDirection);
	qf64 otherMeasure3 = _branch3->GetSimilarityAngleMeasure(otherSideConnectedPt3, _connectedPt3, -otherDir2, m_Parameters->m_NbPointsToComputeDirection);

	Pt pt = _branch2->m_Ptl[otherSideConnectedPt2];
	if(measure1 + otherMeasure3 > otherMeasure1 + measure3){
		// TODO we insert fake connections before to merge, not really nice, improve it?
		_branch2->m_Neighbors.insert(Branch::NeighborsPair(_connectedPt2, _branch1));
		_branch1->m_Neighbors.insert(Branch::NeighborsPair(_connectedPt1, _branch2));
		_branch1->Merge(_branch2, _connectedPt1, _connectedPt2);
	}
	else{
		pt = _branch2->m_Ptl[_connectedPt2];
		// TODO we insert fake connections before to merge, not really nice, improve it?
		_branch2->m_Neighbors.insert(Branch::NeighborsPair(otherSideConnectedPt2, _branch1));
		_branch1->m_Neighbors.insert(Branch::NeighborsPair(_connectedPt1, _branch2));
		_branch1->Merge(_branch2, _connectedPt1, otherSideConnectedPt2);
	}

	if(pt.pos[PT_X] == _branch1->m_Ptl[0].pos[PT_X] && pt.pos[PT_Y] == _branch1->m_Ptl[0].pos[PT_Y]){
		_connectedPt1 = 0;
	}
	else{
		_connectedPt1 = static_cast<Branch::IdType>(_branch1->m_Ptl.size() - 1);
	}

	// TODO we insert fake connections before to merge, not really nice, improve it?
	_branch1->m_Neighbors.insert(Branch::NeighborsPair(_connectedPt1, _branch3));
	_branch3->m_Neighbors.insert(Branch::NeighborsPair(_connectedPt3, _branch1));
	_branch1->Merge(_branch3, _connectedPt1, _connectedPt3);
}

void Tree::MergeLongBranches(void){
	qbool somethingNew = Q_TRUE;
	while(somethingNew == Q_TRUE){
		somethingNew = Q_FALSE;

		qbool branchListIteratorInvalid = Q_FALSE;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			// if the branch has been deleted in this for loop
			if(branch == NULL){
				continue;
			}

			for(Branch::Neighbors::iterator neighborIterator = branch->m_Neighbors.begin(); neighborIterator != branch->m_Neighbors.end(); ++neighborIterator){
				Branch::IdType connectedPt = neighborIterator->first;
				Branch::IdType farthestEndingPoint = branch->GetFarthesEndingPoint(connectedPt);
				Branch *branch2 = neighborIterator->second;
				Branch::Neighbors::iterator neighborIterator2 = branch2->FindNeighborBranch(branch, neighborIterator->first);
				Branch::IdType connectedPt2 = neighborIterator2->first;
				Branch::IdType farthestEndingPoint2 = branch2->GetFarthesEndingPoint(connectedPt2);

				// simple connections
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				Branch::IdType length = ABS(connectedPt - farthestEndingPoint) + 1;
				Branch::IdType length2 = ABS(connectedPt2 - farthestEndingPoint2) + 1;
				if(length + length2 > branch->m_Ptl.size() && length + length2 > branch2->m_Ptl.size()){
					Branch *newBranch = branch;
					if(length < branch->m_Ptl.size()){
						if(connectedPt < farthestEndingPoint){
							newBranch = branch->Divide(connectedPt, farthestEndingPoint);
							connectedPt = 0;
						}
						else{
							newBranch = branch->Divide(farthestEndingPoint, connectedPt);
							connectedPt = static_cast<Branch::IdType>(newBranch->m_Ptl.size() - 1);
						}
					}
					Branch *newBranch2 = branch2;
					if(length2 < branch2->m_Ptl.size()){
						if(connectedPt2 < farthestEndingPoint2){
							newBranch2 = branch2->Divide(connectedPt2, farthestEndingPoint2);
							connectedPt2 = 0;
						}
						else{
							newBranch2 = branch2->Divide(farthestEndingPoint2, connectedPt2);
							connectedPt2 = static_cast<Branch::IdType>(newBranch2->m_Ptl.size() - 1);
						}
					}

					newBranch->Merge(newBranch2, connectedPt, connectedPt2);
					if(newBranch != branch){
						m_BranchesList.push_back(newBranch);
						branchListIteratorInvalid = Q_TRUE;
					}

					if(newBranch2 == branch2){
						BranchList::iterator itBranch2 = std::find(m_BranchesList.begin(), m_BranchesList.end(), branch2);
						*itBranch2 = NULL;
					}
					SAFE_DELETE_UNIQUE(newBranch2);

					somethingNew = Q_TRUE;
					break;
				}
			}

			if(branchListIteratorInvalid == Q_TRUE){
				break;
			}
		}
	}

	const Branch * const branchToDelete = NULL;
	m_BranchesList.erase(std::remove(m_BranchesList.begin(), m_BranchesList.end(), branchToDelete), m_BranchesList.end());
}

void Tree::MergeLoops(void){
	qbool somethingNew = Q_TRUE;
	while(somethingNew == Q_TRUE){
		somethingNew = Q_FALSE;

		qbool branchListIteratorInvalid = Q_FALSE;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			// if the branch has been deleted in this for loop
			if(branch == NULL){
				continue;
			}

			for(Branch::Neighbors::iterator neighborIterator = branch->m_Neighbors.begin(); neighborIterator != branch->m_Neighbors.end(); ++neighborIterator){
				Branch::IdType connectedPt = neighborIterator->first;
				Branch *branch2 = neighborIterator->second;
				Branch::Neighbors::iterator neighborIterator2 = branch2->FindNeighborBranch(branch, neighborIterator->first);
				Branch::IdType connectedPt2 = neighborIterator2->first;

				// branch2 is a loop from the beginning to the end (third example in Fig. 2, MICCAI paper)
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if(branch2->IsLooping(m_Parameters->m_LoopSizeMin, m_Parameters->m_CloseDistanceMax) != NOT_LOOPING){
					if(branch->IsEndingPoints(connectedPt) == Q_FALSE){
						//if(branch->GetNeighborsNbAtCrossingPt(connectedPt) == 1){
						if(Q_TRUE){
							Branch *newBranch = branch->Divide(connectedPt + 1, static_cast<Branch::IdType>(branch->m_Ptl.size() - 1));
							MergeLoop(branch, connectedPt, branch2, connectedPt2, newBranch, 0);

							BranchList::iterator itBranch2 = std::find(m_BranchesList.begin(), m_BranchesList.end(), branch2);
							SAFE_DELETE_UNIQUE(branch2);
							*itBranch2 = NULL;
							SAFE_DELETE_UNIQUE(newBranch);

							somethingNew = Q_TRUE;
							break;
						}
					}
				}
				// (second example in Fig. 2, MICCAI paper)
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//else if(branch2->IsEndingPoints(connectedPt2) == Q_TRUE){ // comment this to be more flexible, had to add a part to divide branch2
				else{
					//if(branch->GetNeighborsNbAtCrossingPt(connectedPt) == 1){
					if(Q_TRUE){
						Branch::IdType otherConnectedPt = branch->IsLooping(m_Parameters->m_LoopSizeMin, m_Parameters->m_CloseDistanceMax, connectedPt);
						if(otherConnectedPt != NOT_LOOPING){
							// we have to add this part if we comment the else if{branch2->IsEndingPoints(connectedPt2) == Q_TRUE} before
							if(branch2->IsEndingPoints(connectedPt2) == Q_FALSE){
								Branch::IdType otherConnectedPt2 = branch2->GetFarthesEndingPoint(connectedPt2);
								Branch *newBranch = NULL;
								if(otherConnectedPt2 == 0){
									newBranch = branch2->Divide(connectedPt2 + 1, static_cast<Branch::IdType>(branch2->m_Ptl.size() - 1));
								}
								else{
									newBranch = branch2->Divide(0, connectedPt2 - 1);
									connectedPt2 = 0;
								}
								m_BranchesList.push_back(newBranch);
								branchListIteratorInvalid = Q_TRUE;
							}

							Branch *newBranch = NULL;
							if(otherConnectedPt == 0){
								newBranch = branch->Divide(0, connectedPt - 1);
								MergeLoop(branch, 0, newBranch, 0, branch2, connectedPt2);
							}
							else{
								newBranch = branch->Divide(connectedPt + 1, static_cast<Branch::IdType>(branch->m_Ptl.size() - 1));
								MergeLoop(branch, connectedPt, newBranch, 0, branch2, connectedPt2);
							}

							BranchList::iterator itBranch2 = std::find(m_BranchesList.begin(), m_BranchesList.end(), branch2);
							SAFE_DELETE_UNIQUE(branch2);
							*itBranch2 = NULL;
							SAFE_DELETE_UNIQUE(newBranch);

							somethingNew = Q_TRUE;
							break;
						}
					}
				}
			}

			if(branchListIteratorInvalid == Q_TRUE){
				break;
			}
		}
	}

	somethingNew = Q_TRUE;
	while(somethingNew == Q_TRUE){
		somethingNew = Q_FALSE;

		qbool branchListIteratorInvalid = Q_FALSE;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			// if the branch has been deleted in this for loop
			if(branch == NULL){
				continue;
			}

			for(Branch::Neighbors::iterator neighborIterator = branch->m_Neighbors.begin(); neighborIterator != branch->m_Neighbors.end(); ++neighborIterator){
				Branch::IdType connectedPt = neighborIterator->first;
				Branch *branch2 = neighborIterator->second;
				Branch::Neighbors::iterator neighborIterator2 = branch2->FindNeighborBranch(branch, neighborIterator->first);
				Branch::IdType connectedPt2 = neighborIterator2->first;

				// loop (fourth example in Fig. 2, MICCAI paper)
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if(branch2->IsLooping(m_Parameters->m_LoopSizeMin, m_Parameters->m_CloseDistanceMax) == NOT_LOOPING && branch2->IsEndingPoints(connectedPt2) == Q_TRUE){
					//if(branch->GetNeighborsNbAtCrossingPt(connectedPt) == 1){
					if(Q_TRUE){
						Branch::IdType otherConnectedPt = branch->IsLooping(m_Parameters->m_LoopSizeMin, m_Parameters->m_CloseDistanceMax, connectedPt);
						if(otherConnectedPt == NOT_LOOPING){
							Branch::IdType otherConnectedPt2 = branch2->GetOtherSide(connectedPt2);
							Branch::IdType otherConnectedPt1 = branch2->GetClosestPointFromBranch(otherConnectedPt2, branch);
							if(ABS(otherConnectedPt1 - connectedPt) > static_cast<Branch::IdType>(m_Parameters->m_LoopSizeMin)
								&& GetSquareDistance(branch2->m_Ptl[otherConnectedPt2].pos, branch->m_Ptl[otherConnectedPt1].pos) <= m_Parameters->m_CloseDistanceMax*m_Parameters->m_CloseDistanceMax){
								if(connectedPt > otherConnectedPt1){
									// TODO we insert fake connections before to merge, not really nice, improve it?
									branch2->m_Neighbors.insert(Branch::NeighborsPair(otherConnectedPt2, branch));
									branch->m_Neighbors.insert(Branch::NeighborsPair(otherConnectedPt1, branch2));

									Branch::IdType tmpPt = otherConnectedPt1;
									otherConnectedPt1 = connectedPt;
									connectedPt = tmpPt;
									tmpPt = otherConnectedPt2;
									otherConnectedPt2 = connectedPt2;
									connectedPt2 = tmpPt;
								}

								Branch *newBranch = branch->Divide(connectedPt + 1, static_cast<Branch::IdType>(branch->m_Ptl.size() - 1));
								//if(otherConnectedPt1 - connectedPt - 1 - 1 == newBranch->m_Ptl.size() - 1){
								// should not happen, this case is already cover above
								//}
								//else{
								Branch *newLoop = newBranch->Divide(0, otherConnectedPt1 - connectedPt - 1 - 1);
								//}
								Pt pt = branch2->m_Ptl[otherConnectedPt2];
								branch->Merge(branch2, connectedPt, connectedPt2);

								if(pt.pos[PT_X] == branch->m_Ptl[0].pos[PT_X] && pt.pos[PT_Y] == branch->m_Ptl[0].pos[PT_Y]){
									connectedPt = 0;
								}
								else{
									connectedPt = static_cast<Branch::IdType>(branch->m_Ptl.size() - 1);
								}

								MergeLoop(branch, connectedPt, newLoop, 0, newBranch, 0);

								BranchList::iterator itBranch2 = std::find(m_BranchesList.begin(), m_BranchesList.end(), branch2);
								SAFE_DELETE_UNIQUE(branch2);
								*itBranch2 = NULL;
								SAFE_DELETE_UNIQUE(newBranch);
								SAFE_DELETE_UNIQUE(newLoop);

								somethingNew = Q_TRUE;
								break;
							}
						}
					}
				}
			}

			if(branchListIteratorInvalid == Q_TRUE){
				break;
			}
		}
	}

	// internal loop (first example in Fig. 2, MICCAI paper)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	somethingNew = Q_TRUE;
	while(somethingNew == Q_TRUE){
		somethingNew = Q_FALSE;

		qbool branchListIteratorInvalid = Q_FALSE;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			// if the branch has been deleted in this for loop
			if(branch == NULL){
				continue;
			}

			Branch::IdType possibleId = NO_ID;
			Branch::IdType possibleId2 = NO_ID;
			qf64 possibleSqrDist = m_Parameters->m_CloseDistanceMax*m_Parameters->m_CloseDistanceMax + UTIL_SMALL_EPSILON;
			for(Branch::IdType id = 0; id < static_cast<Branch::IdType>(branch->m_Ptl.size()) - static_cast<Branch::IdType>(m_Parameters->m_LoopSizeMin); ++id){
				qf64 minSqrDist = UTIL_BIG_POSITIVE_NUMBER;
				Branch::IdType minId2 = NO_ID;
				for(Branch::IdType id2 = id + m_Parameters->m_LoopSizeMin; id2 < branch->m_Ptl.size(); ++id2){
					qf64 sqrDist = GetSquareDistance(branch->m_Ptl[id].pos, branch->m_Ptl[id2].pos);
					if(sqrDist < minSqrDist){
						minId2 = id2;
						minSqrDist = sqrDist;
					}
				}

				if(minSqrDist < possibleSqrDist){
					possibleId = id;
					possibleId2 = minId2;
					possibleSqrDist = minSqrDist;
				}

				// this line checks that we have the minimum local of the potential loop
				if(possibleId != NO_ID && (id == possibleId + m_Parameters->m_LoopSizeMin/2 || id == branch->m_Ptl.size() - m_Parameters->m_LoopSizeMin - 1)){
					// to avoid special case
					if(possibleId == 0){
						possibleId = 1;
					}
					if(possibleId2 == branch->m_Ptl.size() - 1){
						possibleId2 = static_cast<Branch::IdType>(branch->m_Ptl.size() - 2);
					}

					Branch::IdType connectedPt1 = possibleId;
					Branch::IdType otherSideConnectedPt1 = 0;
					Branch::IdType connectedPt2 = possibleId + 1;
					Branch::IdType otherSideConnectedPt2 = possibleId2 - 1;
					Branch::IdType connectedPt3 = possibleId2;
					Branch::IdType otherSideConnectedPt3 = static_cast<Branch::IdType>(branch->m_Ptl.size() - 1);
					
					Vector2 dir2 = branch->GetDirection(otherSideConnectedPt2, connectedPt2, m_Parameters->m_NbPointsToComputeDirection);
					Vector2 otherDir2 = branch->GetDirection(connectedPt2, otherSideConnectedPt2, m_Parameters->m_NbPointsToComputeDirection);

					qf64 measure1 = branch->GetSimilarityAngleMeasure(otherSideConnectedPt1, connectedPt1, -dir2, m_Parameters->m_NbPointsToComputeDirection);
					qf64 otherMeasure1 = branch->GetSimilarityAngleMeasure(otherSideConnectedPt1, connectedPt1, -otherDir2, m_Parameters->m_NbPointsToComputeDirection);
					qf64 measure3 = branch->GetSimilarityAngleMeasure(otherSideConnectedPt3, connectedPt3, -dir2, m_Parameters->m_NbPointsToComputeDirection);
					qf64 otherMeasure3 = branch->GetSimilarityAngleMeasure(otherSideConnectedPt3, connectedPt3, -otherDir2, m_Parameters->m_NbPointsToComputeDirection);

					// if the loop is not in the good side
					if(
					(((connectedPt1 - otherSideConnectedPt1) < m_Parameters->m_NbPointsToComputeDirection)
						&& ((connectedPt1 - otherSideConnectedPt1) < (otherSideConnectedPt3 - connectedPt3))
						&& otherMeasure3 < measure3 - DOT_PRODUCT_MARGIN)
					// TODO otherMeasure3 < measure3 - 0.005 check distance between points and do ratio distance/dot product to avoid picking one completely far away
					||
					(((otherSideConnectedPt3 - connectedPt3) < m_Parameters->m_NbPointsToComputeDirection)
						&& ((otherSideConnectedPt3 - connectedPt3) < (connectedPt1 - otherSideConnectedPt1))
						&& measure1 < otherMeasure1 - DOT_PRODUCT_MARGIN)
					// TODO measure1 < otherMeasure1 - 0.005 check distance between points and do ratio distance/dot product to avoid picking one completely far away
					||
					(((connectedPt1 - otherSideConnectedPt1) >= m_Parameters->m_NbPointsToComputeDirection)
						&& ((otherSideConnectedPt3 - connectedPt3) >= m_Parameters->m_NbPointsToComputeDirection)
						&& (measure1 + otherMeasure3 < otherMeasure1 + measure3))
					){
						Branch::IdType endIdLoop = (((possibleId2 - 1) - (possibleId + 1)) + 1)/2;
						for(Branch::IdType idLoop = 0; idLoop < endIdLoop; ++idLoop){
							branch->SwitchPosition(possibleId + 1 + idLoop, possibleId2 - 1 - idLoop);
						}
					}
					id = possibleId2 + 1;

					possibleId = NO_ID;
					possibleId2 = NO_ID;
					possibleSqrDist = m_Parameters->m_CloseDistanceMax*m_Parameters->m_CloseDistanceMax + UTIL_SMALL_EPSILON;
				}
			}

			if(branchListIteratorInvalid == Q_TRUE){
				break;
			}
		}
	}

	const Branch * const branchToDelete = NULL;
	m_BranchesList.erase(std::remove(m_BranchesList.begin(), m_BranchesList.end(), branchToDelete), m_BranchesList.end());
}

void Tree::MergeAndRemoveGaps(void){
	qu32 oldCloseDistanceMax = m_Parameters->m_CloseDistanceMax;
	
	//const qsize_t LIST_SIZE = 4;
	//qu32 closeDistanceMaxList[LIST_SIZE] = {10, 20, 30, 40};
	const qsize_t LIST_SIZE = 1;
	qu32 closeDistanceMaxList[LIST_SIZE] = {m_Parameters->m_BigCloseDistanceMax};
	for(qsize_t i = 0; i < LIST_SIZE; ++i){
		m_Parameters->m_CloseDistanceMax = closeDistanceMaxList[i];

		BranchList branchesList = m_BranchesList;
		m_BranchesList.clear();
		for(BranchList::iterator it = branchesList.begin(); it != branchesList.end(); ++it){
			Branch *branch = *it;
			AddBranch(branch->m_Ptl);
			SAFE_DELETE_UNIQUE(branch);
		}

		MergeLongBranches();
		MergeLoops();
	}

#define _REMOVE_GAPS
#ifdef _REMOVE_GAPS
	qbool somethingNew = Q_TRUE;
	while(somethingNew == Q_TRUE){
		somethingNew = Q_FALSE;
		
		qbool branchListIteratorInvalid = Q_FALSE;
		for(BranchList::iterator it = m_BranchesList.begin(); it != m_BranchesList.end(); ++it){
			Branch *branch = *it;
			// if the branch has been deleted in this for loop
			if(branch == NULL){
				continue;
			}

			for(Branch::Neighbors::iterator neighborIterator = branch->m_Neighbors.begin(); neighborIterator != branch->m_Neighbors.end(); ++neighborIterator){
				Branch::IdType connectedPt = neighborIterator->first;
				Branch *branch2 = neighborIterator->second;
				Branch::Neighbors::iterator neighborIterator2 = branch2->FindNeighborBranch(branch, neighborIterator->first);
				Branch::IdType connectedPt2 = neighborIterator2->first;

				// loop with missing part
				////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//if(branch2->IsEndingPoints(connectedPt2) == Q_TRUE){
				if(Q_TRUE){
					//if(branch->GetNeighborsNbAtCrossingPt(connectedPt) == 1){ // comment it because not sure if it is useful at some point?
					if(Q_TRUE){
						if(branch->m_Ptl.size() > branch2->m_Ptl.size() && branch2->m_Ptl.size() > m_Parameters->m_LoopSizeMin
						&& connectedPt < branch->m_Ptl.size() - 1){
							Branch *branch3 = branch->Divide(connectedPt + 1, static_cast<Branch::IdType>(branch->m_Ptl.size() - 1));
							Branch::IdType connectedPt3 = 0;

							MergeLoop(branch, connectedPt, branch2, connectedPt2, branch3, connectedPt3, Q_TRUE);

							BranchList::iterator itBranch2 = std::find(m_BranchesList.begin(), m_BranchesList.end(), branch2);
							SAFE_DELETE_UNIQUE(branch2);
							*itBranch2 = NULL;
							SAFE_DELETE_UNIQUE(branch3);
							somethingNew = Q_TRUE;
							break;
						}
					}
				}
			}

			if(branchListIteratorInvalid == Q_TRUE){
				break;
			}
		}
	}

	const Branch * const branchToDelete = NULL;
	m_BranchesList.erase(std::remove(m_BranchesList.begin(), m_BranchesList.end(), branchToDelete), m_BranchesList.end());
#endif

	m_Parameters->m_CloseDistanceMax = oldCloseDistanceMax;
}

END_Q_NAMESPACE
