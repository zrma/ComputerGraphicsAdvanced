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
	VOID*		pV;		// �������� lock������ ������ ��
	VOID*		pI;		// �ε������� lock������ ������ ��
	set<int>	idxBones;

	m_dwFVF = ZSkinnedVertex::FVF;
	m_nVerts = pMesh->m_vtxFinal.size();;
	m_bboxMax = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );
	m_bboxMin = D3DXVECTOR3( pMesh->m_vtxFinal[0].p.x, pMesh->m_vtxFinal[0].p.y, pMesh->m_vtxFinal[0].p.z );

	for( i = 0 ;  i < m_nVerts ; i++ )
	{
		// �ε��� ���� set�� �����صд�
		for( j = 0 ; j < 4 ; j++ )
		{
			idx = ( ( pMesh->m_vtxFinal[i].i >> (j*8) ) & 0x000000ff );
			idxBones.insert( idx );
		}

		// �ƽ���ũ��Ʈ�� ������� �����ڴ� �̻��� ��찡 ���Ƽ� ���� ����Ѵ�
		if( pMesh->m_vtxFinal[i].p.x > m_bboxMax.x ) m_bboxMax.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y > m_bboxMax.y ) m_bboxMax.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z > m_bboxMax.z ) m_bboxMax.z = pMesh->m_vtxFinal[i].p.z;
		if( pMesh->m_vtxFinal[i].p.x < m_bboxMin.x ) m_bboxMin.x = pMesh->m_vtxFinal[i].p.x;
		if( pMesh->m_vtxFinal[i].p.y < m_bboxMin.y ) m_bboxMin.y = pMesh->m_vtxFinal[i].p.y;
		if( pMesh->m_vtxFinal[i].p.z < m_bboxMin.z ) m_bboxMin.z = pMesh->m_vtxFinal[i].p.z;
	}

	// ���� ��ġ�� �ȵǹǷ� STL�� set�����̳ʸ� ����Ѵ�
	for( set<int>::iterator it = idxBones.begin() ; it != idxBones.end() ; it++ )
	{
		m_idxBones.push_back( *it );
	}

	idxBones.clear();

	// ���� ���� ����
	m_pDev->CreateVertexBuffer( m_nVerts * sizeof(ZSkinnedVertex), 0, m_dwFVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	m_pVB->Lock( 0, m_nVerts * sizeof(ZSkinnedVertex), (void**)&pV, 0 );
	memcpy( pV, &pMesh->m_vtxFinal[0], m_nVerts * sizeof(ZSkinnedVertex) );
	m_pVB->Unlock();

	D3DCAPS9 caps;
	m_pDev->GetDeviceCaps( &caps );
	m_nTriangles = pMesh->m_idxFinal.size();

	// �ε����� 32��Ʈ �ε����� �����ϸ� 32��Ʈ �ε��� ���� ����
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
	else // �ƴ϶�� 16��Ʈ �ε����� �ε��� ���� ����
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
		m = m * *pTM;	// NodeManager�� TM���
		m_pDev->SetTransform( D3DTS_WORLDMATRIX(idx), &m );
	}
}

int ZFFSkinnedMesh::Draw( D3DXMATRIX* pTM )
{
	_ApplyPalette( pTM );

	m_pDev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );
	// blend weight�� 4��(��Ÿ �ƴ�! 4��!)
	m_pDev->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );

	m_pDev->SetStreamSource( 0, m_pVB, 0, sizeof(ZSkinnedVertex) );
	m_pDev->SetFVF( m_dwFVF );
	m_pDev->SetIndices( m_pIB );
	m_pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nVerts, 0, m_nTriangles );
    m_pDev->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
    m_pDev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	return 1;
}

