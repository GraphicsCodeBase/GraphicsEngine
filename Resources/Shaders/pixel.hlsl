struct PSInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
    float4 color : COLOR0;
};

float4 main(PSInput input) : SV_Target
{
    // Simply return the interpolated color from the vertex shader
    return input.color;
}