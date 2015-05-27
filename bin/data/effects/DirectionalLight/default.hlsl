cbuffer MatrixBuffer
{
    float3 LightDirection;
    float4 LightColor;
    float2 Resolution;
};
tbuffer textureBuffer
{
    Texture2D NormalMap;
    Texture2D DepthMap;
};


SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


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

    Output.Position = pos;

    Output.UV = input.UV;

    Output.Color = input.Color;

    Output.Normal = input.Normal;

    return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float2 UV = CalcTexCoord(input.Position);
    float4 norm = (NormalMap.Sample(SampleType, UV) *2 )-1;
    if(norm.a < 0.1)
     discard;
    
    float Factor = dot(norm.xyz,-LightDirection);

    float4 lights = float4(LightColor.rgb*Factor, LightColor.a);
    
    output.color = lights;

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