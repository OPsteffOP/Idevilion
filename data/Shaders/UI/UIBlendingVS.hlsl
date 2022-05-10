struct Input
{
	float2 position : POSITION0;
	float2 texCoord : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Output main(in Input input)
{
	Output output;

	output.position = float4(input.position, 0, 1);
	output.texCoord = input.texCoord;

	return output;
}