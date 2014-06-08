#include "ZMesh.h"

ZMesh::ZMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZNode( pDev, pMesh, pSM )
{
	m_isSkinned = pMesh->m_isSkinned ? TRUE : FALSE;
	m_nNodeType = NODE_MESH;

	m_pVB = NULL;
	m_pIB = NULL;
}

ZMesh::~ZMesh()
{
	S_REL( m_pVB );
	S_REL( m_pIB );
}