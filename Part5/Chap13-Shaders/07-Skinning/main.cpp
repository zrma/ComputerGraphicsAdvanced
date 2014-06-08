/**-----------------------------------------------------------------------------
 * \brief ĳ���� �ִϸ��̼�
 * ����: main.cpp
 *
 * ����: �������̴��� ����� ��Ű��
 *       
 *------------------------------------------------------------------------------
 */

#define MAINBODY

#include <d3d9.h>
#include <d3dx9.h>
#include "ZCamera.h"
#include "ZFLog.h"
#include "ZCParser.h"
#include "ZCParsedData.h"
#include "ZDefine.h"
#include "ZNodeMgr.h"
#include "ZShaderMgr.h"

#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"Vertex Shader Skinning"
#define FNAME_XML		"guard.xml"

#define DEF_SKINMETHOD	ZNodeMgr::SKINMETHOD_VS

/**-----------------------------------------------------------------------------
 *  ��������
 *------------------------------------------------------------------------------
 */
HWND					g_hwnd = NULL;

LPDIRECT3D9             g_pD3D       = NULL; // D3D ����̽��� ������ D3D��ü����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // �������� ���� D3D����̽�

D3DXMATRIXA16			g_matWorld;
D3DXMATRIXA16			g_matView;
D3DXMATRIXA16			g_matProj;

float					g_xRot = 0.0f;
float					g_yRot = 0.0f;
DWORD					g_dwMouseX = 0;			// ���콺�� ��ǥ
DWORD					g_dwMouseY = 0;			// ���콺�� ��ǥ
BOOL					g_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?
BOOL					g_bAnimate = FALSE;
int						g_nDrawMode = 2;		// 0 = all, 1 = bone, 2 = mesh
BOOL					g_bBBox = FALSE;
BOOL					g_bActive = TRUE;
BOOL					g_bLight = TRUE;

float					g_fFrames = 0.0f;

ZCamera*				g_pCamera = NULL;	// Camera Ŭ����
ZNodeMgr*				g_pNodeMgr = NULL;	// ��带 �����ϴ� Node ManagerŬ����
ZShaderMgr*				g_pShaderMgr = NULL;// ���̴��� �����ϴ� Ŭ����

/**-----------------------------------------------------------------------------
 * Direct3D �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
    // ����̽��� �����ϱ����� D3D��ü ����
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // ����̽��� ������ ����ü
    // ������ ������Ʈ�� �׸����̱⶧����, �̹����� Z���۰� �ʿ��ϴ�.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	D3DCAPS9 caps;
	DWORD dwVSProcess;
	// ����̽��� �ɷ°�(caps)�� �о�´�
	g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

	// �����ϴ� �������̴� ������ 1.0���϶�� SW���̴���, 1.0�̻��̸� HW���̴��� �����Ѵ�.
	dwVSProcess = ( caps.VertexShaderVersion < D3DVS_VERSION(1,0) ) ? D3DCREATE_SOFTWARE_VERTEXPROCESSING : D3DCREATE_HARDWARE_VERTEXPROCESSING;

    /// ����̽� ����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      dwVSProcess, 
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // �⺻�ø�, CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    // Z���۱���� �Ҵ�.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	

    return S_OK;
}

/**-----------------------------------------------------------------------------
 * ��� ����
 *------------------------------------------------------------------------------
 */
