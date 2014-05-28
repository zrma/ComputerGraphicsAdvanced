#include "stdafx.h"
#include "Chapter2.h"

// Direct3D9를 사용하기 위한 헤더
#include <d3dx9.h>

// TimeGetTime() 함수를 사용하기 위한 헤더
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////

// D3D 디바이스를 생성할 D3D 객체 변수
LPDIRECT3D9             g_pD3D = NULL; 

// 렌더링에 사용될 D3D 디바이스
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;

// 버텍스를 보관 할 버퍼
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

// 사용자가 사용 할 버텍스를 정의
struct CUSTOMVERTEX
{
	// rhw값이 없는 버텍스!
	FLOAT x, y, z;
	
	// 버텍스 색상 정보
	DWORD color;
};

// 구조체와 함께 반드시 플래그 선언도 해 주어야 한다.
//
// 커스텀버텍스 구조체에 관한 정보를 나타내는 FVF값
// 구조체는 X,Y,Z 값과 Diffuse 색상 값으로 이루어져 있다
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)



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

	//////////////////////////////////////////////////////////////////////////
	// IDirect3D9 생성 -> IDirect3DDevice9 생성
	// IDirect3DDevice9 해제 -> IDirect3D9 해제

	D3DPRESENT_PARAMETERS d3dpp;
	// 반드시 초기화 할 것!
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	
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

	// 광원 끄기
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 버텍스 버퍼 생성
//
// CreateVertexBuffer() 함수를 이용해 만들어진 버텍스 버퍼는 위치가 고정되어 있지 않다.
// 사용 할 때에는 반드시 Lock()으로 주소를 얻어 온 후, 버퍼에 내용을 써 넣고 Unlock() 해야 한다.
//
// 버텍스 버퍼나 인덱스 버퍼는 기본 시스템 메모리 외에도 디바이스 메모리(그래픽 카드 메모리)에 생성 될 수 있다.
// 대부분의 그래픽 카드에서는 이렇게 할 경우 속도가 매우 향상 된다.
//////////////////////////////////////////////////////////////////////////
HRESULT InitVB()
{
	// 삼각형을 렌더링 하기 위해 세 개의 버텍스 선언
	CUSTOMVERTEX vertices[] =
	{
		{ -1.0f, -1.0f, 0.0f, 0xffff0000, },
		{ 1.0f, -1.0f, 0.0f, 0xff0000ff, },
		{ 0.0f, 1.0f, 0.0f, 0xffffffff, },
	};

	// 버텍스 버퍼 생성
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// 버텍스 버퍼를 값으로 채운다. 
	VOID* pVertices;
	if ( FAILED( g_pVB->Lock( 0, sizeof( vertices ), (void**)&pVertices, 0 ) ) )
	{
		return E_FAIL;
	}
	memcpy( pVertices, vertices, sizeof( vertices ) );
	g_pVB->Unlock();
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 객체 정리 - 순서 주의!
//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
// 행렬 세팅
//
// World, View, Projection
//////////////////////////////////////////////////////////////////////////
VOID SetupMatrices()
{
	// World
	D3DXMATRIXA16 worldMatrix;
	
	// float 연산의 정밀도를 위해서 1000으로 나머지 연산
	UINT  time = timeGetTime() % 1000;
	
	// 1초마다 한바퀴씩(2 * pi) 회전 할 각도
	FLOAT angle = time * ( 2.0f * D3DX_PI ) / 1000.0f;
	
	// Y축 기준으로 회전하는 행렬 생성
	D3DXMatrixRotationY( &worldMatrix, angle );
	
	// 생성한 회전 행렬을 World 행렬로 디바이스에 설정
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );
	

	//////////////////////////////////////////////////////////////////////////
	// View 행렬을 정의하기 위해서 세가지 값이 필요하다.
	// 원점, 시점, 업벡터
	//////////////////////////////////////////////////////////////////////////
	
	// 1. 눈의 위치			( 0, 3.0, -5)
	D3DXVECTOR3 eyePoint( 0.0f, 3.0f, -5.0f );
	
	// 2. 눈이 바라보는 위치	( 0, 0, 0 )
	D3DXVECTOR3 lookAtPoint( 0.0f, 0.0f, 0.0f );
	
	// 3. 업벡터				( 0, 1, 0 )
	D3DXVECTOR3 upVector( 0.0f, 1.0f, 0.0f );
	
	D3DXMATRIXA16 viewMatrix;
	// 1, 2, 3의 값으로 View 행렬 생성
	D3DXMatrixLookAtLH( &viewMatrix, &eyePoint, &lookAtPoint, &upVector );
	
	// 생성한 View 행렬을 디바이스에 설정
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );
	
	// Projection 행렬을 정의하기 위해서는 시야각(FOV=Field Of View)과 종횡비(aspect ratio), 클리핑 평면 값이 필요하다.
	D3DXMATRIXA16 projMatrix;

	/// matProj   : 값이 설정될 행렬
	/// D3DX_PI/4 : FOV(D3DX_PI/4 = 45도)
	/// 1.0f      : 종횡비
	/// 1.0f      : 근접 클리핑 평면(near clipping plane)
	/// 100.0f    : 원거리 클리핑 평면(far clipping plane)
	D3DXMatrixPerspectiveFovLH(

		// 행렬을 얻어올 변수
		&projMatrix,

		// FOV(D3DX_PI/4 = 45도)
		D3DX_PI / 4,
		
		// 종횡비 1:1
		1.0f,
		
		// Near Plane
		1.0f,
		
		// Far Plane
		100.0f
		
		);

	// 생성한 Projection 행렬을 디바이스에 설정
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &projMatrix );
}


//////////////////////////////////////////////////////////////////////////
// 화면 그리기
//////////////////////////////////////////////////////////////////////////
VOID Render()
{
	if ( NULL == g_pd3dDevice )
	{
		return;
	}

	// 후면 버퍼 지우기
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳
		
		//////////////////////////////////////////////////////////////////////////
		// 이 내부는 짧고 간결할 수록 좋다
		//////////////////////////////////////////////////////////////////////////

		// 행렬 설정
		SetupMatrices();

		// 버텍스 내용물 그리기
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
		
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
		if ( SUCCEEDED (InitVB()))
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

	// 등록 된 클래스 제거
	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	
	return 0;
}