#ifndef _ZSIMPLEPARSER_H_
#define _ZSIMPLEPARSER_H_

#include <stdio.h>
#include "define.h"
#include "ZBspTree.h"

#define		START_BRACE		'{'		
#define		END_BRACE		'}'
#define		COMMA			','
#define		START_BRACKET	'['
#define		END_BRACKET		']'
#define		BLANK			' '

class BSPMAPDATA
{
public:
	BSPVERTEX*	pVerts;
	TRIINDEX*	pIndices;
	DWORD		nVertCount;
	DWORD		nFaceCount;
public:
	BSPMAPDATA()
	{
		pVerts = NULL;
		pIndices = NULL;
		nVertCount = 0;
		nFaceCount = 0;
	}
	~BSPMAPDATA() {}
};

class ZSimpleParser
{
private:
	void	_GetToken( FILE* fp, LPSTR lpszToken );
	void	_GetIndex( LPSTR lpszOther, WORD* first, WORD* second, WORD* third );
	void	_GetVertex( LPSTR lpszOther, float* x, float* y, float* z );
	void	_SetVertices( FILE* fp, BSPMAPDATA* pData );
	void	_SetFaceIndex( FILE* fp, BSPMAPDATA* pData );
	void	_GetFormerNOther( FILE* fp, LPSTR lpszFormer, LPSTR lpszOther );
public:
	ZSimpleParser();
	~ZSimpleParser();
	BOOL Parse( LPSTR szFileName, BSPMAPDATA* pData );
};

#endif // _ZSIMPLEPARSER_H_