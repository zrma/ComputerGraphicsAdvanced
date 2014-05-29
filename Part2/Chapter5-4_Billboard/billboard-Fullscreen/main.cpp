/**-----------------------------------------------------------------------------
 * \brief ������ó��
 * ����: main.cpp
 *
 * ����: ������ ó������� ��������
 *       
 *------------------------------------------------------------------------------
 */

#define MAINBODY

#include <d3d9.h>
#include <d3dx9.h>
#include "ZCamera.h"
#include "ZFLog.h"
#include "ZWater.h"
#include <new.h>

#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"Billboard"

/**-----------------------------------------------------------------------------
 *  ��������
 *------------------------------------------------------------------------------
 */
HWND					g_hwnd = NULL;

LPDIRECT3D9             g_pD3D       = NULL; // D3D ����̽��� ������ D3D��ü����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // �������� ���� D3D����̽�
LPDIRECT3DTEXTURE9		g_pTexBillboard[4] = { NULL, NULL, NULL, NULL };// ������� ����� �ؽ�ó

D3DXMATRIXA16			g_matAni;
D3DXMATRIXA16			g_matWorld;
D3DXMATRIXA16			g_matView;
D3DXMATRIXA16			g_matProj;

DWORD					g_dwMouseX = 0;			// ���콺�� ��ǥ
DWORD					g_dwMouseY = 0;			// ���콺�� ��ǥ
BOOL					g_bBillboard = TRUE;	// ������ó���� �Ұ��ΰ�?
BOOL					g_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?

ZCamera*				g_pCamera = NULL;	// Camera Ŭ����
ZWater*					g_pWater = NULL;

/**-----------------------------------------------------------------------------
 * Direct3D �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
    // ����̽��� �����ϱ����� D3D��ü ����
	if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return E_FAIL;
	}

    // ����̽��� ������ ����ü
    // ������ ������Ʈ�� �׸����̱⶧����, �̹����� Z���۰� �ʿ��ϴ�.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    /// ����̽� ����
	if ( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )
	{
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
	/// ���� ��� ����
	D3DXMatrixIdentity( &g_matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

    /// �� ����� ����
    D3DXVECTOR3 vEyePt( 0.0f, 5.0f, (float)-3.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 5.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

    /// ���� �������� ���
	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	/// �������� �ø��� �������� ���
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 200.0f );

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
	// ������� ����� �ؽ�ó �̹���
	D3DXCreateTextureFromFile( g_pd3dDevice, "tree01S.dds", &g_pTexBillboard[0] );
	D3DXCreateTextureFromFile( g_pd3dDevice, "tree02S.dds", &g_pTexBillboard[1] );
	D3DXCreateTextureFromFile( g_pd3dDevice, "tree35S.dds", &g_pTexBillboard[2] );
	D3DXCreateTextureFromFile( g_pd3dDevice, "explosion.jpg", &g_pTexBillboard[3] );


	// ������ ���콺 ��ġ ����
	POINT	pt;
	GetCursorPos( &pt );
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
	return S_OK;
}

HRESULT InitObjects()
{
	g_pLog = new ZFLog( ZF_LOG_TARGET_WINDOW );

	// g_pCamera = new ZCamera;
	g_pCamera = (ZCamera*)_aligned_malloc( sizeof( ZCamera ), 16 );
	new ( g_pCamera ) ZCamera();

	g_pWater = new ZWater;
	g_pWater->Create( g_pd3dDevice, 64, 64, 100 );

	return S_OK;
}

void DeleteObjects()
{
	/// ��ϵ� Ŭ���� �Ұ�
	S_DEL( g_pWater );
	S_DEL( g_pLog );
	
	// S_DEL( g_pCamera );

	if ( g_pCamera )
	{
		g_pCamera->~ZCamera();
		_aligned_free( g_pCamera );
	}
}

/**-----------------------------------------------------------------------------
 * �ʱ�ȭ ��ü�� �Ұ�
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
	for ( int i = 0; i < 4; i++ )
	{
		S_REL( g_pTexBillboard[i] );
	}
	S_REL( g_pd3dDevice );
	S_REL( g_pD3D );
}


/**-----------------------------------------------------------------------------
 * ���� ����
 *------------------------------------------------------------------------------
 */
