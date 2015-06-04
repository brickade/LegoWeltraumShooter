#define KERNEL_SIZE 16

matrix InvertViewProjection;
matrix CamView;
float2 NoiseScale;
float2 Resolution;
float g_sample_rad = 0.1;
float g_intensity = 2.0;
float g_scale = 0.1;
float g_bias = 0.0;


Texture2D NormalMap;
Texture2D DepthMap;
Texture2D NoiseTexture;


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
};

struct PixelShaderOutput
{
  float4 color: SV_TARGET0;
};

float2 CalcTexCoord(float4 Position)
{
    return Position.xy / Resolution;
}

float3 getPosition(float2 uv)
{	
        float4 worldpos;

        float depth = DepthMap.Sample(TextureSampler, uv).r;

  	    worldpos.x = uv.x * 2.0f -1.0f;

  	    worldpos.y = (1.0f-uv.y) * 2.0f -1.0f;

        worldpos.z = depth;

        worldpos.w = 1.0f;

        worldpos = mul(worldpos, InvertViewProjection);

  	    worldpos /= worldpos.w;
 
        return worldpos.rgb;
}

float3 getNormal(float2 uv)
{
	return normalize(NormalMap.Sample(TextureSampler, uv).xyz * 2.0f - 1.0f);
}

float2 getRandom(float2 uv)
{
	return normalize(NoiseTexture.Sample(TextureSampler, Resolution*uv/NoiseScale).xy * 2.0f - 1.0f);
}

float doAmbientOcclusion(in float2 tcoord,in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = getPosition(tcoord + uv) - p;
    float3 v = normalize(diff);
    float d = length(diff)*g_scale;

    return max(0.0, dot(cnorm, v) - g_bias)*(1.0 / (1.0 + d))*g_intensity;
}



VertexShaderOutput VS_MAIN(VertexShaderInput input)
{
  VertexShaderOutput Output = (VertexShaderOutput)0;
  
  float4 pos = float4(input.Position.xyz, 1);

  Output.Position = pos;
  
  return Output;
}

PixelShaderOutput PS_MAIN(VertexShaderOutput input)
{
  PixelShaderOutput output;

	float4 Kernels[KERNEL_SIZE] =
	{
		float4(0.355512, 	-0.709318, 	-0.102371,	0.0 ),
		float4(0.534186, 	0.71511, 	-0.115167,	0.0 ),
		float4(-0.87866, 	0.157139, 	-0.115167,	0.0 ),
		float4(0.140679, 	-0.475516, 	-0.0639818,	0.0 ),
		float4(-0.0796121, 	0.158842, 	-0.677075,	0.0 ),
		float4(-0.0759516, 	-0.101676, 	-0.483625,	0.0 ),
		float4(0.12493, 	-0.0223423,	-0.483625,	0.0 ),
		float4(-0.0720074, 	0.243395, 	-0.967251,	0.0 ),
		float4(-0.207641, 	0.414286, 	0.187755,	0.0 ),
		float4(-0.277332, 	-0.371262, 	0.187755,	0.0 ),
		float4(0.63864, 	-0.114214, 	0.262857,	0.0 ),
		float4(-0.184051, 	0.622119, 	0.262857,	0.0 ),
		float4(0.110007, 	-0.219486, 	0.435574,	0.0 ),
		float4(0.235085, 	0.314707, 	0.696918,	0.0 ),
		float4(-0.290012, 	0.0518654, 	0.522688,	0.0 ),
		float4(0.0975089, 	-0.329594, 	0.609803,	0.0 )

	};

  float2 uv = CalcTexCoord(input.Position); //same as UV Coordinate from Vertex
  float3 p = getPosition(uv);
  float3 n = getNormal(uv);
  float2 rand = getRandom(uv);

  float4 viewpos = mul(float4(p,1),CamView);
  float ao = 0.0f;
  float rad = g_sample_rad/viewpos.z;

  for (int j = 0; j < KERNEL_SIZE; ++j)
  {
    float2 coord = reflect(Kernels[j].xy,rand)*rad;
    ao += doAmbientOcclusion(uv,coord, p, n);
  }
  ao /= KERNEL_SIZE;

  ao = 1-(ao);
  output.color = float4(ao,ao,ao,1);

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