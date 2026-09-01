cbuffer ObjectConstant : register(b0)
{
    float4x4 wvp; //column-major
}

Texture2D diffuseTexture : register(t0);
SamplerState linearSampler : register(s0);

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    // output.position = float4(input.position, 1.0f);
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return diffuseTexture.Sample(linearSampler, input.uv);
}
