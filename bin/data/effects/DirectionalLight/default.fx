matrix InvertViewProjection;
float3 LightDirection;
float4 LightColor;
float2 Resolution;
float3 CameraPosition;
float specularIntensity = 1.0f;
float specularPower = 200;

Texture2D NormalMap;
Texture2D DepthMap;

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

float4 CalcPosition(float Depth,float2 UV)
{
        float4 worldpos;
  	worldpos.x = UV.x * 2.0f -1.0f;
  	worldpos.y = (1.0f-UV.y) * 2.0f -1.0f;
        worldpos.z = Depth;
        worldpos.w = 1.0;
  	worldpos = mul(worldpos,InvertViewProjection);
  	worldpos /= worldpos.w;
	return worldpos;
} 

float CalcSpecular(float3 lightVector,float4 pos,float4 norm)
{ 
	float3 reflectionfloattor = normalize(reflect(-lightVector,norm.xyz));
	float3 directionToCamera = normalize(CameraPosition - pos.xyz);
	float cosAngle = max(0.0,dot(directionToCamera,reflectionfloattor));
 	return specularIntensity * pow(cosAngle,specularPower);
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
    float4 norm = (NormalMap.Sample(TextureSampler, UV) *2 )-1;
    if(norm.a < 0.1)
     discard;
    float4 depth = DepthMap.Sample(TextureSampler,UV);
    float4 pos = CalcPosition(depth.r,UV);

    float3 lightVector = -normalize(LightDirection);
    float Factor = max(0,dot(norm.xyz,lightVector));
    float3 diffuseLight = Factor*LightColor.rgb;
    output.color = float4(diffuseLight,1);
  return output;
}

RasterizerState Culling
{
  CullMode = 1;
};

technique10 Main
{
  pass p0
  {
    SetRasterizerState(Culling);
    SetVertexShader(CompileShader(vs_4_0, VS_MAIN()));
    SetGeometryShader(NULL);
    SetPixelShader(CompileShader(ps_4_0, PS_MAIN()));
   }
};