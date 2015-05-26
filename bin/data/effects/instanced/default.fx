float4x4 View;
float4x4 Projection;

Texture2D Diffuse;

SamplerState TextureSampler
{
  Filter = MIN_MAG_MIP_Linear;
  AddressU = WRAP;
  AddressV = WRAP;
};
struct VertexShaderInput
{
  float3 Position : POSITION0;
  float2 UV       : TEXCOORD0;
  float3 Color    : COLOR0;
  float3 Normal   : NORMAL0;
  row_major float4x4 Offset   : MATRIX;
};

struct VertexShaderOutput
{
  float4 PositionOut : SV_POSITION;
  float4 Position : POSITION0;
  float2 UV       : TEXCOORD0;
  float3 Color    : COLOR0;
  float3 Normal   : NORMAL0;
};

struct PixelShaderOutput
{
  float4 colorMap: SV_TARGET0;
  float4 normalMap: SV_TARGET1;
};


VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;
  
  float4 pos = float4(input.Position.xyz, 1);

  float4x4 Model = input.Offset;

  float4x4 MVP = mul(mul(Model,View),Projection);

  Output.Position = mul(pos,MVP);
  Output.PositionOut = mul(pos,MVP);

  Output.UV = input.UV;

  Output.Color = input.Color;

  Output.Normal = normalize(mul(input.Normal,(float3x3)Model));
  
  return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input)
{
  PixelShaderOutput output;

  float4 blend = Diffuse.Sample(TextureSampler, input.UV);
  output.colorMap = blend;
  output.normalMap = float4(input.Normal,1);

  return output;
}


technique10 Main
{
  pass p0
  {
    SetVertexShader(CompileShader(vs_4_0, VS_MAIN()));
    SetGeometryShader(NULL);
    SetPixelShader(CompileShader(ps_4_0, PS_MAIN()));
   }
};