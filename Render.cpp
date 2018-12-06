#include "Render.h"



Render::Render()
{
}


Render::~Render()
{
}


void Render::RenderPBR(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, SimpleVertexShader * vertexShader, SimplePixelShader * pixelShader, GameEntity * entity, Camera * camera, float m, float r, ID3D11DeviceContext* context)
{
	vertexShader->SetMatrix4x4("world", entity->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetFloat3("albedo", XMFLOAT3(0.85f, 0.74f, 0.60f));
	pixelShader->SetFloat("metallic", m);
	pixelShader->SetFloat("roughness", r);
	pixelShader->SetFloat("a0", 1.0f);

	pixelShader->SetFloat3("lightPosition1", XMFLOAT3(10.0f, 10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition2", XMFLOAT3(10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition3", XMFLOAT3(-10.0f, 10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition4", XMFLOAT3(-10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightColor", XMFLOAT3(300.0f, 300.0f, 300.0f));

	pixelShader->SetFloat3("cameraPos", camera->GetPosition());

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	vertexBuffer = entity->GetMesh()->GetVertexBuffer();
	indexBuffer = entity->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(entity->GetMesh()->GetIndexCount(), 0, 0);
}

void Render::RenderRBRMaterial(ID3D11Buffer *& vertexBuffer, ID3D11Buffer *& indexBuffer, SimpleVertexShader *& vertexShader, SimplePixelShader *& pixelShader, GameEntity *& entity, Camera *& camera, float & m, float & r, ID3D11DeviceContext *& context, ID3D11SamplerState *& sampler, ID3D11ShaderResourceView *& skyIrradianceMap)
{
	vertexShader->SetMatrix4x4("world", entity->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("irradianceMap", skyIrradianceMap);
	pixelShader->SetShaderResourceView("albedoMap", entity->GetMaterial()->GetAlbedoMapSRV());
	pixelShader->SetShaderResourceView("normalMap", entity->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetShaderResourceView("metallicMap", entity->GetMaterial()->GetMetallicMapSRV());
	pixelShader->SetShaderResourceView("roughnessMap", entity ->GetMaterial()->GetRoughnessMapSRV());
	pixelShader->SetSamplerState("basicSampler", sampler);

	pixelShader->SetFloat("a0", 1.0f);

	pixelShader->SetFloat3("lightPosition1", XMFLOAT3(10.0f, 10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition2", XMFLOAT3(10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition3", XMFLOAT3(-10.0f, 10.0f, -10.0f));
	pixelShader->SetFloat3("lightPosition4", XMFLOAT3(-10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat3("lightColor", XMFLOAT3(300.0f, 300.0f, 300.0f));

	pixelShader->SetFloat3("cameraPos", camera->GetPosition());

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();


	vertexBuffer = entity->GetMesh()->GetVertexBuffer();
	indexBuffer = entity->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(entity->GetMesh()->GetIndexCount(), 0, 0);
}

void Render::RenderSkyBox(ID3D11Buffer *& vertexBuffer, ID3D11Buffer *& indexBuffer, SimpleVertexShader *& vertexShader, SimplePixelShader *& pixelShader, Mesh *& skyMesh, Camera *& camera, ID3D11DeviceContext *& context, ID3D11ShaderResourceView *& skySRV, ID3D11RasterizerState *& skyRasterizerState, ID3D11DepthStencilState *& skyDepthState)
{
	context->HSSetShader(0, 0, 0);
	context->DSSetShader(0, 0, 0);

	vertexBuffer = skyMesh->GetVertexBuffer();
	indexBuffer = skyMesh->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();


	pixelShader->SetShaderResourceView("Sky", skySRV);
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();


	context->RSSetState(skyRasterizerState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(skyMesh->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}
