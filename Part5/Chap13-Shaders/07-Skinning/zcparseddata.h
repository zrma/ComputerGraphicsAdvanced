#ifndef _ZCPARSEDDATA_H_
#define _ZCPARSEDDATA_H_

#include "define.h"
#include "zcmesh.h"

/// XML파일을 파싱한 결과 클래스
class ZCParsedData
{
public:
	Info					m_info;				/// 이 파일에 관한 여러가지 정보
	vector<Material>		m_materialTable;	/// 재질배열
	list<ZCMesh*>			m_meshList;			/// 메시(노드) 리스트
public:
	ZCParsedData() {}
	~ZCParsedData() 
	{ 
		stl_wipe_vector( m_materialTable );
		stl_wipe( m_meshList );
	}
};

#endif // _ZCPARSEDDATA_H_


