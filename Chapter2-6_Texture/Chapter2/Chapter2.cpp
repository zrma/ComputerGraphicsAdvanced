#include "stdafx.h"
#include "Chapter2.h"

#include <d3dx9.h>
// Direct3D9를 사용하기 위한 헤더

#include <mmsystem.h>
// TimeGetTime() 함수를 사용하기 위한 헤더


// SHOW_HOW_TO_USE_TCI가 선언된 것과 선언되지 않은 것의 컴파일 결과를 반드시 비교해 보자.
// #define SHOW_HOW_TO_USE_TCI

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DTEXTURE9      g_pTexture = NULL;
// 텍스쳐 정보

struct CUSTOMVERTEX
{
	D3DXVECTOR3	position;
	D3DCOLOR    color;
#ifndef SHOW_HOW_TO_USE_TCI
	FLOAT       tu, tv;   /// 텍스쳐 좌표
#endif
};

#ifdef SHOW_HOW_TO_USE_TCI
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#else
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#endif

//////////////////////////////////////////////////////////////////////////
// Direct3D 초기화
//////////////////////////////////////////////////////////////////////////
HRESULT InitD3D( HWND hWnd )
{
	// 디바이스를 생성하기 위해서 D3D 객체 생성
	if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return E_FAIL;
	}
	
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	
	// 이번에는 복잡한 오브젝트를 그릴 것이므로 Z버퍼가 필요하다.
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	
	// 디바이스 생성
	if ( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// 디바이스 상태정보를 처리할경우 여기에서 한다.

	// 컬링 기능 끄기
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// Z버퍼 기능 켜기
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	// g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	
	// g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	// g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );

	// 조명 기능 끄기
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 기하 생성
//////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
	// D3DXCreateTextureFromFile() 함수를 사용해서 파일로부터 텍스쳐 생성
	if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"Earth.bmp", &g_pTexture ) ) )
	{
		// 현재폴더에 파일이 없으면 상위 폴더 검색
		if ( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, L"..\\Earth.bmp", &g_pTexture ) ) )
		{
			// 텍스쳐 생성 실패
			MessageBox( NULL, L"Could not find Earth.bmp", L"Textures.exe", MB_OK );
			return E_FAIL;
		}
	}

	// 버텍스 버퍼 생성
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
	
	// 실린더 만들기 for 루프
	// 텍스쳐의 u,v좌표값을 0.0 ~ 1.0 사이의 값으로 채워넣고 있다.
	for ( DWORD i = 0; i < 50; ++i )
	{
		FLOAT theta = ( 2 * D3DX_PI * i ) / ( 50 - 1 );

		pVertices[2 * i + 0].position = D3DXVECTOR3( sinf( theta ), -1.0f, cosf( theta ) );
		pVertices[2 * i + 0].color = 0xffffffff;
#ifndef SHOW_HOW_TO_USE_TCI
		// SHOW_HOW_TO_USE_TCI가 선언되어 있으면 텍스쳐 좌표를 생성하지 않음
		
		pVertices[2 * i + 0].tu = ( (FLOAT)i ) / ( 50 - 1 );	
		// 텍스쳐의 u좌표 0/49, 1/49, 2/49 ... 49/49 (즉 0.0 ~ 1.0)
		pVertices[2 * i + 0].tv = 1.0f;
		// 텍스쳐의 v좌표 1.0
#endif

		pVertices[2 * i + 1].position = D3DXVECTOR3( sinf( theta ), 1.0f, cosf( theta ) );
		pVertices[2 * i + 1].color = 0xff808080;
#ifndef SHOW_HOW_TO_USE_TCI
		pVertices[2 * i + 1].tu = ( (FLOAT)i ) / ( 50 - 1 );
		/// 텍스쳐의 u좌표 0/49, 1/49, 2/49 ... 49/49 (즉 0.0 ~ 1.0)
		pVertices[2 * i + 1].tv = 0.0f;
		// 텍스쳐의 v좌표 0.0
#endif
	}
	g_pVB->Unlock();
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 객체 정리 - 순서 주의!
//////////////////////////////////////////////////////////////////////////
VOID Cleanup()
{
	if ( g_pTexture != NULL )
	{
		g_pTexture->Release();
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

//////////////////////////////////////////////////////////////////////////
// 행렬 세팅
//
// World, View, Projection
//////////////////////////////////////////////////////////////////////////
VOID SetupMatrices()
{
	// World
	D3DXMATRIXA16 worldMatrix;
	D3DXMatrixIdentity( &worldMatrix );
	// 단위 행렬로 설정
	D3DXMatrixRotationX( &worldMatrix, timeGetTime() / 1000.0f );
	// X축을 중심으로 Rotate 행렬 생성
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );

	// View
	D3DXVECTOR3 eyePoint( 0.0f, 3.0f, -5.0f );
	D3DXVECTOR3 lookAtPoint( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 upVector( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 viewMatrix;
	D3DXMatrixLookAtLH( &viewMatrix, &eyePoint, &lookAtPoint, &upVector );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );

	// Projection
	D3DXMATRIXA16 projMatrix;
	D3DXMatrixPerspectiveFovLH( &projMatrix, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &projMatrix );
}

//////////////////////////////////////////////////////////////////////////
// 화면 그리기
//////////////////////////////////////////////////////////////////////////
VOID Render()
{
	if ( NULL == g_pd3dDevice )
		return;

	// 후면 버퍼와 Z버퍼 지우기
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳
		
		//////////////////////////////////////////////////////////////////////////
		// 이 내부는 짧고 간결할 수록 좋다
		//////////////////////////////////////////////////////////////////////////
		
		// 행렬 설정
		SetupMatrices();

		// 생성한 텍스쳐를 0번 텍스쳐 스테이지에 올린다.
		// 텍스쳐 스테이지는 여러 장의 텍스쳐와 색 정보를 섞어서 출력할때 사용
		// 여기서는 텍스쳐의 색과 정점의 색정보를 modulate 연산으로 섞어서 출력
		g_pd3dDevice->SetTexture( 0, g_pTexture );
		// 0번 텍스쳐 스테이지에 텍스쳐 고정
		
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		// MODULATE 연산으로 색 섞기

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		// 1번째 인자는 텍스쳐

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		// 2번째 인자는 버텍스의 DIFFUSE

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );	
		// alpha 연산은 사용하지 않기

#ifdef SHOW_HOW_TO_USE_TCI
		// D3D의 텍스쳐 좌표 생성 기능을 사용하는 예를 보여준다.
		// 여기서는 카메라 좌표계에서의 정점 정보를 사용해서 텍스쳐 좌표를 생성한다.
		// 4x4크기의 텍스쳐 변환 행렬을 텍스쳐 좌표 인덱스(TCI=Texture Coord Index) 전달 인자로 
		// 사용해서 x,y,z TCI좌표를 u,v텍스쳐 좌표로 변환한다.

		// 사용한 것은 단순히 (-1.0 ~ +1.0) 값을 (0.0 ~ 1.0) 사이의 값으로 변환하는 행렬이다.
		// World, View, Projection을 거친 정점은 (-1.0 ~ +1.0) 사이의 값을 갖게 된다.
		// tu =  0.5*x + 0.5
		// tv = -0.5*y + 0.5
		
		D3DXMATRIXA16 mat;
		mat._11 = 0.25f; mat._12 = 0.00f; mat._13 = 0.00f; mat._14 = 0.00f;
		mat._21 = 0.00f; mat._22 = -0.25f; mat._23 = 0.00f; mat._24 = 0.00f;
		mat._31 = 0.00f; mat._32 = 0.00f; mat._33 = 1.00f; mat._34 = 0.00f;
		mat._41 = 0.50f; mat._42 = 0.50f; mat._43 = 0.00f; mat._44 = 1.00f;

		g_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
		// 텍스쳐 변환행렬

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		// 2차원 텍스쳐 사용

		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		// 카메라 좌표계 변환
#endif

		// 버텍스 내용물 그리기
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2 );
		// 버텍스를 연결하는 방식이 D3DPT_TRIANGLESTRIP
		
		// 렌더링 종료
		g_pd3dDevice->EndScene();
	}

	// 버퍼 스왑!
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//////////////////////////////////////////////////////////////////////////
// 윈도우 프로시져
//////////////////////////////////////////////////////////////////////////
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_DESTROY:
			Cleanup();
			PostQuitMessage( 0 );
			return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT APIENTRY _tWinMain( _In_ HINSTANCE hInstance,
						_In_opt_ HINSTANCE hPrevInstance,
						_In_ LPTSTR    lpCmdLine,
						_In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nCmdShow );

	// 윈도우 클래스 등록
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, L"D3D Tutorial", NULL };
	RegisterClassEx( &wc );

	// 윈도우 생성
	HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorials",
							  WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
							  GetDesktopWindow(), NULL, wc.hInstance, NULL );

	// Direct3D 초기화
	if ( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// 버텍스 버퍼 초기화
		if ( SUCCEEDED (InitGeometry()))
		{
			// 윈도우 출력
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			// 메시지 루프
			MSG msg;
			ZeroMemory( &msg, sizeof( msg ) );
			while ( msg.message != WM_QUIT)
			{
				// 메시지 큐에 메시지가 있으면 메시지 처리
				if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					Render();
				}
			}
		}
	}

	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	// 등록 된 클래스 제거

	return 0;
}