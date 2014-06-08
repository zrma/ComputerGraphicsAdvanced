#ifndef _ZNODE_H_
#define _ZNODE_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZTrack.h"
#include "ZShaderMgr.h"

/**
 * ��� 3D������Ʈ�� �ֻ��� Ŭ����
 *
 */
class ZNode
{
public:
	enum NODETYPE { NODE_NODE, NODE_BONE, NODE_MESH };	// ���� �����ϴ� ����� ����
protected:

	int						m_nNodeType;	/// ����� ����
	int						m_nObjectID;	/// ������Ʈ ID
	int						m_nParentID;	/// �θ���ID
	int						m_nMaterialID;	/// ������ ID
	D3DXVECTOR3				m_bboxMaxOrig;	/// �ٿ�� �ڽ��� ���� �ִ밪(max)
	D3DXVECTOR3				m_bboxMinOrig;	/// �ٿ�� �ڽ��� ���� �ּҰ�(min)
	D3DXVECTOR3				m_bboxMax;		/// �ٿ�� �ڽ��� ���� �ִ밪(max)
	D3DXVECTOR3				m_bboxMin;		/// �ٿ�� �ڽ��� ���� �ּҰ�(min)
	D3DXMATRIXA16			m_matWorld;		/// world TM ���(�Һ�)
	D3DXMATRIXA16			m_matLocal;		/// local TM ���(�Һ�)

	/// m_matTM = m_matLocal * m_matAni * (mom's m_matTM) * (grandmom's m_matTM) * ...
	D3DXMATRIXA16			m_matTM;		/// ���� TM ���(��ȭ)
	ZTrack*					m_pTrack;		/// �ִϸ��̼� Ʈ������
	LPDIRECT3DDEVICE9		m_pDev;			/// D3D����̽�
	ZShaderMgr*				m_pShaderMgr;	/// ���̴� �޴���
public:
	ZNode( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZNode();

	/// ����� ����
	int GetNodeType() { return m_nNodeType; }

	/// �޽��� ID���� ��´�
	int	GetObjectID() { return m_nObjectID; }

	/// �θ�޽��� ID���� ��´�
	int	GetParentID() { return m_nParentID; }

	int	GetMaterialID() { return m_nMaterialID; }

	D3DXMATRIXA16* GetMatrixWorld() { return &m_matWorld; }

	D3DXMATRIXA16* GetMatrixLocal() { return &m_matLocal; }

	/// ������ TM���� ��´�
	D3DXMATRIXA16* GetMatrixTM() { return &m_matTM; }

	/// �������� ���� ��´�
	void GetBBox( D3DXVECTOR3* pMin, D3DXVECTOR3* pMax )
	{
		D3DXVec3TransformCoord( &m_bboxMin, &m_bboxMinOrig, &m_matTM );
		D3DXVec3TransformCoord( &m_bboxMax, &m_bboxMaxOrig, &m_matTM );
		*pMin = m_bboxMin; *pMax = m_bboxMax; 
	}

	/// ZTrack���κ��� m_matTM�� �����
	virtual D3DXMATRIXA16* Animate( float frame, D3DXMATRIXA16* pParentTM );

	/// ��带 �׸���.
	virtual int	Draw( D3DXMATRIX* pTM ) { return 1; }

	/// �����ڸ� �׸���
	virtual int	DrawBBox( D3DXMATRIX* pTM );
};

#endif // _ZNODE_H_