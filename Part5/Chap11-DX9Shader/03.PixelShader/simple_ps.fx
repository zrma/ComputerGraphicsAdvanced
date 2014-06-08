// made by newtype@chol.com

sampler2D Sampler

// 입력정점형식
struct PS_IN
{
	float4	d : COLOR0;
	float2	t : TEXCOORD0;
};

// 출력정점형식
struct PS_OUT
{
	float4	d : COLOR;
};

// 픽셀쉐이더 선언
PS_OUT main( PS_IN In )
{
	PS_OUT	o = (PS_OUT)0;
	
	// 출력 color = texture + diffuse
	o.d = tex2D(Sampler,In.t) + In.d;
	
    return o;
}
