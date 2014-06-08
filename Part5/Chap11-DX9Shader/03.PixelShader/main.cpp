/**-----------------------------------------------------------------------------
 * \brief 픽셀쉐이더 예제
 * 파일: main.cpp
 *
 * 설명: 초간단 픽셀쉐이더를 사용해서 출력한다.
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
 *  전역변수
 *------------------------------------------------------------------------------
 */
HWND					g_hwnd = NULL;

LPDIRECT3D9             g_pD3D       = NULL; // D3D 디바이스를 생성할 D3D객체변수
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // 렌더링에 사용될 D3D디바이스
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; /// 정점을 보관할 정점버퍼
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; /// 인덱스를 보관할 인덱스버퍼
LPDIRECT3DTEXTURE9      g_pTexture   = NULL; /// 텍스처

LPDIRECT3DPIXELSHADER9	g_pPS = NULL;			/// 픽셀 쉐이더

float					g_xRot = 0.0f;	/// x축 회전
float					g_yRot = 0.0f;	/// y축 회전
D3DXMATRIXA16			g_matWorld;		/// world행렬
D3DXMATRIXA16			g_matView;		/// view행렬
D3DXMATRIXA16			g_matProj;		/// projection행렬

BOOL					g_bWireframe = FALSE;	// 와이어프레임으로 그릴것인가?
BOOL					g_bActive = TRUE;		// 실행중인가?
BOOL					g_bUseVS = FALSE;		// 정점쉐이더를 사용할 것인가?
float					g_fFrames = 0.0f;

/// 사용자 정점을 정의할 구조체
struct MYVERTEX
{
	enum { FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1) };
    D3DXVECTOR3	p;		/// 정점의 변환된 좌표
	DWORD		d;		/// 정점의 색깔
	D3DXVECTOR2	t;		/// 텍스처 좌표
};

struct MYINDEX
{
	WORD	_0, _1, _2;		/// 일반적으로 인덱스는 16비트의 크기를 갖는다.
};

/**-----------------------------------------------------------------------------
 * Direct3D 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
    // 디바이스를 생성하기위한 D3D객체 생성
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // 디바이스를 생성할 구조체
    // 복잡한 오브젝트를 그릴것이기때문에, 이번에는 Z버퍼가 필요하다.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	D3DCAPS9 caps;
	DWORD dwPSProcess;
	// 디바이스의 능력값(caps)을 읽어온다
	g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

	// 지원하는 픽셀쉐이더 버전이 1.0이하라면 REF드라이버를, 1.0이상이면 HW드라이버를 생성한다.
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

    // 기본컬링, CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    // Z버퍼기능을 켠다.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}

/**-----------------------------------------------------------------------------
 * 행렬 설정
 *------------------------------------------------------------------------------
 */
void InitMatrix()
{
	/// 월드행렬
    D3DXMatrixIdentity( &g_matWorld );						/// 월드행렬을 단위행렬으로 설정
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );	/// 디바이스에 월드행렬 설정

    /// 뷰행렬을 설정
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

    /// 프로젝션 행렬 설정
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );
}

void InitTexture()
{
	D3DXCreateTextureFromFile( g_pd3dDevice, "tex.jpg", &g_pTexture );
}

/**-----------------------------------------------------------------------------
 * 정점버퍼를 생성하고 정점값을 채워넣는다.
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

    /// 정점버퍼 생성
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 3*sizeof(MYVERTEX), 0, MYVERTEX::FVF, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    /// 정점버퍼를 값으로 채운다. 
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

	// simple.vs 파일을 읽어와서 정점쉐이더 인터페이스를 생성한다.
	if( FAILED( D3DXAssembleShaderFromFile( "simple.ps", NULL, NULL, 0, &pCode, NULL ) ) )
		return E_FAIL;

	g_pd3dDevice->CreatePixelShader( (DWORD*)pCode->GetBufferPointer(), &g_pPS);

	S_REL( pCode );

	return S_OK;
}

/**-----------------------------------------------------------------------------
 * 기하정보 초기화
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
	g_pLog = new ZFLog( ZF_LOG_TARGET_WINDOW );	// 로깅객체 초기화

	return S_OK;
}

void DeleteObjects()
{
	/// 등록된 클래스 소거
	S_DEL( g_pLog );
}

/**-----------------------------------------------------------------------------
 * 초기화 객체들 소거
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
 * 광원 설정
 *------------------------------------------------------------------------------
 */
