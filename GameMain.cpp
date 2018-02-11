#include "stdafx.h"
#include "GameMain.h"

#include "./Mesh/ObjectMesh.h"
#include "./Render\RenderTexture.h"
#include "./Render/Frustum.h"

#include "./ProceduralTexture/GrassTexture.h"
#include "./ProceduralTexture/MosaicTile.h"
#include "./ProceduralTexture/PerlinNoise.h"

#include "./Terrain/Skydome.h"
#include "./Terrain/Skyplane.h"
#include "./Terrain/Landscape.h"
#include "./Terrain/Water.h"
#include "./Terrain/TerrainGrass.h"
#include "./Terrain/Cloud.h"
#include "./Terrain/RainCone.h"


#include "./Object/TestCube.h"
#include "./Object/Mirror.h"
#include "./Object/OrthoWindowPlane.h"

#include "../Shader/NormalMapShader.h"
#include "../Shader/DepthShadowShader.h"
#include "../Shader/TextureShader.h"
#include "../Shader/SkydomeShader.h"
#include "../Shader/ShadowShader.h"
#include "../Shader/BlurShader.h"
#include "../Shader/TerrainShader.h"
#include "../Shader/ColorShader.h"
#include "../Shader/WaterShader.h"
#include "../Shader/SkyplaneShader.h"
#include "../Shader/InstanceTextureShader.h"
#include "../Shader/RainShader.h"



bool GameMain::landscapeWireFrame = false;

void GameMain::Initialize()
{
	depthShadowTexture = new RenderTexture(1280, 1280);
	shadowTexture = new RenderTexture();
	blurShadowTexture = new RenderTexture();
	lakeReflectionTexture = new RenderTexture();
	lakeRefractionTexture = new RenderTexture();
	mainRendering = new RenderTexture();
	postRendering = new RenderTexture();


	UserInterface::Get();
	LightManager::Get();
	frustum = new Frustum();
	testcube = new TestCube();
	testplane = new Mirror();
	skydome = new Skydome();
	cloud = new Skyplane();
	landscape = new Landscape();
	lake = new Water();
	grass = new TerrainGrass();
	//volumeCloud = new Cloud();
	rainCone = new RainCone();

	grassTexture = new GrassTexture();
	//mosaicTile = new MosaicTile();
	noise = new PerlinNoise();



	orthoWindow = new OrthoWindowPlane();

	normalMapShader = new NormalMapShader();
	depthShadowShader = new DepthShadowShader();
	textureShader = new TextureShader();
	skydomeShader = new SkydomeShader();
	shadowShader = new ShadowShader();
	blurShader = new BlurShader();
	terrainShader = new TerrainShader();
	colorShader = new ColorShader();
	skyplaneShader = new SkyplaneShader();
	waterShader = new WaterShader();
	instanceShader = new InstanceTextureShader();
	rainShader = new RainShader();








	depthShadowTexture->Initialize();
	shadowTexture->Initialize();
	blurShadowTexture->Initialize();
	lakeReflectionTexture->Initialize();
	lakeRefractionTexture->Initialize();
	mainRendering->Initialize(3);
	postRendering->Initialize();


	landscape->Initialize();
	grass->Initialize(landscape);
	cloud->Initialize();
	noise->MakePerlinNoise();
	//volumeCloud->Initialize();
	rainCone->Initialize();


	grassTexture->DrawTexture();
	landscape->SetTexture(grassTexture->diffuse, nullptr, nullptr);
	grass->setDiffuseMap(grassTexture->getGrassTexture());
	LodOff = false;
}

