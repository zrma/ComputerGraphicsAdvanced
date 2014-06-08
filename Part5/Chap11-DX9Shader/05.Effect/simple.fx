//
// 간단한 정점쉐이더
// 

// 변환행렬
float4x4 matW: WORLD;
float4x4 matV: VIEW;
float4x4 matP: PROJECTION;

// 텍스처
texture	tex0;

// 입력정점 형식
struct VS_INPUT
{
    float3 pos  : POSITION;
    float4 diff : COLOR0;
    float2 tex  : TEXCOORD0;
};

// 출력정점 형식
struct VS_OUTPUT
{
    float4 pos  : POSITION;
    float4 diff : COLOR0;
    float2 tex  : TEXCOORD0;
};

// VS라는 정점쉐이더 함수 선언
VS_OUTPUT VS( VS_INPUT In )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
    
    float4x4 WorldView = mul(matW, matV);	// matW와 matV행렬을 곱해서 WorldView행렬생성
    float3 P = mul(float4(In.pos, 1), (float4x3)WorldView);  // view공간에서의 위치계산
    Out.pos  = mul(float4(P, 1), matP);	// 투영공간에서의 위치계산
	Out.diff = In.diff;					// 입력색깔을 출력색깔로
	Out.tex.x = In.tex.x;				// 텍스처의 x좌표는 그대로...
	Out.tex.y = 1.0f - In.tex.y;		// 텍스처의 y좌표를 뒤집는다

    return Out;
}

// 텍스처 샘플러 상태, 
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


// MyShader 테크닉선언
technique MyShader
{
    pass P0	// 최초의 0번째 패스
    {
        // 광원은 끈다 
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