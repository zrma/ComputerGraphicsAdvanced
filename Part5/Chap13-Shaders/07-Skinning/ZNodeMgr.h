#ifndef _ZNODEMGR_H_
#define _ZNODEMGR_H_
#include "define.h"
#include "ZCParsedData.h"
#include "zdefine.h"
#include "ZMesh.h"
#include "ZBone.h"
#include "ZRigidMesh.h"
#include "ZSkinnedMesh.h"
#include "ZFFSkinnedMesh.h"
#include "ZSWSkinnedMesh.h"
#include "ZVSSkinnedMesh.h"
#include "ZShaderMgr.h"


/**
 * 노드들을 관리하기위한 클래스
 *
 */
class ZNodeMgr
{
public:
	enum SKINMETHOD
	{ 
		SKINMETHOD_SW,	// SoftWare 스키닝
		SKINMETHOD_FF,	// D3D Fixed Function 스키닝
		SKINMETHOD_VS	// Vertex Shader 스키닝
	};

protected:
	ZObjectInfo				m_info;			/// 현재 노드전체에 대한 정보
	vector<ZMaterial>		m_materials;	/// 재질값 배열
	vector<ZNode*>			m_nodes;		/// 노드들
	LPDIRECT3DDEVICE9		m_pDev;			/// D3D디바이스
	D3DXMATRIXA16			m_matTM;		/// 자식 노드 전체에 적용될 TM
	vector<D3DXMATRIX>		m_matPalette;
	D3DXVECTOR3				m_bboxMaxOrig;	/// 바운딩 박스의 최초 최대값(max)
	D3DXVECTOR3				m_bboxMinOrig;	/// 바운딩 박스의 최초 최소값(min)
	float					m_fRadius;
	
	ZShaderMgr*				m_pShaderMgr;	/// 쉐이더를 관리하는 클래스의 포인터

	BOOL _IsBone( int n )
	{
		for( int i = 0 ; i < m_info.BoneTable.size() ; i++ )
			if( m_info.BoneTable[i] == n ) return TRUE;

		return FALSE;
	}

public:
	ZNodeMgr( LPDIRECT3DDEVICE9 pDev, ZCParsedData* pData, SKINMETHOD sm, ZShaderMgr* pSM );
	~ZNodeMgr();

	/// 정보구조체의 포인터를 반환한다
	ZObjectInfo*	GetInfo() { return &m_info; }

	/// TM을 얻어온다
	D3DXMATRIXA16*	GetTM() { &m_matTM; }

	/// TM을 셋팅한다
	void SetTM( D3DXMATRIXA16* pTM ) { m_matTM = *pTM; }
	float GetRadius() { return m_fRadius; }

	/// 애니메이션 행렬을 만든다
	int Animate( float fFrame );

	/// 노드전체를 그린다.
	int Draw( int nNodeType );
	int DrawBBox( int nNodeType );
};

#endif // _ZNODEMGR_H_