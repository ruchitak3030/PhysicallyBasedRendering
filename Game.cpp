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

	

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}


Game::~Game()
{

	if (vertexBuffer)vertexBuffer->Release();
	if (indexBuffer)indexBuffer->Release();

	delete camera;


	delete vertexShader;
	delete pixelShader;
	delete skyBoxVertexShader;
	delete skyBoxPixelShader;
	delete pbrVertexShader;
	delete pbrPixelShader;

	delete sphereMesh;
	delete skyMesh;

	delete sphereMaterial;
	
	delete sky;

	delete sphere;
	for (size_t i = 0; i < numrows; i++)
		for (size_t j = 0; j < numcolumns; j++)
		{
			delete spheres[i][j];
		}
	

	sampler->Release();
	sphereTextureSRV->Release();
	sphereNormalMapSRV->Release();
	skyTextureSRV->Release();

	skyRasterizerState->Release();
	skyDepthState->Release();


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
	HRESULT m = CreateDDSTextureFromFile(device, L"Debug/Textures/Stormy.dds", 0, &skyTextureSRV);

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
	CreateWICTextureFromFile(device, context, L"Debug/Textures/sphere.tif", 0, &sphereTextureSRV);
	CreateWICTextureFromFile(device, context, L"Debug/Textures/sphereNormal.tif", 0, &sphereNormalMapSRV);
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


	sphereMaterial = new Material(vertexShader, pixelShader, sphereTextureSRV, sphereNormalMapSRV, sampler);

}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	sky = new GameEntity(skyMesh, sphereMaterial);
	sky->SetScale(1.0f, 1.0f, 1.0f);
	sky->SetPosition(0.0f, 0.0f, 0.0f);


	//sphere1 = new GameEntity(sphereMesh);
	//sphere1->SetScale(2.0f, 2.0f, 2.0f);
	//sphere1->SetPosition(6.0f, -2.0f, 0.0f);
	//sphere2 = new GameEntity(sphereMesh);
	//sphere2->SetScale(2.0f, 2.0f, 2.0f);
	//sphere2->SetPosition(3.0f, -2.0f, 0.0f);
	//sphere3 = new GameEntity(sphereMesh);
	//sphere3->SetScale(2.0f, 2.0f, 2.0f);
	//sphere3->SetPosition(0.0f, -2.0f, 0.0f);
	//sphere4 = new GameEntity(sphereMesh);
	//sphere4->SetScale(2.0f, 2.0f, 2.0f);
	//sphere4->SetPosition(-3.0f, -2.0f, 0.0f);
	//sphere5 = new GameEntity(sphereMesh);
	//sphere5->SetScale(2.0f, 2.0f, 2.0f);
	//sphere5->SetPosition(-6.0f, -2.0f, 0.0f);

	sphere = new GameEntity(sphereMesh);
	sphere->SetPosition(0.0, 0.0, 0.0f);
	sphere->SetScale(1.0f, 1.0f, 1.0f);

	for (size_t row = 0; row < numrows; ++row)
		for (size_t col = 0; col < numcolumns; ++col)
		{
			spheres[row][col] = new GameEntity(sphereMesh);			
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

	sphere->UpdateWorldMatrix();
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

	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//Light Position
	float r = 0.0f;
	for (size_t i = 0; i < numrows; i++)
	{
		float m = 0.0f;
		for (size_t j = 0; j < numcolumns; j++)
		{
			pbrVertexShader->SetMatrix4x4("world", spheres[i][j]->GetWorldMatrix());
			pbrVertexShader->SetMatrix4x4("view", camera->GetView());
			pbrVertexShader->SetMatrix4x4("projection", camera->GetProjection());

			pbrVertexShader->CopyAllBufferData();
			pbrVertexShader->SetShader();

			pbrPixelShader->SetFloat3("albedo", XMFLOAT3(0.5f, 0.0f, 0.0f));
			pbrPixelShader->SetFloat("metallic", m);
			pbrPixelShader->SetFloat("roughness", r);
			pbrPixelShader->SetFloat("a0", 1.0f);

			pbrPixelShader->SetFloat3("lightPosition1", XMFLOAT3(10.0f, 10.0f, -10.0f));
			pbrPixelShader->SetFloat3("lightPosition2", XMFLOAT3(10.0f, -10.0f, -10.0f));
			pbrPixelShader->SetFloat3("lightPosition3", XMFLOAT3(-10.0f, 10.0f, -10.0f));
			pbrPixelShader->SetFloat3("lightPosition4", XMFLOAT3(-10.0f, -10.0f, -10.0f));
			pbrPixelShader->SetFloat3("lightColor", XMFLOAT3(300.0f, 300.0f, 300.0f));

			pbrPixelShader->SetFloat3("cameraPos", camera->GetPosition());

			pbrPixelShader->CopyAllBufferData();
			pbrPixelShader->SetShader();


			vertexBuffer = spheres[i][j]->GetMesh()->GetVertexBuffer();
			indexBuffer = spheres[i][j]->GetMesh()->GetIndexBuffer();

			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			context->DrawIndexed(spheres[i][j]->GetMesh()->GetIndexCount(), 0, 0);
			m += 0.10f;
		}

		r += 0.10f;
	}
	
	
	/******************************************************************************************** /
	/*DRAW SKYBOX*/
	/*******************************************************************************************/

	context->HSSetShader(0, 0, 0);
	context->DSSetShader(0, 0, 0);

	vertexBuffer = sky->GetMesh()->GetVertexBuffer();
	indexBuffer = sky->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	skyBoxVertexShader->SetMatrix4x4("view", camera->GetView());
	skyBoxVertexShader->SetMatrix4x4("projection", camera->GetProjection());
	skyBoxVertexShader->CopyAllBufferData();
	skyBoxVertexShader->SetShader();


	skyBoxPixelShader->SetShaderResourceView("Sky", skyTextureSRV);
	skyBoxPixelShader->CopyAllBufferData();
	skyBoxPixelShader->SetShader();


	context->RSSetState(skyRasterizerState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	context->DrawIndexed(sky->GetMesh()->GetIndexCount(), 0, 0);

	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);


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