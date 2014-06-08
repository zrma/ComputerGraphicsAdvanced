/**-----------------------------------------------------------------------------
 * \brief �ȼ����̴� ����
 * ����: main.cpp
 *
 * ����: �ʰ��� �ȼ����̴��� ����ؼ� ����Ѵ�.
 *       
 *------------------------------------------------------------------------------
 */

#define MAINBODY

#include <d3d9.h>
#include <d3dx9.h>
#include "ZFLog.h"

#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"Pixel Shader Tutorial"

/**-----------------------------------------------------------------------------
 *  ��������
 *------------------------------------------------------------------------------
 */
HWND					g_hwnd = NULL;

LPDIRECT3D9             g_pD3D       = NULL; // D3D ����̽��� ������ D3D��ü����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // �������� ���� D3D����̽�
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; /// ������ ������ ��������
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; /// �ε����� ������ �ε�������
LPDIRECT3DTEXTURE9      g_pTexture   = NULL; /// �ؽ�ó

LPDIRECT3DPIXELSHADER9	g_pPS = NULL;			/// �ȼ� ���̴�

float					g_xRot = 0.0f;	/// x�� ȸ��
float					g_yRot = 0.0f;	/// y�� ȸ��
D3DXMATRIXA16			g_matWorld;		/// world���
D3DXMATRIXA16			g_matView;		/// view���
D3DXMATRIXA16			g_matProj;		/// projection���

BOOL					g_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?
BOOL					g_bActive = TRUE;		// �������ΰ�?
BOOL					g_bUseVS = FALSE;		// �������̴��� ����� ���ΰ�?
float					g_fFrames = 0.0f;

/// ����� ������ ������ ����ü
struct MYVERTEX
{
	enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
    D3DXVECTOR3	p;		/// ������ ��ȯ�� ��ǥ
	DWORD		d;		/// ������ ����
	D3DXVECTOR2	t;		/// �ؽ�ó ��ǥ
};

struct MYINDEX
{
	WORD	_0, _1, _2;		/// �Ϲ������� �ε����� 16��Ʈ�� ũ�⸦ ���´�.
};

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
	DWORD dwPSProcess;
	// ����̽��� �ɷ°�(caps)�� �о�´�
	g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

	// �����ϴ� �ȼ����̴� ������ 1.0���϶�� REF����̹���, 1.0�̻��̸� HW����̹��� �����Ѵ�.
	dwPSProcess = ( caps.PixelShaderVersion < D3DPS_VERSION(1,0) ) ? 0 : 1;

	if( dwPSProcess )
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
										  D3DCREATE_HARDWARE_VERTEXPROCESSING, 
										  &d3dpp, &g_pd3dDevice ) ) )
			return E_FAIL;
	}
	else
	{
		if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
										  D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
										  &d3dpp, &g_pd3dDevice ) ) )
			return E_FAIL;
	}

    // �⺻�ø�, CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    // Z���۱���� �Ҵ�.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}

/**-----------------------------------------------------------------------------
 * ��� ����
 *------------------------------------------------------------------------------
 */
void InitMatrix()
{
	/// �������
    D3DXMatrixIdentity( &g_matWorld );						/// ��������� ����������� ����
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );	/// ����̽��� ������� ����

    /// ������� ����
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

    /// �������� ��� ����
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );
}

void InitTexture()
{
	D3DXCreateTextureFromFile( g_pd3dDevice, "tex.jpg", &g_pTexture );
}

