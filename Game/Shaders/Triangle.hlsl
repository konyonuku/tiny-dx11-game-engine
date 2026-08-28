cbuffer ObjectConstant : register(b0)
{
    float4x4 world; //column-major
}

struct VSInput
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color    : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    // output.position = float4(input.position, 1.0f);
    output.position = mul(float4(input.position, 1.0f), world);
    output.color = input.color;
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return input.color;
}
