#ifndef __BRANCH_HEADER_
#define __BRANCH_HEADER_
#include "common/util/Util.h"
#include "common/structure/PtList.h"

#define NOT_LOOPING (-1)
#define NO_ID (-1)

BEGIN_Q_NAMESPACE

class Branch{
public:
	enum{
		INFO_1,
		INFO_2,
		INFO_COUNT
	};

	enum{
		FLAG_1,
		FLAG_2,
		FLAG_COUNT
	};

	typedef qs16 IdType;
	typedef std::pair<IdType, Branch*> NeighborsPair;
	typedef std::multimap<IdType, Branch*> Neighbors;

	Branch(PtList _ptl) : m_Ptl(_ptl){
		qMemset(m_Info, 0, INFO_COUNT*sizeof(m_Info[0]));
		for(qsize_t i = 0; i < FLAG_COUNT; ++i){
			m_Flag[i] = Q_FALSE;
		}
	}

	qbool IsEndingPoints(IdType _id){
		if(_id == 0 || _id == m_Ptl.size() - 1)
		{
			return Q_TRUE;
		}
		return Q_FALSE;
	}

	IdType GetOtherSide(IdType _id){
		if(_id == 0){
			return static_cast<IdType>(m_Ptl.size() - 1);
		}
		return 0;
	}

	IdType GetDistanceFromClosestEndingPoints(IdType _id){
		qAssert(_id >= 0 && _id < m_Ptl.size());
		return static_cast<IdType>(MIN(_id, (m_Ptl.size() - 1) - _id));
	}

	IdType GetFarthesEndingPoint(IdType _id){
		qAssert(_id >= 0 && _id < m_Ptl.size());
		if(_id >= (m_Ptl.size() - 1) - _id){
			return 0;
		}
		return static_cast<IdType>(m_Ptl.size() - 1);
	}

	IdType GetRelatedConnectedPt(IdType _id, IdType _closePtDistanceMax){
		IdType connectedPt = _id;
		IdType minDist = _closePtDistanceMax + 1;
		for(Branch::Neighbors::iterator nb = m_Neighbors.begin(); nb != m_Neighbors.end(); ++nb){
			Branch::IdType diff = nb->first - _id;
			if(ABS(diff) < minDist){
				connectedPt = nb->first;
				break;
			}
		}

		if(connectedPt <= _closePtDistanceMax && connectedPt < static_cast<Branch::IdType>(m_Ptl.size()/2)){
			connectedPt = 0;
		}
		else if(connectedPt >= static_cast<Branch::IdType>(m_Ptl.size() - 1) - _closePtDistanceMax){
			connectedPt = static_cast<Branch::IdType>(m_Ptl.size() - 1);
		}
		return connectedPt;
	}

	qu32 GetCrossingPointsNb(void){
		qu32 nb = 0;
		for(Neighbors::iterator it = m_Neighbors.begin(); it != m_Neighbors.end(); ++it){
			IdType key = it->first;
			nb = nb + 1;
			do{
				++it;
			}while(it != m_Neighbors.end() && key == it->first);
			--it;
		}
		return nb;
	}

	qu32 GetCrossingPointsNbWithBranch(Branch *_branch){
		qu32 nb = 0;
		for(Neighbors::iterator it = m_Neighbors.begin(); it != m_Neighbors.end(); ++it){
			if(it->second == _branch){
				nb = nb + 1;
			}
		}
		return nb;
	}

	qu32 GetNeighborsNbAtCrossingPt(IdType _connectedPoint){
		qu32 neighborsNb = 0;
		std::pair<Neighbors::iterator, Neighbors::iterator> itPair = m_Neighbors.equal_range(_connectedPoint);
		for(Neighbors::iterator it = itPair.first; it != itPair.second; ++it){
			neighborsNb = neighborsNb + 1;
		}
		return neighborsNb;
	}

