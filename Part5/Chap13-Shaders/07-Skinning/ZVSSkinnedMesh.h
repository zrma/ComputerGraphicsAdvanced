#ifndef _ZVSSKINNEDMESH_H_
#define _ZVSSKINNEDMESH_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZNode.h"
#include "ZTrack.h"
#include "ZMesh.h"
#include "ZSkinnedMesh.h"
#include "ZShaderMgr.h"

/**
 * D3D�� Fixed Function ��Ű�� �޽� Ŭ����
 *
 */
class ZVSSkinnedMesh : public ZSkinnedMesh
{
protected:
	int		_CreateVIB( ZCMesh* pMesh );
	void	_ApplyPalette( D3DXMATRIX* pTM );
public:
	ZVSSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZVSSkinnedMesh();

	virtual int	Draw( D3DXMATRIX* pTM );
};


#endif // _ZVSSKINNEDMESH_H_