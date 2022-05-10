struct Input
{
	float2 position : POSITION0;
	float2 texCoord : TEXCOORD0;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

cbuffer Data : register(b1)
{
	float SrcTopLeftX;
	float SrcTopLeftY;
	float SrcWidth;
	float SrcHeight;

	float DstPositionX;
	float DstPositionY;
	float DstWidth;
	float DstHeight;
};

Output main(in Input input)
{
	Output output;

	// Position
	float2 position = input.position;
	position += float2(1, 1); // [-1, 1] to [0, 2]
	position /= float2(2, 2); // [0, 2] to [0, 1]

	position *= float2(DstWidth, DstHeight); // scale
	position += float2(DstPositionX, DstPositionY); // translate

	position *= float2(2, 2); // [0, 1] to [0, 2]
	position -= float2(1, 1); // [0, 2] to [-1, 1]

	// Texcoord
	float2 texCoord = input.texCoord;
	texCoord *= float2(SrcWidth, SrcHeight); // scale
	texCoord += float2(SrcTopLeftX, SrcTopLeftY); // translate

	output.position = float4(position, 0, 1);
	output.texCoord = texCoord;

	return output;
}