	IdType IsLooping(qu32 _loopSizeMin, qu32 _closeDistanceMax, IdType _connectedPoint = 0){
		if(static_cast<qu32>(_connectedPoint + 1) > _loopSizeMin){
			qf64 sqrDist = GetSquareDistance(m_Ptl[0].pos, m_Ptl[_connectedPoint].pos);
			if(sqrDist <= _closeDistanceMax*_closeDistanceMax){
				return 0;
			}
		}

		if(m_Ptl.size() - _connectedPoint > _loopSizeMin){
			qf64 sqrDist = GetSquareDistance(m_Ptl[_connectedPoint].pos, m_Ptl[m_Ptl.size() - 1].pos);
			if(sqrDist <= _closeDistanceMax*_closeDistanceMax){
				return static_cast<IdType>(m_Ptl.size() - 1);
			}
		}

		return NOT_LOOPING;
	}

	IdType GetClosestPointFromBranch(IdType _connectedPt, Branch *_branch2){
		qf64 sqrDistMin = UTIL_BIG_POSITIVE_NUMBER;
		Branch::IdType connectedPt2 = 0;
		for(qsize_t i = 0; i < _branch2->m_Ptl.size(); ++i){
			qf64 sqrDist = GetSquareDistance(m_Ptl[_connectedPt].pos, _branch2->m_Ptl[i].pos);
			if(sqrDist < sqrDistMin){
				sqrDistMin = sqrDist;
				connectedPt2 = static_cast<Branch::IdType>(i);
			}
		}
		return connectedPt2;
	}

	// TODO we take the connection the closest but we never really sure that we return the correct iterator (maybe there is a special case). We should have a crossing point structure.
	Neighbors::iterator FindNeighborBranch(Branch *_branch, IdType _connectedPoint, qbool _hasToBeThere = Q_TRUE){
		qf64 sqrDistMin = UTIL_BIG_POSITIVE_NUMBER;
		Neighbors::iterator itMin = m_Neighbors.end();
		for(Neighbors::iterator it = m_Neighbors.begin(); it != m_Neighbors.end(); ++it){
			if(it->second == _branch){
				qf64 sqrDist = GetSquareDistance(m_Ptl[it->first].pos, _branch->m_Ptl[_connectedPoint].pos);
				if(sqrDist < sqrDistMin){
					itMin = it;
					sqrDistMin = sqrDist;
				}
			}
		}
		if(itMin == m_Neighbors.end() && _hasToBeThere == Q_TRUE){
			qPrintStdErrAndStop("should find the branch");
		}
		return itMin;
	}

	Neighbors::iterator FindNeighborBranchAtConnection(Branch *_branch, IdType _connectedPoint, qbool _hasToBeThere = Q_TRUE){
		std::pair<Neighbors::iterator, Neighbors::iterator> itPair = m_Neighbors.equal_range(_connectedPoint);
		for(Neighbors::iterator it = itPair.first; it != itPair.second; ++it){
			if(it->second == _branch){
				return it;
			}
		}
		if(_hasToBeThere == Q_TRUE){
			qPrintStdErrAndStop("should find the branch");
		}
		return m_Neighbors.end();
	}
	
	Branch* GetAnotherNeighborBranch(IdType _connectedPt, Branch *_branch){
		std::pair<Branch::Neighbors::iterator, Branch::Neighbors::iterator> itPair = m_Neighbors.equal_range(_connectedPt);
		for(Branch::Neighbors::iterator tmpIt = itPair.first; tmpIt != itPair.second; ++tmpIt){
			if(tmpIt->second != _branch){
				return tmpIt->second;
			}
		}
		qPrintStdErrAndStop("should find the branch");
		return NULL;
	}

