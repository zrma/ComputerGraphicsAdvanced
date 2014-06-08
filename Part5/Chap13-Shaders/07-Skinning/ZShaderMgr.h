#ifndef _ZSHADERMGR_H_
#define _ZSHADERMGR_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"
#include "ZVSMgr.h"
#include "ZPSMgr.h"
#include "ZFXMgr.h"

class ZShaderMgr
{
	ZVSMgr*		m_pVSMgr;
	ZPSMgr*		m_pPSMgr;
	ZFXMgr*		m_pFXMgr;
public:
	ZShaderMgr( LPDIRECT3DDEVICE9 pDev )
	{
		m_pVSMgr = new ZVSMgr( pDev );
		m_pPSMgr = new ZPSMgr( pDev );
		m_pFXMgr = new ZFXMgr( pDev );
	}
	
	~ZShaderMgr()
	{
		S_DEL( m_pVSMgr );
		S_DEL( m_pPSMgr );
		S_DEL( m_pFXMgr );
	}
	
	ZVSMgr* GetVSMgr() { return m_pVSMgr; }
	ZPSMgr* GetPSMgr() { return m_pPSMgr; }
	ZFXMgr* GetFXMgr() { return m_pFXMgr; }
};

#endif // _ZSHADERMGR_H_

