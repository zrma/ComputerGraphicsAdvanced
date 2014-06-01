/**-----------------------------------------------------------------------------
 * \brief D3D API�� ����� ��Ű�� ����
 * ����: Skinning.cpp
 *
 * ����: D3D�� �⺻ API����� ����� ��Ʈ���� �ȷ�Ʈ ��� ��Ű��
 *
 *------------------------------------------------------------------------------
 */

#include <d3d9.h>
#include <d3dx9.h>



/**-----------------------------------------------------------------------------
 *  ��������
 *------------------------------------------------------------------------------
 */
LPDIRECT3D9             g_pD3D       = NULL; /// D3D ����̽��� ������ D3D��ü����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; /// �������� ���� D3D����̽�
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; /// ������ ������ ��������
LPDIRECT3DINDEXBUFFER9	g_pIB        = NULL; /// �ε����� ������ �ε�������
LPDIRECT3DTEXTURE9      g_pTexture   = NULL; /// �ؽ�ó

D3DXMATRIXA16			g_mat0;	/// 0�� ���
D3DXMATRIXA16			g_mat1;	/// 1�� ���
D3DXMATRIXA16			g_mat2;	/// 1�� ���

/// ����� ������ ������ ����ü
struct CUSTOMVERTEX
{
    D3DXVECTOR3	position;		/// ������ ��ȯ�� ��ǥ
	FLOAT		b[3];			/// blend weight
	DWORD		index;			/// blend index
    DWORD		color;			/// ������ ����
	FLOAT		tu,tv;			/// �ؽ�ó ��ǥ
};

/// ����� ���� ����ü�� ���� ������ ��Ÿ���� FVF��
/// D3DFVF_XYZB4 : 4���� blend�� 
/// D3DFVF_LASTBETA_UBYTE4 : ������ DWORD index���� unisgned byte�� 4���� ��Ÿ��
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct MYINDEX
{
	WORD	_0, _1, _2;		/// �Ϲ������� �ε����� 16��Ʈ�� ũ�⸦ ���´�.
};


/**-----------------------------------------------------------------------------
 * Direct3D �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
    /// ����̽��� �����ϱ����� D3D��ü ����
	if ( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		return E_FAIL;
	}
    /// ����̽��� ������ ����ü
    /// ������ ������Ʈ�� �׸����̱⶧����, �̹����� Z���۰� �ʿ��ϴ�.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    /// ����̽� ����
	if ( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

    /// �ø������ ����.
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    /// Z���۱���� �Ҵ�.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    /// ������ ������ �����Ƿ�, ��������� ����.
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	/// matrix palette ���
    g_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE );

	/// blend weight�� 4��(��Ÿ �ƴ�! 4��!)
    g_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_3WEIGHTS );

    return S_OK;
}




/**-----------------------------------------------------------------------------
 * �������۸� �����ϰ� �������� ä���ִ´�.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
    /// �������� ����
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
    for( DWORD i = 0 ; i < 50 ; ++i )
    {
		FLOAT theta = ( 2 * D3DX_PI*i ) / ( 50 - 1 );

        pVertices[2*i+0].position	= D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].b[0]		= 1.0f;
        pVertices[2*i+0].b[1]		= 0.0f;
        pVertices[2*i+0].b[2]		= 0.0f;
        pVertices[2*i+0].index		= 0x0000;			/// 0�� ����ġ�� 0�� ����� ������ 1.0��ŭ ����
        pVertices[2*i+0].color		= 0xffffffff;
        pVertices[2*i+0].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+0].tv       = 1.0f;

        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].b[0]		= 0.5f;
        pVertices[2*i+1].b[1]		= 0.5f;
        pVertices[2*i+1].b[2]		= 0.0f;
        pVertices[2*i+1].index		= 0x0201;			/// 0�� ����ġ�� 1�� ����� ������ 0.5��ŭ ����
        pVertices[2*i+1].color    = 0xff808080;			
        pVertices[2*i+1].tu       = ((FLOAT)i)/(50-1);
        pVertices[2*i+1].tv       = 0.0f;
    }
    g_pVB->Unlock();


    return S_OK;
}


/**-----------------------------------------------------------------------------
 * �ε������۸� �����ϰ� �ε������� ä���ִ´�.
 *------------------------------------------------------------------------------
 */
