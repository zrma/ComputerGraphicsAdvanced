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
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; 

LPD3DXMESH              g_pMesh = NULL;
// �޽� ��ü

D3DMATERIAL9*           g_pMeshMaterials = NULL;
// �޽ÿ��� ����� ����

LPDIRECT3DTEXTURE9*     g_pMeshTextures = NULL;
// �޽ÿ��� ����� �ؽ���

DWORD                   g_dwNumMaterials = 0L;
// �޽ÿ��� ��� �� ���� ����

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

	// �ֺ��������� �ִ����
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ���� ����
//////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
	// ������ �ӽ÷� ������ ���ۼ���
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// .x������ �޽÷� �о���δ�. �̶� ���������� �Բ� ����
	if ( FAILED( D3DXLoadMeshFromX( L"Girl.x", D3DXMESH_SYSTEMMEM, g_pd3dDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh ) ) )
	{
		/// ���� ������ ������ ������ �������� �˻�
		if ( FAILED( D3DXLoadMeshFromX( L"..\\Girl.x", D3DXMESH_SYSTEMMEM, g_pd3dDevice, NULL,
			&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh ) ) )
		{
			MessageBox( NULL, L"Could not find Girl.x", L"Meshes.exe", MB_OK );
			return E_FAIL;
		}
	}

	if ( !( g_pMesh->GetFVF() & D3DFVF_NORMAL ) )
	{
		// ���� ������ ������ ���� �ʴٸ� �޽��� �����ϰ� D3DFVF_NORMAL�� �߰��Ѵ�.
		ID3DXMesh* pTempMesh = 0;
		g_pMesh->CloneMeshFVF( D3DXMESH_MANAGED, g_pMesh->GetFVF() | D3DFVF_NORMAL, g_pd3dDevice, &pTempMesh );

		// ������ ����Ѵ�.
		D3DXComputeNormals( pTempMesh, 0 );

		g_pMesh->Release();
		// ���� �޽��� �����Ѵ�
		g_pMesh = pTempMesh;
		// ���� �޽��� ������ ���� �޽��� �����Ѵ�.
	}

	// ���� ������ �ؽ��� ������ ���� �̱�
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	// ���� ������ŭ ���� ����ü �迭 ����
	if ( NULL == g_pMeshMaterials )
	{
		return E_OUTOFMEMORY;
	}

	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	// ���� ������ŭ �ؽ��� �迭 ����
	if ( NULL == g_pMeshTextures )
	{
		return E_OUTOFMEMORY;
	}

	for ( DWORD i = 0; i < g_dwNumMaterials; ++i )
	{
		// ���� ���� ����
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// �ֺ� ���� ������ Diffuse ������ ����
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if ( d3dxMaterials[i].pTextureFilename != NULL &&
			 lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
		{
			// �ؽ��� �б�
			if ( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice,
				d3dxMaterials[i].pTextureFilename,
				&g_pMeshTextures[i] ) ) )
			{
				// �ؽ��İ� ���� ������ ������ �������� �˻�
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

	// �ӽ÷� ������ �������� �Ұ�
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// ��ü ���� - ���� ����!
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