VOID SetupLights()
{
    /// ����(material)����
    /// ������ ����̽��� �� �ϳ��� ������ �� �ִ�.
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );			/// ���������� �Ҵ�

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );		/// ȯ�汤��(ambient light)�� �� ����
}

/**-----------------------------------------------------------------------------
 * Status���� ���
 *------------------------------------------------------------------------------
 */
void LogStatus( void )
{
	g_pLog->Log( "Wireframe:%d", g_bWireframe );
	g_pLog->Log( "BillBoard:%d", g_bBillboard );
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
	if ( GetAsyncKeyState( 'A' ) )
	{
		g_pCamera->MoveLocalZ( 0.5f );	// ī�޶� ����!
	}
	if ( GetAsyncKeyState( 'Z' ) )
	{
		g_pCamera->MoveLocalZ( -0.5f );	// ī�޶� ����!
	}
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
	D3DXMatrixIdentity( &g_matAni );

	SetupLights();
	ProcessInputs();

	LogFPS();
}

void DrawBillboard()
{
	static int nStep = 0;
	static DWORD	nTick = 0;

	// ����ä���� ����ؼ� �����ؽ�ó ȿ���� ����
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0x08 );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	struct UV
	{
		float u, v;
	};

	struct MYVERTEX
	{
		enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 };
		float px, py, pz;
		UV uv;
	};

	// ������ ����
	static MYVERTEX vtx[4] =
	{
		{ -1, 0, 0, 0, 1 },
		{ -1, 4, 0, 0, 0 },
		{ 1, 0, 0, 1, 1 },
		{ 1, 4, 0, 1, 0 }
	};
	static UV tblUV[16][4] =
	{
		{ { 0.00f, 0.25f }, { 0.00f, 0.00f }, { 0.25f, 0.25f }, { 0.25f, 0.00f } },
		{ { 0.25f, 0.25f }, { 0.25f, 0.00f }, { 0.50f, 0.25f }, { 0.50f, 0.00f } },
		{ { 0.50f, 0.25f }, { 0.50f, 0.00f }, { 0.75f, 0.25f }, { 0.75f, 0.00f } },
		{ { 0.75f, 0.25f }, { 0.75f, 0.00f }, { 1.00f, 0.25f }, { 1.00f, 0.00f } },

		{ { 0.00f, 0.50f }, { 0.00f, 0.25f }, { 0.25f, 0.50f }, { 0.25f, 0.25f } },
		{ { 0.25f, 0.50f }, { 0.25f, 0.25f }, { 0.50f, 0.50f }, { 0.50f, 0.25f } },
		{ { 0.50f, 0.50f }, { 0.50f, 0.25f }, { 0.75f, 0.50f }, { 0.75f, 0.25f } },
		{ { 0.75f, 0.50f }, { 0.75f, 0.25f }, { 1.00f, 0.50f }, { 1.00f, 0.25f } },

		{ { 0.00f, 0.75f }, { 0.00f, 0.50f }, { 0.25f, 0.75f }, { 0.25f, 0.50f } },
		{ { 0.25f, 0.75f }, { 0.25f, 0.50f }, { 0.50f, 0.75f }, { 0.50f, 0.50f } },
		{ { 0.50f, 0.75f }, { 0.50f, 0.50f }, { 0.75f, 0.75f }, { 0.75f, 0.50f } },
		{ { 0.75f, 0.75f }, { 0.75f, 0.50f }, { 1.00f, 0.75f }, { 1.00f, 0.50f } },

		{ { 0.00f, 1.00f }, { 0.00f, 0.75f }, { 0.25f, 1.00f }, { 0.25f, 0.75f } },
		{ { 0.25f, 1.00f }, { 0.25f, 0.75f }, { 0.50f, 1.00f }, { 0.50f, 0.75f } },
		{ { 0.50f, 1.00f }, { 0.50f, 0.75f }, { 0.75f, 1.00f }, { 0.75f, 0.75f } },
		{ { 0.75f, 1.00f }, { 0.75f, 0.75f }, { 1.00f, 1.00f }, { 1.00f, 0.75f } },
	};

	D3DXMATRIXA16	matBillboard;
	D3DXMatrixIdentity( &matBillboard );


	if ( ( GetTickCount() - nTick ) > 100 )
	{
		nStep = ( nStep++ ) % 16;
		nTick = GetTickCount();
	}

	// 0�� �ؽ�ó�� ������ �ؽ�ó�� �ø���
	g_pd3dDevice->SetTexture( 1, NULL );
	g_pd3dDevice->SetFVF( MYVERTEX::FVF );

	// Y�� ������
	if ( g_bBillboard )
	{
		// Y�� ȸ������� _11, _13, _31, _33�� ��Ŀ� ȸ������ ����
		// ī�޶��� Y�� ȸ����İ��� �о ������� ����� X,Z���� ������
		// Y�� ȸ�� ������ ����� ����� �ִ�
		matBillboard._11 = g_pCamera->GetViewMatrix()->_11;
		matBillboard._13 = g_pCamera->GetViewMatrix()->_13;
		matBillboard._31 = g_pCamera->GetViewMatrix()->_31;
		matBillboard._33 = g_pCamera->GetViewMatrix()->_33;
		D3DXMatrixInverse( &matBillboard, NULL, &matBillboard );
	}

	// �������� ��ǥ�� �ٲ㰡�� ������ ���� ��´�
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	for ( int z = 0; z <= 40; z += 5 )
	{
		for ( int x = 0; x <= 40; x += 5 )
		{
			matBillboard._41 = static_cast<float>( x - 20 );
			matBillboard._42 = 0;
			matBillboard._43 = static_cast<float>( z - 20 );
			g_pd3dDevice->SetTexture( 0, g_pTexBillboard[( x + z ) % 4] );
			if ( ( x + z ) % 4 == 3 )
				continue;
			else
			{
				vtx[0].uv.u = 0;
				vtx[0].uv.v = 1;
				vtx[1].uv.u = 0;
				vtx[1].uv.v = 0;
				vtx[2].uv.u = 1;
				vtx[2].uv.v = 1;
				vtx[3].uv.u = 1;
				vtx[3].uv.v = 0;
				g_pd3dDevice->SetTransform( D3DTS_WORLD, &matBillboard );
				g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vtx, sizeof( MYVERTEX ) );
			}
		}
	}

	// �������� ��ǥ�� �ٲ㰡�� ���� �̹����� ��´�
	g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
	g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	// ��ü ȭ�� ������
	if ( g_bBillboard )
	{
		matBillboard = *g_pCamera->GetViewMatrix();
		D3DXMatrixInverse( &matBillboard, NULL, &matBillboard );
	}
	// for ( int z = 0; z <= 40; z += 5 )
	for ( int z = 40; z >= 0; z -= 5 )
	{
		for ( int x = 40; x >= 0; x -= 5 )
		{
			matBillboard._41 = static_cast<float>( x - 20 );
			matBillboard._42 = 0;
			matBillboard._43 = static_cast<float>( z - 20 );
			g_pd3dDevice->SetTexture( 0, g_pTexBillboard[( x + z ) % 4] );
			if ( ( x + z ) % 4 == 3 )
			{
				vtx[0].uv = tblUV[nStep][0];
				vtx[1].uv = tblUV[nStep][1];
				vtx[2].uv = tblUV[nStep][2];
				vtx[3].uv = tblUV[nStep][3];
				g_pd3dDevice->SetTransform( D3DTS_WORLD, &matBillboard );
				g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vtx, sizeof( MYVERTEX ) );
			}
		}
	}

	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
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
		g_pWater->Draw();

		DrawBillboard();
		/// ������ ����
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
					g_bBillboard = !g_bBillboard;
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

	srand( GetTickCount() );

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
					else
					/// ó���� �޽����� ������ Render()�Լ� ȣ��
						Render();
				}
			}
		}
    }

	DeleteObjects();
    UnregisterClass( "BasicFrame", wc.hInstance );
    return 0;
}
