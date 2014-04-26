/**-----------------------------------------------------------------------------
 * \brief Ű������ ����� �ִϸ��̼�
 * ����: Animate.cpp
 *
 * ����: �ִϸ��̼��� ���� ��ǥ���� ����� Ű������ �ִϸ��̼��̴�.
 *       �츮�� Position�� Rotation�� Ű���� �����, �̵� Ű�� ����(interpoate)
 *       �ϴ� �ִϸ��̼��� ������ �� ���̴�.
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

D3DXMATRIXA16			g_matTMParent;	/// �θ��� TM
D3DXMATRIXA16			g_matRParent;	/// �θ��� ȸ�����

D3DXMATRIXA16			g_matTMChild;	/// �ڽ��� TM
D3DXMATRIXA16			g_matRChild;	/// �ڽ��� ȸ�����

float					g_fRot = 0.0f;

/// ����� ������ ������ ����ü
struct CUSTOMVERTEX
{
    FLOAT x, y, z;	/// ������ ��ȯ�� ��ǥ
    DWORD color;	/// ������ ����
};

/// ����� ���� ����ü�� ���� ������ ��Ÿ���� FVF��
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct MYINDEX
{
	WORD	_0, _1, _2;		/// �Ϲ������� �ε����� 16��Ʈ�� ũ�⸦ ���´�.
};

/// �ִϸ��̼� Ű���� �����ϱ����� �迭
D3DXVECTOR3		g_aniPos[2];		/// ��ġ(position)Ű ��
D3DXQUATERNION	g_aniRot[2];		/// ȸ��(quaternion)Ű ��

/**-----------------------------------------------------------------------------
 * Direct3D �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitD3D( HWND hWnd )
{
    /// ����̽��� �����ϱ����� D3D��ü ����
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

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
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    /// �ø������ ����.
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

    /// Z���۱���� �Ҵ�.
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    /// ������ ������ �����Ƿ�, ��������� ����.
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    return S_OK;
}




/**-----------------------------------------------------------------------------
 * �������۸� �����ϰ� �������� ä���ִ´�.
 *------------------------------------------------------------------------------
 */
HRESULT InitVB()
{
    /// ����(cube)�� �������ϱ����� 8���� ������ ����
    CUSTOMVERTEX vertices[] =
    {
		{ -1,  1,  1 , 0xffff0000 },		/// v0
		{  1,  1,  1 , 0xff00ff00 },		/// v1
		{  1,  1, -1 , 0xff0000ff },		/// v2
		{ -1,  1, -1 , 0xffffff00 },		/// v3

		{ -1, -1,  1 , 0xff00ffff },		/// v4
		{  1, -1,  1 , 0xffff00ff },		/// v5
		{  1, -1, -1 , 0xff000000 },		/// v6
		{ -1, -1, -1 , 0xffffffff },		/// v7
    };

    /// �������� ����
    /// 8���� ����������� ������ �޸𸮸� �Ҵ��Ѵ�.
    /// FVF�� �����Ͽ� ������ �������� ������ �����Ѵ�.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 8*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    /// �������۸� ������ ä���. 
    /// ���������� Lock()�Լ��� ȣ���Ͽ� �����͸� ���´�.
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    g_pVB->Unlock();

    return S_OK;
}


/**-----------------------------------------------------------------------------
 * �ε������۸� �����ϰ� �ε������� ä���ִ´�.
 *------------------------------------------------------------------------------
 */
HRESULT InitIB()
{
    /// ����(cube)�� �������ϱ����� 12���� ���� ����
    MYINDEX	indices[] =
    {
		{ 0, 1, 2 }, { 0, 2, 3 },	/// ����
		{ 4, 6, 5 }, { 4, 7, 6 },	/// �Ʒ���
		{ 0, 3, 7 }, { 0, 7, 4 },	/// �޸�
		{ 1, 5, 6 }, { 1, 6, 2 },	/// ������
		{ 3, 2, 6 }, { 3, 6, 7 },	/// �ո�
		{ 0, 4, 5 }, { 0, 5, 1 }	/// �޸�
    };

    /// �ε������� ����
	/// D3DFMT_INDEX16�� �ε����� ������ 16��Ʈ ��� ���̴�.
	/// �츮�� MYINDEX ����ü���� WORD������ ���������Ƿ� D3DFMT_INDEX16�� ����Ѵ�.
    if( FAILED( g_pd3dDevice->CreateIndexBuffer( 12 * sizeof(MYINDEX), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL ) ) )
    {
        return E_FAIL;
    }

    /// �ε������۸� ������ ä���. 
    /// �ε��������� Lock()�Լ��� ȣ���Ͽ� �����͸� ���´�.
    VOID* pIndices;
    if( FAILED( g_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 ) ) )
        return E_FAIL;
    memcpy( pIndices, indices, sizeof(indices) );
    g_pIB->Unlock();

    return S_OK;
}

