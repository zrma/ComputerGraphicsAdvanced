#ifndef _ZMESH_H_
#define _ZMESH_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZCMesh.h"
#include "zdefine.h"
#include "ZNode.h"
#include "ZTrack.h"
#include "ZShaderMgr.h"

/**
 * 3D������Ʈ�� �޽üӼ��� ���� Ŭ����
 *
 */
class ZMesh : public ZNode
{
protected:
	BOOL					m_isSkinned;	/// ��Ű���� ����Ǵ� �޽��ΰ�?

	DWORD					m_nVerts;		/// ��ü ������ ����
	DWORD					m_nTriangles;	/// �׷��� �ﰢ���� ����
	DWORD					m_dwFVF;		/// ������ fvf��
	LPDIRECT3DVERTEXBUFFER9	m_pVB;			/// ���� ����
	LPDIRECT3DINDEXBUFFER9	m_pIB;			/// �ε��� ����

public:
	ZMesh( LPDIRECT3DDEVICE9 pDev, ZCMesh* pMesh, ZShaderMgr* pSM );
	~ZMesh();
	
	BOOL	IsSkinned() { return m_isSkinned; }
	/// �޽ø� �׸���
	virtual int	Draw( D3DXMATRIX* pTM ) { return 1; }
};

#endif // _ZMESH_H_