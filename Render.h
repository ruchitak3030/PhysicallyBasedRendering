#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include "Vertex.h"
#include "GameEntity.h"
#include "Mesh.h"
#include "Camera.h"
#include <DirectXMath.h>

using namespace DirectX;
class Render
{
public:
	Render();
	~Render();

	void RenderPBR(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, GameEntity* entity, Camera* camera, float m, float r, ID3D11DeviceContext* context);
	void RenderRBRMaterial(ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer, SimpleVertexShader* &vertexShader, SimplePixelShader* &pixelShader, GameEntity* &entity, Camera* &camera, float &m, float &r, ID3D11DeviceContext* &context, ID3D11SamplerState* &sampler, ID3D11ShaderResourceView* &skyIrradianceMap);
	void RenderSkyBox(ID3D11Buffer* &vertexBuffer, ID3D11Buffer* &indexBuffer, SimpleVertexShader* &vertexShader, SimplePixelShader* &pixelShader, Mesh* &skyMesh, Camera* &camera, ID3D11DeviceContext* &context, ID3D11ShaderResourceView* &skySRV, ID3D11RasterizerState* &skyRasterizerState, ID3D11DepthStencilState* &skyDepthState);

private:
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
};