	// TODO compute a better direction using all the neighborhood
	Vector2 GetDirection(IdType _fromPt, IdType _toPt, IdType _nbPointsToComputeDirection){
		Vector2 dir(1.0, 0.0);
		if(_toPt > _fromPt){
			_fromPt = MAX(_fromPt, MAX(_toPt - _nbPointsToComputeDirection, 0));
		}
		else if(_toPt < _fromPt){
			_fromPt = static_cast<IdType>(MIN(_fromPt, MIN(_toPt + _nbPointsToComputeDirection, m_Ptl.size() -1)));
		}
		dir = Vector2(m_Ptl[_toPt].pos[PT_X], m_Ptl[_toPt].pos[PT_Y]) - Vector2(m_Ptl[_fromPt].pos[PT_X], m_Ptl[_fromPt].pos[PT_Y]);
		dir.normalize();
		// if _toPt == _fromPt
		return dir;
	}

	qf64 GetSimilarityAngleMeasure(IdType _fromPt, IdType _toPt, Vector2 _dir2, IdType _nbPointsToComputeDirection){
		Vector2 dir1 = GetDirection(_fromPt, _toPt, _nbPointsToComputeDirection);
		return dir1.dot(_dir2);
	}

	Neighbors::iterator GetNeighbors(Branch *_branch2){
		for(Neighbors::iterator it = m_Neighbors.begin(); it != m_Neighbors.end(); ++it){
			if(it->second == _branch2){
				return it;
			}
		}
		return m_Neighbors.end();
	}

	qbool IsConnectionAlreadyExist(IdType _connectedPt1, Branch *_branch2, IdType _connectedPt2){
		/*if(FindNeighborBranchAtConnection(_branch2, _connectedPt1, Q_FALSE) != m_Neighbors.end()){
			return Q_TRUE;
		}

		if(_branch2->FindNeighborBranchAtConnection(this, _connectedPt2, Q_FALSE) != _branch2->m_Neighbors.end()){
			return Q_TRUE;
		}*/

		// we are more restrictive: only one connection between two branches
		if(GetNeighbors(_branch2) != m_Neighbors.end()){
			return Q_TRUE;
		}

		return Q_FALSE;
	}

	void InsertConnection(IdType _connectedPt1, Branch *_branch2, IdType _connectedPt2){
		if(_branch2 == this){
			return;
		}

		Neighbors::iterator neighborIterator = GetNeighbors(_branch2);
		if(neighborIterator != m_Neighbors.end()){
			//return;

			// check if the new connection is not more close
			Neighbors::iterator neighborIterator2 = _branch2->FindNeighborBranch(this, neighborIterator->first, Q_TRUE);
			if(GetSquareDistance(m_Ptl[neighborIterator->first].pos, _branch2->m_Ptl[neighborIterator2->first].pos) < GetSquareDistance(m_Ptl[_connectedPt1].pos, _branch2->m_Ptl[_connectedPt2].pos)){
				return;
			}
			EraseAllConnectionsWith(_branch2);
			_branch2->EraseAllConnectionsWith(this);
		}

		m_Neighbors.insert(NeighborsPair(_connectedPt1, _branch2));
		_branch2->m_Neighbors.insert(NeighborsPair(_connectedPt2, this));

		/*if(IsConnectionAlreadyExist(_connectedPt1, _branch2, _connectedPt2) == Q_FALSE){
			m_Neighbors.insert(NeighborsPair(_connectedPt1, _branch2));
			_branch2->m_Neighbors.insert(NeighborsPair(_connectedPt2, this));
		}*/
	}

	void ConnectNeighborsToNewNeighbor(IdType _connectedPt1, Branch *_branch2, IdType _connectedPt2){
		// TODO not the best solution because a "chain" of neighbors could happen but let's hope in practice it does not happen.
		// In theory we should have something like "crossing point" structure where all the neighbors (connected points) fit in a circle.
		// And if it's not the case another crossing point should be created close the previous one.
		std::pair<Neighbors::iterator, Neighbors::iterator> itPair = m_Neighbors.equal_range(_connectedPt1);
		for(Neighbors::iterator nb = itPair.first; nb != itPair.second; ++nb){
			Branch *branch3 = nb->second;
			Branch::IdType connectedPt3 = branch3->FindNeighborBranch(this, nb->first)->first;
			_branch2->InsertConnection(_connectedPt2, branch3, connectedPt3);
		}

		//m_Neighbors.insert(Branch::NeighborsPair(_connectedPt1, _branch2));
	}

