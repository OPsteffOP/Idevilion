struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

cbuffer Data : register(b1)
{
	float4 TextColor;
};

Texture2D<float> FontBitmapTexture;
SamplerState FontBitmapTextureSampler;

float4 main(Input input) : SV_TARGET
{
	float value = FontBitmapTexture.Sample(FontBitmapTextureSampler, input.texCoord);
	if(value > 0.5f)
		return TextColor;
	else if(value > 0.47f)
	{
		float smoothValue = smoothstep(0.45f, 0.5f, value);
		return float4(TextColor.rgb, smoothValue * TextColor.a);
	}
	else
		return float4(0, 0, 0, 0);
}