#ifndef _ZQUADTREE_H_
#define _ZQUADTREE_H_

#include "define.h"

/**
 * @brief QuadTree�� ���̽� Ŭ����
 *
 */ 
class ZQuadTree
{
	/// ����Ʈ���� �����Ǵ� 4���� �ڳʰ��� ���� �����
	enum		CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

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
private:
	/// �ڽ� ��带 �߰��Ѵ�.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	
	/// 4���� �ڳʰ��� �����Ѵ�.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	
	/// Quadtree�� 4���� ���� Ʈ���� �κк���(subdivide)�Ѵ�.
	BOOL		_SubDivide();
	
	/// ���� ��尡 ����� ������ ����ΰ�?
	BOOL		_IsVisible() { return ( m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL] <= 1 ); }
	
	/// ����� �������� �ε����� �����Ѵ�.
	int			_GenTriIndex( int nTriangles, LPVOID pIndex );
	
	/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
	void		_Destroy();
public:
				
				/// ���� ��Ʈ��� ������
				ZQuadTree( int cx, int cy );
				
				/// ���� �ڽĳ�� ������
				ZQuadTree( ZQuadTree* pParent );
				
				/// �Ҹ���
				~ZQuadTree();
				
	/// QuadTree�� �����Ѵ�.
	BOOL		Build();

	///	�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
	int			GenerateIndex( LPVOID pIB );
};

#endif // _ZQUADTREE_H_
