cbuffer MatrixBuffer
{
	matrix Scale;
	matrix Rotation;
	matrix Translation;
	matrix View;
	matrix Projection;
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
  float4 PositionOut : SV_POSITION;
  float4 Position : POSITION0;
  float3 UV       : TEXCOORD0;
  float3 Color    : COLOR0;
  float3 Normal   : NORMAL;
};

struct PixelShaderOutput
{
  float4 colorMap: SV_TARGET0;
};

VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;
  
  float4 pos = float4(input.Position.xyz,1);

  float4x4 Model = mul(mul(Scale,Rotation),Translation);

  float4x4 MVP = mul(mul(Model,View),Projection);

  Output.Position = mul(pos,MVP).xyww;
  Output.PositionOut = mul(pos,MVP);

  Output.UV = input.Position;

  Output.Color = input.Color;

  Output.Normal = normalize(mul(input.Normal,(float3x3)Model));
  
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