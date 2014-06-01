#include "ZBspBase.h"
#include "ZSimpleParser.h"

//////////////////////////////////////////////////////////////////////
// ZBspFace
//////////////////////////////////////////////////////////////////////

ZBspFace::ZBspFace()
{
	m_bUsed		= FALSE;
	m_pNext		= NULL;
	m_pVerts	= NULL;
	m_nVertCount= 0;
}

BOOL ZBspFace::MakePlane( BSPVERTEX* pV, int n )
{
	if( n < 3 )
		return FALSE;

	D3DXPlaneFromPoints( &m_plane, (D3DXVECTOR3*)pV, (D3DXVECTOR3*)(pV+1), (D3DXVECTOR3*)(pV+2) );
	m_pVerts = pV;
	m_nVertCount = n;

	return TRUE;
}

void ZBspFace::_Destroy()
{
	DELS( m_pVerts );
	DEL( m_pNext );
}


//////////////////////////////////////////////////////////////////////
// ZBspNode
//////////////////////////////////////////////////////////////////////

ZBspNode::ZBspNode()
{
	m_nNodeType		= ZBspNode::NODETYPE_NODE;
	m_vMax			= D3DXVECTOR3( BSP_BOGUS, BSP_BOGUS, BSP_BOGUS );
	m_vMin			= D3DXVECTOR3( -BSP_BOGUS, -BSP_BOGUS, -BSP_BOGUS );
	m_pNode[NODE_FRONT]		= NULL;
	m_pNode[NODE_BACK]		= NULL;
}

void ZBspNode::Render( LPDIRECT3DDEVICE9 pDev )
{
	if( IsLeaf() )
	{
		for( ZBspFace* p = m_pFaces ; p ; p = p->GetNext() )
		{
			pDev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, p->GetVerts(), sizeof( BSPVERTEX ) );
		}
	}
	if( m_pNode[NODE_BACK] )
		m_pNode[NODE_BACK]->Render( pDev );
	if( m_pNode[NODE_FRONT] )
		m_pNode[NODE_FRONT]->Render( pDev );
}

void ZBspNode::_Destroy()
{
	DEL( m_pFaces );
	DEL( m_pNode[NODE_FRONT] );
	DEL( m_pNode[NODE_BACK] );
}

BOOL ZBspNode::DetectCollision( D3DXVECTOR3* pV )
{
	if( IsSolid() )
	{
		return TRUE;
	}

	ZClassifyByPlane::_LOCATION	l;

	l = ZClassifyByPlane::WhereIsVertex( &m_plane, pV );

	if( l == ZClassifyByPlane::FRONT )
	{
		if( m_pNode[NODE_FRONT] ) return m_pNode[NODE_FRONT]->DetectCollision( pV );
		else return FALSE;
	}

	if( l == ZClassifyByPlane::BACK )
	{
		if( m_pNode[NODE_FRONT] ) return m_pNode[NODE_BACK]->DetectCollision( pV );
		else return FALSE;
	}

	return FALSE;
}

