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
	void LoadSkyBox();
	void LoadMesh();
	void LoadTextures();
	void CreateMaterials();
	void CreateBasicGeometry();	
	void SetUpImageBasedLighting();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* skyBoxVertexShader;
	SimplePixelShader* skyBoxPixelShader;
	SimpleVertexShader* pbrVertexShader;
	SimplePixelShader* pbrPixelShader;
	SimplePixelShader* convolutionPixelShader;

	//Skybox Stuff
	ID3D11ShaderResourceView* skyTextureSRV;
	ID3D11RasterizerState* skyRasterizerState;
	ID3D11DepthStencilState* skyDepthState;

	ID3D11ShaderResourceView* skyIrradianceMapSRV;

	//Iamge Based Lighting Stuff
	ID3D11Texture2D* skyIBLTexture;
	ID3D11RenderTargetView* skyIBLRTV[6];
	ID3D11ShaderResourceView* skyIBLSRV;

	//Mesh
	Mesh* sphereMesh;
	Mesh* skyMesh;

	//GameEntities
	GameEntity* sky;
	GameEntity* spheres[8][8];
	int numrows = 8;
	int numcolumns = 8;
	float spacing = 1.0f;
	


	//Camera
	Camera* camera;

	//Textures
	ID3D11ShaderResourceView* ironrustAlbedoMapSRV;
	ID3D11ShaderResourceView* ironrustNormalMapSRV;
	ID3D11ShaderResourceView* ironrustMetallicMapSRV;
	ID3D11ShaderResourceView* ironrustRoughnessMapSRV;

	ID3D11SamplerState* sampler;

	//Materials
	Material* ironrustMat;


	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

