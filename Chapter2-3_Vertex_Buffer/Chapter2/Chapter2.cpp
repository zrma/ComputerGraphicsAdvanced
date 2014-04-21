#include "stdafx.h"
#include "Chapter2.h"

#include <d3d9.h>
// Direct3D9�� ����ϱ� ���� ���

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
	FLOAT x, y, z, rhw;
	// rhw���� ������ ��ȯ�� �Ϸ�� ���� - ȭ�� UI � ���

	DWORD color;
	// ���ؽ� ���� ����
};

// Ŀ���ҹ��ؽ� ����ü�� ���� ������ ��Ÿ���� FVF��
// ����ü�� X,Y,Z,RHW ���� Diffuse ���� ������ �̷���� �ִ�
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
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
	// ����̽� ������ ���� ������ ��Ƽ� �Ѱ� �� ����ü
	/*
	typedef struct D3DPRESENT_PARAMETERS
	{
		UINT                BackBufferWidth;
		UINT                BackBufferHeight;
		D3DFORMAT           BackBufferFormat;
		UINT                BackBufferCount;
		// �� ���۸� ���� ����
		
		D3DMULTISAMPLE_TYPE MultiSampleType;
		DWORD               MultiSampleQuality;
		// ��Ƽ���ø��� ���� ����

		D3DSWAPEFFECT       SwapEffect;
		// ���� ���� ���� ȿ��
		// http://blog.naver.com/bastard9/140114992482 ����

		HWND                hDeviceWindow;
		// â �ڵ�

		BOOL                Windowed;
		// True - â ��� / False - Ǯ ��ũ��

		BOOL                EnableAutoDepthStencil;
		D3DFORMAT           AutoDepthStencilFormat;
		// ���� ���ٽ� ���� ����� ���� ����

		DWORD               Flags;
		// ���� �÷���
		// http://msdn.microsoft.com/en-us/library/windows/desktop/bb172586(v=vs.85).aspx ����

		UINT                FullScreen_RefreshRateInHz;
		UINT                PresentationInterval;
		// ȭ�� �ֻ��� ���� ����

	} D3DPRESENT_PARAMETERS, *LPD3DPRESENT_PARAMETERS;
	*/
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	// �ݵ�� �ʱ�ȭ �� ��!

	d3dpp.Windowed = TRUE;
	// â ���

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	// ���� ȿ������ SWAPȿ��
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb172612(v=vs.85).aspx ����

	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	// ���� ����ȭ�� ��忡 ���缭 �ĸ���۸� ����

	//////////////////////////////////////////////////////////////////////////
	// ����̽��� ������ ���� �������� �����Ѵ�.
	//
	// 1. ����Ʈ �׷��� ī�带 ��� (��κ� �׷��� ī�尡 1��)
	// 2. HAL ����̽��� ���� (HW ���� ��ġ ���)
	// 3. ���� ó���� ��� ī�忡�� �����ϴ� SWó���� ���� (HW�� ������ ��� ���� ���� ������ ����)
	//////////////////////////////////////////////////////////////////////////
	if ( FAILED( g_pD3D->CreateDevice(
		
		D3DADAPTER_DEFAULT,
		// ����Ͱ� ���� ���� ��� ���� ����

		D3DDEVTYPE_HAL,
		// ��� ����̽��� ������ ����
		// D3DDEVTYPE_HAL / D3DDEVTYPE_SW / D3DDEVTYPE_REF
		//
		// D3DDEVTYPE_HAL - �ϵ���� ������ �����ϴ� ����̽�

		hWnd,
		// ������ �ڵ�
		// ��ü ȭ�� ����� ���, �ֻ��� �����츸 ����

		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		// ���̴� ���ؽ� ������ ����Ʈ���������� �� ������ �ϵ���������� �� ������

		&d3dpp,
		// ����̽� ���� ����ü ������

		&g_pd3dDevice
		// ���� ���� ����
		) ) )
	{
		return E_FAIL;
	}

	// ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.

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
		{ 150.0f, 50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{ 50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};

	// ���ؽ� ���� ����
	// 3���� ����� ���ؽ��� ������ �޸𸮸� �Ҵ�
	// FVF�� �����Ͽ� ���ؽ��� ������ �����Ѵ�
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		3 * sizeof( CUSTOMVERTEX ),
		// ���� �� ���ؽ� ������ ����Ʈ ���� ũ��

		0,
		// ���ؽ� ���� ����, ó�� ���(SW, HW) ����

		D3DFVF_CUSTOMVERTEX,
		// Ŀ���� ���ؽ� ����ü FVF �÷��� ��

		D3DPOOL_DEFAULT,
		// ���ؽ� ���۰� ���� �� �޸��� ��ġ(�׷��� ī��, �ý��� �޸�)�� ���� ���

		&g_pVB,
		// ���� ���� ���ؽ� ���� ����

		NULL
		// ���� �Ǿ���
		) ) )
	{
		return E_FAIL;
	}

	// ���ؽ� ���۸� ������ ä���. 
	// ���ؽ� ������ Lock()�Լ��� ȣ���Ͽ� �����͸� �����´�.
	VOID* pVertices;
	if ( FAILED( g_pVB->Lock( 
		0,
		// Lock ���� ������
		// ���� ���ڿ� �Բ� ��� 0�̸� ��ü ����

		sizeof( vertices ),
		// Lock�� �� ���� ũ��
		// ���� ���ڿ� �Բ� ��� 0�̸� ��ü ����

		(void**)&pVertices,
		// Lock���� �޾ƿ��� �޸� ������ �����͸� ���� ����

		0
		// Lock�� ������ �� �Բ� ����� �÷���
		) ) )
	{
		return E_FAIL;
	}
	memcpy( pVertices, vertices, sizeof( vertices ) );
	
	g_pVB->Unlock();
	// �ݵ�� Unlock �ؾ� �Ѵ�. Unlock�� ���� ���� ��� �׷���ī�尡 �ٿ� �� �� ����!

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

		// ���ؽ� ������ ���빰�� �׸���

		// 1. ���ؽ� ������ ��� �ִ� ���ؽ� ���۸� ��� ��Ʈ������ �Ҵ�
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );

		// 2. D3D���� ���ؽ� ���̴� ������ �����Ѵ�.
		// ��κ��� ��쿡�� FVF�� ����
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

		// 3. ���� ������ ����ϱ� ���� DrawPrimitive() �Լ� ȣ��
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
		// DrawPrimitive - �ﰢ���� �׷��ִ� �Լ�
		// ù��° ���� - ���ؽ��� �̾���� ���
		
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

	// ������ Ŭ���� ����� ���� ����ü
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
	// ������ Ŭ���� ���

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