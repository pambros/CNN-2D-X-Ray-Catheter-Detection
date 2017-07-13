#ifndef __PT_LIST_HEADER_
#define __PT_LIST_HEADER_
#include "common/util/Util.h"
#ifdef NEED_MATRIX
	#include "common/maths/Matrix.h"
#endif
#ifdef NEED_FILE
	#include "common/util/File.h"
#endif

#include "common/maths/Vector.h"

#include <vector>

// REFS MeVisLab XMarker and XMarkerList structure-like (MeVisLab/Standard/Sources/ML/MLBase/mlXMarkerList.h, MeVis Medical Solutions AG)

BEGIN_Q_NAMESPACE
	typedef enum { 
		PT_X = 0,
		PT_Y = 1,
		PT_Z,
		PT_C,
		PT_T,
		PT_U
	} PT_INDEX;

	class Pt {
	public:
		Vector6 pos;
		Vector3 vec;
		qs64 type;
		qchar8 *_name;

		Pt(void) : pos(0., 0., 0., 0., 0., 0.), vec(0., 0., 0.), type(0), _name(NULL){}
		Pt(const Vector2 &_pos) : pos(_pos[0], _pos[1], 0., 0., 0., 0.), vec(0., 0., 0.), type(0), _name(NULL){}
		Pt(const Vector3 &_pos) : pos(_pos[0], _pos[1], _pos[2], 0., 0., 0.), vec(0., 0., 0.), type(0), _name(NULL){}
		Pt(const Vector6 &_pos) : pos(_pos), vec(0., 0., 0.), type(0), _name(NULL){}

		qf64 &x() { return pos[PT_X]; }
		qf64 &y() { return pos[PT_Y]; }
		qf64 &z() { return pos[PT_Z]; }
		qf64 &c() { return pos[PT_C]; }
		qf64 &t() { return pos[PT_T]; }
		qf64 &u() { return pos[PT_U]; }

		const qf64 &x() const { return pos[PT_X]; }
		const qf64 &y() const { return pos[PT_Y]; }
		const qf64 &z() const { return pos[PT_Z]; }
		const qf64 &c() const { return pos[PT_C]; }
		const qf64 &t() const { return pos[PT_T]; }
		const qf64 &u() const { return pos[PT_U]; }

		qf64 vx() const { return vec[PT_X]; }
		qf64 vy() const { return vec[PT_Y]; }
		qf64 vz() const { return vec[PT_Z]; }

		void setName(const char *newName){
			SAFE_DELETE(_name);
			if(newName != NULL){
				_name = Q_NEW qchar8[strlen(newName) + 1];
				qStrcpy(_name, strlen(newName)+1, newName);
			}
			else {
				_name = NULL;
			}
		}
	};

	class PtList {
	public:
		typedef enum {
			FLAG_POS_X = (1<<0),
			FLAG_POS_Y = (1<<1),
			FLAG_POS_Z = (1<<2),
			FLAG_POS_C = (1<<3),
			FLAG_POS_T = (1<<4),
			FLAG_POS_U = (1<<5),
			FLAG_VEC_X = (1<<6),
			FLAG_VEC_Y = (1<<7),
			FLAG_VEC_Z = (1<<8),
			FLAG_TYPE = (1<<9)
		} Flags;

		PtList(void){
		
		}

#ifdef NEED_FILE
		PtList(const qString &_fileName){
			std::vector<Vector6> vec;
			/*qu32 iErr =*/ FileReadVector6Set(_fileName, vec);
			for(std::vector<Vector6>::iterator it = vec.begin(); it != vec.end(); ++it){
				m_Vec.push_back(Pt(*it));
			}
		}
#endif

		~PtList(void){
		}

		PtList(const PtList &_ptl){
			m_Vec = _ptl.m_Vec;
		}

#ifdef NEED_FILE
		Q_DLL qu32 SaveFile(const qString &_fileName, qu32 _flags);
#endif
		
		inline const Pt& at(const q::qsize_t _i) const {
			qAssert(_i < m_Vec.size());
			return m_Vec[_i];
		}

		inline Pt& at(const q::qsize_t _i){
			qAssert(_i < m_Vec.size());
			return m_Vec[_i];
		}

		inline const Pt& operator[](const q::qsize_t _i) const {
			qAssert(_i < m_Vec.size());
			return m_Vec[_i];
		}

		inline Pt& operator[](const q::qsize_t _i){
			qAssert(_i < m_Vec.size());
			return m_Vec[_i];
		}

		inline Pt& front(void){
			qAssert(m_Vec.size() > 0);
			return m_Vec[0];
		}

		inline const Pt& front(void) const{
			qAssert(m_Vec.size() > 0);
			return m_Vec[0];
		}

		inline Pt& back(void){
			qAssert(m_Vec.size() > 0);
			return m_Vec[m_Vec.size() - 1];
		}

		inline const Pt& back(void) const{
			qAssert(m_Vec.size() > 0);
			return m_Vec[m_Vec.size() - 1];
		}

		inline qsize_t getSize(void) const {
			return m_Vec.size();
		}

		inline qsize_t size(void) const {
			return getSize();
		}

		inline qbool empty(void) const {
			return getSize() == 0;
		}

		inline void clear(void){
			m_Vec.clear();
		}

		inline void appendItem(Pt _pt){
			m_Vec.push_back(_pt);
		}

		inline void push_back(Pt _pt){
			appendItem(_pt);
		}

#ifdef NEED_MATRIX
		void Transform(const q::Matrix44 &_mat, qbool _divideByHomogeneousCoordinate = Q_FALSE){
			for(std::vector<Pt>::iterator it = m_Vec.begin(); it != m_Vec.end(); ++it){
				Pt &m = (*it);
				Vector4 position = Vector4(m.pos[PT_X], m.pos[PT_Y], m.pos[PT_Z], 1.0);
				position = _mat*position;
				if(_divideByHomogeneousCoordinate == Q_TRUE){
					position[AXE_X] = position[AXE_X]/position[3];
					position[AXE_Y] = position[AXE_Y]/position[3];
					position[AXE_Z] = position[AXE_Z]/position[3];
				}
				m.pos[PT_X] = position[AXE_X];
				m.pos[PT_Y] = position[AXE_Y];
				m.pos[PT_Z] = position[AXE_Z];
			}
		}
#endif

		qsize_t GetClosestPtId(const Pt &_pt){
			qsize_t id = 0;
			qf64 minDist = UTIL_BIG_POSITIVE_NUMBER;
			qsize_t minId = 0;
			for(std::vector<Pt>::iterator it = m_Vec.begin(); it != m_Vec.end(); ++it){
				qf64 currentDist = GetSquareDistance(_pt.pos, it->pos);
				if(currentDist < minDist){
					minDist = currentDist;
					minId = id;
				}
				++id;
			}
			return minId;
		}

		qf64 GetLength(void){
			qf64 length = 0.;
			if(m_Vec.size() > 0){
				for(qu32 id = 0; id < m_Vec.size() - 1; ++id){
					//length = length + GetSquareDistance(m_Vec[id].pos, m_Vec[id + 1].pos);
					length = length + GetDistance(m_Vec[id].pos, m_Vec[id + 1].pos);
				}
			}
			return length;
		}

	private:
		std::vector<Pt> m_Vec;
	};

END_Q_NAMESPACE

#endif
