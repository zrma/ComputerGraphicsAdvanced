#ifndef _ZQUADTREE_H_
#define _ZQUADTREE_H_

#include "define.h"
#include "ZFrustum.h"
#include "ZFLog.h"

/**
 * @brief QuadTree�� ���̽� Ŭ����
 *
 */ 
class ZQuadTree
{
	/// ����Ʈ���� �����Ǵ� 4���� �ڳʰ��� ���� �����
	enum			CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

	/// ����Ʈ���� �������Ұ��� ����
	enum			QuadLocation { FRUSTUM_OUT = 0,				/// �������ҿ��� �������
								   FRUSTUM_PARTIALLY_IN = 1,	/// �������ҿ� �κ�����
								   FRUSTUM_COMPLETELY_IN = 2,	/// �������ҿ� ��������
								   FRUSTUM_UNKNOWN = -1 };		/// �𸣰���(^^;)

private:
	ZQuadTree*	m_pChild[4];		/// QuadTree�� 4���� �ڽĳ��

	int			m_nCenter;			/// QuadTree�� ������ ù��° ��
	int			m_nCorner[4];		/// QuadTree�� ������ �ι�° ��
									///    TopLeft(TL)      TopRight(TR)
									///              0------1
									///              |      |
									///              |      |
									///              2------3
									/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// �������ҿ��� �ø��� ����ΰ�?
	float		m_fRadius;			/// ��带 ���δ� ��豸(bounding sphere)�� ������
private:
	/// �ڽ� ��带 �߰��Ѵ�.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// 4���� �ڳʰ��� �����Ѵ�.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// Quadtree�� 4���� ���� Ʈ���� �κк���(subdivide)�Ѵ�.
	BOOL		_SubDivide();		// Quadtree�� subdivide�Ѵ�.

	/// ����� �������� �ε����� �����Ѵ�.
	int			_GenTriIndex( int nTris, LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );

	/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
	void		_Destroy();

	/// ���� ������ �Ÿ��� ���Ѵ�.
	float		_GetDistance( D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 ) 
				{
					return D3DXVec3Length( &(*pv2 - *pv1) );
				}

	/// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
	int			_GetLODLevel( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
				{
					float d = _GetDistance( (D3DXVECTOR3*)(pHeightMap+m_nCenter), pCamera );
					return max( (int)( d * fLODRatio ), 1 );
				}

	/// ���� ��尡 LOD������� ����  ����� ������ ����ΰ�?
	BOOL		_IsVisible( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
				{ 
					return ( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel( pHeightMap, pCamera, fLODRatio ) ); 
				}

	/// �����尡 �������ҿ� ���ԵǴ°�?
	int			_IsInFrustum( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum );

	/// _IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
	void		_FrustumCull( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum );
public:

				/// ���� ��Ʈ��� ������
				ZQuadTree( int cx, int cy );

				/// ���� �ڽĳ�� ������
				ZQuadTree( ZQuadTree* pParent );

				/// �Ҹ���
				~ZQuadTree();

	/// QuadTree�� �����Ѵ�.
	BOOL		Build( TERRAINVERTEX* pHeightMap );	// ������ ������ �������� QuadTree�� build�Ѵ�.

	///	�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
	int			GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );
};

#endif // _ZQUADTREE_H_
