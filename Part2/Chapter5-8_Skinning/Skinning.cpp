/**-----------------------------------------------------------------------------
 * \brief D3D API를 사용한 스키닝 예제
 * 파일: Skinning.cpp
 *
 * 설명: D3D의 기본 API기능을 사용한 매트릭스 팔레트 방식 스키닝
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
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; /// 인덱스를 보관할 인덱스버퍼
LPDIRECT3DTEXTURE9      g_pTexture   = NULL; /// 텍스처

D3DXMATRIXA16			g_mat0;	/// 0번 행렬
D3DXMATRIXA16			g_mat1;	/// 1번 행렬
D3DXMATRIXA16			g_mat2;	/// 1번 행렬

/// 사용자 정점을 정의할 구조체
struct CUSTOMVERTEX
{
    D3DXVECTOR3	position;		/// 정점의 변환된 좌표
	FLOAT		b[3];			/// blend weight
	DWORD		index;			/// blend index
    DWORD		color;			/// 정점의 색깔
	FLOAT		tu,tv;			/// 텍스처 좌표
};

/// 사용자 정점 구조체에 관한 정보를 나타내는 FVF값
/// D3DFVF_XYZB4 : 4개의 blend값 
/// D3DFVF_LASTBETA_UBYTE4 : 마지막 DWORD index값은 unisgned byte형 4개를 나타냄
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_DIFFUSE|D3DFVF_TEX1)

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

	/// matrix palette 사용
    g_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );

	/// blend weight는 4개(오타 아님! 4개!)
    g_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );

    return S_OK;
}




/**-----------------------------------------------------------------------------
 * 정점버퍼를 생성하고 정점값을 채워넣는다.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
    /// 정점버퍼 생성
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 50 * 2 * sizeof( CUSTOMVERTEX ),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

    CUSTOMVERTEX* pVertices;
	if ( FAILED( g_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
	{
		return E_FAIL;
	}
    for( DWORD i = 0 ; i < 50 ; ++i )
    {
		FLOAT theta = ( 2 * D3DX_PI*i ) / ( 50 - 1 );

        pVertices[2*i+0].position	= D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].b[0]		= 1.0f;
        pVertices[2*i+0].b[1]		= 0.0f;
        pVertices[2*i+0].b[2]		= 0.0f;
        pVertices[2*i+0].index		= 0x0000;			/// 0번 가중치는 0번 행렬의 영향을 1.0만큼 받음
        pVertices[2*i+0].color		= 0xffffffff;
        pVertices[2*i+0].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+0].tv       = 1.0f;

        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].b[0]		= 0.5f;
        pVertices[2*i+1].b[1]		= 0.5f;
        pVertices[2*i+1].b[2]		= 0.0f;
        pVertices[2*i+1].index		= 0x0201;			/// 0번 가중치는 1번 행렬의 영향을 0.5만큼 받음
        pVertices[2*i+1].color    = 0xff808080;			
        pVertices[2*i+1].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+1].tv       = 0.0f;
    }
    g_pVB->Unlock();


    return S_OK;
}


/**-----------------------------------------------------------------------------
 * 인덱스버퍼를 생성하고 인덱스값을 채워넣는다.
 *------------------------------------------------------------------------------
 */
HRESULT InitIB()
{
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
	if ( FAILED( InitIB() ) )
	{
		return E_FAIL;
	}

    if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "lake.bmp", &g_pTexture ) ) )
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
	/// 월드행렬 설정
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    /// 뷰행렬을 설정
    D3DXVECTOR3 vEyePt( 0.0f, 2.0f,-3.0f );
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
	/// 0번 행렬은 단위행렬
	D3DXMatrixIdentity( &g_mat0 );

	/// 0 ~ 2PI 까지(0~360도) 값을 변화시킴 Fixed Point기법 사용
	DWORD d = GetTickCount() % ( (int)((D3DX_PI*2) * 1000) );
	/// Y축 회전행렬
    D3DXMatrixRotationY( &g_mat1, d / 1000.0f );
	D3DXMatrixRotationX( &g_mat2, d / 1000.0f );
}



/**-----------------------------------------------------------------------------
 * 초기화 객체들 소거
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
	if ( g_pTexture != NULL )
	{
		g_pTexture->Release();
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
void DrawMesh( void )
{
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );
}

/**-----------------------------------------------------------------------------
 * 화면 그리기
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// 후면버퍼와 Z버퍼 초기화
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	/// 애니메이션 행렬설정
	Animate();
    /// 렌더링 시작
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {

		/// 0번 매트릭스 팔레트에 단위행렬
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(0), &g_mat0 );
		/// 1번 매트릭스 팔레트에 회전행렬
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(1), &g_mat1 );
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(2), &g_mat2 );

        g_pd3dDevice->SetTexture( 0, g_pTexture );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		DrawMesh();

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
    HWND hWnd = CreateWindow( "BasicFrame", "Skinning",
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
				/// 처리할 메시지가 없으면 Render()함수 호출
					Render();
			}
		}
    }

	/// 등록된 클래스 소거
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
