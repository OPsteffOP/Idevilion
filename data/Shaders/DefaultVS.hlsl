cbuffer CameraData
{
	float CameraWidth;
	float CameraHeight;
	float CameraScale;
	float CameraPositionX;
	float CameraPositionY;
	float _Reserved[3];
};

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

cbuffer Data
{
	float DestPositionX;
	float DestPositionY;
	float DestWidth;
	float DestHeight;

	float SrcPositionX;
	float SrcPositionY;
	float SrcWidth;
	float SrcHeight;
};

Output main(in Input input)
{
	Output output;

	// Position
	float2 position = input.position;
	position += float2(1, 1); // [-1, 1] to [0, 2]
	position /= float2(2, 2); // [0, 2] to [0, 1]

	position *= float2(DestWidth, DestHeight); // scale
	position += float2(DestPositionX, DestPositionY); // translate
	position -= float2(CameraPositionX, CameraPositionY);
	position /= float2(CameraWidth * CameraScale, CameraHeight * CameraScale);

	position *= float2(2, 2); // [0, 1] to [0, 2]
	position -= float2(1, 1); // [0, 2] to [-1, 1]

	// Texcoord
	float2 texCoord = input.texCoord;
	texCoord *= float2(SrcWidth, SrcHeight); // scale
	texCoord += float2(SrcPositionX, SrcPositionY); // translate

	output.position = float4(position, 0, 1);
	output.texCoord = texCoord;

	return output;
}