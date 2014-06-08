#ifndef _ZSKINNEDMESH_H_
#define _ZSKINNEDMESH_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZNode.h"
#include "ZTrack.h"
#include "ZMesh.h"
#include "ZShaderMgr.h"

/**
 * 스키닝 메시 클래스의 BASE클래스
 *
 */
class ZSkinnedMesh : public ZMesh
{
protected:
	vector<int>				m_idxBones;	/// 메시에 영향을 미치는 뼈대의 인덱스
	vector<D3DXMATRIX>*		m_pMatPalette;

public:
	ZSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZSkinnedMesh();
	
	// 최초에 한번 불린다.
	void SetMatrixPalette( vector<D3DXMATRIX>* pPal ) { m_pMatPalette = pPal; }

	vector<int>& GetBones() { return m_idxBones; }
	virtual int	Draw( D3DXMATRIX* pTM ) { return 1; }
};


#endif // _ZSKINNEDMESH_H_