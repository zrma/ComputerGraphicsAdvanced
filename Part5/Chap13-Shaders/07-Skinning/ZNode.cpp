#include "ZNode.h"

ZNode::ZNode( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM )
{
	m_pDev = pDev;
	m_pShaderMgr = pSM;
	m_nNodeType = NODE_NODE;
	m_nObjectID = pMesh->m_nObjectID;
	m_nParentID = pMesh->m_nParentID;

	m_bboxMaxOrig.x = pMesh->m_bboxMax.x;
	m_bboxMaxOrig.y = pMesh->m_bboxMax.y;
	m_bboxMaxOrig.z = pMesh->m_bboxMax.z;
	m_bboxMinOrig.x = pMesh->m_bboxMin.x;
	m_bboxMinOrig.y = pMesh->m_bboxMin.y;
	m_bboxMinOrig.z = pMesh->m_bboxMin.z;
	m_nMaterialID = pMesh->m_nMaterialID;

	// 기본 local TM복사
	memcpy( &m_matLocal, &pMesh->m_tmLocal, sizeof m_matLocal );
	memcpy( &m_matWorld, &pMesh->m_tmWorld, sizeof m_matLocal );
	m_matTM = m_matLocal;

	m_pTrack = new ZTrack( pMesh->m_track );
}

D3DXMATRIXA16* ZNode::Animate( float frame, D3DXMATRIXA16* pParentTM )
{
	D3DXMATRIXA16*	pmatAni;

	// 애니메이션 행렬을 만든다.
	pmatAni = m_pTrack->Animate( frame );
	m_matTM = m_matLocal * *pmatAni;

	// 만약 pos키값이 없으면 local TM의 좌표를 사용한다
	if( pmatAni->_41 == 0.0f && pmatAni->_42 == 0.0f && pmatAni->_43 == 0.0f )
	{
		m_matTM._41 = m_matLocal._41;
		m_matTM._42 = m_matLocal._42;
		m_matTM._43 = m_matLocal._43;
	}
	else	// pos키값을 좌표값으로 적용한다(이렇게 하지 않으면 TM의 pos성분이 두번적용된다)
	{
		m_matTM._41 = pmatAni->_41;
		m_matTM._42 = pmatAni->_42;
		m_matTM._43 = pmatAni->_43;
	}

	m_matTM = m_matTM * *pParentTM;
	return &m_matTM;
}

ZNode::~ZNode()
{
	S_DEL( m_pTrack );
}

int	ZNode::DrawBBox( D3DXMATRIX* pTM )
{
	BOXVERTEX vtx[8];
	vtx[0] = BOXVERTEX( m_bboxMinOrig.x, m_bboxMaxOrig.y, m_bboxMaxOrig.z, 0xffff0000 );
	vtx[1] = BOXVERTEX( m_bboxMaxOrig.x, m_bboxMaxOrig.y, m_bboxMaxOrig.z, 0xffff0000 );
	vtx[2] = BOXVERTEX( m_bboxMaxOrig.x, m_bboxMaxOrig.y, m_bboxMinOrig.z, 0xffff0000 );
	vtx[3] = BOXVERTEX( m_bboxMinOrig.x, m_bboxMaxOrig.y, m_bboxMinOrig.z, 0xffff0000 );
	vtx[4] = BOXVERTEX( m_bboxMinOrig.x, m_bboxMinOrig.y, m_bboxMaxOrig.z, 0xffff0000 );
	vtx[5] = BOXVERTEX( m_bboxMaxOrig.x, m_bboxMinOrig.y, m_bboxMaxOrig.z, 0xffff0000 );
	vtx[6] = BOXVERTEX( m_bboxMaxOrig.x, m_bboxMinOrig.y, m_bboxMinOrig.z, 0xffff0000 );
	vtx[7] = BOXVERTEX( m_bboxMinOrig.x, m_bboxMinOrig.y, m_bboxMinOrig.z, 0xffff0000 );

	Index3w		idx[12] = 
	{ 
		{ 0, 1, 2 }, { 0, 2, 3 },	/// 윗면
		{ 4, 6, 5 }, { 4, 7, 6 },	/// 아랫면
		{ 0, 3, 7 }, { 0, 7, 4 },	/// 왼면
		{ 1, 5, 6 }, { 1, 6, 2 },	/// 오른면
		{ 3, 2, 6 }, { 3, 6, 7 },	/// 앞면
		{ 0, 4, 5 }, { 0, 5, 1 }	/// 뒷면
	};

	m_pDev->MultiplyTransform( D3DTS_WORLD, &m_matTM );
	m_pDev->SetFVF( BOXVERTEX::FVF );
	m_pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 8, 12, idx, D3DFMT_INDEX16, vtx, sizeof BOXVERTEX );

	return 1;
}

