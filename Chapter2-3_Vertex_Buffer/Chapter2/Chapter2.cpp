#include "stdafx.h"
#include "Chapter2.h"

#include <d3d9.h>
// Direct3D9를 사용하기 위한 헤더

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
// D3D 디바이스를 생성할 D3D 객체 변수

LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
// 렌더링에 사용될 D3D 디바이스

LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
// 버텍스를 보관 할 버퍼

// 사용자가 사용 할 버텍스를 정의
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	// rhw값이 있으면 변환이 완료된 정점 - 화면 UI 등에 사용

	DWORD color;
	// 버텍스 색상 정보
};

// 커스텀버텍스 구조체에 관한 정보를 나타내는 FVF값
// 구조체는 X,Y,Z,RHW 값과 Diffuse 색상 값으로 이루어져 있다
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
// 구조체와 함께 반드시 플래그 선언도 해 주어야 한다.


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
	// 디바이스 생성을 위한 정보를 담아서 넘겨 줄 구조체
	/*
	typedef struct D3DPRESENT_PARAMETERS
	{
		UINT                BackBufferWidth;
		UINT                BackBufferHeight;
		D3DFORMAT           BackBufferFormat;
		UINT                BackBufferCount;
		// 백 버퍼를 위한 인자
		
		D3DMULTISAMPLE_TYPE MultiSampleType;
		DWORD               MultiSampleQuality;
		// 멀티샘플링을 위한 인자

		D3DSWAPEFFECT       SwapEffect;
		// 더블 버퍼 스왑 효과
		// http://blog.naver.com/bastard9/140114992482 참고

		HWND                hDeviceWindow;
		// 창 핸들

		BOOL                Windowed;
		// True - 창 모드 / False - 풀 스크린

		BOOL                EnableAutoDepthStencil;
		D3DFORMAT           AutoDepthStencilFormat;
		// 깊이 스텐실 버퍼 사용을 위한 인자

		DWORD               Flags;
		// 각종 플래그
		// http://msdn.microsoft.com/en-us/library/windows/desktop/bb172586(v=vs.85).aspx 참고

		UINT                FullScreen_RefreshRateInHz;
		UINT                PresentationInterval;
		// 화면 주사율 관련 인자

	} D3DPRESENT_PARAMETERS, *LPD3DPRESENT_PARAMETERS;
	*/
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	// 반드시 초기화 할 것!

	d3dpp.Windowed = TRUE;
	// 창 모드

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	// 가장 효율적인 SWAP효과
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb172612(v=vs.85).aspx 참고

	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	// 현재 바탕화면 모드에 맞춰서 후면버퍼를 생성

	//////////////////////////////////////////////////////////////////////////
	// 디바이스를 다음과 같은 설정으로 생성한다.
	//
	// 1. 디폴트 그래픽 카드를 사용 (대부분 그래픽 카드가 1개)
	// 2. HAL 디바이스를 생성 (HW 가속 장치 사용)
	// 3. 정점 처리는 모든 카드에서 지원하는 SW처리로 생성 (HW로 생성할 경우 더욱 높은 성능을 낸다)
	//////////////////////////////////////////////////////////////////////////
	if ( FAILED( g_pD3D->CreateDevice(
		
		D3DADAPTER_DEFAULT,
		// 모니터가 여러 대일 경우 지정 가능

		D3DDEVTYPE_HAL,
		// 출력 디바이스의 종류를 결정
		// D3DDEVTYPE_HAL / D3DDEVTYPE_SW / D3DDEVTYPE_REF
		//
		// D3DDEVTYPE_HAL - 하드웨어 가속을 지원하는 디바이스

		hWnd,
		// 윈도우 핸들
		// 전체 화면 모드의 경우, 최상위 윈도우만 가능

		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		// 셰이더 버텍스 연산을 소프트웨어적으로 할 것인지 하드웨어적으로 할 것인지

		&d3dpp,
		// 디바이스 정보 구조체 포인터

		&g_pd3dDevice
		// 리턴 받을 변수
		) ) )
	{
		return E_FAIL;
	}

	// 디바이스 상태정보를 처리할경우 여기에서 한다.

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
		{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};

	// 버텍스 버퍼 생성
	// 3개의 사용자 버텍스를 보관할 메모리를 할당
	// FVF를 지정하여 버텍스의 형식을 지정한다
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		3 * sizeof( CUSTOMVERTEX ),
		// 생성 할 버텍스 버퍼의 바이트 단위 크기

		0,
		// 버텍스 버퍼 종류, 처리 방식(SW, HW) 지정

		D3DFVF_CUSTOMVERTEX,
		// 커스텀 버텍스 구조체 FVF 플래그 값

		D3DPOOL_DEFAULT,
		// 버텍스 버퍼가 저장 될 메모리의 위치(그래픽 카드, 시스템 메모리)와 관리 방식

		&g_pVB,
		// 리턴 받을 버텍스 버퍼 변수

		NULL
		// 예약 되었음
		) ) )
	{
		return E_FAIL;
	}

	// 버텍스 버퍼를 값으로 채운다. 
	// 버텍스 버퍼의 Lock()함수를 호출하여 포인터를 가져온다.
	VOID* pVertices;
	if ( FAILED( g_pVB->Lock( 
		0,
		// Lock 버퍼 시작점
		// 다음 인자와 함께 모두 0이면 전체 버퍼

		sizeof( vertices ),
		// Lock을 할 버퍼 크기
		// 이전 인자와 함께 모두 0이면 전체 버퍼

		(void**)&pVertices,
		// Lock으로 받아오는 메모리 영역의 포인터를 담을 변수

		0
		// Lock을 수행할 때 함께 사용할 플래그
		) ) )
	{
		return E_FAIL;
	}
	memcpy( pVertices, vertices, sizeof( vertices ) );
	
	g_pVB->Unlock();
	// 반드시 Unlock 해야 한다. Unlock을 하지 않을 경우 그래픽카드가 다운 될 수 있음!

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
// 화면 그리기
//////////////////////////////////////////////////////////////////////////
VOID Render()
{
	if ( NULL == g_pd3dDevice )
		return;

	// 후면 버퍼 지우기
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳

		// 버텍스 버퍼의 내용물을 그린다

		// 1. 버텍스 정보가 담겨 있는 버텍스 버퍼를 출력 스트림으로 할당
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );

		// 2. D3D에게 버텍스 셰이더 정보를 지정한다.
		// 대부분의 경우에는 FVF만 지정
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		// 3. 기하 정보를 출력하기 위해 DrawPrimitive() 함수 호출
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
		// DrawPrimitive - 삼각형을 그려주는 함수
		// 첫번째 인자 - 버텍스를 이어나가는 방식
		
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

	// 윈도우 클래스 등록을 위한 구조체
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ),
		CS_CLASSDC,
		MsgProc,
		0L,
		0L,
		GetModuleHandle( NULL ),
		NULL,
		NULL,
		NULL,
		NULL,
		L"D3D Tutorial",
		NULL
	};

	RegisterClassEx( &wc );
	// 윈도우 클래스 등록

	HWND hWnd = CreateWindow( L"D3D Tutorial",
							  L"D3D Tutorials",
							  WS_OVERLAPPEDWINDOW,
							  100,
							  100,
							  300,
							  300,
							  GetDesktopWindow(),
							  NULL,
							  wc.hInstance,
							  NULL );

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

	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	// 등록 된 클래스 제거

	return 0;
}