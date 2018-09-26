static const float PI = 3.14159265359;

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

//Normal Distribution Function = approximates the surface's microfacets that are aligned the halfway vector influenced by the surface roughness.
float NDFGGXTR(float3 normal, float3 halfway, float alpha)
{
	float alpha2 = alpha * alpha;
	float NdotH = max(dot(normal, halfway), 0.0f);
	float denominator = PI * (pow(pow(NdotH, 2) * (alpha2 - 1) + 1, 2);

	float numerator = alpha2;
	return numerator / denominator;
}

//Geometry function = approximates the relativesurface areawhere its micro surface details overshadow each othercausing lights to be occluded
float GSchlickGGX(float NdotV, float k)
{
	float numerator = NdotV;
	float denominator = NdotV * (1.0 - k) + k;

	return numerator / denominator;
}

//Smith's method takes into account both the view direction and light direction
float GSmith(float3 normal, float3 view, float3 lightDir, float k)
{
	float NdotV = max(dot(normal, view), 0.0f);
	float NdotL = max(dot(normal, lighDir), 0.0f);
	float ggx1 = GSchlickGGX(NdotV, k);
	float ggx2 = GSchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

//Fresnel Equation = calculates the ratio of light getting reflected over light getting refracted
float3 FSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow((1 - cosTheta), 5.0);
}
float4 main() : SV_TARGET
{
	//Reflectance Equation
	//1) Calculate radiance
	//2) Calculate cosTheta
	//3) Calculate Cook- Tolerance BRDF



	//1) Calculate Radiance (Li)
	////1)Light Direction
	


	//2) calculate cosTheta 
	//Dot Product of Light Dir and surface normal


	//3) Calculate Cook - Tolerance BRDF (fr)
	// fr = kd * flambart + ks * fcook_lambart
	
	
	
	// kd = ratio of light that gets refracted


	//flambart = c/PI
	//c = surface color
	//divide by PI to normalize


	// ks = ratio of light that gets refracted



	//fcook_tolerance = DFG/4*(normal*Wo)*(surface normal * Wi)
	//wo = view direction
	//wi = light direction
	//D = Distribution Function
	//F = Fresnel Equation
	//G = Geometry Function	




	//D == approximates the relative surface area of microfacets to halfway vector(h)
	//Trowbridge-Reitz GGX
	//h = halfway vector
	//n = surface normal
	//alpha = surface roughness





	//G == approximates the relativesurface areawhere its micro surface details overshadow each othercausing lights to be occluded
	//Commbination of GGX and Schlick-GGX
	//k = based on direct or IBL
	//n = surface normal
	//v = view direction
	//Smith = uses Schlick-GGX for both view and light direction





	//F = describes the ratio of light getting reflected over light getting refracted
	//FSchlick to approximate the fresnel (for dielectrics/non-metals)
	//Fo = base reflectivity (indices of refraction/IOR)





	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}