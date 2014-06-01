#include "ZBspTree.h"
#include "ZSimpleParser.h"

//////////////////////////////////////////////////////////////////////
// ZBspTree
//////////////////////////////////////////////////////////////////////

ZBspTree::ZBspTree()
{
	m_nVertCount	= 0;
	m_nFaceCount	= 0;
	m_pVerts		= NULL;
	m_pIndices		= NULL;
	m_pRootFace		= NULL;
	m_pRootNode		= NULL;
}

BOOL ZBspTree::Build( LPSTR szFileName )
{
	BSPMAPDATA		data;
	ZSimpleParser*	parser;
	parser = new ZSimpleParser;
	if( !parser->Parse( szFileName, &data ) ) return FALSE;
	DEL( parser );

	m_nFaceCount = data.nFaceCount;
	m_nVertCount = data.nVertCount;
	m_pVerts = data.pVerts;
	m_pIndices = data.pIndices;

	_CreateFaceList();

	m_pRootNode = new ZBspNode();
	m_pRootNode->SetFaces( m_pRootFace );
	_SubDivide( m_pRootNode );

	return TRUE;
}

void ZBspTree::Render( LPDIRECT3DDEVICE9 pDev )
{
	pDev->SetFVF( BSPVERTEX::FVF );
//	pDev->SetStreamSource( 0, m_pVB, 0, sizeof(BSPVERTEX) );
	pDev->SetIndices( NULL );

//	pDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nVertexCount,
//				m_nFaceCount, m_pTriangleIndex, D3DFMT_INDEX16, m_pPos, sizeof( BSPVERTEX )  );
	
	m_pRootNode->Render( pDev );
}

BOOL ZBspTree::DetectCollision( D3DXVECTOR3* pV )
{
	return m_pRootNode->DetectCollision( pV );
}

BOOL ZBspTree::_CompareNormal( D3DXVECTOR3* pvA0, D3DXVECTOR3* pvA1, D3DXVECTOR3* pvA2, 
							   D3DXVECTOR3* pvB0, D3DXVECTOR3* pvB1, D3DXVECTOR3* pvB2 )
{
	D3DXVECTOR3	v10, v20;
	D3DXVECTOR3 vNormalA, vNormalB;

	v10 = *pvA1 - *pvA0;
	v20 = *pvA2 - *pvA0;
	D3DXVec3Cross( &vNormalA, &v10, &v20 );
	D3DXVec3Normalize( &vNormalA, &vNormalA );
	
	v10 = *pvB1 - *pvB0;
	v20 = *pvB2 - *pvB0;
	D3DXVec3Cross( &vNormalB, &v10, &v20 );
	D3DXVec3Normalize( &vNormalB, &vNormalB );

	if( vNormalA.x == vNormalB.x &&
		vNormalA.y == vNormalB.y &&
		vNormalA.z == vNormalB.z )
		return TRUE;

//	if( IS_IN_RANGE( vNormalA.x - vNormalB.x, -BSP_EPSILON, BSP_EPSILON ) &&
//		IS_IN_RANGE( vNormalA.y - vNormalB.y, -BSP_EPSILON, BSP_EPSILON ) &&
//		IS_IN_RANGE( vNormalA.z - vNormalB.z, -BSP_EPSILON, BSP_EPSILON ) )
//		return TRUE;

	return FALSE;
}

BOOL ZBspTree::_IntersectLine( D3DXVECTOR3* pvOut, D3DXPLANE* pPlane, D3DXVECTOR3* pvA, D3DXVECTOR3* pvB )
{
	int res;
	D3DXVECTOR3 n( pPlane->a, pPlane->b, pPlane->c );
	D3DXVECTOR3 t = *pvB - *pvA;
	float lineLength = D3DXVec3Dot( &t, &n );

	res = ZClassifyByPlane::WhereIsVertex( pPlane, pvA );
	
	if( res == ZClassifyByPlane::ON )
		return FALSE;

	res = ZClassifyByPlane::WhereIsVertex( pPlane, pvB );
	if( res == ZClassifyByPlane::ON  )
		return FALSE;

	if( fabsf( lineLength ) < 0.001 )
		return FALSE;

	float aDot = D3DXVec3Dot( pvA, &n );
	float bDot = D3DXVec3Dot( pvB, &n );
	float scale = ( -(pPlane->d) - aDot ) / ( bDot - aDot );
	
	if( scale < 0.0f )
		return FALSE;
	if( scale > 1.0f )
		return FALSE;

	*pvOut = *pvA + ( scale * ( *pvB - *pvA ) );
	return TRUE;
}

