#ifndef _ZBSPTREE_H_
#define _ZBSPTREE_H_
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include "define.h"
#include "ZBspBase.h"

class ZBspTree
{
	DWORD		m_nVertCount;
	DWORD		m_nFaceCount;
	BSPVERTEX*	m_pVerts;
	TRIINDEX*	m_pIndices;
	ZBspFace*	m_pRootFace;
	ZBspNode*	m_pRootNode;

private:
	BOOL		_CompareNormal( D3DXVECTOR3* pvA0, D3DXVECTOR3* pvA1, D3DXVECTOR3* pvA2, 
								D3DXVECTOR3* pvB0, D3DXVECTOR3* pvB1, D3DXVECTOR3* pvB2 );
	BOOL		_IntersectLine( D3DXVECTOR3* pvOut, D3DXPLANE* pPlane, D3DXVECTOR3* pvA, D3DXVECTOR3* pvB );
	void		_Split( D3DXPLANE* pPlane, ZBspFace* pFaces, ZBspFace** a , ZBspFace** b );
	void		_MakeLeaf( ZBspNode* pLeaf );
	int			_GetFaceNodeCount( ZBspFace* pFace );
	void		_GetSortedFaceList( D3DXPLANE* pPlane, ZBspFace* pFaces, ZBspFace** fr , ZBspFace** bk );
	ZBspFace*	_FindBestSplitter( ZBspFace* pList );
	void		_SubDivide( ZBspNode* p );
	BOOL		_CreateFaceList();
	void		_Destroy();

public:
	ZBspTree();
	~ZBspTree()	{ _Destroy(); }
	BOOL		Build( LPSTR szFileName );
	void		Render( LPDIRECT3DDEVICE9 pDev );
	BOOL		DetectCollision( D3DXVECTOR3* pV );
};

#endif // _ZBSPTREE_H_