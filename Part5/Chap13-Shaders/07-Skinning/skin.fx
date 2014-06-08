//
// ������ �������̴�
// 

// ��ȯ���
float3	 vLight;
float4x4 matV;
float4x4 matP;
float4x4 matPal[50];

// �Է����� ����
struct VS_INPUT
{
    float3 p : POSITION;
    float3 n : NORMAL;
    float2 t : TEXCOORD0;
    float4 b : TEXCOORD1;
    float4 i : TEXCOORD2;
};

// ������� ����
struct VS_OUTPUT
{
	float4 p : POSITION;
	float4 d : COLOR0;
	float2 t : TEXCOORD;
};

// �������̴� �Լ� ����
VS_OUTPUT main( VS_INPUT In )
{
	// �������
    VS_OUTPUT Out = (VS_OUTPUT)0;

	// �ӽú���
	float3 p;
	float3 p2 = float3( 0, 0, 0 );

	// ù��° ��Ʈ���� �ȷ�Ʈ����
	p = mul( float4(In.p, 1), matPal[In.i.x] ); // p = v * mat[index1]
	p *= In.b.x;								// p = p * weight[index1]
	p2 = float4(p,1);							// p2 = p2 + p

	// �ι�° ��Ʈ���� �ȷ�Ʈ����
	p = mul( float4(In.p, 1), matPal[In.i.y] );
	p *= In.b.y;
	p2 += float4(p,1);
	    
	// ����° ��Ʈ���� �ȷ�Ʈ����
	p = mul( float4(In.p, 1), matPal[In.i.z] );
	p *= In.b.z;
	p2 += float4(p,1);
	
	// �׹�° ��Ʈ���� �ȷ�Ʈ����
	p = mul( float4(In.p, 1), matPal[In.i.w] );
	p *= In.b.w;
	p2 += float4(p,1);
	
	p = mul( float4(p2,1), (float4x3)matV );	// p = p2 * matView
    Out.p = mul( float4(p, 1), matP );			// Out.p = p * matProj
    float3 N = normalize(mul(In.n, (float3x3)matV)); // ������ �������͸� view��������
    float3 L = vLight;
    Out.d = max( dot( N, L ), 0 );			// dot( ��������, �������� )
	Out.t = In.t;
    return Out;
}
