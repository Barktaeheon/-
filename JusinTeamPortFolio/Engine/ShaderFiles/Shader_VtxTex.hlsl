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


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}

PS_OUT PS_ALPHABLEND(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// uv ȸ��
	// uv = In.vTexUV;
	////uv.y += g_Time;
	//uv.xy -= 0.5f;
	//float s = -sin(5.f * g_Time);
	//float c = cos(5.f * g_Time);
	//float2x2 rotationMatrix = float2x2(c, -s, s, c);
	//rotationMatrix *= 0.5;
	//rotationMatrix += 0.5;
	//rotationMatrix = rotationMatrix * 2 - 1;
	//uv.xy = mul(uv.xy, rotationMatrix);
	//uv.xy += 0.5f;	

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}

//------------------------------------------------------------------------------------------

texture2D	g_AlphaTexture;
texture2D	g_DistortionTexture;
float g_Time;

struct VS_OUT_FIRE
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;

	float2 vNoiseUV1 : TEXCOORD1;
	float2 vNoiseUV2 : TEXCOORD2;
	float2 vNoiseUV3 : TEXCOORD3;
};


VS_OUT_FIRE VS_FIRE(VS_IN In)
{
	VS_OUT_FIRE		Out = (VS_OUT_FIRE)0;

	vector		vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	float3 scrollSpeeds = float3(1.3f, 2.1f, 2.3f);
	float3 scales = float3(1.0f, 2.0f, 3.0f);
	Out.vNoiseUV1 = (In.vTexUV * scales.x);
	Out.vNoiseUV1.y = Out.vNoiseUV1.y + (g_Time * scrollSpeeds.x);

	Out.vNoiseUV2 = (In.vTexUV * scales.y);
	Out.vNoiseUV2.y = Out.vNoiseUV2.y + (g_Time * scrollSpeeds.y);

	Out.vNoiseUV3 = (In.vTexUV * scales.z);
	Out.vNoiseUV3.y = Out.vNoiseUV3.y + (g_Time * scrollSpeeds.z);

	return Out;
}

struct PS_IN_FIRE
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;

	float2 vNoiseUV1 : TEXCOORD1;
	float2 vNoiseUV2 : TEXCOORD2;
	float2 vNoiseUV3 : TEXCOORD3;
};

float4 PS_EFFECT_FIRE(PS_IN_FIRE In) : SV_TARGET0
{
	float2 distortion1 = float2(0.1f, 0.2f);
	float2 distortion2 = float2(0.1f, 0.3f);
	float2 distortion3 = float2(0.1f, 0.1f);

	float distortionScale = 0.8f;
	float distortionBias = 0.5f;

	vector noise1 = g_DistortionTexture.Sample(DefaultSampler, In.vNoiseUV1);
	vector noise2 = g_DistortionTexture.Sample(DefaultSampler, In.vNoiseUV2);
	vector noise3 = g_DistortionTexture.Sample(DefaultSampler, In.vNoiseUV3);

	noise1 = (noise1 - 0.5f) * 2.0f;
	noise2 = (noise2 - 0.5f) * 2.0f;
	noise3 = (noise3 - 0.5f) * 2.0f;

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	vector finalNoise = noise1 + noise2 + noise3;

	// �ְ� ������ �� ���̾ ���� ���� �Է� �ؽ�ó Y ��ǥ�� �ְ��մϴ�.
	// ���� ȿ������ �Ҳ��� ���� �Ÿ��� �ؽ�ó�� ���� �����̸� ������ �������ϴ�.
	float perturb = ((1.0f - In.vTexUV.y) * distortionScale) + distortionBias;

	float2 noiseCoords = (finalNoise.xy * perturb) + In.vTexUV;

	vector fireColor = g_DiffuseTexture.Sample(ClampSampler, noiseCoords.xy);

	vector alphaColor = g_AlphaTexture.Sample(DefaultSampler, In.vTexUV);

	// ȭ���� ���� ������ �Ҿ����ϰ� �ְ� �� ���� �ؽ�ó ������ �����մϴ�.
	fireColor.a = alphaColor.r;

	return fireColor;
}

//----------------------------------------------------------------------------------------

technique11			DefaultTechnique
{
	//0 : Renderer_NonLight
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0.f);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1 : Renderer_AlphaBlend
	pass AlphaBlend
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHABLEND();
	}

	// 2 : Renderer_AlphaBlend
	pass Fire
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FIRE();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_EFFECT_FIRE();
	}
}
