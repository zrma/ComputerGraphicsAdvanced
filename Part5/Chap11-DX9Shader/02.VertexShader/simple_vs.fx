// made by newtype@chol.com

// world * view * projection
float4x4    matWVP;

// 입력정점형식
struct VS_IN
{
    float3  p : POSITION;       // 위치(지역 좌표계)
    float3  d : COLOR0;         // 색깔
    float2  t : TEXCOORD0;      // 텍스처 좌표
};

// 출력정점형식
struct VS_OUT
{
    float4  p : POSITION;       // 위치(스크린 좌표계)
    float3  d : COLOR0;         // 색깔
    float2  t : TEXCOORD0;      // 텍스처 좌표
};

// 정점쉐이더
VS_OUT main( VS_IN In )         // VS_IN입력, VS_OUT출력
{
    VS_OUT  o = (VS_OUT)0;      // 출력할 변수 초기화

    // o.p = In.p * matWVP      
    o.p = mul( float4(In.p,1), matWVP );    // 입력정점의 위치좌표를 스크린좌표계로 변환
    o.d = In.d;                             // 색깔 값은 고정
    o.t = float2( In.t.x, 1 - In.t.y );     // 텍스처의 y좌표를 뒤집는다
    
    return o;                               // 출력정점을 반환
}
