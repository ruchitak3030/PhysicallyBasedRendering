#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include <DirectXMath.h>


using namespace DirectX;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void LoadMesh();
	void LoadTextures();
	void CreateMaterials();
	void CreateBasicGeometry();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	//Mesh
	Mesh* sphereMesh;

	//GameEntities
	GameEntity* sphere1;
	GameEntity* sphere2;
	GameEntity* sphere3;
	GameEntity* sphere4;
	GameEntity* sphere5;

	//Camera
	Camera* camera;

	//Textures
	ID3D11ShaderResourceView* sphereTextureSRV;
	ID3D11ShaderResourceView* sphereNormalMapSRV;
	ID3D11SamplerState* sampler;

	//Materials
	Material* sphereMaterial;


	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