void ZBspTree::_Split( D3DXPLANE* pPlane, ZBspFace* pFaces, ZBspFace** a , ZBspFace** b )
{
	ZBspFace* frontList = NULL, * backList = NULL;
	BSPVERTEX vFrontList[20], vBackList[20], vFirst;
	BSPVERTEX vIntersectPoint, vPointA, vPointB;
	WORD wFrontCnt = 0, wBackCnt = 0, wCnt = 0, wCurrentVec = 0;

	vFirst = *(pFaces->GetVerts());

	switch( ZClassifyByPlane::WhereIsVertex( pPlane, &(vFirst.p) ) )
	{
		case ZClassifyByPlane::FRONT :
			vFrontList[wFrontCnt++] = vFirst;
			break;
		case ZClassifyByPlane::BACK :
			vBackList[wBackCnt++] = vFirst;
			break;
		case ZClassifyByPlane::ON :
			vFrontList[wFrontCnt++] = vFirst;
			vBackList[wBackCnt++] = vFirst;
			break;
		default:
			break;
	}

	for( wCnt = 1 ; wCnt < pFaces->GetVertCount() + 1; wCnt++ )
	{
		if( wCnt == pFaces->GetVertCount() )
			wCurrentVec = 0;
		else
			wCurrentVec = wCnt;

		vPointA = *(pFaces->GetVerts() + wCnt-1);
		vPointB = *(pFaces->GetVerts() + wCurrentVec);

		int result = ZClassifyByPlane::WhereIsVertex( pPlane, &(vPointB.p) );
		if( result == ZClassifyByPlane::ON )
		{
			vBackList[wBackCnt++] = *(pFaces->GetVerts() + wCurrentVec);
			vFrontList[wFrontCnt++] = *(pFaces->GetVerts() + wCurrentVec);
		}
		else
		{
			if( _IntersectLine( &(vIntersectPoint.p), pPlane, &(vPointA.p), &(vPointB.p) ))
			{
				if( result == ZClassifyByPlane::FRONT )
				{
					vBackList[wBackCnt++] = vIntersectPoint;
					vFrontList[wFrontCnt++] = vIntersectPoint;
					if( wCurrentVec )
						vFrontList[wFrontCnt++] = *(pFaces->GetVerts() + wCurrentVec);
				}
				if( result == ZClassifyByPlane::BACK )
				{
					vFrontList[wFrontCnt++] = vIntersectPoint;
					vBackList[wBackCnt++] = vIntersectPoint;
					if( wCurrentVec )
						vBackList[wBackCnt++] = *(pFaces->GetVerts() + wCurrentVec);
				}
			}// end - if( IntersectLine() )
			else
			{
				if( result == ZClassifyByPlane::FRONT )
				{
					if( wCurrentVec )
						vFrontList[wFrontCnt++] = *(pFaces->GetVerts() + wCurrentVec);
				}
				if( result == ZClassifyByPlane::BACK )
				{
					if( wCurrentVec )
						vBackList[wBackCnt++] = *(pFaces->GetVerts() + wCurrentVec);
				}
			}// end - else( IntersectLine() )
		}// end - else( result == FACE_ON )
	}// end - for(;;)
	
	if( wFrontCnt == wBackCnt )
	{
		frontList = new ZBspFace;
		BSPVERTEX* tmp1 = new BSPVERTEX[3];
		tmp1[0] = vFrontList[0];
		tmp1[1] = vFrontList[1];
		tmp1[2] = vFrontList[2];
		frontList->MakePlane( tmp1, 3 );

		backList = new ZBspFace;
		BSPVERTEX* tmp2 = new BSPVERTEX[3];
		tmp2[0] = vBackList[0];
		tmp2[1] = vBackList[1];
		tmp2[2] = vBackList[2];
		backList->MakePlane( tmp2, 3 );
	}
	else if( wFrontCnt > wBackCnt )
	{
		frontList = new ZBspFace;
		BSPVERTEX* tmp1 = new BSPVERTEX[3];
		tmp1[0] = vFrontList[0];
		tmp1[1] = vFrontList[1];
		tmp1[2] = vFrontList[2];
		frontList->MakePlane( tmp1, 3 );

		ZBspFace* next = new ZBspFace;
		BSPVERTEX* tmp2 = new BSPVERTEX[3];
		tmp2[0] = vFrontList[0];
		tmp2[1] = vFrontList[2];
		tmp2[2] = vFrontList[3];
		next->MakePlane( tmp2, 3 );

		frontList->AddNext( next );

		backList = new ZBspFace;
		BSPVERTEX* tmp3 = new BSPVERTEX[3];
		tmp3[0] = vBackList[0];
		tmp3[1] = vBackList[1];
		tmp3[2] = vBackList[2];

		backList->MakePlane( tmp3, 3 );
	}
	else if( wBackCnt > wFrontCnt )
	{
		backList = new ZBspFace;
		BSPVERTEX* tmp1 = new BSPVERTEX[3];
		tmp1[0] = vBackList[0];
		tmp1[1] = vBackList[1];
		tmp1[2] = vBackList[2];
		backList->MakePlane( tmp1, 3 );

		ZBspFace* next = new ZBspFace;
		BSPVERTEX* tmp2 = new BSPVERTEX[3];
		tmp2[0] = vBackList[0];
		tmp2[1] = vBackList[2];
		tmp2[2] = vBackList[3];
		next->MakePlane( tmp2, 3 );

		backList->AddNext( next );

		frontList = new ZBspFace;
		BSPVERTEX* tmp3 = new BSPVERTEX[3];
		tmp3[0] = vFrontList[0];
		tmp3[1] = vFrontList[1];
		tmp3[2] = vFrontList[2];

		frontList->MakePlane( tmp3, 3 );
	}

//나중에 메모리에서 지워주기 위해 root에 다 붙임
/*	if( frontList )
		m_pSplitFaceRoot->AddTail( frontList );
	if( backList )
		m_pSplitFaceRoot->AddTail( backList );
*/
	*a = frontList;
	*b = backList;
}

