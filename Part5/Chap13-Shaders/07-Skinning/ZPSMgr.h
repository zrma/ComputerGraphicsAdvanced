#ifndef _ZPSMGR_H_
#define _ZPSMGR_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"

struct ZPSTable
{
	string							name;
	LPDIRECT3DVERTEXDECLARATION9	pDecl;		/// �������� ����
	LPDIRECT3DPIXELSHADER9			pShader;	/// �ȼ� ���̴�
};

class ZPSMgr
{
	LPDIRECT3DDEVICE9		m_pDev;
	list<ZPSTable*>			m_PSTable;
public:
	ZPSMgr( LPDIRECT3DDEVICE9 pDev ) { m_pDev = pDev; }
	~ZPSMgr();

	int Add( char* fname, DWORD fvf );
	ZPSTable* Get( char* fname );
};

#endif // _ZPSMGR_H_