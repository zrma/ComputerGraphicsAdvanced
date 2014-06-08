#include "ZVSMgr.h"

ZVSMgr::~ZVSMgr()
{
	list<ZVSTable*>::iterator it;
	for( it = m_VSTable.begin() ; it != m_VSTable.end() ; it++ )
	{
		S_REL( (*it)->pDecl );
		S_REL( (*it)->pShader );
	}
	stl_wipe( m_VSTable );
}

int ZVSMgr::Add( char *fname, DWORD fvf )
{
	D3DVERTEXELEMENT9	decl[MAX_FVF_DECL_SIZE];

	if( Get( fname ) ) return 1;

	ZVSTable*	p = new ZVSTable;
	p->name = fname;

	// FVF�� ����ؼ� ���������� �ڵ����� ä���ִ´�
	D3DXDeclaratorFromFVF( fvf, decl );
	// ������������ m_pShaderDecl�� �����Ѵ�.
	m_pDev->CreateVertexDeclaration( decl, &p->pDecl );
	LPD3DXBUFFER pCode;

	// �������̴� ������ �о�ͼ� �������̴� �������̽��� �����Ѵ�.
	if( FAILED( D3DXAssembleShaderFromFile( fname, NULL, NULL, 0, &pCode, NULL ) ) )
		return 0;

	m_pDev->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &p->pShader );

	S_REL( pCode );

	m_VSTable.push_back( p );

	return 1;
}

ZVSTable* ZVSMgr::Get( char* fname )
{
	list<ZVSTable*>::iterator it;
	for( it = m_VSTable.begin() ; it != m_VSTable.end() ; it++ )
		if( (*it)->name == fname ) return *it;

	return NULL;
}

