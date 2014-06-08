#ifndef _ZSWSkinnedMesh_H_
#define _ZSWSkinnedMesh_H_

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
 * 직접구현한 SW 스키닝 메시 클래스
 *
 */
class ZSWSkinnedMesh : public ZSkinnedMesh
{
protected:
	/// 소프트웨어 스키닝을 위해서 정점버퍼와 똑같은 정점배열을 한 카피 보관해 둔다.
	ZSWSkinnedVertex*		m_pvtx;		/// skinning 정점 배열
	ZRigidVertex*			m_pvtxDraw;	/// rigid 정점 배열
	// 소프트웨어 스키닝을 위해서 인덱스버퍼와 똑같은 인덱스배열을 한 카피 보관해 둔다.
	void*					m_pidx;		/// index는 16비트, 32비트 2가지를 지원해야
										/// 하므로 void형 포인터를 사용했다
	D3DFORMAT				m_fmtIdx;	/// D3DFMT_INDEX16 or D3DFMT_INDEX32
	
	int _CreateVIB( ZCMesh* pMesh );
	void _ApplyPalette( D3DXMATRIX* pTM );

public:
	ZSWSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZSWSkinnedMesh();

	virtual int	Draw( D3DXMATRIX* pTM );
};


#endif // _ZSWSkinnedMesh_H_