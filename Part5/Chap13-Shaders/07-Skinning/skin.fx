//
// 간단한 정점쉐이더
// 

// 변환행렬
float3	 vLight;
float4x4 matV;
float4x4 matP;
float4x4 matPal[50];

// 입력정점 형식
struct VS_INPUT
{
    float3 p : POSITION;
    float3 n : NORMAL;
    float2 t : TEXCOORD0;
    float4 b : TEXCOORD1;
    float4 i : TEXCOORD2;
};

// 출력정점 형식
struct VS_OUTPUT
{
	float4 p : POSITION;
	float4 d : COLOR0;
	float2 t : TEXCOORD;
};

// 정점쉐이더 함수 선언
VS_OUTPUT main( VS_INPUT In )
{
	// 출력정점
    VS_OUTPUT Out = (VS_OUTPUT)0;

	// 임시변수
	float3 p;
	float3 p2 = float3( 0, 0, 0 );

	// 첫번째 매트릭스 팔레트연산
	p = mul( float4(In.p, 1), matPal[In.i.x] ); // p = v * mat[index1]
	p *= In.b.x;								// p = p * weight[index1]
	p2 = float4(p,1);							// p2 = p2 + p

	// 두번째 매트릭스 팔레트연산
	p = mul( float4(In.p, 1), matPal[In.i.y] );
	p *= In.b.y;
	p2 += float4(p,1);
	    
	// 세번째 매트릭스 팔레트연산
	p = mul( float4(In.p, 1), matPal[In.i.z] );
	p *= In.b.z;
	p2 += float4(p,1);
	
	// 네번째 매트릭스 팔레트연산
	p = mul( float4(In.p, 1), matPal[In.i.w] );
	p *= In.b.w;
	p2 += float4(p,1);
	
	p = mul( float4(p2,1), (float4x3)matV );	// p = p2 * matView
    Out.p = mul( float4(p, 1), matP );			// Out.p = p * matProj
    float3 N = normalize(mul(In.n, (float3x3)matV)); // 정점의 법선벡터를 view공간으로
    float3 L = vLight;
    Out.d = max( dot( N, L ), 0 );			// dot( 법선벡터, 광원벡터 )
	Out.t = In.t;
    return Out;
}
