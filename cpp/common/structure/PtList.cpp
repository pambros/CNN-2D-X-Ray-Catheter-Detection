#include "common/structure/PtList.h"

BEGIN_Q_NAMESPACE

#ifdef NEED_FILE
	qu32 PtList::SaveFile(const qString &_fileName, qu32 _flags){
		qu32 iErr = 0;
		qFile *file = NULL;
		try{
			qu32 err = qFOpen(file, _fileName.c_str(), "wb");
			if (err == 0){
				throw gDefaultException;
			}

			for(std::vector<Pt>::iterator it = m_Vec.begin(); it != m_Vec.end(); ++it){
				qString line = "";
				char buffer[MAX_STR_BUFFER];
				if(_flags & FLAG_POS_X){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[0]);
					line += qString(buffer);
				}
				if(_flags & FLAG_POS_Y){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[1]);
					line += qString(buffer);
				}
				if(_flags & FLAG_POS_Z){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[2]);
					line += qString(buffer);
				}
				if(_flags & FLAG_POS_C){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[3]);
					line += qString(buffer);
				}
				if(_flags & FLAG_POS_T){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[4]);
					line += qString(buffer);
				}
				if(_flags & FLAG_POS_U){
					qSprintf(buffer, MAX_STR_BUFFER, "%f ", (*it).pos[5]);
					line += qString(buffer);
				}
				if(_flags & FLAG_TYPE){
					qSprintf(buffer, MAX_STR_BUFFER, "%lld ", (*it).type);
					line += qString(buffer);
				}

				int iErr = file->Fprintf("%s\n", line.c_str());
				if(iErr < 0){
					throw gDefaultException;
				}
			}
		}
		catch (q::qDefaultException){
			q::qPrint("PtList::SaveFile Error during saving %s\n", _fileName.c_str());
			qFClose(file);
			throw gDefaultException;
		}

		qFClose(file);

		return iErr;
	}
#endif

END_Q_NAMESPACE
