// made by newtype@chol.com

sampler2D Sampler

// �Է���������
struct PS_IN
{
	float4	d : COLOR0;
	float2	t : TEXCOORD0;
};

// �����������
struct PS_OUT
{
	float4	d : COLOR;
};

// �ȼ����̴� ����
PS_OUT main( PS_IN In )
{
	PS_OUT	o = (PS_OUT)0;
	
	// ��� color = texture + diffuse
	o.d = tex2D(Sampler,In.t) + In.d;
	
    return o;
}
