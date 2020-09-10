cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

PixelInput main( VertexInput input )
{
	PixelInput output;
	input.position.w = 1.0f;
	
	// Model space
	float4 position = mul(input.position, world);

	// World space
	position = mul(position, view);

	// View Space
	position = mul(position, projection);

	// Clip Space

	output.position = position;

	output.colour = input.colour;

	output.normal = mul(input.normal, (float3x3)world);

	output.uv = input.uv;

	return output;
}