cbuffer MatrixBuffer
{
    matrix Scale;
    matrix Rotation;
    matrix Translation;
    matrix View;
    matrix Projection;
    float textureID;
    float2 Resolution;
    float3 ambient = float3(0.1, 0.1, 0.1);
    float BlurSize = 4;
};
tbuffer textureBuffer
{
    Texture2D DiffuseMap;
    Texture2D NormalMap;
    Texture2D DepthMap;
    Texture2D LightMap;
    Texture2D SSAOMap;
};

SamplerState TextureSampler;


struct VertexShaderInput
{
    float3 Position : POSITION0;
    float2 UV       : TEXCOORD0;
    float3 Color    : COLOR0;
    float3 Normal   : NORMAL;
};

struct VertexShaderOutput
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD0;
    float3 Color    : COLOR0;
    float3 Normal   : NORMAL;
};

struct PixelShaderOutput
{
    float4 color: SV_TARGET0;
};

float2 CalcTexCoord(float4 Position)
{
    return Position.xy / Resolution;
}

VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
    VertexShaderOutput Output = (VertexShaderOutput)0;

    float4 pos = float4(input.Position.xyz, 1);


        float4x4 Model = mul(mul(Scale, Rotation), Translation);
        float4x4 MVP = mul(mul(Model, View), Projection);

        Output.Position = mul(pos, MVP);

    Output.UV = input.UV;

    Output.Color = input.Color;

    Output.Normal = input.Normal;

    return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float2 TexCoord = CalcTexCoord(input.Position);

        float4 blend = DiffuseMap.Sample(TextureSampler, TexCoord);
        float4 norm = (NormalMap.Sample(TextureSampler, TexCoord) * 2) - 1;
        float4 depth = DepthMap.Sample(TextureSampler, TexCoord);
        float4 light = LightMap.Sample(TextureSampler, TexCoord);

        float ssao = 0.0;
    float2 texelSize = 1.0 / Resolution;
        float2 hlim = float2(float(-BlurSize) * 0.5 + 0.5, float(-BlurSize) * 0.5 + 0.5);
        for (int y = 0; y < BlurSize; ++y)
        {
            for (int x = 0; x < BlurSize; ++x)
            {
                float2 offset = (hlim + float2(float(x), float(y))) * texelSize;
                    ssao += SSAOMap.Sample(TextureSampler,TexCoord + offset).r;
            }
        }
    ssao = ssao / float(BlurSize * BlurSize);

    float3 color = (blend.xyz * (ambient + light.xyz) + light.a) * ssao;
    blend = float4(color, blend.a);
    if (blend.a < 0.1)
        discard;


    if (textureID == 0.0)
        output.color = blend;
    else if (textureID == 1.0)
        output.color = norm;
    else if (textureID == 2.0)
        output.color = float4(1 - depth.r, 1 - depth.r, 1 - depth.r, 1);
    else if (textureID == 3.0)
        output.color = float4(light.xyz + ambient, 1);
    else
        output.color = float4(ssao,ssao,ssao, 1);
    return output;
}


technique10 Main
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
    }
};