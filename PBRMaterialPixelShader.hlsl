TextureCube irradianceMap		: register(t0);
Texture2D albedoMap				: register(t1);
Texture2D normalMap				: register(t2);
Texture2D metallicMap			: register(t3);
Texture2D roughnessMap			: register(t4);
SamplerState basicSampler		: register(s0);

cbuffer externalData	: register(b0)
{
	float ao;

	float3 lightPosition1;
	float3 lightPosition2;
	float3 lightPosition3;
	float3 lightPosition4;
	float3 lightColor;

	float3 cameraPos;
}
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

static const float PI = 3.14159235359;

//Normal Distribution Function = approximates the surface's microfacets that are aligned the halfway vector influenced by the surface roughness.
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return num / denom;
}

//Geometry function = approximates the relativesurface areawhere its micro surface details overshadow each othercausing lights to be occluded
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r*r) / 8.0f; 

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

//Smith's method takes into account both the view direction and light direction
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

//Fresnel Equation = calculates the ratio of light getting reflected over light getting refracted
float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow((1 - cosTheta), 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

void CalculateRadiance(VertexToPixel input, float3 V, float3 N, float3 albedo, float metallic, float roughness, float3 lightPos, float3 lightCol, float3 F0, out float3 radiance)
{
	//Calculate per-light radiance
	float3 L = normalize(lightPos - input.worldPos);
	float3 H = normalize(V + L);

	float distance = length(lightPos - input.worldPos);
	float attenuation = 1.0f / (distance * distance);
	float3 rad = lightColor * attenuation;

	//Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

	//Energy of light that gets reflected
	float3 kS = F;

	//Energy of light that gets refracted
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;

	kD *= 1.0f - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
	float3 specular = numerator / max(denominator, 0.001f);

	//Each light's outgoing reflectance value
	float NdotL = max(dot(N, L), 0.0f);
	radiance = (((kD * albedo / PI) + specular) * rad * NdotL);
}

float4 main(VertexToPixel input) : SV_TARGET
{

	//Sampling all the textures
	//Albedo Map
	float3 albedo = pow(albedoMap.Sample(basicSampler,input.uv).rgb, 2.2f);

	//Normal Map
	input.tangent = normalize(input.tangent);

	float3 normalFromMap = normalMap.Sample(basicSampler, input.uv).xyz * 2 - 1;

	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);
	input.normal = normalize(mul(normalFromMap, TBN));

	float3 normalVec = input.normal;

	//Metallic Map
	float metallic = metallicMap.Sample(basicSampler, input.uv).r;

	//Roughness Map
	float roughness = roughnessMap.Sample(basicSampler, input.uv).r;

	float3 viewDir = normalize(cameraPos - input.worldPos);

	float3 R = reflect(-viewDir, normalVec);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	//Reflectance equation
	float3 L0 = float3(0.0f, 0.0f, 0.0f);
	float3 radiance = float3(0.0f, 0.0f, 0.0f);

	CalculateRadiance(input, viewDir, normalVec, albedo, metallic, roughness, lightPosition1, lightColor, F0, radiance);
	L0 += radiance;
	
	CalculateRadiance(input, viewDir, normalVec, albedo, metallic, roughness, lightPosition2, lightColor, F0, radiance);
	L0 += radiance;

	CalculateRadiance(input, viewDir, normalVec, albedo, metallic, roughness, lightPosition3, lightColor, F0, radiance);
	L0 += radiance;

	CalculateRadiance(input, viewDir, normalVec, albedo, metallic, roughness, lightPosition4, lightColor, F0, radiance);
	L0 += radiance;


	//Direct lighting
	float3 irradiance = irradianceMap.Sample(basicSampler, normalVec).rgb;
	float3 kS = FresnelSchlickRoughness(max(dot(normalVec, viewDir), 0.0f), F0, roughness);
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	float3 diffuse = albedo * irradiance;
	float3 ambient = (kD * diffuse) * ao;

	float3 color = ambient + L0;


	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, float3(1.0f /2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

	return float4(color, 1.0f);
}

