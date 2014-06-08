#ifndef _ZVSMGR_H_
#define _ZVSMGR_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"

struct ZVSTable
{
	string							name;
	LPDIRECT3DVERTEXDECLARATION9	pDecl;		/// 정점선언 정보
	LPDIRECT3DVERTEXSHADER9			pShader;	/// 정점쉐이더
};

class ZVSMgr
{
	LPDIRECT3DDEVICE9		m_pDev;
	list<ZVSTable*>			m_VSTable;
public:
	ZVSMgr( LPDIRECT3DDEVICE9 pDev ) { m_pDev = pDev; }
	~ZVSMgr();

	int Add( char* fname, DWORD fvf );
	ZVSTable* Get( char* fname );
};

#endif // _ZVSMGR_H_