void ZBspTree::_MakeLeaf( ZBspNode* pLeaf )
{
	DWORD	i;

	pLeaf->SetNodeType( ZBspNode::NODETYPE_LEAF );
	D3DXPlaneFromPoints( pLeaf->GetPlane(), 
						 &(pLeaf->GetFaces()->GetVerts()+0)->p,
						 &(pLeaf->GetFaces()->GetVerts()+1)->p,
						 &(pLeaf->GetFaces()->GetVerts()+2)->p );
	ZBspFace* faceRoot = pLeaf->GetFaces();
	ZBspFace * tf;
	for ( tf = faceRoot; tf; tf = tf->GetNext() )
	{
		for ( i = 0; i < tf->GetVertCount(); i++)
		{
			// max 확장.
			if ( (tf->GetVerts()+i)->p.x > (pLeaf->GetMax())->x )
				(pLeaf->GetMax())->x = (tf->GetVerts()+i)->p.x;
			if ( (tf->GetVerts()+i)->p.y > (pLeaf->GetMax())->y )
				(pLeaf->GetMax())->y = (tf->GetVerts()+i)->p.y;
			if ( (tf->GetVerts()+i)->p.z > (pLeaf->GetMax())->z )
				(pLeaf->GetMax())->z = (tf->GetVerts()+i)->p.z;

			// min 확장
			if ( (tf->GetVerts()+i)->p.x < (pLeaf->GetMin())->x )
				(pLeaf->GetMin())->x = (tf->GetVerts()+i)->p.x;
			if ( (tf->GetVerts()+i)->p.y < (pLeaf->GetMin())->y )
				(pLeaf->GetMin())->y = (tf->GetVerts()+i)->p.y;
			if ( (tf->GetVerts()+i)->p.z < (pLeaf->GetMin())->z )
				(pLeaf->GetMin())->z = (tf->GetVerts()+i)->p.z;
		}
	}
}

int	ZBspTree::_GetFaceNodeCount( ZBspFace* pFace )
{
	ZBspFace * temp = pFace;
	int i=0;
	while(temp)
	{
		i++;
		temp = temp->GetNext();
	}
	return i;
}

void ZBspTree::_GetSortedFaceList( D3DXPLANE* pPlane, ZBspFace* pFaces, ZBspFace** fr , ZBspFace** bk )
{
	ZBspFace * pFront = NULL, * pBack = NULL, * pNext = NULL;
	ZBspFace * pCurrentFace = NULL;
	if( !pFaces )
	{
		*fr = NULL;
		*bk = NULL;
		return;
	}

	for( pCurrentFace = pFaces ; pCurrentFace ; pCurrentFace = pFaces )
	{
		pFaces = pFaces->GetNext();
		D3DXVECTOR3 planeNormal( pPlane->a, pPlane->b, pPlane->c );
		D3DXVECTOR3 currentNormal( pCurrentFace->GetPlane()->a, pCurrentFace->GetPlane()->b, pCurrentFace->GetPlane()->c );
		float val;
		int res = ZClassifyByPlane::WhereIsFace( pPlane, pCurrentFace->GetVerts(), pCurrentFace->GetVertCount() );
		switch( res )
		{
			case ZClassifyByPlane::FRONT:
				pCurrentFace->AddNext( pFront );
				pFront = pCurrentFace;
				break;

			case ZClassifyByPlane::BACK:
				pCurrentFace->AddNext( pBack );
				pBack = pCurrentFace;
				break;

			case ZClassifyByPlane::ON:
				pCurrentFace->SetUsed( TRUE );

				val = D3DXVec3Dot( &planeNormal, &currentNormal );
				if( val >= 0.0f )
				{
					pCurrentFace->AddNext( pFront );
					pFront = pCurrentFace;
				}
				else
				{
					pCurrentFace->AddNext( pBack );
					pBack = pCurrentFace;
				}
				break;

			case ZClassifyByPlane::SPLIT:
				ZBspFace * front = NULL, * back = NULL;
				_Split( pPlane, pCurrentFace, &front, &back );
				if( !front && !back )
					break;
				if( front->GetNext() )
				{
					front->GetNext()->AddNext( pFront );
					pFront = front;
				}
				else 
				{
					front->AddNext( pFront );
					pFront = front;
				}
				if( back->GetNext() )
				{
					back->GetNext()->AddNext( pBack );
					pBack = back;
				}
				else
				{
					back->AddNext( pBack );
					pBack = back;
				}

				// 분할된 face는 새롭게 front, back으로 분할되었으므로
				// 메모리에서 반드시 삭제해야한다.
				pCurrentFace->AddNext( NULL );
				delete pCurrentFace;
				break;
		}
	}
	*fr = pFront;
	*bk = pBack;
}

ZBspFace* ZBspTree::_FindBestSplitter( ZBspFace* pList )
{
	ZBspFace *cface = NULL;
	ZBspFace *curr = NULL;
	ZBspFace *best = NULL; // current best plane
	float bscore = BSP_BOGUS;
	float score = BSP_BOGUS;
	int res = 0;

	// no list
	if( !pList )
		return 0;

	int f, b, s, o;
	for( cface = pList; cface; cface = cface->GetNext() )
	{
		if ( cface->GetUsed() )
			continue;

		f = 0; b = 0; s = 0; o = 0;
/*
		figure out how many faces are in front, behind, on or spanning with
		respect to cface.  
*/
		for ( curr = pList; curr; curr = curr->GetNext() )      
		{
			// this face has been used as a split plane, don't consider
			if ( curr->GetUsed() )
				continue;

			// don't compare cface to cface
			if (curr != cface)
			{
				res = ZClassifyByPlane::WhereIsFace( cface->GetPlane(), curr->GetVerts(), curr->GetVertCount() );
				if ( res == ZClassifyByPlane::FRONT )
					f++;
				else if ( res == ZClassifyByPlane::BACK )
					b++;
				else if ( res == ZClassifyByPlane::ON )
					o++;
				else
					s++;
			}
		}

		// calculate a score for the current plane, this can change
		score = (float)((2*s) + abs(f - b) + o); 
//		score = abs(f - b);
		// this plane is better than what we have, make it the best
		if (score && score < bscore) 
		{
			 bscore = score;
			 best = cface;
		}
	}

	return best;
}

void ZBspTree::_SubDivide( ZBspNode* p )
{
	ZBspNode * front_node = NULL;
	ZBspNode * back_node = NULL;

	ZBspFace* bestface = _FindBestSplitter( p->GetFaces() );
	
	if ( bestface == NULL )
	{
		_MakeLeaf( p );
		return;
	}

	*(p->GetPlane()) = *(bestface->GetPlane());

	ZBspFace * fr = NULL;
	ZBspFace * bk = NULL;
	
	_GetSortedFaceList ( bestface->GetPlane() , p->GetFaces(), &fr , &bk );
	p->SetFaces( NULL );	// Face정보는 Leaf노드에서만 갖고있어야 한다.

	if ( bk )
	{
		back_node = new ZBspNode();
		back_node->SetFaces( bk );
		p->SetBackNode( back_node );
		_SubDivide( p->GetBackNode() );
	}
	else
	{
		back_node = new ZBspNode();
		back_node->SetFaces( NULL );
		back_node->SetNodeType( ZBspNode::NODETYPE_SOLIDLEAF );
		p->SetBackNode( back_node );
	}

	if ( fr )
	{
		front_node = new ZBspNode();
		front_node->SetFaces( fr );
		p->SetFrontNode( front_node );
		_SubDivide( p->GetFrontNode() );
	}
}

BOOL ZBspTree::_CreateFaceList()
{
	DWORD index;
	ZBspFace* p;
	for( DWORD i = 0 ; i < m_nFaceCount ; i++ )
	{
		index = 0;
		p = new ZBspFace;
		BSPVERTEX* v = new BSPVERTEX[3];
		v[index++] = m_pVerts[m_pIndices[i]._0];
		v[index++] = m_pVerts[m_pIndices[i]._1];
		v[index++] = m_pVerts[m_pIndices[i]._2];
		p->MakePlane( v, index );
		p->AddNext( m_pRootFace );
		m_pRootFace = p;
	}

	return TRUE;
}

void ZBspTree::_Destroy()
{
	m_nVertCount	= 0;
	m_nFaceCount	= 0;
	DELS( m_pVerts );
	DELS( m_pIndices );
	DEL( m_pRootNode );
}

