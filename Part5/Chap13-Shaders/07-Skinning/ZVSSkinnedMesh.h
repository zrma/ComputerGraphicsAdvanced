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
 * D3D의 Fixed Function 스키닝 메시 클래스
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