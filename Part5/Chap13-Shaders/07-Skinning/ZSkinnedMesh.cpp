#include "ZSkinnedMesh.h"

ZSkinnedMesh::ZSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM ) : ZMesh( pDev, pMesh, pSM )
{
	m_pMatPalette = NULL;
}

ZSkinnedMesh::~ZSkinnedMesh()
{
	m_pMatPalette = NULL;
	stl_wipe_vector( m_idxBones );
}
