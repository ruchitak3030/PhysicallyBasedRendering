#include "Material.h"




Material::Material(ID3D11ShaderResourceView * albedoMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * metallicMap, ID3D11ShaderResourceView * roughnessMap, ID3D11SamplerState * sampler)
{
	albedoSRV = albedoMap;
	normalSRV = normalMap;
	metallicSRV = metallicMap;
	roughnessSRV = roughnessMap;
	m_sampler = sampler;
}

Material::~Material()
{
}


ID3D11ShaderResourceView * Material::GetAlbedoMapSRV()
{
	return albedoSRV;
}

ID3D11ShaderResourceView * Material::GetNormalMapSRV()
{
	return normalSRV;
}

ID3D11ShaderResourceView * Material::GetMetallicMapSRV()
{
	return metallicSRV;
}

ID3D11ShaderResourceView * Material::GetRoughnessMapSRV()
{
	return roughnessSRV;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return m_sampler;
}