void GameMain::Destroy()
{
	SAFE_DELETE(skydome);
	SAFE_DELETE(landscape);
	UserInterface::Delete();

	SAFE_DELETE(depthShadowTexture);
	SAFE_DELETE(shadowTexture);
	SAFE_DELETE(blurShadowTexture);
	SAFE_DELETE(lakeReflectionTexture);
	SAFE_DELETE(lakeRefractionTexture);
	SAFE_DELETE(mainRendering);
	SAFE_DELETE(postRendering);

	SAFE_DELETE(frustum);

	SAFE_DELETE(testcube);
	SAFE_DELETE(testplane);
	SAFE_DELETE(skydome);
	SAFE_DELETE(cloud);
	SAFE_DELETE(landscape);
	SAFE_DELETE(lake);
	SAFE_DELETE(grass);
	//SAFE_DELETE(volumeCloud);
	SAFE_DELETE(rainCone);

	SAFE_DELETE(grassTexture);
	SAFE_DELETE(noise);

	SAFE_DELETE(orthoWindow);

	SAFE_DELETE(normalMapShader);
	SAFE_DELETE(depthShadowShader);
	SAFE_DELETE(textureShader);
	SAFE_DELETE(skydomeShader);
	SAFE_DELETE(shadowShader);
	SAFE_DELETE(blurShader);
	SAFE_DELETE(terrainShader);
	SAFE_DELETE(colorShader);
	SAFE_DELETE(skyplaneShader);
	SAFE_DELETE(waterShader);
	SAFE_DELETE(instanceShader);
	SAFE_DELETE(rainShader);

}

void GameMain::Update()
{

	ControlCamera();


	LightManager::Get()->Update();;
	//	Camera::Get()->Update();

	D3DXMATRIX view, projection;

	Camera::Get()->GetView(&view);
	D3D::Get()->GetProjection(&projection);
	frustum->SetFrustum(Camera::screenDepth, view, projection);




	skydome->Update();
	landscape->Update();
	cloud->Update();
	rainCone->Update();
	//volumeCloud->Update();

	testcube->Update();





	if (Keyboard::Get()->KeyUp(VK_SPACE)) {
		//depthShadowTexture->SaveTexture(L"depthShadow.png");
		//shadowTexture->SaveTexture(L"shadow.png");
		//blurShadowTexture->SaveTexture(L"blur.png");
		//lakeRefractionTexture->SaveTexture(L"Mirror.png");
		//mainRendering->SaveTexture(L"main.png");
		LodOff = !LodOff;
		testvalue++;
		if (testvalue == 7)
			testvalue = 0;
		//noise->MakePerlinNoise();
		//D3D::Get()->TestMultiTexture(0);
	}
	//if (!LodOff)		landscape->changeLOD(frustum);


	if (Keyboard::Get()->KeyUp('P')) {
		landscapeWireFrame = !landscapeWireFrame;
	}

}

