/**-----------------------------------------------------------------------------
 * \brief 높이맵
 * 파일: HeightMap.cpp
 *
 * 설명: HeightMap구현-Triangle List
 *       
 *------------------------------------------------------------------------------
 */
#define MAINBODY

#include <d3d9.h>
#include <d3dx9.h>
#include "ZFLog.h"

#define WINDOW_W		500
#define WINDOW_H		500
#define WINDOW_TITLE	"HeightMap-TList"
#define BMP_HEIGHTMAP	"map128.bmp"
// #define BMP_HEIGHTMAP	"height64.bmp"
// #define BMP_HEIGHTMAP	"height128.bmp"

/**-----------------------------------------------------------------------------
 *  전역변수
 *------------------------------------------------------------------------------
 */
HWND					g_hwnd = NULL;

LPDIRECT3D9             g_pD3D       = NULL; /// D3D 디바이스를 생성할 D3D객체변수
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; /// 렌더링에 사용될 D3D디바이스
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; /// 정점을 보관할 정점버퍼
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; /// 인덱스를 보관할 인덱스버퍼

LPDIRECT3DTEXTURE9		g_pTexHeight = NULL; /// Texture 높이맵
LPDIRECT3DTEXTURE9		g_pTexDiffuse= NULL; /// Texture 색깔맵
LPDIRECT3DTEXTURE9		g_pTexNormal = NULL; /// Texture 노멀맵
D3DXMATRIXA16			g_matAni;

DWORD					g_cxHeight = 0;
DWORD					g_czHeight = 0;

D3DXVECTOR3				g_VecDir;

/// 사용자 정점을 정의할 구조체
struct CUSTOMVERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR2		t;
};

/// 사용자 정점 구조체에 관한 정보를 나타내는 FVF값
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

struct MYINDEX
{
	WORD	_0, _1, _2;		/// WORD, 16비트 인덱스
};

