Texture2D colorMap1 : register(t0);
Texture2D colorMap2 : register(t1);
SamplerState colorSampler : register(s0);

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex1: TEXCOORD0;
	float2 tex2: TEXCOORD1;
};

float4 main(PSInput i) : SV_TARGET
{
	// TODO : 1.02 Sample both textures using their respective texture coordinates
	float4 colTex1 = colorMap1.Sample(colorSampler, i.tex1);
	float4 colTex2 = colorMap2.Sample(colorSampler, i.tex2);

	// TODO : 1.03 For now return only the second color
	//return colTex2;

	// TODO : 1.09 Change the shader to so that the two color are alpha-blended based on alpha channel of the second one
	float4 col = lerp(colTex1, colTex2, colTex2.a);
	return col;
}