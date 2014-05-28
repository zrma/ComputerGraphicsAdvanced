﻿/**-----------------------------------------------------------------------------
 * \brief 라이트매핑 기법의 이해
 * 파일: LightMap.cpp
 *
 * 설명: 라이트맵핑이란 Quake류의 게임에서 벽면등에 실제 광원을 사용하지 않고
 *       마치 광원이 있는것 같은 효과를 내는 기법을 말한다.
 *       실제 게임에서는 벽면을 BSP트리로 분할하면서 광원들과의 연산을 통해서
 *       라이트맵을 생성하지만 여기서는 만들어진 라이트맵을 사용하였다.
 *       
 *------------------------------------------------------------------------------
 */

#include <d3d9.h>
#include <d3dx9.h>



/**-----------------------------------------------------------------------------
 *  전역변수
 *------------------------------------------------------------------------------
 */
LPDIRECT3D9             g_pD3D       = NULL; /// D3D 디바이스를 생성할 D3D객체변수
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; /// 렌더링에 사용될 D3D디바이스
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; /// 정점을 보관할 정점버퍼
LPDIRECT3DTEXTURE9		g_pTex0		 = NULL; /// Texture 0(벽면)
LPDIRECT3DTEXTURE9		g_pTex1		 = NULL; /// Texture 1(라이트맵)

D3DXMATRIXA16			g_matAni;

/// 사용자 정점을 정의할 구조체
struct CUSTOMVERTEX
{
    FLOAT x, y, z;	/// 정점의 변환된 좌표
    DWORD color;	/// 정점의 색깔
	FLOAT u,v;
};

/// 사용자 정점 구조체에 관한 정보를 나타내는 FVF값
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0))


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
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
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

    /// 컬링기능을 끈다.
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    /// Z버퍼기능을 켠다.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    /// 정점에 색깔값이 있으므로, 광원기능을 끈다.
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}




/**-----------------------------------------------------------------------------
 * 정점버퍼를 생성하고 정점값을 채워넣는다.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
    /// 상자(cube)를 렌더링하기위해 8개의 정점을 선언
    CUSTOMVERTEX vertices[] =
    {
		{ -1,  1, 0 , 0xffffffff, 0, 0 },		/// v0
		{  1,  1, 0 , 0xffffffff, 1, 0 },		/// v1
		{ -1, -1, 0 , 0xffffffff, 0, 1 },		/// v2
		{  1, -1, 0 , 0xffffffff, 1, 1 },		/// v3
    };

    /// 정점버퍼 생성
    /// 4개의 사용자정점을 보관할 메모리를 할당한다.
    /// FVF를 지정하여 보관할 데이터의 형식을 지정한다.
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

    /// 정점버퍼를 값으로 채운다. 
    /// 정점버퍼의 Lock()함수를 호출하여 포인터를 얻어온다.
    VOID* pVertices;
	if ( FAILED( g_pVB->Lock( 0, sizeof( vertices ), (void**)&pVertices, 0 ) ) )
	{
		return E_FAIL;
	}
    memcpy( pVertices, vertices, sizeof(vertices) );
    g_pVB->Unlock();

    return S_OK;
}

HRESULT InitTexture()
{
	if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "env2.bmp", &g_pTex0 ) ) )
	{
		return E_FAIL;
	}
	if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "spotlite.bmp", &g_pTex1 ) ) )
	{
		return E_FAIL;
	}
	return S_OK;
}

/**-----------------------------------------------------------------------------
 * 기하정보 초기화
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	if ( FAILED( InitVB() ) )
	{
		return E_FAIL;
	}
	if ( FAILED( InitTexture() ) )
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
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-3.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    /// 프로젝션 행렬 설정
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

/**-----------------------------------------------------------------------------
 * 애니메이션 행렬생성
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	D3DXMatrixIdentity( &g_matAni );
//	D3DXMatrixRotationZ( &g_matAni, GetTickCount()/500.0f ); 	/// 메시의 Z축 회전행렬
}



/**-----------------------------------------------------------------------------
 * 초기화 객체들 소거
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
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
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}

/**-----------------------------------------------------------------------------
 * 화면 그리기
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// 후면버퍼와 Z버퍼 초기화
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	/// 애니메이션 행렬설정
	Animate();
    /// 렌더링 시작
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        g_pd3dDevice->SetTexture( 0, g_pTex0 );		/// 0번 텍스쳐 스테이지에 텍스쳐 고정(벽면)
        g_pd3dDevice->SetTexture( 1, g_pTex1 );		/// 1번 텍스쳐 스테이지에 텍스쳐 고정(라이트맵)

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );	/// 0번 텍스처 : 0번 텍스처 인덱스 사용
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );	/// 1번 텍스처 : 0번 텍스처 인덱스 사용
		g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0번 텍스처 스테이지의 확대 필터
		g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 1번 텍스처 스테이지의 확대 필터
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );	/// MODULATE연산으로 색깔을 섞음
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

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
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
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
    HWND hWnd = CreateWindow( "BasicFrame", "LightMapping",
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    /// Direct3D 초기화
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
			/// 카메라 행렬설정
			SetupCamera();

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

	/// 등록된 클래스 소거
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