void GameMain::PreRender()
{
	
	D3DXMATRIX world, view, projection;
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);

	//������ �������� ��ü������ �Ÿ�(depth)���
	{

		depthShadowTexture->SetTarget();
		depthShadowTexture->Clear(0, 0, 0, 1);

		//testplane->Render();
		//depthShadowShader->Render(testplane->indexCount, testplane->world, view, projection);


		testcube->Render();
		for (int i = 0; i < 6; i++) {
			depthShadowShader->Render(testcube->indexCount, testcube->world[i], view, projection);
		}

		landscape->Render();
		depthShadowShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection);


	}
	//��ϵ� depth�� �������� �׸��� ����
	//TODO ���⼭ �ؽ��ĸ� ������ ���� �� �ؽ��ĺ��� bias�� ���� �������־�� �Ѵ�.
	{
		shadowTexture->SetTarget();
		shadowTexture->Clear();

		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);

		//testplane->Render();
		//shadowShader->Render(testplane->indexCount, testplane->world,  view, projection,*depthShadowTexture->GetShadowResourceView());

		testcube->Render();
		for (int i = 0; i < 6; i++) {
			shadowShader->Render(testcube->indexCount, testcube->world[i], view, projection, *depthShadowTexture->GetShadowResourceView());
		}


		landscape->Render();
		shadowShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, *depthShadowTexture->GetShadowResourceView());
	}
	//����� �׸��ڸ� blur ó��
	{
		blurShadowTexture->SetTarget(true);
		blurShadowTexture->Clear(0, 0, 0, 1);


		Camera::Get()->GetDefaultView(&view);
		D3D::Get()->GetOrthoProjection(&projection);

		orthoWindow->Render();
		blurShader->Render(orthoWindow->GetIndexCount(),orthoWindow->GetWorld(), view, projection, *shadowTexture->GetShadowResourceView());
	}

	//ȣ���� �ݻ���� �׸���
	{
		lakeReflectionTexture->SetTarget();
		lakeReflectionTexture->Clear();


		//�ݻ�� ����� ���� ���迡 ���� �����������Ƿ� �ø�尡 �ݴ��
		Rasterizer::Get()->SetFrontCullMode();

		D3DXPLANE waterPlane = lake->getwaterPlane();
		//�ݻ�� ��, �������� �ޱ�
		Camera::Get()->GetMirrorView(&view);
		D3D::Get()->GetProjection(&projection);




		//��ī�̵�, ���� �׸���
		D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
		Rasterizer::Get()->SetOffCullMode();
		{
			//�ݻ�� ��鿡 ���缭 ī�޶��� ��ġ, ��ī�� �÷����� ��ġ���� �������־�� ��
			D3DXMATRIX world;
			D3DXVECTOR3 camPos;
			Camera::Get()->GetPosition(&camPos);
			camPos.y = -camPos.y + (lake->GetWaterHeigh() * 2.0f);
			D3DXMatrixTranslation(&world, camPos.x, camPos.y, camPos.z);


			D3DXVECTOR3 lightDir;
			LightManager::Get()->GetLightDirection(&lightDir);


			skydome->Render();
			skydomeShader->Render(skydome->getIndexCount(), world, view, projection, lightDir);
			D3D::Get()->SetBlender(D3D::BL_state::Add);
			cloud->Render();
			skyplaneShader->Render(cloud->getIndexCount(), world, view, projection, cloud->getPerlinMap());
			D3D::Get()->SetBlender(D3D::BL_state::Off);
		}
		D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
		Rasterizer::Get()->SetFrontCullMode();

		testcube->Render();
		for (int i = 0; i < 6; i++) {
			normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		}

		if (landscapeWireFrame)
			Rasterizer::Get()->SetWireframe();

		landscape->Render();
		terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView(), waterPlane);
		Rasterizer::Get()->SetOnCullMode();
	}

	//ȣ���� ���� ��� �׸��� 
	//�̰� ������ �ƴ϶� �׳� ���õ� ����
	//TODO : ������ ǥ���ϱ� ���ؼ� ��� �ؾ��ұ�?
	{
		lakeRefractionTexture->SetTarget();
		lakeRefractionTexture->Clear();

		//��, �������� �ޱ�
		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);

		//testcube->Render();
		//for (int i = 0; i < 6; i++) {
		//	normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		//}


		//ȣ�� �Ʒ����� �������ϰ� Ŭ������ �ٲ��ش�
		D3DXPLANE clipPlane = lake->getwaterPlane();
		clipPlane *= -1;


		if (landscapeWireFrame)
			Rasterizer::Get()->SetWireframe();
		landscape->Render();
		terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView(), clipPlane);
		Rasterizer::Get()->SetSolid();
	}

	//���� ��� ������(2�� �ؽ��Ŀ��� �ͷ�����  ���̰� �����Ѵ�)
	{

		mainRendering->SetTarget();
		mainRendering->Clear(0,0,0,1);

		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);

		D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
		Rasterizer::Get()->SetOffCullMode();
		{
		
			D3DXVECTOR3 lightDir;
			
			LightManager::Get()->GetLightDirection(&lightDir);
			
			skydome->Render();
			skydomeShader->Render(skydome->getIndexCount(), skydome->getWorld(), view, projection, lightDir);
		
		
			D3D::Get()->SetBlender(D3D::BL_state::Add);
			cloud->Render();
			skyplaneShader->Render(cloud->getIndexCount(), cloud->getWorld(), view, projection, cloud->getPerlinMap());

			rainCone->Render();
			rainShader->Render(rainCone->getIndexCount(), rainCone->getWorld(), view, projection,
				*mainRendering->GetShadowResourceView(1), D3DXCOLOR(1, 1, 1, 1));
			D3D::Get()->SetBlender(D3D::BL_state::Off);
		}
		D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
		Rasterizer::Get()->SetOnCullMode();



		
		testcube->Render();
		for (int i = 0; i < 6; i++) {
			normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		}
		
		landscape->Render();
		terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView(), lake->getwaterPlane());
		
		
		
		
		lake->Render();
		waterShader->Render(lake->indexCount, lake->world, view, projection,
			lake->getNormalTexture(), cloud->getPerlinMap(),
			*lakeReflectionTexture->GetShadowResourceView(),
			*lakeRefractionTexture->GetShadowResourceView());




		D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
		D3D::Get()->SetBlender(D3D::BL_state::Add);
		{
			rainCone->Render();
			rainShader->Render(rainCone->getIndexCount(), rainCone->getWorld(), view, projection,
				*mainRendering->GetShadowResourceView(1), D3DXCOLOR(1, 1, 1, 1));
		}
		D3D::Get()->SetBlender(D3D::BL_state::Off);
		D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
	}
}

