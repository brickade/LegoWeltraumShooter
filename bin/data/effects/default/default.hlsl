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
	Texture2D Diffuse;
};

SamplerState TextureSampler;


struct VertexShaderInput
{
  float3 Position : POSITION0;
  float2 UV       : TEXCOORD0;
  float3 Color    : COLOR0;
  float3 Normal   : NORMAL;
  row_major matrix Offset   : MATRIX;
  float4 IColor    : COLOR1;
};

struct VertexShaderOutput
{
  float4 PositionOut : SV_POSITION;
  float4 Position : POSITION0;
  float2 UV       : TEXCOORD0;
  float4 Color    : COLOR0;
  float3 Normal   : NORMAL;
};

struct PixelShaderOutput
{
  float4 colorMap: SV_TARGET0;
  float4 normalMap: SV_TARGET1;
  float4 positionMap: SV_TARGET2;
};

VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;
  
  float4 pos = float4(input.Position.xyz,1);

  float4x4 Model = mul(mul(Scale,Rotation),Translation);

  float4x4 MVP = mul(mul(Model,View),Projection);

  Output.Position = mul(pos,MVP);
  Output.PositionOut = mul(pos,MVP);

  Output.UV = input.UV;

  Output.Color = float4(input.Color,1);


  Output.Normal = mul(input.Normal,(float3x3)Model);
  
  return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input) : SV_TARGET
{
  PixelShaderOutput output;

  float4 blend = float4(input.Color);
  blend *= dot(input.Normal,-float3(0,0,1));
  blend.a = 1;
  output.colorMap = blend;
  output.normalMap = (float4(input.Normal,1) +1)/2;
  output.positionMap = float4(input.Position.xyz,1);


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