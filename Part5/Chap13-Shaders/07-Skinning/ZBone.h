#ifndef _ZBONE_H_
#define _ZBONE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZNode.h"
#include "ZTrack.h"
#include "ZShaderMgr.h"

/// single mesh container
class ZBone : public ZNode
{
protected:
	D3DXMATRIXA16			m_matSkin;		/// Skin TM 행렬
public:
	ZBone( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZBone();

	D3DXMATRIXA16* GetMatrixSkin() { return &m_matSkin; }
	D3DXMATRIXA16* Animate( float frame, D3DXMATRIXA16* pParent );

	/// 메시를 그린다
	virtual int	Draw( D3DXMATRIX* pTM );
};

#endif // _ZBONE_H_