#ifndef _ZCMESH_H_
#define _ZCMESH_H_

#include "define.h"

/// �Ľ̵� �������
class ZCMesh
{
public:
	string					m_strNodeType;
	int						m_nObjectID;			/// ������Ʈ ID, ������Ʈ���� ZParser.Info.strObjectNameTable�� �ִ�.
	int						m_nObjectHashID;		/// hashed string �˰����� ����ϱ� ���� ����
	int						m_nParentID;			/// �θ���ID

	bool					m_isSkinned;			/// skinned mesh == true
	Vector3f				m_bboxMax;				/// �ٿ�� �ڽ��� �ִ밪(max)
	Vector3f				m_bboxMin;				/// �ٿ�� �ڽ��� �ּҰ�(min)
	Mat4x4					m_tmLocal;				/// Local TM ���
	Mat4x4					m_tmWorld;				/// World TM ���
	vector<Vector3f>		m_vertex;				/// ������ ��ġ����
	vector<Vector3f>		m_normal;				/// ������ �������
	vector<Vector2f>		m_tvertex;				/// �ؽ��� ����

	int						m_nMaterialID;			/// ������ ID
	int						m_nMaterialCount;		/// �޽ÿ��� ������� ������ ����
	vector<IndexWithID>		m_triindex;				/// �ﰢ���� �ε���
	vector<Index3i>			m_texindex;				/// �ؽ����� �ε���
	vector<VertWeight>		m_vertweight;			/// ������ ����ġ��
	Track					m_track;				/// �ִϸ��̼� Ʈ������
public:
	vector<VertMerged>		m_vtxFinal;				/// �������� �������� ������ ���� ����
	vector<Index3i>			m_idxFinal;				/// �ε���
public:
	ZCMesh()
	{
		m_nParentID = -1;
		m_nObjectID = -1;
		m_nObjectHashID = -1;
		m_isSkinned = false;
	}

	~ZCMesh()
	{
//		stl_wipe_vector( m_nChildIDs );
		stl_wipe_vector( m_vertex );
		stl_wipe_vector( m_normal );
		stl_wipe_vector( m_tvertex );
		stl_wipe_vector( m_triindex );
		stl_wipe_vector( m_texindex );
		stl_wipe_vector( m_vertweight );
		stl_wipe_vector( m_vtxFinal );
		stl_wipe_vector( m_idxFinal );
	}
};

#endif // _ZCMESH_H_