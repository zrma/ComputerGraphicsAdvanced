#include "ZRigidMesh.h"

ZRigidMesh::ZRigidMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZMesh( pDev, pMesh, pSM )
{
	_CreateVIB( pMesh );
}

ZRigidMesh::~ZRigidMesh()
{
}

int ZRigidMesh::_CreateVIB( ZCMesh* pMesh )
{
	int		i;
	VOID*	pV;		// 정점버퍼 lock했을때 얻어오는 값
	VOID*	pI;		// 인덱스버퍼 lock했을때 얻어오는 값
	vector<ZRigidVertex>	vtx;

	m_nVerts = pMesh->m_vtxFinal.size();;
	m_dwFVF = ZRigidVertex::FVF;
	m_bboxMax = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	m_bboxMin = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	vtx.resize( m_nVerts );

	for( i = 0 ;  i < m_nVerts ; i++ )
	{
		vtx[i].p.x = pMesh->m_vtxFinal[i].p.x;
		vtx[i].p.y = pMesh->m_vtxFinal[i].p.y;
		vtx[i].p.z = pMesh->m_vtxFinal[i].p.z;
		vtx[i].n.x = pMesh->m_vtxFinal[i].n.x;
		vtx[i].n.y = pMesh->m_vtxFinal[i].n.y;
		vtx[i].n.z = pMesh->m_vtxFinal[i].n.z;
		vtx[i].t.x = pMesh->m_vtxFinal[i].t.x;
		vtx[i].t.y = pMesh->m_vtxFinal[i].t.y;

		// 맥스스크립트로 얻어지는 경계상자는 이상할 경우가 많아서 직접 계산한다
		if( pMesh->m_vtxFinal[i].p.x > m_bboxMax.x ) m_bboxMax.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y > m_bboxMax.y ) m_bboxMax.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z > m_bboxMax.z ) m_bboxMax.z = pMesh->m_vtxFinal[i].p.z;
		if( pMesh->m_vtxFinal[i].p.x < m_bboxMin.x ) m_bboxMin.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y < m_bboxMin.y ) m_bboxMin.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z < m_bboxMin.z ) m_bboxMin.z = pMesh->m_vtxFinal[i].p.z;
	}
	// 정점 버퍼 생성
	m_pDev->CreateVertexBuffer( m_nVerts * sizeof(ZRigidVertex), 0, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	m_pVB->Lock( 0, m_nVerts * sizeof(ZRigidVertex), (void**)&pV, 0 );
	memcpy( pV, &vtx[0], m_nVerts * sizeof(ZRigidVertex) );
	m_pVB->Unlock();
	stl_wipe_vector( vtx );

	// 인덱스 생성
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

int ZRigidMesh::Draw( D3DXMATRIX* pTM )
{
	m_pDev->MultiplyTransform( D3DTS_WORLD, &m_matTM );
	m_pDev->SetStreamSource( 0, m_pVB, 0, sizeof(ZRigidVertex) );
	m_pDev->SetFVF( m_dwFVF );
	m_pDev->SetIndices( m_pIB );
	m_pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nVerts, 0, m_nTriangles );
	return 1;
}