	void InsertConnectionWithNeighbors(IdType _connectedPt1, Branch *_branch2, IdType _connectedPt2){
		ConnectNeighborsToNewNeighbor(_connectedPt1, _branch2, _connectedPt2);
		_branch2->ConnectNeighborsToNewNeighbor(_connectedPt2, this, _connectedPt1);
		InsertConnection(_connectedPt1, _branch2, _connectedPt2);
	}

	// internal function, should not be used
	void TransferConnectionToNewBranch(Branch *_branch, IdType _add, IdType _mul){
		for(Neighbors::iterator it = _branch->m_Neighbors.begin(); it != _branch->m_Neighbors.end(); ++it){
			Neighbors::iterator neighborIterator3 = it->second->FindNeighborBranch(_branch, it->first);
			IdType connectedPoint3 = neighborIterator3->first;
			it->second->m_Neighbors.erase(neighborIterator3);
			InsertConnection(_add + _mul*it->first, it->second, connectedPoint3);
		}
	}

	void ReplaceBranch(Branch *_branch){
		for(Neighbors::iterator it = _branch->m_Neighbors.begin(); it != _branch->m_Neighbors.end(); ++it){
			Branch *branch2 = it->second;
			for(Neighbors::iterator it2 = branch2->m_Neighbors.begin(); it2 != branch2->m_Neighbors.end(); ++it2){
				if(it2->second == _branch){
					it2->second = this;
				}
			}
		}

		m_Neighbors = _branch->m_Neighbors;
		m_Ptl = _branch->m_Ptl;
	}

	void SwitchPosition(IdType _firstId, IdType _secondId){
		Pt pt = m_Ptl[_firstId];
		m_Ptl[_firstId] = m_Ptl[_secondId];
		m_Ptl[_secondId] = pt;

		Branch::Neighbors neighbors;
		std::pair<Branch::Neighbors::iterator, Branch::Neighbors::iterator> itPair = m_Neighbors.equal_range(_firstId);
		for(Branch::Neighbors::iterator tmpIt = itPair.first; tmpIt != itPair.second; ++tmpIt){
			neighbors.insert(NeighborsPair(_secondId, tmpIt->second));
		}
		m_Neighbors.erase(_firstId);

		itPair = m_Neighbors.equal_range(_secondId);
		for(Branch::Neighbors::iterator tmpIt = itPair.first; tmpIt != itPair.second; ++tmpIt){
			neighbors.insert(NeighborsPair(_firstId, tmpIt->second));
		}
		m_Neighbors.erase(_secondId);

		for(Branch::Neighbors::iterator it = neighbors.begin(); it != neighbors.end(); ++it){
			m_Neighbors.insert(NeighborsPair(it->first, it->second));
		}
	}

	void EraseAllConnectionsWith(Branch *_branch){
		for(Branch::Neighbors::iterator nb = m_Neighbors.begin(); nb != m_Neighbors.end();){
			Branch::Neighbors::iterator eraseIter = nb++;
			if(eraseIter->second == _branch){
				m_Neighbors.erase(eraseIter);
			}
		}
	}

	// merge with the branch _branch, you should delete _branch after
	void Merge(Branch *_branch, IdType _connectedPoint, IdType _connectedPoint2);

	// divide the branch in two branches
	// the new branch is returned and should be added in the tree after
	// _fromConnectedPt should be always inferior to _toConnectedPt
	Branch* Divide(IdType _fromConnectedPt, IdType _toConnectedPt);

	PtList m_Ptl;
	Neighbors m_Neighbors;

	qf64 m_Info[INFO_COUNT];
	qbool m_Flag[FLAG_COUNT];
};

END_Q_NAMESPACE

#endif