/**-----------------------------------------------------------------------------
 * Direct3D 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
	/// 디바이스를 생성하기위한 D3D객체 생성
	if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return E_FAIL;
	}
	/// 디바이스를 생성할 구조체
	/// 복잡한 오브젝트를 그릴것이기때문에, 이번에는 Z버퍼가 필요하다.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	/// 디바이스 생성
	if ( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	/// 기본컬링, CCW
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	/// Z버퍼기능을 켠다.
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	
	/// 재질(material)설정
	/// 재질은 디바이스에 단 하나만 설정될 수 있다.
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial( &mtrl );

	/// 광원 설정
	D3DLIGHT9 light;									/// 광원 구조체
	ZeroMemory( &light, sizeof( D3DLIGHT9 ) );			/// 구조체를 0으로 지운다.
	light.Type = D3DLIGHT_DIRECTIONAL;					/// 광원의 종류(점 광원,방향성 광원,스포트라이트)
	light.Diffuse.r = 1.0f;								/// 광원의 색깔과 밝기
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	D3DXVECTOR3 vecDir = D3DXVECTOR3( 0, -1, 0 );		/// 광원 고정
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );	/// 광원의 방향을 단위벡터로 만든다.
	light.Range = 1000.0f;											/// 광원이 다다를수 있는 최대거리
	g_pd3dDevice->SetLight( 0, &light );								/// 디바이스에 0번 광원 설치
	g_pd3dDevice->LightEnable( 0, TRUE );							/// 0번 광원을 켠다
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	return S_OK;
}


/**-----------------------------------------------------------------------------
 * 텍스처 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitTexture()
{
	/// 높이맵 텍스처
	/// D3DFMT_X8R8G8B8와 D3DPOOL_MANAGED를 주기위해서 이 함수를 사용했다.
	if ( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, BMP_HEIGHTMAP, 
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, 
		D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &g_pTexHeight ) ) )
	{
		return E_FAIL;
	}

	/// 색깔맵
	if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "tile2.tga", &g_pTexDiffuse ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}

/**-----------------------------------------------------------------------------
 * 정점버퍼를 생성하고 정점값을 채워넣는다.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
	D3DSURFACE_DESC		ddsd;
	D3DLOCKED_RECT		d3drc;

	g_pTexHeight->GetLevelDesc( 0, &ddsd );	/// 텍스처의 정보
	g_cxHeight = ddsd.Width;				/// 텍스처의 가로크기
	g_czHeight = ddsd.Height;				/// 텍스처의 세로크기
	g_pLog->Log( "Texture Size:[%d,%d]", g_cxHeight, g_czHeight );
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( ddsd.Width*ddsd.Height*sizeof( CUSTOMVERTEX ),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	/// 텍스처 메모리 락!
	g_pTexHeight->LockRect( 0, &d3drc, NULL, D3DLOCK_READONLY );
    VOID* pVertices;
	/// 정점버퍼 락!
	if ( FAILED( g_pVB->Lock( 0, g_cxHeight*g_czHeight*sizeof( CUSTOMVERTEX ), (void**)&pVertices, 0 ) ) )
	{
		return E_FAIL;
	}

	CUSTOMVERTEX	v;
	CUSTOMVERTEX*	pV = (CUSTOMVERTEX*)pVertices;

	D3DXVECTOR3	normalX;
	D3DXVECTOR3 normalZ;

	for( DWORD z = 0 ; z < g_czHeight ; z++ )
	{
		for ( DWORD x = 0; x < g_cxHeight; x++ )
		{
			v.p.x = (float)x - g_cxHeight / 2.0f;		/// 정점의 x좌표(메시를 원점에 정렬)
			v.p.z = -( (float)z - g_czHeight / 2.0f );	/// 정점의 z좌표(메시를 원점에 정렬), z축이 모니터안쪽이므로 -를 곱한다.
			v.p.y = ( (float)( *( (LPDWORD)d3drc.pBits + x + z*( d3drc.Pitch / 4 ) ) & 0x000000ff ) ) / 10.0f;	/// DWORD이므로 pitch/4
			v.t.x = (float)x / ( g_cxHeight - 1 );
			v.t.y = (float)z / ( g_czHeight - 1 );
			*pV++ = v;
		}
	}

    g_pVB->Unlock();
	g_pTexHeight->UnlockRect( 0 );

	HRESULT hr;

	// Create normal map texture the size of the original.
	if ( D3DXCreateTexture( g_pd3dDevice, g_cxHeight, g_czHeight, 0, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexNormal ) == D3D_OK )
	{
		g_pLog->Log( "비어있는 텍스쳐 생성! \n" );

		if ( ( hr = D3DXComputeNormalMap( g_pTexNormal, g_pTexHeight, NULL, D3DX_NORMALMAP_MIRROR, D3DX_CHANNEL_LUMINANCE, 1000 ) ) == D3D_OK )
		{
			g_pLog->Log( "노멀 맵 생성 성공! \n" );
		}
	}

    return S_OK;
}

/**-----------------------------------------------------------------------------
 * 인덱스 버퍼 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitIB()
{
	if ( FAILED( g_pd3dDevice->CreateIndexBuffer( ( g_cxHeight - 1 )*( g_czHeight - 1 ) * 2 * sizeof( MYINDEX ), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	MYINDEX		i;
	MYINDEX*	pI;
	if ( FAILED( g_pIB->Lock( 0, ( g_cxHeight - 1 )*( g_czHeight - 1 ) * 2 * sizeof( MYINDEX ), (void**)&pI, 0 ) ) )
	{
		return E_FAIL;
	}
	for ( DWORD z = 0; z < g_czHeight - 1; z++ )
	{
		for ( DWORD x = 0; x < g_cxHeight - 1; x++ )
		{
			i._0 = static_cast<WORD>( z*g_cxHeight + x );
			i._1 = static_cast<WORD>( z*g_cxHeight + x + 1 );
			i._2 = static_cast<WORD>( ( z + 1 )*g_cxHeight + x );
			*pI++ = i;
			i._0 = static_cast<WORD>( ( z + 1 )*g_cxHeight + x );
			i._1 = static_cast<WORD>( z*g_cxHeight + x + 1 );
			i._2 = static_cast<WORD>( ( z + 1 )*g_cxHeight + x + 1 );
			*pI++ = i;
		}
	}
	g_pIB->Unlock();

	return S_OK;
}


/**-----------------------------------------------------------------------------
 * 기하정보 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	if ( FAILED( InitTexture() ) )
	{
		return E_FAIL;
	}
	if ( FAILED( InitVB() ) )
	{
		return E_FAIL;
	}
	if ( FAILED( InitIB() ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


/**-----------------------------------------------------------------------------
 * 카메라 행렬 설정
 *------------------------------------------------------------------------------
 */
