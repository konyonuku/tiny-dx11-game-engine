cbuffer ObjectConstant : register(b0)
{
    //column-major
    float4x4 wvp; 
    float4x4 world;
};
cbuffer LightConstant : register(b1)
{
    float3 lightDirection;
    float  lightIntensity;
    float3 lightColor;
    float  ambientIntensity;
    float3 cameraPosition;
    float  padding;
};
cbuffer MaterialConstant : register(b2)
{
    float3 diffuseColor;
    float  specularPower;
    float3 specularColor;
    float  padding2;
};

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
    float3 worldPosition : TEXCOORD1;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    float4 localPosition = float4(input.position, 1.0f);
    output.worldPosition = mul(localPosition, world).xyz;
    output.position = mul(localPosition, wvp);
    output.normal = mul(input.normal, (float3x3)world);
    output.uv = input.uv;

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    float3 normal = normalize(input.normal);
    float3 toLight = normalize(-lightDirection);

    float diffuseFactor = saturate(dot(normal, toLight));
    float4 textureColor = diffuseTexture.Sample(linearSampler, input.uv);

    float3 toCamera = normalize(cameraPosition - input.worldPosition);
    float3 reflection = reflect(-toLight, normal);

    float specularFactor = 0.0f;
    if(diffuseFactor > 0.0f) specularFactor = pow(saturate(dot(toCamera, reflection)), specularPower);
     

    float3 diffuse = textureColor.rgb * diffuseColor * lightColor * diffuseFactor * lightIntensity;
    float3 ambient = textureColor.rgb * diffuseColor * ambientIntensity;
    float3 specular = specularColor * lightColor * specularFactor * lightIntensity;

    float3 finalColor = diffuse + ambient + specular;

    return float4(finalColor, textureColor.a);
}
