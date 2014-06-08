#include "ZVSSkinnedMesh.h"

ZVSSkinnedMesh::ZVSSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZSkinnedMesh( pDev, pMesh, pSM )
{
	_CreateVIB( pMesh );
}

ZVSSkinnedMesh::~ZVSSkinnedMesh()
{
}

int ZVSSkinnedMesh::_CreateVIB( ZCMesh* pMesh )
{
	int					i, j;
	float				b[4];
	int					bi[4];
	ZVSSkinnedVertex*	pV;		// 정점버퍼 lock했을때 얻어오는 값
	VOID*				pI;		// 인덱스버퍼 lock했을때 얻어오는 값
	set<int>			idxBones;

	m_dwFVF = ZVSSkinnedVertex::FVF;
	m_nVerts = pMesh->m_vtxFinal.size();;
	m_bboxMax = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	m_bboxMin = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );

	m_pDev->CreateVertexBuffer( m_nVerts * sizeof(ZVSSkinnedVertex), 0, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	m_pVB->Lock( 0, m_nVerts * sizeof(ZVSSkinnedVertex), (void**)&pV, 0 );
	for( i = 0 ;  i < m_nVerts ; i++ )
	{
		// 인덱스 값을 set에 저장해둔다
		for( j = 0 ; j < 4 ; j++ )
		{
			bi[j] = ((pMesh->m_vtxFinal[i].i>>(j*8)) & 0xff);
			idxBones.insert( bi[j] );
		}
		b[0] = pMesh->m_vtxFinal[i].b[0];
		b[1] = pMesh->m_vtxFinal[i].b[1];
		b[2] = pMesh->m_vtxFinal[i].b[2];
		b[3] = 1.0f - ( b[0] + b[1] + b[2] );

		(pV+i)->p = D3DXVECTOR3( pMesh->m_vtxFinal[i].p.x, pMesh->m_vtxFinal[i].p.y, pMesh->m_vtxFinal[i].p.z );
		(pV+i)->n = D3DXVECTOR3( pMesh->m_vtxFinal[i].n.x, pMesh->m_vtxFinal[i].n.y, pMesh->m_vtxFinal[i].n.z );
		(pV+i)->t = D3DXVECTOR2( pMesh->m_vtxFinal[i].t.x, pMesh->m_vtxFinal[i].t.y );
		(pV+i)->b = D3DXVECTOR4( b[3], b[2], b[1], b[0] );
		(pV+i)->i = D3DXVECTOR4( bi[3], bi[2], bi[1], bi[0] );

		// 맥스스크립트로 얻어지는 경계상자는 이상할 경우가 많아서 직접 계산한다
		if( pMesh->m_vtxFinal[i].p.x > m_bboxMax.x ) m_bboxMax.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y > m_bboxMax.y ) m_bboxMax.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z > m_bboxMax.z ) m_bboxMax.z = pMesh->m_vtxFinal[i].p.z;
		if( pMesh->m_vtxFinal[i].p.x < m_bboxMin.x ) m_bboxMin.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y < m_bboxMin.y ) m_bboxMin.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z < m_bboxMin.z ) m_bboxMin.z = pMesh->m_vtxFinal[i].p.z;
	}
	m_pVB->Unlock();

	// 값이 겹치면 안되므로 STL의 set컨테이너를 사용한다
	for( set<int>::iterator it = idxBones.begin() ; it != idxBones.end() ; it++ )
	{
		m_idxBones.push_back( *it );
	}

	idxBones.clear();

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

void ZVSSkinnedMesh::_ApplyPalette( D3DXMATRIX* pTM )
{
	D3DXMATRIX	m;
	int			size = m_idxBones.size();
	int			i;

	size = (*m_pMatPalette).size();
	if( size > 50 ) size = 50;	// 50개의 뼈대만 지원함
	for( i = 0 ; i < size ; i++ )
	{
		m = m_matTM * ((*m_pMatPalette)[i]);
		m = m * *pTM;	// NodeManager의 TM행렬
		D3DXMatrixTranspose( &m, &m );
		m_pDev->SetVertexShaderConstantF( i*4, (float*)&m, 4 );
	}
}

int ZVSSkinnedMesh::Draw( D3DXMATRIX* pTM )
{
	ZVSTable* p = m_pShaderMgr->GetVSMgr()->Get( FNAME_VS_SKIN );
	m_pDev->SetVertexDeclaration( p->pDecl );
	m_pDev->SetVertexShader( p->pShader );

	_ApplyPalette( pTM );

	m_pDev->SetStreamSource( 0, m_pVB, 0, sizeof(ZVSSkinnedVertex) );
//	m_pDev->SetFVF( m_dwFVF );
	m_pDev->SetIndices( m_pIB );
	m_pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nVerts, 0, m_nTriangles );
	m_pDev->SetVertexDeclaration( NULL );
	m_pDev->SetVertexShader( NULL );
	return 1;
}

