#ifndef _ZNODE_H_
#define _ZNODE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZTrack.h"
#include "ZShaderMgr.h"

/**
 * 모든 3D오브젝트의 최상위 클래스
 *
 */
class ZNode
{
public:
	enum NODETYPE { NODE_NODE, NODE_BONE, NODE_MESH };	// 현재 지원하는 노드의 종류
protected:

	int						m_nNodeType;	/// 노드의 종류
	int						m_nObjectID;	/// 오브젝트 ID
	int						m_nParentID;	/// 부모의ID
	int						m_nMaterialID;	/// 재질의 ID
	D3DXVECTOR3				m_bboxMaxOrig;	/// 바운딩 박스의 최초 최대값(max)
	D3DXVECTOR3				m_bboxMinOrig;	/// 바운딩 박스의 최초 최소값(min)
	D3DXVECTOR3				m_bboxMax;		/// 바운딩 박스의 현재 최대값(max)
	D3DXVECTOR3				m_bboxMin;		/// 바운딩 박스의 현재 최소값(min)
	D3DXMATRIXA16			m_matWorld;		/// world TM 행렬(불변)
	D3DXMATRIXA16			m_matLocal;		/// local TM 행렬(불변)

	/// m_matTM = m_matLocal * m_matAni * (mom's m_matTM) * (grandmom's m_matTM) * ...
	D3DXMATRIXA16			m_matTM;		/// 최종 TM 행렬(변화)
	ZTrack*					m_pTrack;		/// 애니메이션 트랙정보
	LPDIRECT3DDEVICE9		m_pDev;			/// D3D디바이스
	ZShaderMgr*				m_pShaderMgr;	/// 쉐이더 메니저
public:
	ZNode( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZNode();

	/// 노드의 종류
	int GetNodeType() { return m_nNodeType; }

	/// 메시의 ID값을 얻는다
	int	GetObjectID() { return m_nObjectID; }

	/// 부모메시의 ID값을 얻는다
	int	GetParentID() { return m_nParentID; }

	int	GetMaterialID() { return m_nMaterialID; }

	D3DXMATRIXA16* GetMatrixWorld() { return &m_matWorld; }

	D3DXMATRIXA16* GetMatrixLocal() { return &m_matLocal; }

	/// 현재의 TM값을 얻는다
	D3DXMATRIXA16* GetMatrixTM() { return &m_matTM; }

	/// 경계상자의 값을 얻는다
	void GetBBox( D3DXVECTOR3* pMin, D3DXVECTOR3* pMax )
	{
		D3DXVec3TransformCoord( &m_bboxMin, &m_bboxMinOrig, &m_matTM );
		D3DXVec3TransformCoord( &m_bboxMax, &m_bboxMaxOrig, &m_matTM );
		*pMin = m_bboxMin; *pMax = m_bboxMax; 
	}

	/// ZTrack으로부터 m_matTM을 만든다
	virtual D3DXMATRIXA16* Animate( float frame, D3DXMATRIXA16* pParentTM );

	/// 노드를 그린다.
	virtual int	Draw( D3DXMATRIX* pTM ) { return 1; }

	/// 경계상자를 그린다
	virtual int	DrawBBox( D3DXMATRIX* pTM );
};

#endif // _ZNODE_H_