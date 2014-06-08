#include "ZSWSkinnedMesh.h"

ZSWSkinnedMesh::ZSWSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZSkinnedMesh( pDev, pMesh, pSM )
{
	_CreateVIB( pMesh );
}

ZSWSkinnedMesh::~ZSWSkinnedMesh()
{
	S_DELS( m_pvtx );
	S_DELS( m_pvtxDraw );
	S_DELS( m_pidx );
}

int ZSWSkinnedMesh::_CreateVIB( ZCMesh* pMesh )
{
	int		i, j;
	set<int>	idxBones;

	m_nVerts = pMesh->m_vtxFinal.size();
	m_dwFVF = ZRigidVertex::FVF;
	m_pvtx = new ZSWSkinnedVertex[m_nVerts];
	m_pvtxDraw = new ZRigidVertex[m_nVerts];

	m_bboxMax = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	m_bboxMin = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );

	for( i = 0 ;  i < m_nVerts ; i++ )
	{
		m_pvtx[i].p.x = pMesh->m_vtxFinal[i].p.x;
		m_pvtx[i].p.y = pMesh->m_vtxFinal[i].p.y;
		m_pvtx[i].p.z = pMesh->m_vtxFinal[i].p.z;
		m_pvtx[i].b[0] = pMesh->m_vtxFinal[i].b[0];
		m_pvtx[i].b[1] = pMesh->m_vtxFinal[i].b[1];
		m_pvtx[i].b[2] = pMesh->m_vtxFinal[i].b[2];
		m_pvtx[i].b[3] = 1.0f - ( m_pvtx[i].b[0] + m_pvtx[i].b[1] + m_pvtx[i].b[2] );
		
		for( j = 0 ; j < 4 ; j++ )
		{
			m_pvtx[i].i[j] = ((pMesh->m_vtxFinal[i].i>>(j*8)) & 0xff);
			idxBones.insert( m_pvtx[i].i[j] );
		}

		m_pvtxDraw[i].n.x = pMesh->m_vtxFinal[i].n.x;
		m_pvtxDraw[i].n.y = pMesh->m_vtxFinal[i].n.y;
		m_pvtxDraw[i].n.z = pMesh->m_vtxFinal[i].n.z;
		m_pvtxDraw[i].t.x = pMesh->m_vtxFinal[i].t.x;
		m_pvtxDraw[i].t.y = pMesh->m_vtxFinal[i].t.y;

		// 맥스스크립트로 얻어지는 경계상자는 이상할 경우가 많아서 직접 계산한다
		if( m_pvtx[i].p.x > m_bboxMax.x ) m_bboxMax.x = m_pvtx[i].p.x;
		if( m_pvtx[i].p.y > m_bboxMax.y ) m_bboxMax.y = m_pvtx[i].p.y;
		if( m_pvtx[i].p.z > m_bboxMax.z ) m_bboxMax.z = m_pvtx[i].p.z;
		if( m_pvtx[i].p.x < m_bboxMin.x ) m_bboxMin.x = m_pvtx[i].p.x;
		if( m_pvtx[i].p.y < m_bboxMin.y ) m_bboxMin.y = m_pvtx[i].p.y;
		if( m_pvtx[i].p.z < m_bboxMin.z ) m_bboxMin.z = m_pvtx[i].p.z;
	}

	// 값이 겹치면 안되므로 STL의 set컨테이너를 사용한다
	for( set<int>::iterator it = idxBones.begin() ; it != idxBones.end() ; it++ )
	{
		m_idxBones.push_back( *it );
	}

	idxBones.clear();

	// 인덱스 생성
	D3DCAPS9 caps;
	m_pDev->GetDeviceCaps( &caps );
	m_nTriangles = pMesh->m_idxFinal.size();

	// 인덱스가 32비트 인덱스를 지원하면 32비트 인덱스 버퍼 생성
	if( caps.MaxVertexIndex > 0x0000ffff )
	{
		m_pidx = (void*)(new Index3i[m_nTriangles]);
		DWORD* pW = (DWORD*)m_pidx;
		m_fmtIdx = D3DFMT_INDEX32;
		for( i = 0 ; i < m_nTriangles ; i++ )
		{
			*(pW+0) = pMesh->m_idxFinal[i].i[0];
			*(pW+1) = pMesh->m_idxFinal[i].i[1];
			*(pW+2) = pMesh->m_idxFinal[i].i[2];
			pW += 3;
		}
	}
	else // 아니라면 16비트 인덱스로 인덱스 버퍼 생성
	{
		m_pidx = (void*)(new Index3w[m_nTriangles]);
		WORD* pW = (WORD*)m_pidx;
		m_fmtIdx = D3DFMT_INDEX16;
		for( i = 0 ; i < m_nTriangles ; i++ )
		{
			*(pW+0) = (WORD)pMesh->m_idxFinal[i].i[0];
			*(pW+1) = (WORD)pMesh->m_idxFinal[i].i[1];
			*(pW+2) = (WORD)pMesh->m_idxFinal[i].i[2];
			pW += 3;
		}
	}

	return 1;
}

void ZSWSkinnedMesh::_ApplyPalette( D3DXMATRIX* pTM )
{
	int				i, j;
	DWORD			idx;
	D3DXMATRIXA16	m;
	D3DXVECTOR3		v;

	for( i = 0 ; i < m_nVerts ; i++ )
	{
		// 정점을 0으로 초기화
		m_pvtxDraw[i].p = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		for( j = 0 ; j < 4 ; j++ )
		{
			idx = m_pvtx[i].i[j];	// 뼈대의 인덱스
			m = m_matTM * ((*m_pMatPalette)[idx]);	// TM_bone행렬
			D3DXVec3TransformCoord( &v, &m_pvtx[i].p, &m );	// V_world(n) = V_local(n) * TM_bone(idx)
			m_pvtxDraw[i].p += ( v * m_pvtx[i].b[j] ); // V_world = V_world(0)+ ... +V_world(3)
		}
	}
}

int ZSWSkinnedMesh::Draw( D3DXMATRIX* pTM )
{
	_ApplyPalette( pTM );
	m_pDev->SetFVF( m_dwFVF );
	m_pDev->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, m_nVerts, m_nTriangles, m_pidx, m_fmtIdx, m_pvtxDraw, sizeof(ZRigidVertex) );
	return 1;
}

