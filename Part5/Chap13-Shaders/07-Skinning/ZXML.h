#ifndef _ZXML_H_
#define _ZXML_H_

/** \brief easy XML...really?
 *
 */

#include <windows.h>
#include <tchar.h>
#include <msxml2.h>
#include <comdef.h>
#include "define.h"

#pragma comment(lib,"msxml2")

class ZXML
{
	IXMLDOMDocument*	m_pXMLDoc;

public:
							ZXML();
							ZXML( LPCTSTR url );
							~ZXML();

	int						Open( LPCTSTR url );
	int						Close();
	IXMLDOMNodeList*		FindElement( LPCTSTR strElement );
	IXMLDOMNodeList*		FindElement( IXMLDOMNode* pNode, LPCTSTR strElement );
	int						GetElementText( IXMLDOMNode* pNode, LPSTR strRet );
	int						GetAttributeText( IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet );
};

#endif // _ZXML_H_