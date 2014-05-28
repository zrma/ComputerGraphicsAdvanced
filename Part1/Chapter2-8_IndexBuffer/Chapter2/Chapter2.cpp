#include "stdafx.h"
#include "Chapter2.h"

#include <d3d9.h>
#include <d3dx9.h>

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9	g_pIB = NULL;

// 커스텀 버텍스 정의 구조체
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};

// 사용자 정점 구조체에 관한 정보를 나타내는 FVF
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct MYINDEX
{
	// 16비트 크기로 인덱스 생성
	WORD	_0, _1, _2;

	// 32비트 크기로 인덱스 생성
	// UINT	_0, _1, _2;
};


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
// 버텍스 버퍼 생성
//////////////////////////////////////////////////////////////////////////
HRESULT InitVB()
{
	// 상자(cube)를 렌더링 하기 위해 8개의 버텍스 선언

	CUSTOMVERTEX vertices[] =
	{
		{ -1, 1, 1, 0xffff0000 },		// v0
		{ 1, 1, 1, 0xff00ff00 },		// v1
		{ 1, 1, -1, 0xff0000ff },		// v2
		{ -1, 1, -1, 0xffffff00 },		// v3
		{ -1, -1, 1, 0xff00ffff },		// v4
		{ 1, -1, 1, 0xffff00ff },		// v5
		{ 1, -1, -1, 0xff000000 },		// v6
		{ -1, -1, -1, 0xffffffff },		// v7
	};

	// 8개의 버텍스를 보관할 버텍스 버퍼를 생성한다.
	// FVF를 지정하여 보관할 데이터의 형식을 지정한다.
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 8 * sizeof( CUSTOMVERTEX ), 0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Lock을 걸어서 메모리를 할당하고 포인터를 받아온다.
	// 할당 받은 메모리에 버텍스 버퍼 값을 채운다.
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
// 인덱스 버퍼 생성
//////////////////////////////////////////////////////////////////////////
HRESULT InitIB()
{

	// 상자(cube)를 렌더링하기위해 12개의 버텍스를 선언
	MYINDEX	indices[] =
	{
		{ 0, 1, 2 }, { 0, 2, 3 },	// 윗면
		{ 4, 6, 5 }, { 4, 7, 6 },	// 아랫면
		{ 0, 3, 7 }, { 0, 7, 4 },	// 왼면
		{ 1, 5, 6 }, { 1, 6, 2 },	// 오른면
		{ 3, 2, 6 }, { 3, 6, 7 },	// 앞면
		{ 0, 4, 5 }, { 0, 5, 1 }	// 뒷면
	};

	//////////////////////////////////////////////////////////////////////////
	// 인덱스버퍼 생성
	// D3DFMT_INDEX16은 인덱스의 단위가 16비트
	// D3DFMT_INDEX32는 인덱스의 단위가 32비트
	// MYINDEX 구조체의 선언형과 맞춰야 한다.
	if ( FAILED( g_pd3dDevice->CreateIndexBuffer( 12 * sizeof( MYINDEX ), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// 인덱스 버퍼 채우기
	// 인덱스 버퍼도 마찬가지로 Lock()으로 메모리 주소를 얻어 온다.

	VOID* pIndices;
	if ( FAILED( g_pIB->Lock( 0, sizeof( indices ), (void**)&pIndices, 0 ) ) )
	{
		return E_FAIL;
	}
	memcpy( pIndices, indices, sizeof( indices ) );

	g_pIB->Unlock();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 객체 정리 - 순서 주의!
//////////////////////////////////////////////////////////////////////////
VOID Cleanup()
{
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

//////////////////////////////////////////////////////////////////////////
// 행렬 세팅
//
// World, View, Projection
//////////////////////////////////////////////////////////////////////////
VOID SetupMatrices()
{
	// World
	D3DXMATRIXA16 worldMatrix;
	
	// 단위 행렬로 설정
	D3DXMatrixIdentity( &worldMatrix );
	
	// GetTickCount - 정밀도 14~16ms
	// timeGetTime() - 1ms
	// QueryPerformanceFrequency() - 1ms 미만 : 알고리즘 성능 측정에 많이 사용
	D3DXMatrixRotationY( &worldMatrix, GetTickCount() / 500.0f );
	
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
	{
		return;
	}

	// 후면 버퍼와 Z버퍼 지우기
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	SetupMatrices();

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳
		
		// 1. 버텍스 데이터가 담겨있는 버텍스 버퍼를 출력 스트림으로 할당
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );

		// 2. D3D에게 버텍스 셰이더 정보 지정. 대부분는 FVF만 지정
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		
		// 3. 인덱스 버퍼를 지정
		g_pd3dDevice->SetIndices( g_pIB );

		// 4. DrawIndexedPrimitive()
		//
		// 인덱스는 DrawIndexdPrimitive()로 그려야 한다.
		g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );
		
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
							  WS_OVERLAPPEDWINDOW, 100, 100, 600, 600,
							  GetDesktopWindow(), NULL, wc.hInstance, NULL );

	// Direct3D 초기화
	if ( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// 버텍스 버퍼 초기화
		if ( SUCCEEDED (InitVB()))
		{
			// 인덱스 버퍼 초기화
			if ( SUCCEEDED(InitIB()) )
			{
				// 윈도우 출력
				ShowWindow( hWnd, SW_SHOWDEFAULT );
				UpdateWindow( hWnd );

				// 메시지 루프
				MSG msg;
				ZeroMemory( &msg, sizeof( msg ) );
				while ( msg.message != WM_QUIT )
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
	}

	// 등록 된 클래스 제거
	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	
	return 0;
}