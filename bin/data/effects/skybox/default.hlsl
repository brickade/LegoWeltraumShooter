cbuffer MatrixBuffer
{
	matrix InvertProjection;
    matrix InvertViewModel;
};
tbuffer textureBuffer
{
	TextureCube Diffuse;
};

SamplerState TextureSampler;


struct VertexShaderInput
{
  float3 Position : POSITION0;
  float2 UV       : TEXCOORD0;
  float3 Color    : COLOR0;
  float3 Normal   : NORMAL;
  float4 Offset   : MATRIX0;
};

struct VertexShaderOutput
{
  float4 Position : SV_POSITION;
  float3 UV       : TEXCOORD0;
};

struct PixelShaderOutput
{
  float4 colorMap: SV_TARGET0;
};

VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;

  float3x3 IMV =(float3x3)InvertViewModel;
  float4x4 IP = InvertProjection;

  float4 Pos = float4(input.Position,1);

  float3 unprojected = (mul(Pos,IP)).xyz;

  Output.Position = Pos;

  Output.UV = mul(unprojected, IMV);
  
  return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input) : SV_TARGET
{
  PixelShaderOutput output;

  float4 blend = Diffuse.Sample(TextureSampler, input.UV);
  output.colorMap = blend;


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