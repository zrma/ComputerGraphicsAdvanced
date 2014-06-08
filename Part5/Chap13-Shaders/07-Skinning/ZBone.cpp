#include "ZBone.h"

ZBone::ZBone( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZNode( pDev, pMesh, pSM )
{
	m_nNodeType = NODE_BONE;
	D3DXMatrixIdentity( &m_matSkin );
}

ZBone::~ZBone()
{
}

D3DXMATRIXA16*  ZBone::Animate( float frame, D3DXMATRIXA16* pParent )
{
	D3DXMATRIXA16*	pmatAni;
	D3DXMATRIXA16	mWI;		// world inverse

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

	m_matTM = m_matTM * *pParent;

	// mWI = inverse(m_matWorld)
	D3DXMatrixInverse( &mWI, NULL, &m_matWorld );
	// skin = inverse(M_world) * M_ani * M_parent
	m_matSkin = mWI * m_matTM;
//	D3DXMatrixIdentity( &m_matSkin );
	return &m_matSkin;
}

int	ZBone::Draw( D3DXMATRIX* pTM )
{

	BOXVERTEX vtx[8];
	vtx[0] = BOXVERTEX( -1,  1,  1 , 0xffff0000 );		/// v0
	vtx[1] = BOXVERTEX(  1,  1,  1 , 0xffff0000 );		/// v1
	vtx[2] = BOXVERTEX(  1,  1, -1 , 0xffff0000 );		/// v2
	vtx[3] = BOXVERTEX( -1,  1, -1 , 0xffff0000 );		/// v3
	vtx[4] = BOXVERTEX( -1, -1,  1 , 0xffff0000 );		/// v4
	vtx[5] = BOXVERTEX(  1, -1,  1 , 0xffff0000 );		/// v5
	vtx[6] = BOXVERTEX(  1, -1, -1 , 0xffff0000 );		/// v6
	vtx[7] = BOXVERTEX( -1, -1, -1 , 0xffff0000 );		/// v7

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

//	ZNode::DrawBBox();
	return 1;
}