void SetupCamera()
{
	/// 월드 행렬 설정
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    /// 뷰 행렬을 설정
    D3DXVECTOR3 vEyePt( 0.0f, 100.0f, -(float)g_czHeight );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    /// 프로젝션 행렬 설정
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

/**-----------------------------------------------------------------------------
 * 광원 설정
 *------------------------------------------------------------------------------
 */
VOID SetupLights()
{
    /// 광원 설정
	g_VecDir = { cosf( GetTickCount() / 350.0f ), 1.0f, sinf( GetTickCount() / 350.0f ) };
	D3DXVec3Normalize( &g_VecDir, &g_VecDir );	/// 광원의 방향을 단위벡터로 만든다.
}

/**-----------------------------------------------------------------------------
 * FPS(Frame Per Second)출력
 *------------------------------------------------------------------------------
 */
void LogFPS( void )
{
	static DWORD	nTick = 0;
	static DWORD	nFPS = 0;

	if ( GetTickCount() - nTick > 1000 )
	{
		nTick = GetTickCount();
		g_pLog->Log( "FPS:%d", nFPS );
		nFPS = 0;
		return;
	}
	nFPS++;
}

/**-----------------------------------------------------------------------------
 * 애니메이션 설정
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	static DWORD t = 0;
	static bool flag = false;
	/// 0 ~ 2PI 까지(0~360도) 값을 변화시킴 Fixed Point기법 사용
	DWORD d = GetTickCount() % ( (int)( ( D3DX_PI * 2 ) * 1000 ) );
	/// Y축 회전행렬
	D3DXMatrixRotationY( &g_matAni, d / 1000.0f );
	// D3DXMatrixIdentity( &g_matAni );

	/// 카메라 행렬설정
	SetupCamera();
	SetupLights();

	if ( d < t )
	{
		flag = !flag;
	}

	g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, flag ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
	t = d;

	LogFPS();
}


/**-----------------------------------------------------------------------------
 * 초기화 객체들 소거
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
	if ( g_pTexHeight != NULL )
	{
		g_pTexHeight->Release();
	}
	if ( g_pTexDiffuse != NULL )
	{
		g_pTexDiffuse->Release();
	}
	if ( g_pIB != NULL )
	{
		g_pIB->Release();
	}
	if ( g_pVB != NULL )
	{
		g_pVB->Release();
	}
	if ( g_pd3dDevice != NULL )
	{
		g_pd3dDevice->Release();
	}
	if ( g_pD3D != NULL )
	{
		g_pD3D->Release();
	}
}


/**-----------------------------------------------------------------------------
 * 메시 그리기
 *------------------------------------------------------------------------------
 */
void DrawMesh( D3DXMATRIXA16* pMat )
{
	g_pd3dDevice->SetTransform( D3DTS_WORLD, pMat );
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
	g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	g_pd3dDevice->SetIndices( g_pIB );
	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, g_cxHeight*g_czHeight, 0, ( g_cxHeight - 1 )*( g_czHeight - 1 ) * 2 );
}

DWORD VectortoRGBA( D3DXVECTOR3* v, FLOAT fHeight )
{
	DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
	DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
	DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
	DWORD a = (DWORD)( 255.0f * fHeight );

	// g_pLog->Log( "R:%d G:%d B:%d A:%d  \n ", r, g, b, a );

	return( ( a << 24L ) + ( r << 16L ) + ( g << 8L ) + ( b << 0L ) );
}

/**-----------------------------------------------------------------------------
 * 화면 그리기
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	/// 후면버퍼와 Z버퍼 초기화
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

	/// 애니메이션 행렬설정
	Animate();
	/// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		g_pd3dDevice->SetTexture( 0, g_pTexNormal );
		g_pd3dDevice->SetTexture( 1, g_pTexDiffuse );							/// 1번 텍스쳐 스테이지에 텍스쳐 고정(색깔맵)
		
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0번 텍스처 스테이지의 확대 필터
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );		/// 0번 텍스처 : 0번 텍스처 인덱스 사용

		g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
		
		DWORD dwFactor = VectortoRGBA( &g_VecDir, 0.0f );	/// 벡터를 RGB로
		g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFactor );	/// RGB로 변환된 벡터를 TextureFactor값으로 등록

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		/// 텍스처의 RGB와 광원벡터를 내적
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3 );		/// 
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );		/// 텍스처와 법선맵을 섞어서 출력
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_ADD );
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		
		DrawMesh( &g_matAni );
		/// 렌더링 종료
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
	switch ( msg )
	{
		case WM_DESTROY:
			Cleanup();
			PostQuitMessage( 0 );
			return 0;
		case WM_KEYDOWN:
			switch ( wParam )
			{
				case VK_ESCAPE:
					PostMessage( hWnd, WM_DESTROY, 0, 0L );
					break;
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
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// 윈도우 생성
	HWND hWnd = CreateWindow( "BasicFrame", WINDOW_TITLE, WS_OVERLAPPEDWINDOW, 100, 100,
							  WINDOW_W, WINDOW_H, GetDesktopWindow(), NULL, wc.hInstance, NULL );

	g_hwnd = hWnd;
	g_pLog = new ZFLog( ZF_LOG_TARGET_WINDOW );
    /// Direct3D 초기화
    if( SUCCEEDED( InitD3D( hWnd ) ) )
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
				else
				{
					/// 처리할 메시지가 없으면 Render()함수 호출
					Render();
				}
			}
		}
    }

	delete g_pLog;
	/// 등록된 클래스 소거
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
