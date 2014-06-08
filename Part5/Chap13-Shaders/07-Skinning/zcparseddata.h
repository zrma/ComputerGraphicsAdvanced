#ifndef _ZCPARSEDDATA_H_
#define _ZCPARSEDDATA_H_

#include "define.h"
#include "zcmesh.h"

/// XML������ �Ľ��� ��� Ŭ����
class ZCParsedData
{
public:
	Info					m_info;				/// �� ���Ͽ� ���� �������� ����
	vector<Material>		m_materialTable;	/// �����迭
	list<ZCMesh*>			m_meshList;			/// �޽�(���) ����Ʈ
public:
	ZCParsedData() {}
	~ZCParsedData() 
	{ 
		stl_wipe_vector( m_materialTable );
		stl_wipe( m_meshList );
	}
};

#endif // _ZCPARSEDDATA_H_


