struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D ColorTexture;
SamplerState ColorTextureSampler;

float4 main(Input input) : SV_TARGET
{
	return ColorTexture.Sample(ColorTextureSampler, input.texCoord);
}