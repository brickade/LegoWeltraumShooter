cbuffer MatrixBuffer
{
	matrix Scale;
	matrix Rotation;
	matrix Translation;
	matrix View;
	matrix Projection;
	float textureID;
	float2 Resolution;
        float3 ambient = float3(0.1,0.1,0.1);
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

  float2 TexCoord = CalcTexCoord(input.Position);

  float4 blend = DiffuseMap.Sample(TextureSampler, TexCoord);
  float4 norm = (NormalMap.Sample(TextureSampler, TexCoord)*2)-1;
  float4 depth = DepthMap.Sample(TextureSampler, TexCoord);
  float4 light = LightMap.Sample(TextureSampler, TexCoord);
  float4 ssao = SSAOMap.Sample(TextureSampler, TexCoord);

  blend = float4((blend.xyz*(light.xyz + ambient))*ssao, blend.a);
  if(blend.a < 0.1)
	discard;


  if(textureID == 0.0)
    output.color = blend;
  else if(textureID == 1.0)
    output.color = norm;
  else if(textureID == 2.0)
    output.color = float4(1-depth.r,1-depth.r,1-depth.r,1);
  else if(textureID == 3.0)
    output.color = float4(light.xyz+ambient,light.a);
  else
    output.color = float4(ssao.r,ssao.g,ssao.b,1);
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