HRESULT InitIB()
{
    return S_OK;
}


/**-----------------------------------------------------------------------------
 * �������� �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	if ( FAILED( InitVB() ) )
	{
		return E_FAIL;
	}
	if ( FAILED( InitIB() ) )
	{
		return E_FAIL;
	}

    if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, "lake.bmp", &g_pTexture ) ) )
    {
        return E_FAIL;
    }

	return S_OK;
}


/**-----------------------------------------------------------------------------
 * ī�޶� ��� ����
 *------------------------------------------------------------------------------
 */
void SetupCamera()
{
	/// ������� ����
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    /// ������� ����
    D3DXVECTOR3 vEyePt( 0.0f, 2.0f,-3.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    /// �������� ��� ����
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

/**-----------------------------------------------------------------------------
 * �ִϸ��̼� ��Ļ���
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	/// 0�� ����� �������
	D3DXMatrixIdentity( &g_mat0 );

	/// 0 ~ 2PI ����(0~360��) ���� ��ȭ��Ŵ Fixed Point��� ���
	DWORD d = GetTickCount() % ( (int)((D3DX_PI*2) * 1000) );
	/// Y�� ȸ�����
    D3DXMatrixRotationY( &g_mat1, d / 1000.0f );
	D3DXMatrixRotationX( &g_mat2, d / 1000.0f );
}



/**-----------------------------------------------------------------------------
 * �ʱ�ȭ ��ü�� �Ұ�
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
	if ( g_pTexture != NULL )
	{
		g_pTexture->Release();
	}
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


/**-----------------------------------------------------------------------------
 * �޽� �׸���
 *------------------------------------------------------------------------------
 */
void DrawMesh( void )
{
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );
}

/**-----------------------------------------------------------------------------
 * ȭ�� �׸���
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// �ĸ���ۿ� Z���� �ʱ�ȭ
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0f, 0 );

	/// �ִϸ��̼� ��ļ���
	Animate();
    /// ������ ����
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {

		/// 0�� ��Ʈ���� �ȷ�Ʈ�� �������
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(0), &g_mat0 );
		/// 1�� ��Ʈ���� �ȷ�Ʈ�� ȸ�����
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(1), &g_mat1 );
		g_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX(2), &g_mat2 );

        g_pd3dDevice->SetTexture( 0, g_pTexture );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		DrawMesh();

        /// ������ ����
        g_pd3dDevice->EndScene();
    }

    /// �ĸ���۸� ���̴� ȭ������!
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




/**-----------------------------------------------------------------------------
 * ������ ���ν���
 *------------------------------------------------------------------------------
 */
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




/**-----------------------------------------------------------------------------
 * ���α׷� ������
 *------------------------------------------------------------------------------
 */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    /// ������ Ŭ���� ���
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "BasicFrame", NULL };
    RegisterClassEx( &wc );

    /// ������ ����
    HWND hWnd = CreateWindow( "BasicFrame", "Skinning",
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    /// Direct3D �ʱ�ȭ
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) )
        {
			/// ī�޶� ��ļ���
			SetupCamera();

        	/// ������ ���
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

        	/// �޽��� ����
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
            	/// �޽���ť�� �޽����� ������ �޽��� ó��
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				/// ó���� �޽����� ������ Render()�Լ� ȣ��
					Render();
			}
		}
    }

	/// ��ϵ� Ŭ���� �Ұ�
    UnregisterClass( "D3D Tutorial", wc.hInstance );
    return 0;
}