void InitMatrix()
{
	/// ���� ��� ����
	D3DXMatrixIdentity( &g_matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

    /// �� ����� ����
    D3DXVECTOR3 vEyePt( 0.0f, g_pNodeMgr->GetRadius(), -(g_pNodeMgr->GetRadius() * 2.0f) );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

    /// ���� �������� ���
	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 5000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	/// ī�޶� �ʱ�ȭ
	g_pCamera->SetView( &vEyePt, &vLookatPt, &vUpVec );
}

/**-----------------------------------------------------------------------------
 * �������� �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	InitMatrix();

	// ������ ���콺 ��ġ ����
	POINT	pt;
	GetCursorPos( &pt );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
	g_xRot = 0.0f;
	g_yRot = 0.0f;
	return S_OK;
}

HRESULT InitShader()
{
	// ���̴� �Ŵ��� ��ü ����
	g_pShaderMgr = new ZShaderMgr( g_pd3dDevice );

	// ���� ���̴�(skin.vs)����
	if( !g_pShaderMgr->GetVSMgr()->Add( FNAME_VS_SKIN, ZVSSkinnedVertex::FVF ) )
		return E_FAIL;

	return S_OK;
}

HRESULT LoadXML( char* fname )
{
	ZCParser*		pParser = NULL;		// �ļ���ü
	ZCParsedData*	pData = NULL;		// �Ľ̵����� ��ü

	pParser = new ZCParser();			// �ļ� �ʱ�ȭ
	pData = new ZCParsedData();			// �Ľ̵����� �ʱ�ȭ
	if( !pParser->Parse( pData, fname ) ) return E_FAIL;	// �ļ� �۵�!

	list<ZCMesh*>::iterator		it;	// STL�ݺ��� ����
	for( it = pData->m_meshList.begin() ; it != pData->m_meshList.end() ; it++ )
	{
		g_pLog->Log( "-------------------------------------" );
		g_pLog->Log( "ID:[%d,%s]", (*it)->m_nObjectID, pData->m_info.strObjectNameTable[(*it)->m_nObjectID].c_str() );
		g_pLog->Log( "Parent:[%d,%s]", (*it)->m_nParentID, (*it)->m_nParentID == -1 ? "undefined" : pData->m_info.strObjectNameTable[(*it)->m_nParentID].c_str() );
		g_pLog->Log( "Vertex:[%d]", (*it)->m_vertex.size() );
		g_pLog->Log( "Index:[%d]", (*it)->m_triindex.size() );
		g_pLog->Log( "MergedVertex:[%d]", (*it)->m_vtxFinal.size() );
		g_pLog->Log( "Skin:[%s]", (*it)->m_isSkinned ? "true" : "false" );
		g_pLog->Log( "-------------------------------------" );
	}

	S_DEL( g_pNodeMgr );
	g_pNodeMgr = new ZNodeMgr( g_pd3dDevice, pData, DEF_SKINMETHOD, g_pShaderMgr );	//��� �Ŵ��� ����

	g_fFrames = g_pNodeMgr->GetInfo()->fAnimationStart;

	S_DEL( pData );		// �ļ���ü ����
	S_DEL( pParser );	// �Ľ̵����� ��ü ����

	g_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?
	g_bAnimate = FALSE;
	g_nDrawMode = 2;		// 0 = all, 1 = bone, 2 = mesh
	g_bBBox = FALSE;
	g_bLight = TRUE;
	InitGeometry();

	return S_OK;
}

HRESULT InitObjects()
{
	S_DEL( g_pCamera );
	S_DEL( g_pLog );

	g_pCamera = new ZCamera;			// ī�޶� �ʱ�ȭ
	g_pLog = new ZFLog( ZF_LOG_TARGET_WINDOW );	// �α밴ü �ʱ�ȭ
	if( FAILED( InitShader() ) ) return E_FAIL;

	return LoadXML( FNAME_XML );
}

void DeleteObjects()
{
	/// ��ϵ� Ŭ���� �Ұ�
	S_DEL( g_pNodeMgr );
	S_DEL( g_pCamera );
	S_DEL( g_pLog );
	S_DEL( g_pShaderMgr );
}

/**-----------------------------------------------------------------------------
 * �ʱ�ȭ ��ü�� �Ұ�
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )       
        g_pD3D->Release();
}


/**-----------------------------------------------------------------------------
 * ���� ����
 *------------------------------------------------------------------------------
 */
VOID SetupLights()
{
	D3DXVECTOR3 vecDir;									/// ���⼺ ����(directional light)�� ���� ���� ����
	D3DLIGHT9 light;									/// ���� ����ü
	/// ���� ����
	if( g_bLight )
	{
		ZeroMemory( &light, sizeof(D3DLIGHT9) );			/// ����ü�� 0���� �����.
		light.Type       = D3DLIGHT_DIRECTIONAL;			/// ������ ����(�� ����,���⼺ ����,����Ʈ����Ʈ)
		light.Diffuse.r  = 1.0f;							/// ������ ����� ���
		light.Diffuse.g  = 1.0f;
		light.Diffuse.b  = 1.0f;
		vecDir = D3DXVECTOR3( 0, 0, -1 );
		D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );	/// ������ ������ �������ͷ� �����.
		light.Range       = 1000.0f;									/// ������ �ٴٸ��� �ִ� �ִ�Ÿ�
		g_pd3dDevice->SetLight( 0, &light );							/// ����̽��� 0�� ���� ��ġ
		g_pd3dDevice->LightEnable( 0, TRUE );							/// 0�� ������ �Ҵ�
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );			/// ���������� �Ҵ�
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00a0a0a0 );		/// ȯ�汤��(ambient light)�� �� ����
	}
	else
	{
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00ffffff );		/// ȯ�汤��(ambient light)�� �� ����
	}

	// ������ skin���̴��� 200��°�� ����(projection)����� �ʿ��ϴ�.
	D3DXMATRIXA16	m;
	D3DXMatrixTranspose( &m, &g_matProj );
	g_pd3dDevice->SetVertexShaderConstantF( 200, (float*)&m, 4 );

	// ������ skin���̴��� 204��°�� ī�޶�(view)����� �ʿ��ϴ�.
	D3DXMatrixTranspose( &m, &g_matView );
	g_pd3dDevice->SetVertexShaderConstantF( 204, (float*)&m, 3 );
	
	// ������ skin���̴��� 207��°�� �������Ͱ� �ʿ��ϴ�.(������ ����Ʈ ���̵�)
	g_pd3dDevice->SetVertexShaderConstantF( 207, (float*)&vecDir, 1 );
}
/**-----------------------------------------------------------------------------
 * Status���� ���
 *------------------------------------------------------------------------------
 */