void InitAnimation()
{
	g_aniPos[0] = D3DXVECTOR3( 0, 0, 0 );	/// ��ġ Ű(0,0,0)
	g_aniPos[1] = D3DXVECTOR3( 5, 5, 5 );	/// ��ġ Ű(5,5,5)

    FLOAT Yaw	= D3DX_PI * 90.0f / 180.0f;		/// Y�� 90�� ȸ��
    FLOAT Pitch	= 0;
    FLOAT Roll	= 0;
	D3DXQuaternionRotationYawPitchRoll( &g_aniRot[0], Yaw, Pitch, Roll );	/// ����� Ű(Y��90��)

    Yaw	= 0;
    Pitch	= D3DX_PI * 90.0f / 180.0f;			/// X�� 90�� ȸ��
    Roll	= 0;
	D3DXQuaternionRotationYawPitchRoll( &g_aniRot[1], Yaw, Pitch, Roll );	/// ����� Ű(X��90��)
}

/**-----------------------------------------------------------------------------
 * �������� �ʱ�ȭ
 *------------------------------------------------------------------------------
 */
HRESULT InitGeometry()
{
	if( FAILED( InitVB() ) ) return E_FAIL;
	if( FAILED( InitIB() ) ) return E_FAIL;

	InitAnimation();

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
    D3DXVECTOR3 vEyePt( 0.0f, 10.0f,-20.0f );
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

/// ��������(Linear Interpolation) �Լ�
float Linear( float v0, float v1, float t )
{
	return v0 * ( 1.0f - t ) + v1 * t;
// ���� �ٷ� �ٲ㵵 �ȴ�.
//	return v0 + t * ( v1 - v0 );
}

/**-----------------------------------------------------------------------------
 * �ִϸ��̼� ��Ļ���
 *------------------------------------------------------------------------------
 */
VOID Animate()
{
	static float t = 0;
	float x, y, z;
	D3DXQUATERNION quat;

	if( t > 1.0f ) t = 0.0f;

	/// ��ġ���� ��������
	x = Linear(g_aniPos[0].x, g_aniPos[1].x, t );
	y = Linear(g_aniPos[0].y, g_aniPos[1].y, t );
	z = Linear(g_aniPos[0].z, g_aniPos[1].z, t );
	D3DXMatrixTranslation( &g_matTMParent, x, y, z );	/// �̵������ ���Ѵ�.

//	���� 4���� ������ 3�ٷ� �ٲܼ� �ִ�.
//	D3DXVECTOR3 v;
//	D3DXVec3Lerp( &v, &g_aniPos[0], &g_aniPos[1], t );
//	D3DXMatrixTranslation( &g_matTMParent, v.x, v.y, v.z );

	/// ȸ������ ���鼱������
	D3DXQuaternionSlerp( &quat, &g_aniRot[0], &g_aniRot[1], t );
	D3DXMatrixRotationQuaternion( &g_matRParent, &quat );	/// ������� ȸ����İ����� ��ȯ
	t += 0.005f;

	/// �ڽĸ޽��� Z�� ȸ�����
    D3DXMatrixRotationZ( &g_matRChild, GetTickCount()/500.0f );
	/// �ڽĸ޽ô� �������κ��� (3,3,3)�Ÿ��� ����
	D3DXMatrixTranslation( &g_matTMChild, 3, 3, 3 );
}



/**-----------------------------------------------------------------------------
 * �ʱ�ȭ ��ü�� �Ұ�
 *------------------------------------------------------------------------------
 */
VOID Cleanup()
{
    if( g_pIB != NULL )        
        g_pIB->Release();

    if( g_pVB != NULL )        
        g_pVB->Release();

    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )       
        g_pD3D->Release();
}


/**-----------------------------------------------------------------------------
 * �޽� �׸���
 *------------------------------------------------------------------------------
 */
void DrawMesh( D3DXMATRIXA16* pMat )
{
    g_pd3dDevice->SetTransform( D3DTS_WORLD, pMat );
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->SetIndices( g_pIB );
	g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );
}

/**-----------------------------------------------------------------------------
 * ȭ�� �׸���
 *------------------------------------------------------------------------------
 */
VOID Render()
{
	D3DXMATRIXA16	matWorld;

    /// �ĸ���ۿ� Z���� �ʱ�ȭ
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	/// �ִϸ��̼� ��ļ���
	Animate();
    /// ������ ����
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		matWorld = g_matRParent * g_matTMParent;
		DrawMesh( &matWorld );		/// �θ� ���� �׸���

		matWorld = g_matRChild * g_matTMChild * matWorld;
//		�ٷ����� ��� ���� ���
//		matWorld = g_matRChild * g_matTMChild * g_matRParent * g_matTMParent;
		DrawMesh( &matWorld );		/// �ڽ� ���� �׸���

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

#define ROT_DELTA	0.1f
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
		case WM_KEYDOWN :
			if( wParam == VK_LEFT ) g_fRot -= ROT_DELTA;
			if( wParam == VK_RIGHT ) g_fRot += ROT_DELTA;
			break;
			
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
    HWND hWnd = CreateWindow( "BasicFrame", "Keyframe Animation",
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
