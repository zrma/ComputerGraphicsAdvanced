#include "ZFXMgr.h"

ZFXMgr::~ZFXMgr()
{
	list<ZFXTable*>::iterator it;
	for( it = m_FXTable.begin() ; it != m_FXTable.end() ; it++ )
	{
		S_REL( (*it)->pDecl );
		S_REL( (*it)->pShader );
	}
	stl_wipe( m_FXTable );
}

int ZFXMgr::Add( char *fname, DWORD fvf )
{
	D3DVERTEXELEMENT9	decl[MAX_FVF_DECL_SIZE];

	if( Get( fname ) ) return 1;

	ZFXTable*	p = new ZFXTable;
	p->name = fname;

	// FVF를 사용해서 정점선언값을 자동으로 채워넣는다
	D3DXDeclaratorFromFVF( fvf, decl );
	// 정점선언값으로 m_pShaderDecl을 생성한다.
	m_pDev->CreateVertexDeclaration( decl, &p->pDecl );

	if( FAILED( D3DXCreateEffectFromFile( m_pDev, fname, NULL, NULL, 0, NULL, &p->pShader, NULL ) ) )
		return 0;

	m_FXTable.push_back( p );

	return 1;
}

ZFXTable* ZFXMgr::Get( char* fname )
{
	list<ZFXTable*>::iterator it;
	for( it = m_FXTable.begin() ; it != m_FXTable.end() ; it++ )
		if( (*it)->name == fname ) return *it;

	return NULL;
}

