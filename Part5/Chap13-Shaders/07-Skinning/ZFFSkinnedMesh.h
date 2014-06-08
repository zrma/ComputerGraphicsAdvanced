#ifndef _ZFFSKINNEDMESH_H_
#define _ZFFSKINNEDMESH_H_

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
class ZFFSkinnedMesh : public ZSkinnedMesh
{
protected:
	int		_CreateVIB( ZCMesh* pMesh );
	void	_ApplyPalette( D3DXMATRIX* pTM );
public:
	ZFFSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZFFSkinnedMesh();

	virtual int	Draw( D3DXMATRIX* pTM );
};


#endif // _ZFFSKINNEDMESH_H_