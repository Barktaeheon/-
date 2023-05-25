#include "Shader_Defines.hpp"

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

float _fTexWidth = 1280.f;
float _fTexHeight = 720.f;

float2 _ScreenSunPos = float2(0.5, 0.1);
int _iNumSamples = 10;
float _fUVNumSamples = 4.f;
float _Exposure = 1.f;
float _Density = 1.f;
float _illuminationDecay = 1.f;
float _Decay = 0.6f;
float2 _SunDirVel = float2(0, -3);

void VolumetricLight(in float2 uv)
{
	//fC �� ���������� �ؽ��� ����
	float4 fC = g_DiffuseTexture.Sample(DefaultSampler, uv);// tex2D(Samp_Texture_Orig, uv);
	float4 PsColor = fC;

	//���ø� ����
	//������ ���������� �ε巯�����ϴٸ�,
	//�׸�ŭ ���ϴ� Ŀ��������.
	int iCount = 20;

	//������������ ������ uv ���� �޾ƿͼ�
	float2 texCoord = uv;

	//uv �� �̵���ų ���⼺�� ���صΰ�,
	float2 deltaTexCoord = texCoord - _ScreenSunPos;

	//������ �е�ġ
	//�߱�ü�� ����� ���� ���� ���ݿ� ������ ��Ĩ�ϴ�.
	deltaTexCoord.x *= 1.f / (float)iCount * _Density;
	deltaTexCoord.y *= 1.f / (float)iCount * _Density;

	//Samp_Texture_Light
	float4 color = vector(0.3f, 0.3f, 0.3f, 0.3f);//tex2D(Samp_Texture_Light, texCoord);

	//������ ����ġ 
	float illuminationDecay = _illuminationDecay;
	float4 sample = float4(1, 1, 1, 1);

	for (int i = 0; i < iCount; i++)
	{
		texCoord -= deltaTexCoord;

		//fTexWidth, fTexHeight �� ��ũ�� ũ�� (1280, 720 ����....) 
		//sun ���⼺ ������,
		//������ ������� ������ ���Ҽ� �ֽ��ϴ�
		texCoord.x += 1.f / _fTexWidth * _SunDirVel.x;
		texCoord.y += 1.f / _fTexHeight * _SunDirVel.y;

		//
		vector vLight = vector(0.3f, 0.3f, 0.3f, 0.3f);

		//������ �־������� ����������
		//����ġ�� �����ݴϴ�.
		vLight *= illuminationDecay;

		//�÷����� ������ŵ�ϴ�.
		color += vLight;

		//����ó��
		illuminationDecay *= _Decay;
	}

	//�������� �ȼ��÷��� ���� �ݴϴ�.
	//���� ���� ���� ���Ѵٸ�,
	//_Exposure ���� ũ�� �ָ� �˴ϴ�.
	PsColor = fC + (color * _Exposure);

	return PsColor;
}



PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	return Out;
}



technique11			DefaultTechnique
{
	pass Light
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}