/**-----------------------------------------------------------------------------
 * �������۸� �����ϰ� �������� ä���ִ´�.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
	MYVERTEX Verts[3];
	Verts[0].p = D3DXVECTOR3(  0.0f,  1.0f, 0.0f );
	Verts[1].p = D3DXVECTOR3(  1.0f, -1.0f, 0.0f );
	Verts[2].p = D3DXVECTOR3( -1.0f, -1.0f, 0.0f );
	Verts[0].d = 0xffff0000;
	Verts[1].d = 0xff00ff00;
	Verts[2].d = 0xff0000ff;
	Verts[0].t = D3DXVECTOR2( 0.5f, 0.0f );
	Verts[1].t = D3DXVECTOR2( 1.0f, 1.0f );
	Verts[2].t = D3DXVECTOR2( 0.0f, 1.0f );

    /// �������� ����
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 3*sizeof(MYVERTEX), 0, MYVERTEX::FVF, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    /// �������۸� ������ ä���. 
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(Verts), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, Verts, sizeof(Verts) );
    g_pVB->Unlock();


	return S_OK;
}

HRESULT InitPS()
{
	LPD3DXBUFFER pCode;

	// simple.vs ������ �о�ͼ� �������̴� �������̽��� �����Ѵ�.
	if( FAILED( D3DXAssembleShaderFromFile( "simple.ps", NULL, NULL, 0, &pCode, NULL ) ) )
		return E_FAIL;

	g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &g_pPS);

	S_REL( pCode );

	return S_OK;
}

/**-----------------------------------------------------------------------------
 * �������� �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	InitMatrix();
	InitTexture();
	InitVB();
	InitPS();
	return S_OK;
}

HRESULT InitObjects()
{
	S_DEL( g_pLog );
	g_pLog = new ZFLog( ZF_LOG_TARGET_WINDOW );	// �α밴ü �ʱ�ȭ

	return S_OK;
}

void DeleteObjects()
{
	/// ��ϵ� Ŭ���� �Ұ�
	S_DEL( g_pLog );
}

/**-----------------------------------------------------------------------------
 * �ʱ�ȭ ��ü�� �Ұ�
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
	S_REL( g_pPS );
	S_REL( g_pIB );
	S_REL( g_pVB );
	S_REL( g_pd3dDevice );
	S_REL( g_pD3D );
}

/**-----------------------------------------------------------------------------
 * ���� ����
 *------------------------------------------------------------------------------
 */
VOID SetupLights()
{
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	/// ������ ������ �����Ƿ� ����off
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00ffffff );	/// ȯ�汤��(ambient light)�� �� ����
}

/**-----------------------------------------------------------------------------
 * Status���� ���
 *------------------------------------------------------------------------------
 */
void LogStatus( void )
{
	g_pLog->Log( "FillMode:%s", g_bWireframe ? "wireframe" : "Solid");
	g_pLog->Log( "VertexShader:%s", g_bUseVS ? "On" : "Off");
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
}

/**-----------------------------------------------------------------------------
 * Ű���� �Է� ó��
 *------------------------------------------------------------------------------
 */
void ProcessKey( void )
{
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

void SetupPS()
{
}

/**-----------------------------------------------------------------------------
 * �ִϸ��̼� ����
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	D3DXMATRIXA16	matX;
	D3DXMATRIXA16	matY;

	D3DXMatrixRotationX( &matX, g_xRot );
	D3DXMatrixRotationY( &matY, g_yRot );
	g_matWorld = matX * matY;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld ); /// ����̽��� ������� ����
	SetupLights();
	SetupPS();
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
		if( g_bUseVS )	// �ȼ� ���̴��� ����Ͽ� ���?
		{
			/// �������𰪰� ���� ����
			g_pd3dDevice->SetPixelShader( g_pPS );
			g_pd3dDevice->SetFVF( MYVERTEX::FVF );
			g_pd3dDevice->SetTexture( 0, g_pTexture );
			g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(MYVERTEX) );
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		}
		else
		{
			// fx�� ����ѵڿ��� �� ���� NULL�� �ؾ� D3D���� ������������ ����� �� �ִ�.
			g_pd3dDevice->SetPixelShader( NULL );
			g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(MYVERTEX) );
			g_pd3dDevice->SetFVF( MYVERTEX::FVF );
			g_pd3dDevice->SetTexture( 0, g_pTexture );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );	/// MODULATE�������� ������ ����
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );	/// ù��° �������� �ؽ��� ��
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	/// �ι�° �������� ���� ��
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );	/// alpha������ ������� ����
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		}

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

		case WM_KEYDOWN : 
			switch( wParam )
			{
				case VK_ESCAPE :
					PostMessage( hWnd, WM_DESTROY, 0, 0L );
					break;
				case '1' :
					g_bWireframe = !g_bWireframe;
					break;
				case '2' : 
					g_bUseVS = !g_bUseVS;
					break;
				case VK_LEFT : g_yRot -= 0.1f; break;
				case VK_RIGHT : g_yRot += 0.1f; break;
				case VK_UP : g_xRot -= 0.1f; break;
				case VK_DOWN : g_xRot += 0.1f; break;
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
