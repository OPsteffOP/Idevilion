cbuffer Data : register(b1)
{
	float CharacterWidth;
	float CharacterHeight;
	float _Reserved2[2];
};

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D<uint> CharacterTexture;

float main(Input input) : SV_TARGET
{
	return CharacterTexture.Load(float3(input.texCoord.x * CharacterWidth, input.texCoord.y * CharacterHeight, 0)) / 255.f;
}