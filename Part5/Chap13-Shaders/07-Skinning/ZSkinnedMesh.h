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
 * ��Ű�� �޽� Ŭ������ BASEŬ����
 *
 */
class ZSkinnedMesh : public ZMesh
{
protected:
	vector<int>				m_idxBones;	/// �޽ÿ� ������ ��ġ�� ������ �ε���
	vector<D3DXMATRIX>*		m_pMatPalette;

public:
	ZSkinnedMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZSkinnedMesh();
	
	// ���ʿ� �ѹ� �Ҹ���.
	void SetMatrixPalette( vector<D3DXMATRIX>* pPal ) { m_pMatPalette = pPal; }

	vector<int>& GetBones() { return m_idxBones; }
	virtual int	Draw( D3DXMATRIX* pTM ) { return 1; }
};


#endif // _ZSKINNEDMESH_H_