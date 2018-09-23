#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
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

	

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vertexBuffer)vertexBuffer->Release();
	if (indexBuffer)indexBuffer->Release();

	delete camera;

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;

	delete sphereMesh;
	delete sphereMaterial;
	delete sphere1;
	delete sphere2;
	delete sphere3;
	delete sphere4;
	delete sphere5;

	sampler->Release();
	sphereTextureSRV->Release();
	sphereNormalMapSRV->Release();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	LoadMesh();
	LoadTextures();
	CreateMaterials();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{

	camera = new Camera(0, 0, - 10);
	camera->UpdateProjectionMatrix((float)width / height);



}

void Game::LoadMesh()
{
	sphereMesh = new Mesh("Debug/Models/sphere.obj", device);
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
	sphere1 = new GameEntity(sphereMesh, sphereMaterial);
	sphere1->SetScale(2.0f, 2.0f, 2.0f);
	sphere1->SetPosition(6.0f, -2.0f, 0.0f);

	sphere2 = new GameEntity(sphereMesh, sphereMaterial);
	sphere2->SetScale(2.0f, 2.0f, 2.0f);
	sphere2->SetPosition(3.0f, -2.0f, 0.0f);

	sphere3 = new GameEntity(sphereMesh, sphereMaterial);
	sphere3->SetScale(2.0f, 2.0f, 2.0f);
	sphere3->SetPosition(0.0f, -2.0f, 0.0f);

	sphere4 = new GameEntity(sphereMesh, sphereMaterial);
	sphere4->SetScale(2.0f, 2.0f, 2.0f);
	sphere4->SetPosition(-3.0f, -2.0f, 0.0f);

	sphere5 = new GameEntity(sphereMesh, sphereMaterial);
	sphere5->SetScale(2.0f, 2.0f, 2.0f);
	sphere5->SetPosition(-6.0f, -2.0f, 0.0f);

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

	sphere1->UpdateWorldMatrix();
	sphere2->UpdateWorldMatrix();
	sphere3->UpdateWorldMatrix();
	sphere4->UpdateWorldMatrix();
	sphere5->UpdateWorldMatrix();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Send data to shader variables.

	//Sphere1
	vertexShader->SetMatrix4x4("world", sphere1->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Texture", sphere1->GetMaterial()->GetTextureSRV());
	pixelShader->SetShaderResourceView("NormalMap", sphere1->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetSamplerState("Sampler", sphere1->GetMaterial()->GetSamplerState());
	pixelShader->SetShader();


	vertexBuffer = sphere1->GetMesh()->GetVertexBuffer();
	indexBuffer = sphere1->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(sphere1->GetMesh()->GetIndexCount(), 0, 0);

	//Sphere2
	vertexShader->SetMatrix4x4("world", sphere2->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Texture", sphere2->GetMaterial()->GetTextureSRV());
	pixelShader->SetShaderResourceView("NormalMap", sphere2->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetSamplerState("Sampler", sphere2->GetMaterial()->GetSamplerState());
	pixelShader->SetShader();


	vertexBuffer = sphere2->GetMesh()->GetVertexBuffer();
	indexBuffer = sphere2->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(sphere2->GetMesh()->GetIndexCount(), 0, 0);

	//Sphere3
	vertexShader->SetMatrix4x4("world", sphere3->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Texture", sphere3->GetMaterial()->GetTextureSRV());
	pixelShader->SetShaderResourceView("NormalMap", sphere3->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetSamplerState("Sampler", sphere3->GetMaterial()->GetSamplerState());
	pixelShader->SetShader();

	vertexBuffer = sphere3->GetMesh()->GetVertexBuffer();
	indexBuffer = sphere3->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(sphere3->GetMesh()->GetIndexCount(), 0, 0);

	//Sphere4
	vertexShader->SetMatrix4x4("world", sphere4->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Texture", sphere4->GetMaterial()->GetTextureSRV());
	pixelShader->SetShaderResourceView("NormalMap", sphere4->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetSamplerState("Sampler", sphere4->GetMaterial()->GetSamplerState());
	pixelShader->SetShader();


	vertexBuffer = sphere4->GetMesh()->GetVertexBuffer();
	indexBuffer = sphere4->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(sphere4->GetMesh()->GetIndexCount(), 0, 0);

	//Sphere5
	vertexShader->SetMatrix4x4("world", sphere5->GetWorldMatrix());
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("Texture", sphere5->GetMaterial()->GetTextureSRV());
	pixelShader->SetShaderResourceView("NormalMap", sphere5->GetMaterial()->GetNormalMapSRV());
	pixelShader->SetSamplerState("Sampler", sphere5->GetMaterial()->GetSamplerState());
	pixelShader->SetShader();


	vertexBuffer = sphere5->GetMesh()->GetVertexBuffer();
	indexBuffer = sphere5->GetMesh()->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(sphere5->GetMesh()->GetIndexCount(), 0, 0);


	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion