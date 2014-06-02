#include "define.h"
#include "ZQuadTree.h"
#include "ZFLog.h"

// 최초 루트노드 생성자
ZQuadTree::ZQuadTree( int cx, int cy )
{
	int		i;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
	}

	// 루트노드의 4개 코너값 설정
	m_nCorner[CORNER_TL]	= 0;
	m_nCorner[CORNER_TR]	= cx - 1;
	m_nCorner[CORNER_BL]	= cx * ( cy - 1 );
	m_nCorner[CORNER_BR]	= cx * cy - 1;
	m_nCenter				= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
								m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	m_fRadius = 0.0f;
	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}

// 하위 자식노드 생성자
ZQuadTree::ZQuadTree( ZQuadTree* pParent )
{
	int		i;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_nCorner[i] = 0;
	}

	m_bCulled = FALSE;
	m_fRadius = 0.0f;
}

// 소멸자
ZQuadTree::~ZQuadTree()
{
	_Destroy();
}

// 메모리에서 쿼드트리를 삭제한다.
void	ZQuadTree::_Destroy()
{
	// 자식 노드들을 소멸 시킨다.
	for( int i = 0 ; i < 4 ; i++ ) DEL( m_pChild[i] );
}


// 4개의 코너값을 셋팅한다.
BOOL	ZQuadTree::_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;
	m_nCenter			 = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;
	return TRUE;
}


// 자식 노드를 추가한다.
ZQuadTree*	ZQuadTree::_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	ZQuadTree*	pChild;

	pChild = new ZQuadTree( this );
	pChild->_SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );

	return pChild;
}

// Quadtree를 4개의 자식 트리로 부분분할(subdivide)한다.
BOOL	ZQuadTree::_SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// 상단변 가운데
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// 하단변 가운데 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// 좌측변 가운데
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// 우측변 가운데
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// 한가운데
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// 더이상 분할이 불가능한가? 그렇다면 SubDivide() 종료
	if( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 4개의 자식노드 추가
	m_pChild[CORNER_TL] = _AddChild( m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = _AddChild( nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = _AddChild( nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = _AddChild( nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR] );
	
	return TRUE;
}

// 출력할 폴리곤의 인덱스를 생성한다.
int		ZQuadTree::_GenTriIndex( int nTris, LPVOID pIndex )
{
	// 컬링된 노드라면 그냥 리턴
	if( m_bCulled )
	{
		m_bCulled = FALSE;
		return nTris;
	}

	// 현재 노드가 출력되어야 하는가?
	if( _IsVisible() )
	{
#ifdef _USE_INDEX16
		LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
		LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif
		*p++ = m_nCorner[0];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[2];
		nTris++;
		*p++ = m_nCorner[2];
		*p++ = m_nCorner[1];
		*p++ = m_nCorner[3];
		nTris++;

		return nTris;
	}

	// 자식 노드들 검색
	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->_GenTriIndex( nTris, pIndex );
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->_GenTriIndex( nTris, pIndex );

	return nTris;
}

int ZQuadTree::_IsInFrustum( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere;

	// 경계구안에 있는가?
//	if( m_fRadius == 0.0f ) g_pLog->Log( "Index:[%d], Radius:[%f]",m_nCenter, m_fRadius );
	bInSphere = pFrustum->IsInSphere( (D3DXVECTOR3*)(pHeightMap+m_nCenter), m_fRadius );
	if( !bInSphere ) return FRUSTUM_OUT;	// 경계구 안에 없으면 점단위의 프러스텀 테스트 생략

	// 쿼드트리의 4군데 경계 프러스텀 테스트
	b[0] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[0]) );
	b[1] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[1]) );
	b[2] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[2]) );
	b[3] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[3]) );

	// 4개모두 프러스텀 안에 있음
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;

	// 일부분이 프러스텀에 있는 경우
	return FRUSTUM_PARTIALLY_IN;
}

// _IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
void	ZQuadTree::_FrustumCull( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum )
{
	int ret;

	ret = _IsInFrustum( pHeightMap, pFrustum );
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// 프러스텀에 완전포함, 하위노드 검색 필요없음
			m_bCulled = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN :		// 프러스텀에 일부포함, 하위노드 검색 필요함
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT :				// 프러스텀에서 완전벗어남, 하위노드 검색 필요없음
			m_bCulled = TRUE;
			return;
	}
	if( m_pChild[0] ) m_pChild[0]->_FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[1] ) m_pChild[1]->_FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[2] ) m_pChild[2]->_FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[3] ) m_pChild[3]->_FrustumCull( pHeightMap, pFrustum );
}

// 쿼드트리를 만든다.
BOOL	ZQuadTree::Build( TERRAINVERTEX* pHeightMap )
{
	if( _SubDivide() )
	{
		// 좌측상단과, 우측 하단의 거리를 구한다.
		D3DXVECTOR3 v = *((D3DXVECTOR3*)(pHeightMap+m_nCorner[CORNER_TL])) - 
					    *((D3DXVECTOR3*)(pHeightMap+m_nCorner[CORNER_BR]));
		// v의 거리값이 이 노드를 감싸는 경계구의 지름이므로, 
		// 2로 나누어 반지름을 구한다.
		m_fRadius	  = D3DXVec3Length( &v ) / 2.0f;
		m_pChild[CORNER_TL]->Build( pHeightMap );
		m_pChild[CORNER_TR]->Build( pHeightMap );
		m_pChild[CORNER_BL]->Build( pHeightMap );
		m_pChild[CORNER_BR]->Build( pHeightMap );
	}
	return TRUE;
}

//	삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환한다.
int		ZQuadTree::GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum )
{
	_FrustumCull( pHeightMap, pFrustum );
	return _GenTriIndex( 0, pIndex );
}

