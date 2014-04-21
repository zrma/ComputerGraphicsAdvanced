#include "stdafx.h"
#include "Chapter2.h"

#include <d3dx9.h>
// Direct3D9를 사용하기 위한 헤더

#include <mmsystem.h>
// TimeGetTime() 함수를 사용하기 위한 헤더

//////////////////////////////////////////////////////////////////////////
// 전역변수
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 

LPD3DXMESH              g_pMesh = NULL;
// 메시 객체

D3DMATERIAL9*           g_pMeshMaterials = NULL;
// 메시에서 사용할 재질

LPDIRECT3DTEXTURE9*     g_pMeshTextures = NULL;
// 메시에서 사용할 텍스쳐

DWORD                   g_dwNumMaterials = 0L;
// 메시에서 사용 될 재질 개수

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

	// 주변광원값을 최대밝기로
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 기하 생성
//////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
	// 재질을 임시로 보관할 버퍼선언
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// .x파일을 메시로 읽어들인다. 이때 재질정보도 함께 읽음
	if ( FAILED( D3DXLoadMeshFromX( L"Girl.x", D3DXMESH_SYSTEMMEM, g_pd3dDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh ) ) )
	{
		/// 현재 폴더에 파일이 없으면 상위폴더 검색
		if ( FAILED( D3DXLoadMeshFromX( L"..\\Girl.x", D3DXMESH_SYSTEMMEM, g_pd3dDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh ) ) )
		{
			MessageBox( NULL, L"Could not find Girl.x", L"Meshes.exe", MB_OK );
			return E_FAIL;
		}
	}

	if ( !( g_pMesh->GetFVF() & D3DFVF_NORMAL ) )
	{
		// 법선 정보를 가지고 있지 않다면 메쉬를 복제하고 D3DFVF_NORMAL을 추가한다.
		ID3DXMesh* pTempMesh = 0;
		g_pMesh->CloneMeshFVF( D3DXMESH_MANAGED, g_pMesh->GetFVF() | D3DFVF_NORMAL, g_pd3dDevice, &pTempMesh );

		// 법선을 계산한다.
		D3DXComputeNormals( pTempMesh, 0 );

		g_pMesh->Release();
		// 기존 메쉬를 제거한다
		g_pMesh = pTempMesh;
		// 기존 메쉬를 법선이 계산된 메쉬로 지정한다.
	}

	// 재질 정보와 텍스쳐 정보를 따로 뽑기
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	// 재질 개수만큼 재질 구조체 배열 생성
	if ( NULL == g_pMeshMaterials )
	{
		return E_OUTOFMEMORY;
	}

	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	// 재질 개수만큼 텍스쳐 배열 생성
	if ( NULL == g_pMeshTextures )
	{
		return E_OUTOFMEMORY;
	}

	for ( DWORD i = 0; i < g_dwNumMaterials; ++i )
	{
		// 재질 정보 복사
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// 주변 광원 정보를 Diffuse 정보로 대입
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if ( d3dxMaterials[i].pTextureFilename != NULL &&
			 lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
		{
			// 텍스쳐 읽기
			if ( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice,
				d3dxMaterials[i].pTextureFilename,
				&g_pMeshTextures[i] ) ) )
			{
				// 텍스쳐가 현재 폴더에 없으면 상위폴더 검색
				const CHAR* strPrefix = "./Resource/";
				CHAR strTexture[MAX_PATH];
				strcpy_s( strTexture, MAX_PATH, strPrefix );
				strcat_s( strTexture, MAX_PATH, d3dxMaterials[i].pTextureFilename );

				if ( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice, strTexture, &g_pMeshTextures[i] ) ) )
				{
					MessageBox( NULL, L"Could not find texture map", L"Meshes.exe", MB_OK );
				}
			}
		}
	}

	// 임시로 생성한 재질버퍼 소거
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// 객체 정리 - 순서 주의!
//////////////////////////////////////////////////////////////////////////
VOID Cleanup()
{
	if ( g_pMeshMaterials != NULL )
	{
		delete[] g_pMeshMaterials;
	}

	if ( g_pMeshTextures )
	{
		for ( DWORD i = 0; i < g_dwNumMaterials; i++ )
		{
			if ( g_pMeshTextures[i] )
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if ( g_pMesh != NULL )
	{
		g_pMesh->Release();
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
	D3DXMatrixRotationY( &worldMatrix, timeGetTime() / 1000.0f );
	// X축을 중심으로 Rotate 행렬 생성
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );

	// View
	D3DXVECTOR3 eyePoint( 0.0f, 7.0f, -30.0f );
	D3DXVECTOR3 lookAtPoint( 0.0f, 7.0f, 0.0f );
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

	// 렌더링 시작
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// 실제 렌더링 명령들이 나열될 곳
		
		//////////////////////////////////////////////////////////////////////////
		// 이 내부는 짧고 간결할 수록 좋다
		//////////////////////////////////////////////////////////////////////////
		
		// 행렬 설정
		SetupMatrices();

		// 메시는 재질이 다른 메시 별로 부분 집합을 이루고 있다.
		// 이들을 루프를 수행해서 모두 그려준다.
		for ( DWORD i = 0; i < g_dwNumMaterials; ++i )
		{
			// 부분집합 메시의 재질과 텍스쳐 설정
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
			g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );

			// 부분집합 메시 출력
			g_pMesh->DrawSubset( i );
			// DrawSubset() - 분할 된 부분 메시별로 따로 그리는 함수
		}

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