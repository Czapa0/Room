cbuffer cbWorld : register(b0) //Vertex Shader constant buffer slot 0
{
	matrix worldMatrix;
};

cbuffer cbView : register(b1) //Vertex Shader constant buffer slot 1
{
	matrix viewMatrix;
};

cbuffer cbProj : register(b2) //Vertex Shader constant buffer slot 2
{
	matrix projMatrix;
};

cbuffer cbTextureTransform : register(b3)
{
	matrix texMatrix;
};

struct VSInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex: TEXCOORD0;
};

PSInput main(VSInput i)
{
	PSInput o = (PSInput)0;
	o.pos = float4(i.pos, 1.0f);

	// TODO : 0.07 Calculate texture coordinates by multiplying local position by texture matrix
	o.tex = mul(texMatrix, o.pos).xy;

	o.pos = mul(worldMatrix, o.pos);
	o.pos = mul(viewMatrix, o.pos);
	o.pos = mul(projMatrix, o.pos);

	return o;
}