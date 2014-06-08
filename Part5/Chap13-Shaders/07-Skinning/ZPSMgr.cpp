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

	// FVF�� ����ؼ� ���������� �ڵ����� ä���ִ´�
	D3DXDeclaratorFromFVF( fvf, decl );
	// ������������ m_pShaderDecl�� �����Ѵ�.
	m_pDev->CreateVertexDeclaration( decl, &p->pDecl );
	LPD3DXBUFFER pCode;

	// �������̴� ������ �о�ͼ� �������̴� �������̽��� �����Ѵ�.
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

