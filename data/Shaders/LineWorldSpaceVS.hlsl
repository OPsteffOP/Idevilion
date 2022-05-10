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
};

struct Output
{
	float4 position : SV_POSITION;
};

Output main(in Input input)
{
	Output output;

	float2 position = input.position;
	position -= float2(CameraPositionX, CameraPositionY);
	position /= float2(CameraWidth * CameraScale, CameraHeight * CameraScale);

	position *= float2(2, 2); // [0, 1] to [0, 2]
	position -= float2(1, 1); // [0, 2] to [-1, 1]

	output.position = float4(position, 0, 1);

	return output;
}