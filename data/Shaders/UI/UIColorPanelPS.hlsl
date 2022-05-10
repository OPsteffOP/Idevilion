cbuffer ColorData : register(b1)
{
	float4 Color;
};

float4 main() : SV_TARGET
{
	return Color;
}