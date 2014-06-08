#ifndef _ZCMESH_H_
#define _ZCMESH_H_

#include "define.h"

/// 파싱된 노드정보
class ZCMesh
{
public:
	string					m_strNodeType;
	int						m_nObjectID;			/// 오브젝트 ID, 오브젝트명은 ZParser.Info.strObjectNameTable에 있다.
	int						m_nObjectHashID;		/// hashed string 알고리즘을 사용하기 위한 변수
	int						m_nParentID;			/// 부모의ID

	bool					m_isSkinned;			/// skinned mesh == true
	Vector3f				m_bboxMax;				/// 바운딩 박스의 최대값(max)
	Vector3f				m_bboxMin;				/// 바운딩 박스의 최소값(min)
	Mat4x4					m_tmLocal;				/// Local TM 행렬
	Mat4x4					m_tmWorld;				/// World TM 행렬
	vector<Vector3f>		m_vertex;				/// 정점의 위치정보
	vector<Vector3f>		m_normal;				/// 정점의 노멀정보
	vector<Vector2f>		m_tvertex;				/// 텍스쳐 정보

	int						m_nMaterialID;			/// 재질의 ID
	int						m_nMaterialCount;		/// 메시에서 사용중인 재질의 개수
	vector<IndexWithID>		m_triindex;				/// 삼각형의 인덱스
	vector<Index3i>			m_texindex;				/// 텍스쳐의 인덱스
	vector<VertWeight>		m_vertweight;			/// 정점의 가중치값
	Track					m_track;				/// 애니메이션 트랙정보
public:
	vector<VertMerged>		m_vtxFinal;				/// 정점관련 정보들을 종합한 최종 정점
	vector<Index3i>			m_idxFinal;				/// 인덱스
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