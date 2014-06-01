#include "ZSimpleParser.h"

ZSimpleParser::ZSimpleParser()
{
}

ZSimpleParser::~ZSimpleParser()
{
}

BOOL ZSimpleParser::Parse( LPSTR szFileName, BSPMAPDATA* pData )
{
	char tmpFileName[_MAX_PATH];
	FILE* fp;
	//VERTEX Á¤º¸
	strcpy(tmpFileName, szFileName);
	strcat(tmpFileName, ".vtx");
	fp = fopen(tmpFileName, "rt");
	if( !fp ) return FALSE;
	_SetVertices( fp, pData );
	fclose( fp );
		
	//FACE INDEX Á¤º¸
	strcpy(tmpFileName, szFileName);
	strcat(tmpFileName, ".idx");
	fp = fopen(tmpFileName, "rt");
	_SetFaceIndex( fp, pData );
	if( !fp ) return FALSE;
	fclose( fp );

	return TRUE;
}

void ZSimpleParser::_GetIndex(LPSTR lpszOther, WORD* first, WORD* second, WORD* third)
{
	char buffer[20];
	int index, i;
	while(*lpszOther++ != START_BRACKET);

	for( index=0, i=0;*lpszOther;lpszOther++)
	{
		if(*lpszOther == END_BRACKET)
			break;
		if(*lpszOther == COMMA)
		{
			buffer[i] = NULL;
			if(index == 0)
				*third = (WORD)(atoi(buffer))-1;
			else if(index == 1)
				*second = (WORD)(atoi(buffer))-1;
			i = 0;
			index++;
		}
		else
			buffer[i++] = *lpszOther;
	}
	buffer[i] = NULL;
	*first = (WORD)(atoi(buffer))-1;
}

void ZSimpleParser::_GetVertex(LPSTR lpszOther, float* x, float* y, float* z)
{
	char buffer[20];
	double buf = 0;
	int index, i;
	while(*lpszOther++ != START_BRACKET);
	//MaxÀÇ yzÁÂÇ¥¿Í D3DÀÇ yz°¡ ¹Ù²î¾î¾ßµÊ.
	for( index=0, i=0;*lpszOther;lpszOther++)
	{
		if(*lpszOther == END_BRACKET)
			break;
		if(*lpszOther == COMMA)
		{
			buffer[i] = NULL;
			if(index == 0)
				*x = (float)atof(buffer);
			else if(index == 1)
				*z = (float)atof(buffer);
			i = 0;
			index++;
		}
		else
			buffer[i++] = *lpszOther;
	}
	buffer[i] = NULL;
	*y = (float)atof(buffer);
}

void ZSimpleParser::_SetVertices(FILE* fp, BSPMAPDATA* pData)
{
	char lpszFormer[_MAX_PATH], lpszOther[_MAX_PATH];
	int index = 0;
	float x, y, z;
	while(1)
	{
		_GetFormerNOther(fp, lpszFormer, lpszOther);
		if(!strcmp(lpszOther, "EOF"))
			break;
		
		else if(!strcmp(lpszFormer, "NAME"))
		{
			index = 0;
		}
		
		else if(!strcmp(lpszFormer, "COUNT"))
		{
			pData->nVertCount = atoi(lpszOther);
			pData->pVerts = new BSPVERTEX[pData->nVertCount];
		}
		else if(pData->nVertCount > 0)
		{
			//GetVertex°¡ yzÁÂÇ¥¸¦ ¹Ù²ãÁÜ
			_GetVertex(lpszOther, &x, &y, &z);
			pData->pVerts[index].p.x = x;
			pData->pVerts[index].p.y = y;
			pData->pVerts[index].p.z = z;
			index++;
		}
	
	}
}

void ZSimpleParser::_SetFaceIndex(FILE* fp, BSPMAPDATA* pData)
{
	char lpszFormer[_MAX_PATH], lpszOther[_MAX_PATH];
	int num = 0; 
	WORD _0 = 0, _1 = 0, _2 = 0;

	while(1)
	{
		_GetFormerNOther(fp, lpszFormer, lpszOther);
		if(!strcmp(lpszOther, "EOF"))
			break;
	
		else if(!strcmp(lpszFormer, "NAME"))
		{
			num = 0;
		}
		else if(!strcmp(lpszFormer, "COUNT"))
		{
			pData->nFaceCount = atoi(lpszOther);
			pData->pIndices = new TRIINDEX[pData->nFaceCount];
		}
		else if(pData->nFaceCount > 0)
		{
			_GetIndex(lpszOther, &_0, &_1, &_2);
			pData->pIndices[num]._0 = _0;
			pData->pIndices[num]._1 = _1;
			pData->pIndices[num]._2 = _2;
			num++;
		}
	}
}

void ZSimpleParser::_GetToken(FILE* fp, LPSTR lpszToken)
{
	char ch = 0;
	while( fgetc(fp)!=START_BRACE );
	while(1)
	{
		ch = fgetc(fp);
		if(ch == END_BRACE)
		{	
			*lpszToken = NULL;
			return;
		}
		*lpszToken = ch;
		lpszToken++;
	}
}

void ZSimpleParser::_GetFormerNOther(FILE* fp, LPSTR lpszFormer, LPSTR lpszOther)
{
	_GetToken(fp, lpszFormer);
	_GetToken(fp, lpszOther);
}

