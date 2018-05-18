#pragma once

class ShaderManager;
class Sampler;
class Rasterizer;
class LightManager;

class RenderTexture;


class Environment;

class TestCube;

class GrassTexture;
class PerlinNoise;


class OrthoWindowPlane;
class FrustumBuffer;
class VPBuffer;
class LightBuffer;
class GAbuffer;

class Frustum;


class Character;
class RenderingManager {
public:
	RenderingManager();
	~RenderingManager();

	void Initianlize(Environment*);

	void AddCharacter(Character* );
	void Test(TestCube* testcube);

	void Update();

	void Render();

	Frustum* GetFrustum() {
		return frustum;
	}
private:

	void TreeRenderTest();



	void LightView();
	void RenderShadow();
	void RenderReflection();
	void RenderRefraction();
	void RenderMain();

	void PostRender();

	void FinalRender();


	ShaderManager * shaderManager;
	Sampler* sampler;
	Rasterizer* rasterizer;
	LightManager* lightManager;

	//RT
	RenderTexture * depthShadowTexture;
	RenderTexture* shadowTexture;
	RenderTexture* lakeReflectionTexture;
	RenderTexture* lakeRefractionTexture;
	RenderTexture* rainTexture;
	RenderTexture* mainRendering;
	RenderTexture* postRendering;



	GrassTexture* grassTexture;
	PerlinNoise* noise;

	//OrthoWindow
	OrthoWindowPlane* orthoWindow;

	//���չ��۵�
	FrustumBuffer* frustumBuffer; //10�� ����
	VPBuffer* vpBuffer;       //12�� ����
	LightBuffer* lightBuffer;//11�� ����
	GAbuffer* gaBuffer;//PS2�� ����

	D3DXMATRIX view, projection;








	//���⼭���� �ܺο��� �޾ƿ��� �͵�
	Frustum* frustum;
	D3DXMATRIX cropMatrix[4];

	//Landscape
	Environment* environment;


	TestCube* testcube;

	vector<Character*> characters;
};