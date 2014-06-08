#ifndef _ZCPARSER_H_
#define _ZCPARSER_H_

#include "zxml.h"
#include "define.h"
#include "zcmesh.h"
#include "zcparseddata.h"

/// �ļ�
class ZCParser
{
	ZXML*		m_pXML;	/// XML������ �Ľ��ϱ� ���� �⺻ ��ü
public:
			ZCParser();
			~ZCParser();
private:
	Index3i		_StrToIndex3i( char str[] );	/// ���ڿ��� Index3i�� ����
	Color3f		_StrToColor3f( char str[] );	/// ���ڿ��� Color3f�� ����
	Vector2f	_StrToVector2f( char str[] );	/// ���ڿ��� Vector2f�� ����
	Vector3f	_StrToVector3f( char str[] );	/// ���ڿ��� Vector3f�� ����
	Vector4f	_StrToVector4f( char str[] );	/// ���ڿ��� Vector4f�� ����

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