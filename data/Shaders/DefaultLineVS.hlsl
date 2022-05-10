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

	output.position = float4(input.position, 0, 1);

	return output;
}