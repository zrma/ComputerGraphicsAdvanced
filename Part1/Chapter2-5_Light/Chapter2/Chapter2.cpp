#include "stdafx.h"
#include "Chapter2.h"

// Direct3D9를 사용하기 위한 헤더
#include <d3dx9.h>

// TimeGetTime() 함수를 사용하기 위한 헤더
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

// 사용자가 사용 할 버텍스를 정의
struct CUSTOMVERTEX
{
	// 버텍스의 3차원 좌표
	D3DXVECTOR3	position;
	
	// 버텍스의 노멀 벡터
	D3DXVECTOR3 normal;
};

// 구조체와 함께 반드시 플래그 선언도 해 주어야 한다.
//
// 커스텀버텍스 구조체에 관한 정보를 나타내는 FVF값
// 구조체는 좌표와 노멀 벡터로 이루어져 있다.
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)



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

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 기하 생성
//////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
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
	for ( DWORD i = 0; i < 50; ++i )
	{
		FLOAT theta = ( 2 * D3DX_PI * i ) / ( 50 - 1 );

		// 실린더의 아래쪽 원통 좌표
		pVertices[2 * i + 0].position = D3DXVECTOR3( sinf( theta ), -1.0f, cosf( theta ) );
		// 실린더의 아래쪽 원통 노멀
		pVertices[2 * i + 0].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
		
		// 실린더의 위쪽 원통 좌표
		pVertices[2 * i + 1].position = D3DXVECTOR3( sinf( theta ), 1.0f, cosf( theta ) );
		// 실린더의 위쪽 원통 노멀
		pVertices[2 * i + 1].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
	}
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
	
	// 단위 행렬로 설정
	D3DXMatrixIdentity( &worldMatrix );
	
	// X축을 중심으로 Rotate 행렬 생성
	D3DXMatrixRotationX( &worldMatrix, timeGetTime() / 500.0f );
	
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
// 광원 설정
//////////////////////////////////////////////////////////////////////////
VOID SetupLights()
{
	// 재질(Material) 설정
	// Material은 디바이스에 단 하나만 설정 가능
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );

	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	g_pd3dDevice->SetMaterial( &mtrl );

	//////////////////////////////////////////////////////////////////////////
	// 광원 설정
	//////////////////////////////////////////////////////////////////////////

	// 빛의 방향
	D3DXVECTOR3 dirVector;

	// 광원 구조체
	D3DLIGHT9 light;

	ZeroMemory( &light, sizeof( D3DLIGHT9 ) );

	// 광원의 종류(점 광원, 방향성 광원, 스포트라이트)
	light.Type = D3DLIGHT_DIRECTIONAL;

	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	// 방향 벡터 설정
	dirVector = D3DXVECTOR3( cosf( timeGetTime() / 350.0f ), 1.0f, sinf( timeGetTime() / 350.0f ) );

	// 단위 벡터로 노멀라이즈
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &dirVector );

	// 광원이 닿을 수 있는 최대거리
	light.Range = 1000.0f;

	// 디바이스에 0번 광원 설치
	g_pd3dDevice->SetLight( 0, &light );

	// 0번 광원 켜기
	g_pd3dDevice->LightEnable( 0, TRUE );

	// 광원 설정 적용
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// Amblient Light 값 설정
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000020 );
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

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳
		
		//////////////////////////////////////////////////////////////////////////
		// 이 내부는 짧고 간결할 수록 좋다
		//////////////////////////////////////////////////////////////////////////

		// 광원 및 재질 설정
		SetupLights();

		// 행렬 설정
		SetupMatrices();

		// 버텍스 내용물 그리기
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		
		// 버텍스를 연결하는 방식이 D3DPT_TRIANGLESTRIP
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2 );
		
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

	// 등록 된 클래스 제거
	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	
	return 0;
}