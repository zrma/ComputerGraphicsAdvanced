//
// ������ �������̴�
// 

// ��ȯ���
float4x4 matW: WORLD;
float4x4 matV: VIEW;
float4x4 matP: PROJECTION;

// �ؽ�ó
texture	tex0;

// �Է����� ����
struct VS_INPUT
{
    float3 pos  : POSITION;
    float4 diff : COLOR0;
    float2 tex  : TEXCOORD0;
};

// ������� ����
struct VS_OUTPUT
{
    float4 pos  : POSITION;
    float4 diff : COLOR0;
    float2 tex  : TEXCOORD0;
};

// VS��� �������̴� �Լ� ����
VS_OUTPUT VS( VS_INPUT In )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float4x4 WorldView = mul(matW, matV);	// matW�� matV����� ���ؼ� WorldView��Ļ���
    float3 P = mul(float4(In.pos, 1), (float4x3)WorldView);  // view���������� ��ġ���
    Out.pos  = mul(float4(P, 1), matP);	// �������������� ��ġ���
	Out.diff = In.diff;					// �Է»����� ��»����
	Out.tex.x = In.tex.x;				// �ؽ�ó�� x��ǥ�� �״��...
	Out.tex.y = 1.0f - In.tex.y;		// �ؽ�ó�� y��ǥ�� �����´�

    return Out;
}

// �ؽ�ó ���÷� ����, 
sampler Sampler = sampler_state
{
    Texture   = (tex0); // g_pd3dDevice->SetTexture( 0, g_pTexture );
    MipFilter = LINEAR; // g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
    MinFilter = LINEAR; // g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    MagFilter = LINEAR; // g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
};

float4 PS(
    float4 Diff : COLOR0,
    float2 Tex  : TEXCOORD0) : COLOR
{
    return tex2D(Sampler, Tex) + Diff;
}


// MyShader ��ũ�м���
technique MyShader
{
    pass P0	// ������ 0��° �н�
    {
        // ������ ���� 
        Lighting       = FALSE;	// g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

        // samplers
        Sampler[0] = (Sampler);

        // texture stages
        ColorOp[0]   = MODULATE; // g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        ColorArg1[0] = TEXTURE;  // g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        ColorArg2[0] = DIFFUSE;  // g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        AlphaOp[0]   = DISABLE;  // g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        ColorOp[1]   = DISABLE;  // g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        AlphaOp[1]   = DISABLE;  // g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}