void LogStatus( void )
{
	g_pLog->Log( "FillMode:%s", g_bWireframe ? "wireframe" : "Solid");
	g_pLog->Log( "Animation:%s", g_bAnimate ? "On" : "Off" );
	g_pLog->Log( "DrawMode:%s", g_nDrawMode == 0 ? "All" : g_nDrawMode == 1 ? "Bone" : "Mesh" );
	g_pLog->Log( "BoundingBox:%s", g_bBBox ? "On" : "Off" );
}


/**-----------------------------------------------------------------------------
 * FPS(Frame Per Second)���
 *------------------------------------------------------------------------------
 */
void LogFPS(void)
{
	static DWORD	nTick = 0;
	static DWORD	nFPS = 0;

	/// 1�ʰ� �����°�?
	if( GetTickCount() - nTick > 1000 )
	{
		nTick = GetTickCount();
		/// FPS�� ���
		g_pLog->Log("FPS:%d", nFPS );

		nFPS = 0;
		LogStatus();	/// ���������� ���⼭ ���(1�ʿ� �ѹ�)
		return;
	}
	nFPS++;
}


/**-----------------------------------------------------------------------------
 * ���콺 �Է� ó��
 *------------------------------------------------------------------------------
 */
void ProcessMouse( void )
{
	POINT	pt;
	float	fDelta = 0.001f;	// ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.

	GetCursorPos( &pt );
	int dx = pt.x - g_dwMouseX;	// ���콺�� ��ȭ��
	int dy = pt.y - g_dwMouseY;	// ���콺�� ��ȭ��

	g_pCamera->RotateLocalX( dy * fDelta );	// ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
	g_pCamera->RotateLocalY( dx * fDelta );	// ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����
	D3DXMATRIXA16*	pmatView = g_pCamera->GetViewMatrix();		// ī�޶� ����� ��´�.
	g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );			// ī�޶� ��� ����
	g_matView = *pmatView;


	// ���콺�� �������� �߾����� �ʱ�ȭ
//	SetCursor( NULL );	// ���콺�� ��Ÿ���� �ʰ� �ʴ�.
	RECT	rc;
	GetClientRect( g_hwnd, &rc );
	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen( g_hwnd, &pt );
	SetCursorPos( pt.x, pt.y );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
}

/**-----------------------------------------------------------------------------
 * Ű���� �Է� ó��
 *------------------------------------------------------------------------------
 */
void ProcessKey( void )
{
	if( GetAsyncKeyState( 'A' ) ) g_pCamera->MoveLocalZ( 0.5f );	// ī�޶� ����!
	if( GetAsyncKeyState( 'Z' ) ) g_pCamera->MoveLocalZ( -0.5f );	// ī�޶� ����!
}

/**-----------------------------------------------------------------------------
 * �Է� ó��
 *------------------------------------------------------------------------------
 */
void ProcessInputs( void )
{
	ProcessMouse();
	ProcessKey();
}

