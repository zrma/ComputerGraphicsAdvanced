#ifndef _ZCPARSER_H_
#define _ZCPARSER_H_

#include "zxml.h"
#include "define.h"
#include "zcmesh.h"
#include "zcparseddata.h"

/// 파서
class ZCParser
{
	ZXML*		m_pXML;	/// XML파일을 파싱하기 위한 기본 객체
public:
			ZCParser();
			~ZCParser();
private:
	Index3i		_StrToIndex3i( char str[] );	/// 문자열을 Index3i로 변형
	Color3f		_StrToColor3f( char str[] );	/// 문자열을 Color3f로 변형
	Vector2f	_StrToVector2f( char str[] );	/// 문자열을 Vector2f로 변형
	Vector3f	_StrToVector3f( char str[] );	/// 문자열을 Vector3f로 변형
	Vector4f	_StrToVector4f( char str[] );	/// 문자열을 Vector4f로 변형

	int		_ParseInfo( ZCParsedData* pData );
	int		_ParseMaterial( ZCParsedData* pData );
	int		_ParseObject( ZCParsedData* pData );
	int		_ParseObjectParent( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectBBox( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectTM( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectVertex( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectVertexNormal( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectTVertex( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectTriIndex( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectTexIndex( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectVertWeight( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_ParseObjectKey( ZCMesh* pMesh, IXMLDOMNode* pNode );
	int		_SplitMeshByFaceMaterial( ZCParsedData* pData );
	void	_MergeBlendWeight( VertMerged& vm, VertWeight& vw );
	int		_MergeVertex( ZCParsedData* pData );

public:
	int		Parse_XML( ZCParsedData* pData, char* lpszFilename );
	int		Parse_Split( ZCParsedData* pData ) { return _SplitMeshByFaceMaterial( pData ); }
	int		Parse_Merge( ZCParsedData* pData ) { return _MergeVertex( pData ); }

	int		Parse( ZCParsedData* pData, char* lpszFilename )
	{
		if( !Parse_XML( pData, lpszFilename ) ) return 0;
		if( !Parse_Split( pData ) ) return 0;
		if( !Parse_Merge( pData ) ) return 0;

		return 1;
	}
};

#endif // _ZPARSER_H_