#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;


Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{


	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;
	skyBoxVertexShader = 0;
	skyBoxPixelShader = 0;
	pbrVertexShader = 0;
	pbrPixelShader = 0;
	convolutionPixelShader = 0;
	pbrMaterialVertexShader = 0;
	pbrMaterialPixelShader = 0;

	

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}


Game::~Game()
{

	if (vertexBuffer) 
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();

	delete camera;


	delete vertexShader;
	delete pixelShader;
	delete skyBoxVertexShader;
	delete skyBoxPixelShader;
	delete pbrVertexShader;
	delete pbrPixelShader;
	delete convolutionPixelShader;
	delete pbrMaterialVertexShader;
	delete pbrMaterialPixelShader;

	delete sphereMesh;
	delete skyMesh;

	delete ironrustMat;
	
	delete sky;

	for (size_t i = 0; i < numrows; i++)
		for (size_t j = 0; j < numcolumns; j++)
		{
			delete spheres[i][j];
		}
	

	sampler->Release();
	ironrustAlbedoMapSRV->Release();
	ironrustNormalMapSRV->Release();
	ironrustMetallicMapSRV->Release();
	ironrustRoughnessMapSRV->Release();
	
	skyTextureSRV->Release();
	skyIrradianceMapSRV->Release();
	skyRasterizerState->Release();
	skyDepthState->Release();


	skyIBLTexture->Release();
	for (int i = 0; i < 6; i++)
	{
		skyIBLRTV[i]->Release();
	}
	skyIBLSRV->Release();


}

void Game::Init()
{

	LoadShaders();
	CreateMatrices();
	LoadSkyBox();
	LoadMesh();
	LoadTextures();
	CreateMaterials();	
	CreateBasicGeometry();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SetUpImageBasedLighting();
}


void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	skyBoxVertexShader = new SimpleVertexShader(device, context);
	if (!skyBoxVertexShader->LoadShaderFile(L"Debug/SkyBoxVertexShader.cso"))
		skyBoxVertexShader->LoadShaderFile(L"SkyBoxVertexShader.cso");

	skyBoxPixelShader = new SimplePixelShader(device, context);
	if (!skyBoxPixelShader->LoadShaderFile(L"Debug/SkyBoxPixelShader.cso"))
		skyBoxPixelShader->LoadShaderFile(L"SkyBoxPixelShader.cso");

	pbrVertexShader = new SimpleVertexShader(device, context);
	if (!pbrVertexShader->LoadShaderFile(L"Debug/PBRVertexShader.cso"))
		pbrVertexShader->LoadShaderFile(L"PBRVertexShader.cso");

	pbrPixelShader = new SimplePixelShader(device, context);
	if (!pbrPixelShader->LoadShaderFile(L"Debug/PBRPixelShader.cso"))
		pbrPixelShader->LoadShaderFile(L"PBRPixelShader.cso");

	convolutionPixelShader = new SimplePixelShader(device, context);
	if (!convolutionPixelShader->LoadShaderFile(L"Debug/ConvolutionPixelShader.cso"))
		convolutionPixelShader->LoadShaderFile(L"ConvolutionPixelShader.cso");

	pbrMaterialVertexShader = new SimpleVertexShader(device, context);
	if (!pbrMaterialVertexShader->LoadShaderFile(L"Debug/PBRMaterialVertexShader.cso"))
		pbrMaterialVertexShader->LoadShaderFile(L"PBRMaterialVertexShader.cso");

	pbrMaterialPixelShader = new SimplePixelShader(device, context);
	if (!pbrMaterialPixelShader->LoadShaderFile(L"Debug/PBRMaterialPixelShader.cso"))
		pbrMaterialPixelShader->LoadShaderFile(L"PBRMaterialPixelShader.cso");




	
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{

	camera = new Camera(0, 0, -14.0f);
	camera->UpdateProjectionMatrix((float)width / height);



}
void Game::LoadSkyBox()
{
	CreateDDSTextureFromFile(device, L"Debug/Textures/SunnyCubeMap.dds", 0, &skyTextureSRV);
	CreateDDSTextureFromFile(device, L"Debug/Textures/skybox1IR.dds", 0, &skyIrradianceMapSRV);

	//Sky Box Setup
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rasterizerDesc, &skyRasterizerState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDesc, &skyDepthState);
}

