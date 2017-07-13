#include "Branch.h"

BEGIN_Q_NAMESPACE

// merge with the branch _branch, you should delete _branch after
void Branch::Merge(Branch *_branch, Branch::IdType _connectedPoint, Branch::IdType _connectedPoint2){
	Neighbors::iterator neighborIterator = FindNeighborBranchAtConnection(_branch, _connectedPoint);
	IdType connectedPoint = neighborIterator->first;
	EraseAllConnectionsWith(_branch);

	Neighbors::iterator neighborIterator2 = _branch->FindNeighborBranchAtConnection(this, _connectedPoint2);
	IdType connectedPoint2 = neighborIterator2->first;
	_branch->EraseAllConnectionsWith(this);

	Branch tmpBranch = Branch(PtList());

	if(connectedPoint == 0){
		if(connectedPoint2 == 0){
			for(IdType i = static_cast<IdType>(m_Ptl.size() - 1); i >= 0; --i){
				tmpBranch.m_Ptl.push_back(m_Ptl[i]);
			}
			for(IdType i = 0; i < _branch->m_Ptl.size(); ++i){
				tmpBranch.m_Ptl.push_back(_branch->m_Ptl[i]);
			}

			tmpBranch.TransferConnectionToNewBranch(this, static_cast<IdType>(m_Ptl.size() - 1), -1); // (m_Ptl.size() - 1) - it->first
			tmpBranch.TransferConnectionToNewBranch(_branch, static_cast<IdType>(m_Ptl.size()), 1); // it->first + m_Ptl.size()
		}
		else if(connectedPoint2 == _branch->m_Ptl.size() - 1){
			tmpBranch.m_Ptl = _branch->m_Ptl;
			for(IdType i = 0; i < m_Ptl.size(); ++i){
				tmpBranch.m_Ptl.push_back(m_Ptl[i]);
			}

			tmpBranch.TransferConnectionToNewBranch(this, static_cast<IdType>(_branch->m_Ptl.size()), 1); // it->first + _branch->m_Ptl.size()
			tmpBranch.TransferConnectionToNewBranch(_branch, 0, 1); // it->first
		}
		else{
			qPrintStdErrAndStop("error should not happen, divide the branch before or TODO this part");
		}
	}
	else if(connectedPoint == m_Ptl.size() - 1){
		if(connectedPoint2 == 0){
			tmpBranch.m_Ptl = m_Ptl;
			for(IdType i = 0; i < _branch->m_Ptl.size(); ++i){
				tmpBranch.m_Ptl.push_back(_branch->m_Ptl[i]);
			}

			tmpBranch.TransferConnectionToNewBranch(this, 0, 1); // it->first
			tmpBranch.TransferConnectionToNewBranch(_branch, static_cast<IdType>(m_Ptl.size()), 1); // it->first + m_Ptl.size()
		}
		else if(connectedPoint2 == _branch->m_Ptl.size() - 1){
			tmpBranch.m_Ptl = m_Ptl;
			for(IdType i = static_cast<IdType>(_branch->m_Ptl.size() - 1); i >= 0; --i){
				tmpBranch.m_Ptl.push_back(_branch->m_Ptl[i]);
			}

			tmpBranch.TransferConnectionToNewBranch(this, 0, 1); // it->first
			tmpBranch.TransferConnectionToNewBranch(_branch, static_cast<IdType>((_branch->m_Ptl.size() - 1) + m_Ptl.size()), -1); // ((_branch->m_Ptl.size() - 1) - it->first) + m_Ptl.size()
		}
		else{
			qPrintStdErrAndStop("error should not happen, divide the branch before or TODO this part");
		}
	}
	else{
		qPrintStdErrAndStop("error should not happen, divide the branch before or TODO this part");
	}

	ReplaceBranch(&tmpBranch);
}

// divide the branch in two branches
// the new branch is returned and should be added in the tree after
// _fromConnectedPt should be always inferior to _toConnectedPt
Branch* Branch::Divide(IdType _fromConnectedPt, IdType _toConnectedPt){
	//qAssert(_fromConnectedPt < _toConnectedPt);
	qAssert(_fromConnectedPt <= _toConnectedPt);
	Branch *branch = Q_NEW Branch(PtList());
	for(qsize_t i = _fromConnectedPt; i <= _toConnectedPt; ++i){
		branch->m_Ptl.push_back(m_Ptl[i]);
	}

	Branch tmpBranch = Branch(PtList());
	IdType incrementPosition = 0;
	IdType connectedPt1 = _fromConnectedPt - 1;
	IdType connectedPt2 = 0;
	if(_fromConnectedPt == 0){
		for(qsize_t i = _toConnectedPt + 1; i < m_Ptl.size(); ++i){
			tmpBranch.m_Ptl.push_back(m_Ptl[i]);
		}
		incrementPosition = _toConnectedPt + 1;
		connectedPt1 = 0;
		connectedPt2 = _toConnectedPt - _fromConnectedPt;
	}
	else{
		for(qsize_t i = 0; i < _fromConnectedPt; ++i){
			tmpBranch.m_Ptl.push_back(m_Ptl[i]);
		}
	}

	for(Neighbors::iterator it = m_Neighbors.begin(); it != m_Neighbors.end(); ++it){
		Branch *branch2 = it->second;
		Branch::Neighbors::iterator neighborIterator2 = branch2->FindNeighborBranch(this, it->first);
		IdType connectedPoint2 = neighborIterator2->first;
		branch2->m_Neighbors.erase(neighborIterator2);
		if(it->first >= _fromConnectedPt && it->first <= _toConnectedPt){
			branch->InsertConnection(it->first - _fromConnectedPt, branch2, connectedPoint2);
		}
		else{
			tmpBranch.InsertConnection(it->first - incrementPosition, branch2, connectedPoint2);
		}
	}

	//tmpBranch.ConnectNeighborsToNewNeighbor(connectedPt1, branch, connectedPt2);
	//branch->ConnectNeighborsToNewNeighbor(connectedPt2, &tmpBranch, connectedPt1);
	tmpBranch.InsertConnectionWithNeighbors(connectedPt1, branch, connectedPt2);
	
	ReplaceBranch(&tmpBranch);

	return branch;
}

END_Q_NAMESPACE
