cbuffer ObjectConstant : register(b0)
{
    //column-major
    float4x4 wvp; 
    float4x4 world;
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
    output.normal = mul(input.normal, (float3x3)world);
    output.uv = input.uv;
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    float3 normal = normalize(input.normal);

    float3 toLight = normalize(float3(-1.0f, +1.0f, -1.0f));
    float directBrightness = saturate(dot(normal, toLight));
    float minBrightness = 0.1f;
    float finalBrightness = saturate(directBrightness + minBrightness);

    float4 textureColor = diffuseTexture.Sample(linearSampler, input.uv);

    return float4(textureColor.rgb * finalBrightness, textureColor.a);
    
}