void Game::LoadMesh()
{
	sphereMesh = new Mesh("Debug/Models/sphere.obj", device);
	skyMesh = new Mesh("Debug/Models/cube.obj", device);
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(device, context, L"Debug/Textures/golden_albedo.tif", 0, &ironrustAlbedoMapSRV);
	CreateWICTextureFromFile(device, context, L"Debug/Textures/golden_normal.tif", 0, &ironrustNormalMapSRV);
	CreateWICTextureFromFile(device, context, L"Debug/Textures/golden_metallic.tif", 0, &ironrustMetallicMapSRV);
	CreateWICTextureFromFile(device, context, L"Debug/Textures/golden_roughness.tif", 0, &ironrustRoughnessMapSRV);
}

void Game::CreateMaterials()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);


	ironrustMat = new Material(ironrustAlbedoMapSRV, ironrustNormalMapSRV, ironrustMetallicMapSRV, ironrustRoughnessMapSRV, sampler);

}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	sky = new GameEntity(skyMesh);
	sky->SetScale(1.0f, 1.0f, 1.0f);
	sky->SetPosition(0.0f, 0.0f, 0.0f);

	for (size_t row = 0; row < numrows; ++row)
		for (size_t col = 0; col < numcolumns; ++col)
		{
			spheres[row][col] = new GameEntity(sphereMesh, ironrustMat);			
		}

	float x = -4.0f;
	float y = 4.0f;
	for (size_t row = 0; row < numrows; row++)
	{
		if (y == -4.0f)
			y = 4.0f;
		for (size_t col = 0; col < numcolumns; col++)
		{
			if (x == 4.0f)
				x = -4.0f;

			spheres[row][col]->SetPosition(x, y, 0.0f);
			spheres[row][col]->SetScale(1.0f, 1.0f, 1.0f);
			x += spacing;
		}
		y -= spacing;
	}


	
}

void Game::SetUpImageBasedLighting()
{
	XMFLOAT3 position = XMFLOAT3(0, 0, 0);
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;

	XMVECTOR tar[] = { XMVectorSet(1,0,0,0), XMVectorSet(-1,0,0,0), XMVectorSet(0,1,0,0), XMVectorSet(0,-1,0,0), XMVectorSet(0,1,0,0), XMVectorSet(0,-1,0,0) };
	XMVECTOR up[] = { XMVectorSet(0,1,0,0), XMVectorSet(0,1,0,0), XMVectorSet(0,0,-1,0), XMVectorSet(0,0,1,0), XMVectorSet(0,1,0,0), XMVectorSet(0,1,0,0) };

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.0f };


	//Texture
	D3D11_TEXTURE2D_DESC skyIBLDesc = {};
	skyIBLDesc.Width = 64;
	skyIBLDesc.Height = 64;
	skyIBLDesc.MipLevels = 1;
	skyIBLDesc.ArraySize = 6;
	skyIBLDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	skyIBLDesc.Usage = D3D11_USAGE_DEFAULT;
	skyIBLDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	skyIBLDesc.CPUAccessFlags = 0;
	skyIBLDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	skyIBLDesc.SampleDesc.Count = 1;
	skyIBLDesc.SampleDesc.Quality = 0;

	device->CreateTexture2D(&skyIBLDesc, 0, &skyIBLTexture);


	//Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC skyIBLRTVDesc;
	ZeroMemory(&skyIBLRTVDesc, sizeof(skyIBLRTVDesc));
	skyIBLRTVDesc.Format = skyIBLDesc.Format;
	skyIBLRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	skyIBLRTVDesc.Texture2DArray.ArraySize = 1;
	skyIBLRTVDesc.Texture2DArray.MipSlice = 0;
	
	for (int i = 0; i < 6; i++)
	{
		skyIBLRTVDesc.Texture2DArray.FirstArraySlice = i;
		device->CreateRenderTargetView(skyIBLTexture, &skyIBLRTVDesc, &skyIBLRTV[i]);
	}


	//Shader Resource Viee
	D3D11_SHADER_RESOURCE_VIEW_DESC skyIBLSRVDesc;
	ZeroMemory(&skyIBLSRVDesc, sizeof(skyIBLSRVDesc));
	skyIBLSRVDesc.Format = skyIBLDesc.Format;
	skyIBLSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	skyIBLSRVDesc.TextureCube.MostDetailedMip = 0;
	skyIBLSRVDesc.TextureCube.MipLevels = 1;

	device->CreateShaderResourceView(skyIBLTexture, &skyIBLSRVDesc, &skyIBLSRV);

	D3D11_VIEWPORT skyIBLViewport;
	skyIBLViewport.Width = 64;
	skyIBLViewport.Height = 64;
	skyIBLViewport.MinDepth = 0.0f;
	skyIBLViewport.MaxDepth = 1.0f;
	skyIBLViewport.TopLeftX = 0.0f;
	skyIBLViewport.TopLeftY = 0.0f;



	for (int i = 0; i < 6; i++)
	{
		XMVECTOR dir = XMVector3Rotate(tar[i], XMQuaternionIdentity());
		XMMATRIX view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&position), dir, up[i]);
		XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixTranspose(view));

		XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.0f, 0.1f, 100.0f);
		XMStoreFloat4x4(&projMatrix, DirectX::XMMatrixTranspose(P));

		context->OMSetRenderTargets(1, &skyIBLRTV[i], 0);
		context->RSSetViewports(1, &skyIBLViewport);
		context->ClearRenderTargetView(skyIBLRTV[i], color);

		vertexBuffer = skyMesh->GetVertexBuffer();
		indexBuffer = skyMesh->GetIndexBuffer();

		skyBoxVertexShader->SetMatrix4x4("view", viewMatrix);
		skyBoxVertexShader->SetMatrix4x4("projection", projMatrix);

		skyBoxVertexShader->CopyAllBufferData();
		skyBoxVertexShader->SetShader();

		convolutionPixelShader->SetShaderResourceView("Sky", skyTextureSRV);
		convolutionPixelShader->SetSamplerState("basicSampler", sampler);

		convolutionPixelShader->CopyAllBufferData();
		convolutionPixelShader->SetShader();

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		context->RSSetState(skyRasterizerState);
		context->OMSetDepthStencilState(skyDepthState, 0);

		context->DrawIndexed(skyMesh->GetIndexCount(), 0, 0);

		//Reset the render states we've changed
		context->RSSetState(0);
		context->OMSetDepthStencilState(0, 0);	
	}
}




// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (camera)
	{
		camera->UpdateProjectionMatrix((float)width / height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (size_t row = 0; row < numrows; row++)
	{
		for (size_t col = 0; col < numcolumns; col++)
		{
			spheres[row][col]->UpdateWorldMatrix();

		}
	}
		
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	const float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f};

	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->RSSetViewports(1, &viewport);
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;

	//Direct Lighting without textures
	/*float r = 0.0f;
	for (size_t i = 0; i < numrows; i++)
	{
		float m = 0.0f;
		for (size_t j = 0; j < numcolumns; j++)
		{
			render.RenderPBR(vertexBuffer, indexBuffer, pbrVertexShader, pbrPixelShader, spheres[i][j], camera, m, r, context);			
			m += 0.10f;
		}

		r += 0.10f;
	}*/

	//IBL with textures
	float r = 0.0f;
	for (size_t i = 0; i < numrows; i++)
	{
		float m = 0.0f;
		for (size_t j = 0; j < numcolumns; j++)
		{
			render.RenderRBRMaterial(vertexBuffer, indexBuffer, pbrMaterialVertexShader, pbrMaterialPixelShader, spheres[i][j], camera, m, r, context, sampler, skyIrradianceMapSRV);
			m += 0.10f;
		}

		r += 0.10f;
	}
	


	
	/******************************************************************************************** /
	/*DRAW SKYBOX*/
	/*******************************************************************************************/

	render.RenderSkyBox(vertexBuffer, indexBuffer, skyBoxVertexShader, skyBoxPixelShader, skyMesh, camera, context, skyTextureSRV, skyRasterizerState, skyDepthState);


	swapChain->Present(0, 0);
}


#pragma region Mouse Input

void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{

	prevMousePos.x = x;
	prevMousePos.y = y;


	SetCapture(hWnd);
}


void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{

	ReleaseCapture();
}


void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{

	if (buttonState & 0x0001) {
		float xDiff = (x - prevMousePos.x) * 0.005f;
		float yDiff = (y - prevMousePos.y) * 0.005f;
		camera->Rotate(yDiff, xDiff);
	}


	prevMousePos.x = x;
	prevMousePos.y = y;
}

void Game::OnMouseWheel(float wheelDelta, int x, int y)
{

}
#pragma endregion