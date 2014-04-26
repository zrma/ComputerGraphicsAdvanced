#include "stdafx.h"
#include "Chapter2.h"

// Direct3D9�� ����ϱ� ���� ���
#include <d3dx9.h>

// TimeGetTime() �Լ��� ����ϱ� ���� ���
#include <mmsystem.h>

//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;

// ����ڰ� ��� �� ���ؽ��� ����
struct CUSTOMVERTEX
{
	// ���ؽ��� 3���� ��ǥ
	D3DXVECTOR3	position;
	
	// ���ؽ��� ��� ����
	D3DXVECTOR3 normal;
};

// ����ü�� �Բ� �ݵ�� �÷��� ���� �� �־�� �Ѵ�.
//
// Ŀ���ҹ��ؽ� ����ü�� ���� ������ ��Ÿ���� FVF��
// ����ü�� ��ǥ�� ��� ���ͷ� �̷���� �ִ�.
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)



//////////////////////////////////////////////////////////////////////////
// Direct3D �ʱ�ȭ
//////////////////////////////////////////////////////////////////////////
HRESULT InitD3D( HWND hWnd )
{
	// ����̽��� �����ϱ� ���ؼ� D3D ��ü ����
	if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return E_FAIL;
	}
	
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	
	// �̹����� ������ ������Ʈ�� �׸� ���̹Ƿ� Z���۰� �ʿ��ϴ�.
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	
	// ����̽� ����
	if ( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.

	// �ø� ��� ����
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	// Z���� ��� �ѱ�
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	// g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	
	// g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	// g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ���� ����
//////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
	// ���ؽ� ���� ����
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
	
	// �Ǹ��� ����� for ����
	for ( DWORD i = 0; i < 50; ++i )
	{
		FLOAT theta = ( 2 * D3DX_PI * i ) / ( 50 - 1 );

		// �Ǹ����� �Ʒ��� ���� ��ǥ
		pVertices[2 * i + 0].position = D3DXVECTOR3( sinf( theta ), -1.0f, cosf( theta ) );
		// �Ǹ����� �Ʒ��� ���� ���
		pVertices[2 * i + 0].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
		
		// �Ǹ����� ���� ���� ��ǥ
		pVertices[2 * i + 1].position = D3DXVECTOR3( sinf( theta ), 1.0f, cosf( theta ) );
		// �Ǹ����� ���� ���� ���
		pVertices[2 * i + 1].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
	}
	g_pVB->Unlock();
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ��ü ���� - ���� ����!
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
// ��� ����
//
// World, View, Projection
//////////////////////////////////////////////////////////////////////////
VOID SetupMatrices()
{
	// World
	D3DXMATRIXA16 worldMatrix;
	
	// ���� ��ķ� ����
	D3DXMatrixIdentity( &worldMatrix );
	
	// X���� �߽����� Rotate ��� ����
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
// ���� ����
//////////////////////////////////////////////////////////////////////////
VOID SetupLights()
{
	// ����(Material) ����
	// Material�� ����̽��� �� �ϳ��� ���� ����
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );

	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	g_pd3dDevice->SetMaterial( &mtrl );

	//////////////////////////////////////////////////////////////////////////
	// ���� ����
	//////////////////////////////////////////////////////////////////////////

	// ���� ����
	D3DXVECTOR3 dirVector;

	// ���� ����ü
	D3DLIGHT9 light;

	ZeroMemory( &light, sizeof( D3DLIGHT9 ) );

	// ������ ����(�� ����, ���⼺ ����, ����Ʈ����Ʈ)
	light.Type = D3DLIGHT_DIRECTIONAL;

	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	// ���� ���� ����
	dirVector = D3DXVECTOR3( cosf( timeGetTime() / 350.0f ), 1.0f, sinf( timeGetTime() / 350.0f ) );

	// ���� ���ͷ� ��ֶ�����
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &dirVector );

	// ������ ���� �� �ִ� �ִ�Ÿ�
	light.Range = 1000.0f;

	// ����̽��� 0�� ���� ��ġ
	g_pd3dDevice->SetLight( 0, &light );

	// 0�� ���� �ѱ�
	g_pd3dDevice->LightEnable( 0, TRUE );

	// ���� ���� ����
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// Amblient Light �� ����
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000020 );
}

//////////////////////////////////////////////////////////////////////////
// ȭ�� �׸���
//////////////////////////////////////////////////////////////////////////
VOID Render()
{
	if ( NULL == g_pd3dDevice )
	{
		return;
	}

	// �ĸ� ���ۿ� Z���� �����
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	// ������ ����
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// ���� ������ ��ɵ��� ������ ��
		
		//////////////////////////////////////////////////////////////////////////
		// �� ���δ� ª�� ������ ���� ����
		//////////////////////////////////////////////////////////////////////////

		// ���� �� ���� ����
		SetupLights();

		// ��� ����
		SetupMatrices();

		// ���ؽ� ���빰 �׸���
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		
		// ���ؽ��� �����ϴ� ����� D3DPT_TRIANGLESTRIP
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2 );
		
		// ������ ����
		g_pd3dDevice->EndScene();
	}

	// ���� ����!
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//////////////////////////////////////////////////////////////////////////
// ������ ���ν���
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

	// ������ Ŭ���� ���
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, L"D3D Tutorial", NULL };
	RegisterClassEx( &wc );

	// ������ ����
	HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorials",
							  WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
							  GetDesktopWindow(), NULL, wc.hInstance, NULL );

	// Direct3D �ʱ�ȭ
	if ( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		// ���ؽ� ���� �ʱ�ȭ
		if ( SUCCEEDED (InitGeometry()))
		{
			// ������ ���
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			// �޽��� ����
			MSG msg;
			ZeroMemory( &msg, sizeof( msg ) );
			while ( msg.message != WM_QUIT)
			{
				// �޽��� ť�� �޽����� ������ �޽��� ó��
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

	// ��� �� Ŭ���� ����
	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	
	return 0;
}