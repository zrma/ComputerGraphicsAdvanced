#ifndef _BSPBASE_H_
#define _BSPBASE_H_
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include "define.h"

#define BSP_EPSILON		Z_EPSILON
#define BSP_BOGUS		99999

struct BSPVERTEX
{
	enum { FVF=D3DFVF_XYZ };
	D3DXVECTOR3	p;
};

class ZClassifyByPlane
{
public:
	enum _LOCATION { FRONT, BACK, ON, SPLIT };
	static _LOCATION WhereIsVertex( D3DXPLANE* pPlane, D3DXVECTOR3* pV )
	{
		float d;
		d = D3DXPlaneDotCoord( pPlane, pV );
		if( d > BSP_EPSILON ) return ZClassifyByPlane::FRONT;
		if( d < -BSP_EPSILON ) return ZClassifyByPlane::BACK ;
		
		return ZClassifyByPlane::ON;
	}

	static _LOCATION WhereIsFace( D3DXPLANE* pPlane, BSPVERTEX* pV, int nVertCount )
	{
		int f = 0, b = 0, o = 0;
		float d;
		for( int i = 0 ; i < nVertCount ; i++ )
		{
			d = D3DXPlaneDotCoord( pPlane, &((pV+i)->p) );
			if( d > BSP_EPSILON ) f++;
			else
			if( d < -BSP_EPSILON ) b++;
			else
			{ f++; b++; o++; }
		}
		
		if( o == nVertCount ) return ZClassifyByPlane::ON;
		if( f == nVertCount ) return ZClassifyByPlane::FRONT;
		if( b == nVertCount ) return ZClassifyByPlane::BACK;
		
		return ZClassifyByPlane::SPLIT;
	}

	static _LOCATION WhereIsFace( D3DXPLANE* pPlane, D3DXVECTOR3* pV, int nVertCount )
	{
		int f = 0, b = 0, o = 0;
		float d;
		for( int i = 0 ; i < nVertCount ; i++ )
		{
			d = D3DXPlaneDotCoord( pPlane, (pV+i) );
			if( d > BSP_EPSILON ) f++;
			else
			if( d < -BSP_EPSILON ) b++;
			else
			{ f++; b++; o++; }
		}
		
		if( f == nVertCount ) return ZClassifyByPlane::FRONT;
		if( b == nVertCount ) return ZClassifyByPlane::BACK;
		if( o == nVertCount ) return ZClassifyByPlane::ON;
		
		return ZClassifyByPlane::SPLIT;
	}

	static _LOCATION WhereIsPolygon( D3DXPLANE* pPlane, D3DXVECTOR3* pV0 )
	{
		return WhereIsFace( pPlane, pV0, 3 );
	}
};


class ZBspFace
{
	ZBspFace*	m_pNext;
	BOOL		m_bUsed;
	BSPVERTEX*	m_pVerts;
	DWORD		m_nVertCount;
	D3DXPLANE	m_plane;
private:
	void		_Destroy();

public:
	ZBspFace();
	~ZBspFace()	{ _Destroy(); }

	BOOL		MakePlane( BSPVERTEX* pV, int n );
	void		AddTail( ZBspFace* p ) { if( m_pNext ) m_pNext->AddTail( p ); else m_pNext = p; }
	void		AddNext( ZBspFace* p ) { m_pNext = p; }
	ZBspFace*	GetNext() { return m_pNext; }
	BSPVERTEX*	GetVerts() { return m_pVerts; }
	DWORD		GetVertCount() { return m_nVertCount; }
	BOOL		GetUsed() { return m_bUsed; }
	void		SetUsed( BOOL b ) { m_bUsed = b; }
	D3DXPLANE*	GetPlane() { return &m_plane; }
};

class ZBspNode
{
public:
	enum	{ NODETYPE_NODE, NODETYPE_LEAF, NODETYPE_SOLIDLEAF };
	enum	{ NODE_FRONT = 0, NODE_BACK = 1 };
private:
	DWORD			m_nNodeType;
	D3DXVECTOR3		m_vMin;
	D3DXVECTOR3		m_vMax;
	D3DXPLANE		m_plane;
	ZBspFace*		m_pFaces;
	ZBspNode*		m_pNode[2];	// NODE_FRONT:front, NODE_BACK:back
private:
	void			_Destroy();
public:
	ZBspNode();
	~ZBspNode()		{ _Destroy(); }

	void			SetFrontNode( ZBspNode* pNode ) { m_pNode[NODE_FRONT] = pNode; }
	void			SetBackNode( ZBspNode* pNode ) { m_pNode[NODE_BACK] = pNode; }
	ZBspNode*		GetFrontNode() { return m_pNode[NODE_FRONT]; }
	ZBspNode*		GetBackNode() { return m_pNode[NODE_BACK]; }
	void			SetNodeType( DWORD n ) { m_nNodeType = n; }
	DWORD			GetNodeType() { return m_nNodeType; }

	BOOL			DetectCollision( D3DXVECTOR3* pV );
	BOOL			IsSolid() { return (m_nNodeType == NODETYPE_SOLIDLEAF); }
	BOOL			IsLeaf() { return (m_nNodeType == NODETYPE_LEAF); }
	D3DXVECTOR3*	GetMin() { return &m_vMin; }
	D3DXVECTOR3*	GetMax() { return &m_vMax; }
	D3DXPLANE*		GetPlane() { return &m_plane; }
	ZBspFace*		GetFaces() { return m_pFaces; }
	void			SetFaces( ZBspFace* p ) { m_pFaces = p; }
//	void			SetFaceList( ZBspFace* pFaces ) { m_pFaces = pFaces; }

	void			Render( LPDIRECT3DDEVICE9 pDev );
};

#endif // _BSPBASE_H_