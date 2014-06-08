#ifndef _ZFXMGR_H_
#define _ZFXMGR_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"

struct ZFXTable
{
	string							name;
	LPDIRECT3DVERTEXDECLARATION9	pDecl;		/// 정점선언 정보
	LPD3DXEFFECT					pShader;	/// fx파일을 사용하기 위한 인터페이스
};

class ZFXMgr
{
	LPDIRECT3DDEVICE9		m_pDev;
	list<ZFXTable*>			m_FXTable;
public:
	ZFXMgr( LPDIRECT3DDEVICE9 pDev ) { m_pDev = pDev; }
	~ZFXMgr();

	int Add( char* fname, DWORD fvf );
	ZFXTable* Get( char* fname );
};

#endif // _ZFXMGR_H_