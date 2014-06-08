#include "ZFFSkinnedMesh.h"

ZFFSkinnedMesh::ZFFSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZSkinnedMesh( pDev, pMesh, pSM )
{
	_CreateVIB( pMesh );
}

ZFFSkinnedMesh::~ZFFSkinnedMesh()
{
}

int ZFFSkinnedMesh::_CreateVIB( ZCMesh* pMesh )
{
	int		i	, j;
	int			idx;
	VOID*		pV;		// 정점버퍼 lock했을때 얻어오는 값
	VOID*		pI;		// 인덱스버퍼 lock했을때 얻어오는 값
	set<int>	idxBones;

	m_dwFVF = ZSkinnedVertex::FVF;
	m_nVerts = pMesh->m_vtxFinal.size();;
	m_bboxMax = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	m_bboxMin = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );

	for( i = 0 ;  i < m_nVerts ; i++ )
	{
		// 인덱스 값을 set에 저장해둔다
		for( j = 0 ; j < 4 ; j++ )
		{
			idx = ( ( pMesh->m_vtxFinal[i].i >> (j*8) ) & 0x000000ff );
			idxBones.insert( idx );
		}

		// 맥스스크립트로 얻어지는 경계상자는 이상할 경우가 많아서 직접 계산한다
		if( pMesh->m_vtxFinal[i].p.x > m_bboxMax.x ) m_bboxMax.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y > m_bboxMax.y ) m_bboxMax.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z > m_bboxMax.z ) m_bboxMax.z = pMesh->m_vtxFinal[i].p.z;
		if( pMesh->m_vtxFinal[i].p.x < m_bboxMin.x ) m_bboxMin.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y < m_bboxMin.y ) m_bboxMin.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z < m_bboxMin.z ) m_bboxMin.z = pMesh->m_vtxFinal[i].p.z;
	}

	// 값이 겹치면 안되므로 STL의 set컨테이너를 사용한다
	for( set<int>::iterator it = idxBones.begin() ; it != idxBones.end() ; it++ )
	{
		m_idxBones.push_back( *it );
	}

	idxBones.clear();

	// 정점 버퍼 생성
	m_pDev->CreateVertexBuffer( m_nVerts * sizeof(ZSkinnedVertex), 0, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	m_pVB->Lock( 0, m_nVerts * sizeof(ZSkinnedVertex), (void**)&pV, 0 );
	memcpy( pV, &pMesh->m_vtxFinal[0], m_nVerts * sizeof(ZSkinnedVertex) );
	m_pVB->Unlock();

	D3DCAPS9 caps;
	m_pDev->GetDeviceCaps( &caps );
	m_nTriangles = pMesh->m_idxFinal.size();

	// 인덱스가 32비트 인덱스를 지원하면 32비트 인덱스 버퍼 생성
	if( caps.MaxVertexIndex > 0x0000ffff )
	{
		m_pDev->CreateIndexBuffer( m_nTriangles * sizeof(Index3i), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL );
		m_pIB->Lock( 0, m_nTriangles * sizeof(Index3i), (void**)&pI, 0 );
		DWORD* pW = (DWORD*)pI;
		for( i = 0 ; i < m_nTriangles ; i++ )
		{
			*(pW+0) = pMesh->m_idxFinal[i].i[0];
			*(pW+1) = pMesh->m_idxFinal[i].i[1];
			*(pW+2) = pMesh->m_idxFinal[i].i[2];
			pW += 3;
		}
		m_pIB->Unlock();
	}
	else // 아니라면 16비트 인덱스로 인덱스 버퍼 생성
	{
		m_pDev->CreateIndexBuffer( m_nTriangles * sizeof(Index3w), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL );
		m_pIB->Lock( 0, m_nTriangles * sizeof(Index3w), (void**)&pI, 0 );
		WORD* pW = (WORD*)pI;
		for( i = 0 ; i < m_nTriangles ; i++ )
		{
			*(pW+0) = (WORD)pMesh->m_idxFinal[i].i[0];
			*(pW+1) = (WORD)pMesh->m_idxFinal[i].i[1];
			*(pW+2) = (WORD)pMesh->m_idxFinal[i].i[2];
			pW += 3;
		}
		m_pIB->Unlock();
	}

	return 1;
}

void ZFFSkinnedMesh::_ApplyPalette( D3DXMATRIX* pTM )
{
	D3DXMATRIX	m;
	int			size = m_idxBones.size();
	int			idx;

	for( int i = 0 ; i < size ; i++ )
	{
		idx = m_idxBones[i];
		m = m_matTM * ((*m_pMatPalette)[idx]);
		m = m * *pTM;	// NodeManager의 TM행렬
		m_pDev->SetTransform( D3DTS_WORLDMATRIX(idx), &m );
	}
}

int ZFFSkinnedMesh::Draw( D3DXMATRIX* pTM )
{
	_ApplyPalette( pTM );

	m_pDev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );
	// blend weight는 4개(오타 아님! 4개!)
	m_pDev->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );

	m_pDev->SetStreamSource( 0, m_pVB, 0, sizeof(ZSkinnedVertex) );
	m_pDev->SetFVF( m_dwFVF );
	m_pDev->SetIndices( m_pIB );
	m_pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nVerts, 0, m_nTriangles );
    m_pDev->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
    m_pDev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	return 1;
}

