#include "ZPSMgr.h"

ZPSMgr::~ZPSMgr()
{
	list<ZPSTable*>::iterator it;
	for( it = m_PSTable.begin() ; it != m_PSTable.end() ; it++ )
	{
		S_REL( (*it)->pDecl );
		S_REL( (*it)->pShader );
	}
	stl_wipe( m_PSTable );
}

int ZPSMgr::Add( char *fname, DWORD fvf )
{
	D3DVERTEXELEMENT9	decl[MAX_FVF_DECL_SIZE];

	if( Get( fname ) ) return 1;

	ZPSTable*	p = new ZPSTable;
	p->name = fname;

	// FVF를 사용해서 정점선언값을 자동으로 채워넣는다
	D3DXDeclaratorFromFVF( fvf, decl );
	// 정점선언값으로 m_pShaderDecl을 생성한다.
	m_pDev->CreateVertexDeclaration( decl, &p->pDecl );
	LPD3DXBUFFER pCode;

	// 정점쉐이더 파일을 읽어와서 정점쉐이더 인터페이스를 생성한다.
	if( FAILED( D3DXAssembleShaderFromFile( fname, NULL, NULL, 0, &pCode, NULL ) ) )
		return 0;

	m_pDev->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &p->pShader );

	S_REL( pCode );

	m_PSTable.push_back( p );

	return 1;
}

ZPSTable* ZPSMgr::Get( char* fname )
{
	list<ZPSTable*>::iterator it;
	for( it = m_PSTable.begin() ; it != m_PSTable.end() ; it++ )
		if( (*it)->name == fname ) return *it;

	return NULL;
}

