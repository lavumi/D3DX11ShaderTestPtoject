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
class CascadeShadowBuffer;
class VPBuffer;
class LightBuffer;

class Frustum;


class Character;


class TextManager;
class RenderingManager {
public:
	RenderingManager();
	~RenderingManager();

	void Initianlize(TextManager*, Environment*);

	void AddCharacter(Character* );
	void Test(TestCube* testcube);

	void Update();

	void Render();
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
	LightManager* light;

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
	CascadeShadowBuffer* shadowBuffer; //10�� ����
	VPBuffer* vpBuffer;       //12�� ����
	LightBuffer* lightBuffer;//11�� ����

	D3DXMATRIX view, projection;








	//���⼭���� �ܺο��� �޾ƿ��� �͵�
	Frustum* frustum;
	D3DXMATRIX cropMatrix[4];

	//Landscape
	Environment* environment;


	TextManager* txtManager;

	TestCube* testcube;

	vector<Character*> characters;
};