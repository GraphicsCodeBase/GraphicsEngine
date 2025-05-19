cbuffer ViewConstants : register(b0)
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float4 color : COLOR0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
    float4 color : COLOR0;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    float4 localPosition = float4(input.position, 1.0f);

    // Transform to world space
    float4 worldPosition = mul(projectionMatrix, localPosition);

    // Transform to view (camera) space
    float4 viewPosition = mul(viewMatrix, worldPosition);

    // Transform to clip space (projection)
    float4 clipPosition = mul(modelMatrix, viewPosition);

    output.position = clipPosition;

    // Pass through additional vertex attributes
    output.texCoord = input.texCoord;
    output.color = input.color;

    return output;
}