VOID SetupLights()
{
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	/// 정점에 색깔이 있으므로 광원off
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00ffffff );	/// 환경광원(ambient light)의 값 설정
}

/**-----------------------------------------------------------------------------
 * Status정보 출력
 *------------------------------------------------------------------------------
 */
void LogStatus( void )
{
	g_pLog->Log( "FillMode:%s", g_bWireframe ? "wireframe" : "Solid");
	g_pLog->Log( "VertexShader:%s", g_bUseVS ? "On" : "Off");
}


/**-----------------------------------------------------------------------------
 * FPS(Frame Per Second)출력
 *------------------------------------------------------------------------------
 */
void LogFPS(void)
{
	static DWORD	nTick = 0;
	static DWORD	nFPS = 0;

	/// 1초가 지났는가?
	if( GetTickCount() - nTick > 1000 )
	{
		nTick = GetTickCount();
		/// FPS값 출력
		g_pLog->Log("FPS:%d", nFPS );

		nFPS = 0;
		LogStatus();	/// 상태정보를 여기서 출력(1초에 한번)
		return;
	}
	nFPS++;
}


/**-----------------------------------------------------------------------------
 * 마우스 입력 처리
 *------------------------------------------------------------------------------
 */
void ProcessMouse( void )
{
}

/**-----------------------------------------------------------------------------
 * 키보드 입력 처리
 *------------------------------------------------------------------------------
 */
void ProcessKey( void )
{
}

/**-----------------------------------------------------------------------------
 * 입력 처리
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
 * 애니메이션 설정
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	D3DXMATRIXA16	matX;
	D3DXMATRIXA16	matY;

	D3DXMatrixRotationX( &matX, g_xRot );
	D3DXMatrixRotationY( &matY, g_yRot );
	g_matWorld = matX * matY;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld ); /// 디바이스에 월드행렬 설정
	SetupLights();
	SetupPS();
	LogFPS();						// 로깅
}


/**-----------------------------------------------------------------------------
 * 화면 그리기
 *------------------------------------------------------------------------------
 */
VOID Render()
{
    /// 후면버퍼와 Z버퍼 초기화
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );

	/// 애니메이션 행렬설정
	Animate();
    /// 렌더링 시작
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		if( g_bUseVS )	// 픽셀 쉐이더를 사용하여 출력?
		{
			/// 정점선언값과 정점 설정
			g_pd3dDevice->SetPixelShader( g_pPS );
			g_pd3dDevice->SetFVF( MYVERTEX::FVF );
			g_pd3dDevice->SetTexture( 0, g_pTexture );
			g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(MYVERTEX) );
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		}
		else
		{
			// fx를 사용한뒤에는 이 값을 NULL로 해야 D3D고정 파이프라인을 사용할 수 있다.
			g_pd3dDevice->SetPixelShader( NULL );
			g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(MYVERTEX) );
			g_pd3dDevice->SetFVF( MYVERTEX::FVF );
			g_pd3dDevice->SetTexture( 0, g_pTexture );
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );	/// MODULATE연산으로 색깔을 섞음
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );	/// 첫번째 섞을색은 텍스쳐 색
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	/// 두번째 섞을색은 정점 색
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );	/// alpha연산은 사용하지 않음
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		}

		g_pd3dDevice->EndScene();
    }

    /// 후면버퍼를 보이는 화면으로!
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




/**-----------------------------------------------------------------------------
 * 윈도우 프로시져
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
 * 프로그램 시작점
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// 윈도우 클래스 등록
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// 윈도우 생성
    HWND hWnd = CreateWindow( "BasicFrame", WINDOW_TITLE,
                              WS_OVERLAPPEDWINDOW, 100, 100, WINDOW_W, WINDOW_H,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	g_hwnd = hWnd;

    /// Direct3D 초기화
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
		if( SUCCEEDED( InitObjects() ) )
		{
			if( SUCCEEDED( InitGeometry() ) )
			{

        		/// 윈도우 출력
				ShowWindow( hWnd, SW_SHOWDEFAULT );
				UpdateWindow( hWnd );

        		/// 메시지 루프
				MSG msg;
				ZeroMemory( &msg, sizeof(msg) );
				while( msg.message!=WM_QUIT )
				{
            		/// 메시지큐에 메시지가 있으면 메시지 처리
					if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
					else /// 처리할 메시지가 없으면 Render()함수 호출
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