/**-----------------------------------------------------------------------------
 * �ִϸ��̼� ����
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	D3DXMATRIXA16	matTM;
	D3DXMATRIXA16	matX;
	D3DXMATRIXA16	matY;

	ZObjectInfo*	pInfo = g_pNodeMgr->GetInfo();

	ProcessInputs();				// �Է�ó��
	SetupLights();					// ��������
	D3DXMatrixRotationX( &matX, g_xRot );
	D3DXMatrixRotationY( &matY, g_yRot );
	matTM = matX * matY;
//	D3DXMatrixScaling( &matTM, 0.1f, 0.1f, 0.1f );
	g_pNodeMgr->SetTM( &matTM );
	g_pNodeMgr->Animate( g_fFrames );		// ��� �ִϸ��̼� ��Ű��
	if( g_bAnimate )
	{
		g_fFrames += 1.0f;
		if( g_fFrames > pInfo->fAnimationEnd ) g_fFrames = pInfo->fAnimationStart;
	}

	LogFPS();						// �α�
}


/**-----------------------------------------------------------------------------
 * ȭ�� �׸���
 *------------------------------------------------------------------------------
 */
VOID Render()
{
    /// �ĸ���ۿ� Z���� �ʱ�ȭ
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );

	/// �ִϸ��̼� ��ļ���
	Animate();
    /// ������ ����
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		if( g_nDrawMode != 4 )
		{
			g_pNodeMgr->Draw( g_nDrawMode );		// ��� �׸���
		}
		
		if( g_bBBox )
			g_pNodeMgr->DrawBBox( g_nDrawMode );	// ������ �׸���

		g_pd3dDevice->EndScene();
    }

    /// �ĸ���۸� ���̴� ȭ������!
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




/**-----------------------------------------------------------------------------
 * ������ ���ν���
 *------------------------------------------------------------------------------
 */
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY :
            Cleanup();
            PostQuitMessage( 0 );
            return 0;

		case WM_RBUTTONDOWN :
		{
			g_bActive = FALSE;

			char	str[512];
			OPENFILENAME ofn = { sizeof(OPENFILENAME), hWnd, NULL,
								 "Mesh Files(*.xml)\0*.xml\0\0",
								 NULL, 0, 1, NULL, 512, str, 512,
								 NULL, "Open XML File", OFN_FILEMUSTEXIST, 0, 1,
								 ".xml", 0, NULL, NULL };

			if( GetOpenFileName( &ofn ) )
				LoadXML( str );

			g_bActive = TRUE;
		}
			break;

		case WM_KEYDOWN : 
			switch( wParam )
			{
				case VK_ESCAPE :
					PostMessage( hWnd, WM_DESTROY, 0, 0L );
					break;
				case VK_LEFT : g_yRot -= 0.1f; break;
				case VK_RIGHT : g_yRot += 0.1f; break;
				case VK_UP : g_xRot -= 0.1f; break;
				case VK_DOWN : g_xRot += 0.1f; break;
				case '5' : 
					g_bLight = !g_bLight;
					break;
				case '1' :
					g_bWireframe = !g_bWireframe;
					break;
				case '2' :
					++g_nDrawMode;
					g_nDrawMode %= 3;
					break;
				case '3' :
					g_bBBox = !g_bBBox;
					break;
				case '4' :
					g_bAnimate = !g_bAnimate;
					break;
			}
			break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




/**-----------------------------------------------------------------------------
 * ���α׷� ������
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// ������ Ŭ���� ���
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// ������ ����
    HWND hWnd = CreateWindow( "BasicFrame", WINDOW_TITLE,
                              WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	g_hwnd = hWnd;

    /// Direct3D �ʱ�ȭ
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
		if( SUCCEEDED( InitObjects() ) )
		{
			if( SUCCEEDED( InitGeometry() ) )
			{

        		/// ������ ���
				ShowWindow( hWnd, SW_SHOWDEFAULT );
				UpdateWindow( hWnd );

        		/// �޽��� ����
				MSG msg;
				ZeroMemory( &msg, sizeof(msg) );
				while( msg.message!=WM_QUIT )
				{
            		/// �޽���ť�� �޽����� ������ �޽��� ó��
					if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
					else /// ó���� �޽����� ������ Render()�Լ� ȣ��
					{
						if( g_bActive ) Render();
					}
				}
			}
		}
    }

	DeleteObjects();
    UnregisterClass( "BasicFrame", wc.hInstance );
    return 0;
}