void GameMain::Render()
{
	D3DXMATRIX world, view, projection;


	Camera::Get()->GetDefaultView(&view);
	D3D::Get()->GetOrthoProjection(&projection);
	D3D::Get()->SetBlender(D3D::BL_state::Off);
	orthoWindow->Render();
	textureShader->Render(orthoWindow->GetIndexCount(), orthoWindow->GetWorld(), view, projection, *mainRendering->GetShadowResourceView(0));

	


		//WATER REFLECTION OLD ver.
	{
		/*
		//���ٽ� ���� ����
		//1.���ٽ� ���۸� �ʱ�ȭ �� �� �� ������ �ſ���� �׸���(���� ���۴� ���� �ֱ⿡ ������ z���۷� ������)
		D3D::Get()->ClearDepthStencil(D3D11_CLEAR_STENCIL,0, 0);

		//�׷��� �� ��(��)�� ���ٽ� ���ۿ� ���
		D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorPlaneRenderState);
		D3D::Get()->SetBlender_Linear();
		lake->Render();
		//colorShader->Render(lake->indexCount, lake->world, view, projection, D3DXCOLOR(0, 1, 0, 1));
		waterShader->Render(lake->indexCount, lake->world, view, projection, nullptr, lake->getNormalTexture(), cloud->getPerlinMap());
		//ȣ�� ��縸 ���ٽ� ���۰� ��ϵǾ�����


		//���̹��۸� �����Ͽ� �ٽ� ȣ�� ���� �׸��� �ְ� ����
		D3D::Get()->ClearDepthStencil(D3D11_CLEAR_DEPTH, 1, 0);


		//�ݻ�� ����� ���� ���迡 ���� �����������Ƿ� �ø�尡 �ݴ��
		Rasterizer::Get()->SetFrontCullMode();


		//�ݻ�� ��, �������� �ޱ�
		Camera::Get()->GetMirrorView(&view);
		D3D::Get()->GetProjection(&projection);


		//ȣ���鿡�� �׸����� ���� ���ٽ� �������ش�
		D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorSkyplaneState);



		//��ī�̺並 �׸��� ī�޶� ��ġ�� �ٽ� ��������� �Ѵ�.
		D3DXMATRIX world;
		D3DXVECTOR3 camPos;
		Camera::Get()->GetPosition(&camPos);

		camPos.y = -camPos.y + (lake->GetWaterHeigh() * 2.0f);

		D3DXMatrixTranslation(&world, camPos.x, camPos.y, camPos.z);


		cloud->Render();
		skyplaneShader->Render(cloud->getIndexCount(), world, view, projection, cloud->getDiffuseMap(), cloud->getPerlinMap());



		//������ ����ȥ���� ����
		D3D::Get()->SetBlender_AddBlend();

		D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorObjectRenderState);
		testcube->Render();
		for (int i = 0; i < 6; i++) {
		normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		}


		landscape->Render();
		terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView());


		*/
	}

	D3D::Get()->SetBlender(D3D::BL_state::Off);
	Rasterizer::Get()->SetOnCullMode();
	D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
}

void GameMain::ControlCamera()
{
	if (Keyboard::Get()->KeyPress('W'))
		Camera::Get()->MoveForward();
	else if (Keyboard::Get()->KeyPress('S'))
		Camera::Get()->MoveBackward();

	if (Keyboard::Get()->KeyPress('A'))
		Camera::Get()->MoveLeft();
	else if (Keyboard::Get()->KeyPress('D'))
		Camera::Get()->MoveRight();

	if (Keyboard::Get()->KeyPress('E'))
		Camera::Get()->MoveUp();
	else if (Keyboard::Get()->KeyPress('Q'))
		Camera::Get()->MoveDown();

	if (Mouse::Get()->ButtonPress(1))
	{
		D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

		Camera::Get()->Rotate(D3DXVECTOR2(move.y, move.x));
	}
}

