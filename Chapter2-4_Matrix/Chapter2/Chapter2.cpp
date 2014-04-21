#include "stdafx.h"
#include "Chapter2.h"

#include <d3dx9.h>
// Direct3D9�� ����ϱ� ���� ���

#include <mmsystem.h>
// TimeGetTime() �Լ��� ����ϱ� ���� ���

//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL; 
// D3D ����̽��� ������ D3D ��ü ����

LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 
// �������� ���� D3D ����̽�

LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
// ���ؽ��� ���� �� ����

// ����ڰ� ��� �� ���ؽ��� ����
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	// rhw���� ���� ���ؽ�!

	DWORD color;
	// ���ؽ� ���� ����
};

// Ŀ���ҹ��ؽ� ����ü�� ���� ������ ��Ÿ���� FVF��
// ����ü�� X,Y,Z ���� Diffuse ���� ������ �̷���� �ִ�
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
// ����ü�� �Բ� �ݵ�� �÷��� ���� �� �־�� �Ѵ�.


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
	//////////////////////////////////////////////////////////////////////////
	// IDirect3D9 ���� -> IDirect3DDevice9 ����
	// IDirect3DDevice9 ���� -> IDirect3D9 ����

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	// �ݵ�� �ʱ�ȭ �� ��!

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	
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

	// ���� ����
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	// g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ���ؽ� ���� ����
//
// CreateVertexBuffer() �Լ��� �̿��� ������� ���ؽ� ���۴� ��ġ�� �����Ǿ� ���� �ʴ�.
// ��� �� ������ �ݵ�� Lock()���� �ּҸ� ��� �� ��, ���ۿ� ������ �� �ְ� Unlock() �ؾ� �Ѵ�.
//
// ���ؽ� ���۳� �ε��� ���۴� �⺻ �ý��� �޸� �ܿ��� ����̽� �޸�(�׷��� ī�� �޸�)�� ���� �� �� �ִ�.
// ��κ��� �׷��� ī�忡���� �̷��� �� ��� �ӵ��� �ſ� ��� �ȴ�.
//////////////////////////////////////////////////////////////////////////
HRESULT InitVB()
{
	// �ﰢ���� ������ �ϱ� ���� �� ���� ���ؽ� ����
	CUSTOMVERTEX vertices[] =
	{
		{ -1.0f, -1.0f, 0.0f, 0xffff0000, },
		{ 1.0f, -1.0f, 0.0f, 0xff0000ff, },
		{ 0.0f, 1.0f, 0.0f, 0xffffffff, },
	};

	// ���ؽ� ���� ����
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// ���ؽ� ���۸� ������ ä���. 
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
// ��ü ���� - ���� ����!
//////////////////////////////////////////////////////////////////////////
VOID Cleanup()
{
	if ( g_pVB != NULL )
		g_pVB->Release();

	if ( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if ( g_pD3D != NULL )
		g_pD3D->Release();
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
	
	UINT  time = timeGetTime() % 1000;
	// float ������ ���е��� ���ؼ� 1000���� ������ ����

	FLOAT angle = time * ( 2.0f * D3DX_PI ) / 1000.0f;
	// 1�ʸ��� �ѹ�����(2 * pi) ȸ�� �� ����

	D3DXMatrixRotationY( &worldMatrix, angle );
	// Y�� �������� ȸ���ϴ� ��� ����

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &worldMatrix );
	// ������ ȸ�� ����� World ��ķ� ����̽��� ����

	//////////////////////////////////////////////////////////////////////////
	// View ����� �����ϱ� ���ؼ� ������ ���� �ʿ��ϴ�.
	// ����, ����, ������
	//////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 eyePoint( 0.0f, 3.0f, -5.0f );
	// 1. ���� ��ġ			( 0, 3.0, -5)

	D3DXVECTOR3 lookAtPoint( 0.0f, 0.0f, 0.0f );
	// 2. ���� �ٶ󺸴� ��ġ	( 0, 0, 0 )

	D3DXVECTOR3 upVector( 0.0f, 1.0f, 0.0f );
	// 3. ������				( 0, 1, 0 )

	D3DXMATRIXA16 viewMatrix;
	D3DXMatrixLookAtLH( &viewMatrix, &eyePoint, &lookAtPoint, &upVector );
	// 1, 2, 3�� ������ View ��� ����

	g_pd3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );
	// ������ View ����� ����̽��� ����

	// Projection ����� �����ϱ� ���ؼ��� �þ߰�(FOV=Field Of View)�� ��Ⱦ��(aspect ratio), Ŭ���� ��� ���� �ʿ��ϴ�.
	D3DXMATRIXA16 projMatrix;

	/// matProj   : ���� ������ ���
	/// D3DX_PI/4 : FOV(D3DX_PI/4 = 45��)
	/// 1.0f      : ��Ⱦ��
	/// 1.0f      : ���� Ŭ���� ���(near clipping plane)
	/// 100.0f    : ���Ÿ� Ŭ���� ���(far clipping plane)
	D3DXMatrixPerspectiveFovLH(
		&projMatrix,
		// ����� ���� ����

		D3DX_PI / 4,
		// FOV(D3DX_PI/4 = 45��)

		1.0f,
		// ��Ⱦ�� 1:1

		1.0f,
		// Near Plane

		100.0f
		// Far Plane
		);

	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &projMatrix );
	// ������ Projection ����� ����̽��� ����
}


//////////////////////////////////////////////////////////////////////////
// ȭ�� �׸���
//////////////////////////////////////////////////////////////////////////
VOID Render()
{
	if ( NULL == g_pd3dDevice )
		return;

	// �ĸ� ���� �����
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 20, 0, 0 ), 1.0f, 0 );

	// ������ ����
	if ( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// ���� ������ ��ɵ��� ������ ��
		
		//////////////////////////////////////////////////////////////////////////
		// �� ���δ� ª�� ������ ���� ����
		//////////////////////////////////////////////////////////////////////////

		// ��� ����
		SetupMatrices();

		// ���ؽ� ���빰 �׸���
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
		
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
		if ( SUCCEEDED (InitVB()))
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

	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	// ��� �� Ŭ���� ����

	return 0;
}