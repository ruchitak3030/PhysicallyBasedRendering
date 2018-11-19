#pragma once
#include "SimpleShader.h"

class Material
{
public:
	//Material(ID3D11ShaderResourceView* albedoMap, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* metallicMap, ID3D11ShaderResourceView* roughnessMap, ID3D11SamplerState* sampler);
	Material(ID3D11ShaderResourceView* albedoMap, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* metallicMap, ID3D11ShaderResourceView* roughnessMap, ID3D11SamplerState* sampler);
	~Material();

	ID3D11ShaderResourceView* GetAlbedoMapSRV();
	ID3D11ShaderResourceView* GetNormalMapSRV();
	ID3D11ShaderResourceView* GetMetallicMapSRV();
	ID3D11ShaderResourceView* GetRoughnessMapSRV();
	ID3D11SamplerState* GetSamplerState();

private:
	ID3D11ShaderResourceView* albedoSRV;
	ID3D11ShaderResourceView* normalSRV;
	ID3D11ShaderResourceView* metallicSRV;
	ID3D11ShaderResourceView* roughnessSRV;
	ID3D11SamplerState* m_sampler;
};

