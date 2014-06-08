// made by newtype@chol.com

// world * view * projection
float4x4    matWVP;

// �Է���������
struct VS_IN
{
    float3  p : POSITION;       // ��ġ(���� ��ǥ��)
    float3  d : COLOR0;         // ����
    float2  t : TEXCOORD0;      // �ؽ�ó ��ǥ
};

// �����������
struct VS_OUT
{
    float4  p : POSITION;       // ��ġ(��ũ�� ��ǥ��)
    float3  d : COLOR0;         // ����
    float2  t : TEXCOORD0;      // �ؽ�ó ��ǥ
};

// �������̴�
VS_OUT main( VS_IN In )         // VS_IN�Է�, VS_OUT���
{
    VS_OUT  o = (VS_OUT)0;      // ����� ���� �ʱ�ȭ

    // o.p = In.p * matWVP      
    o.p = mul( float4(In.p,1), matWVP );    // �Է������� ��ġ��ǥ�� ��ũ����ǥ��� ��ȯ
    o.d = In.d;                             // ���� ���� ����
    o.t = float2( In.t.x, 1 - In.t.y );     // �ؽ�ó�� y��ǥ�� �����´�
    
    return o;                               // ��������� ��ȯ
}
