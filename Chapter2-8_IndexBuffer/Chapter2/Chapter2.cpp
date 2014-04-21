#include "stdafx.h"
#include "Chapter2.h"

#include <d3d9.h>
#include <d3dx9.h>


//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////
LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9	g_pIB = NULL;


// Ŀ���� ���ؽ� ���� ����ü
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};

// ����� ���� ����ü�� ���� ������ ��Ÿ���� FVF
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct MYINDEX
{
	// 16��Ʈ ũ��� �ε��� ����
	DWORD	_0, _1, _2;
	
	// 32��Ʈ ũ��� �ε��� ����
	// UINT	_0, _1, _2;
};


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

	// ���� ��� ����
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ���ؽ� ���� ����
//////////////////////////////////////////////////////////////////////////
HRESULT InitVB()
{
	// ����(cube)�� ������ �ϱ� ���� 8���� ���ؽ� ����

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

	// 8���� ���ؽ��� ������ ���ؽ� ���۸� �����Ѵ�.
	// FVF�� �����Ͽ� ������ �������� ������ �����Ѵ�.
	
	if ( FAILED( g_pd3dDevice->CreateVertexBuffer( 8 * sizeof( CUSTOMVERTEX ), 0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Lock�� �ɾ �޸𸮸� �Ҵ��ϰ� �����͸� �޾ƿ´�.
	// �Ҵ� ���� �޸𸮿� ���ؽ� ���� ���� ä���.
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
// �ε��� ���� ����
//////////////////////////////////////////////////////////////////////////
HRESULT InitIB()
{

	// ����(cube)�� �������ϱ����� 12���� ���ؽ��� ����
	MYINDEX	indices[] =
	{
		{ 0, 1, 2 }, { 0, 2, 3 },	// ����
		{ 4, 6, 5 }, { 4, 7, 6 },	// �Ʒ���
		{ 0, 3, 7 }, { 0, 7, 4 },	// �޸�
		{ 1, 5, 6 }, { 1, 6, 2 },	// ������
		{ 3, 2, 6 }, { 3, 6, 7 },	// �ո�
		{ 0, 4, 5 }, { 0, 5, 1 }	// �޸�
	};

	//////////////////////////////////////////////////////////////////////////
	// �ε������� ����
	// D3DFMT_INDEX16�� �ε����� ������ 16��Ʈ
	// D3DFMT_INDEX32�� �ε����� ������ 32��Ʈ
	// MYINDEX ����ü�� �������� ����� �Ѵ�.
	if ( FAILED( g_pd3dDevice->CreateIndexBuffer( 12 * sizeof( MYINDEX ), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// �ε��� ���� ä���
	// �ε��� ���۵� ���������� Lock()���� �޸� �ּҸ� ��� �´�.

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
// ��ü ���� - ���� ����!
//////////////////////////////////////////////////////////////////////////
VOID Cleanup()
{
	if ( g_pMeshMaterials != NULL )
		delete[] g_pMeshMaterials;

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
		g_pMesh->Release();

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
	D3DXMatrixIdentity( &worldMatrix );
	// ���� ��ķ� ����
	D3DXMatrixRotationY( &worldMatrix, timeGetTime() / 1000.0f );
	// X���� �߽����� Rotate ��� ����
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
		
		// ��� ����
		SetupMatrices();

		// �޽ô� ������ �ٸ� �޽� ���� �κ� ������ �̷�� �ִ�.
		// �̵��� ������ �����ؼ� ��� �׷��ش�.
		for ( DWORD i = 0; i < g_dwNumMaterials; ++i )
		{
			// �κ����� �޽��� ������ �ؽ��� ����
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
			g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );

			// �κ����� �޽� ���
			g_pMesh->DrawSubset( i );
			// DrawSubset() - ���� �� �κ� �޽ú��� ���� �׸��� �Լ�
		}

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
							  WS_OVERLAPPEDWINDOW, 100, 100, 600, 600,
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

	UnregisterClass( L"D3D Tutorial", wc.hInstance );
	// ��� �� Ŭ���� ����

	return 0;
}