cbuffer MatrixBuffer
{
	matrix Scale;
	matrix Rotation;
	matrix Translation;
	matrix View;
	matrix Projection;
	float textureID;
    float3 ambient;
};
tbuffer textureBuffer
{
	Texture2D DiffuseMap;
	Texture2D NormalMap;
	Texture2D PositionMap;
	Texture2D DepthMap;
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

VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;
  
  float4 pos = float4(input.Position.xyz, 1);


  float4x4 Model = mul(mul(Scale,Rotation),Translation);
  float4x4 MVP = mul(mul(Model,View),Projection);

  Output.Position = mul(pos,MVP);

  Output.UV = input.UV;

  Output.Color = input.Color;

  Output.Normal = input.Normal;
  
  return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input)
{
  PixelShaderOutput output;

  float4 blend = DiffuseMap.Sample(TextureSampler, input.UV);
  float4 norm = NormalMap.Sample(TextureSampler, input.UV);
  float4 pos = PositionMap.Sample(TextureSampler, input.UV);
  float4 depth = DepthMap.Sample(TextureSampler, input.UV);

  float3 lightDir = float3(1,0,-1);
  lightDir = normalize(lightDir);
  float intensity = dot(norm.xyz,lightDir);
  blend.r *= intensity;
  blend.g *= intensity;
  blend.b *= intensity;
  blend.rgb += ambient;

  if(textureID == 0.0)
    output.color = blend;
  else if(textureID == 1.0)
    output.color = norm;
  else if(textureID == 2.0)
    output.color = float4(depth.r,depth.r,depth.r,1);
  else
    output.color = pos;

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