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
#include "../Object/Character.h"

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
#include "../Shader/FBXModelShader.h"
#include "../Shader/GrassShader.h"




#include "../Shader/VPBuffer.h"





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
	rainTexture = new RenderTexture();

	Camera::Get();
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
	grassShader = new GrassShader();

	fbxShader = new FBXModelShader();


	player = new Character();


	depthShadowTexture->Initialize();
	shadowTexture->Initialize();
	blurShadowTexture->Initialize();
	lakeReflectionTexture->Initialize();
	lakeRefractionTexture->Initialize();
	mainRendering->Initialize(3);
	rainTexture->Initialize();
	postRendering->Initialize();


	landscape->Initialize();
	grass->Initialize(landscape);
	cloud->Initialize();
	noise->MakePerlinNoise();
	//volumeCloud->Initialize();
	rainCone->Initialize();


	player->Initialize(fbxShader, landscape);





	grassTexture->DrawTexture();
	landscape->SetTexture(grassTexture->diffuse, nullptr, nullptr);
	grass->setDiffuseMap(grassTexture->getGrassTexture());
	LodOff = false;
	






	vpBuffer = new VPBuffer();

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
	SAFE_DELETE(rainTexture);
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
	//	SAFE_DELETE(instanceShader);
	SAFE_DELETE(rainShader);

	SAFE_DELETE(player);
}

void GameMain::Update()
{

	ControlCamera();
	player->Update();

	Camera::Get()->Update();



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


	//player->Move(0.5f, 0.5f);

	static int i = 0;

	if (Keyboard::Get()->KeyUp(VK_SPACE)) {
		//depthShadowTexture->SaveTexture(L"depthShadow.png");
		//shadowTexture->SaveTexture(L"shadow.png");
		//blurShadowTexture->SaveTexture(L"blur.png");
		//lakeRefractionTexture->SaveTexture(L"Mirror.png");
		//mainRendering->SaveTexture(L"main.png",1);
		LodOff = !LodOff;
		//testvalue++;
		//if (testvalue == 7)
		//	testvalue = 0;
		//noise->MakePerlinNoise();
		//D3D::Get()->TestMultiTexture(0);

		Camera::Get()->SetFallowCamera(nullptr);
		i++;
		if (i > 3)
			i = 0;
		player->SetAni(i);
	}
	if (!LodOff) {
		landscape->changeLOD(frustum);
		//Camera::Get()->SetFallowCamera(nullptr);

	}
	else
	{
		//player->SetCamera(Camera::Get());
	}

	


	if (Keyboard::Get()->KeyUp('P')) {
		landscapeWireFrame = !landscapeWireFrame;
	}

}

void GameMain::PreRender()
{

	D3DXMATRIX world, view, projection;
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);

	//광원의 시점에서 물체까지의 거리(depth)기록
	{

		depthShadowTexture->SetTarget();
		depthShadowTexture->Clear(0, 0, 0, 1);

		depthShadowShader->Render();
		testcube->Render();
		
		depthShadowShader->Render();
		landscape->Render();
		


	}
	//기록된 depth를 바탕으로 그림자 연산
	//TODO 여기서 텍스쳐를 여러개 쓰고 각 텍스쳐별로 bias를 따로 설정해주어야 한다. ( cascade shadow)
	{
		shadowTexture->SetTarget();
		shadowTexture->Clear();

		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);

		vpBuffer->SetVPMatrix(view, projection);

		shadowShader->Render(*depthShadowTexture->GetShadowResourceView());
		testcube->Render();



		
		shadowShader->Render(*depthShadowTexture->GetShadowResourceView());
		landscape->Render();
	}
	//연산된 그림자를 blur 처리
	{
		blurShadowTexture->SetTarget(true);
		blurShadowTexture->Clear(0, 0, 0, 1);


		Camera::Get()->GetDefaultView(&view);
		D3D::Get()->GetOrthoProjection(&projection);
		vpBuffer->SetVPMatrix(view, projection);

		
		
		blurShader->Render(*shadowTexture->GetShadowResourceView());
		orthoWindow->Render();
	}

	//호수의 반사평면 그리기
	{
		lakeReflectionTexture->SetTarget();
		lakeReflectionTexture->Clear();


		//반사된 세계는 현실 세계에 대해 뒤집혀있으므로 컬모드가 반대다
		Rasterizer::Get()->SetFrontCullMode();

		//반사된 뷰, 프로젝션 받기
		Camera::Get()->GetMirrorView(&view);
		D3D::Get()->GetProjection(&projection);
		vpBuffer->SetVPMatrix(view, projection);



		//스카이돔, 구름 그리기
		D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
		Rasterizer::Get()->SetOffCullMode();
		{
			//반사된 평면에 맞춰서 카메라의 위치, 스카이 플레인의 위치까지 보정해주어야 함
			//TODO 이부분 개선 필요할듯
			D3DXMATRIX world;
			D3DXVECTOR3 camPos;
			Camera::Get()->GetPosition(&camPos);
			camPos.y = -camPos.y + (lake->GetWaterHeigh() * 2.0f);
			D3DXMatrixTranslation(&world, camPos.x, camPos.y, camPos.z);

			skydomeShader->Render();
			skydome->Render();
			
			D3D::Get()->SetBlender(D3D::BL_state::Add);
			skyplaneShader->Render(cloud->getPerlinMap());
			cloud->Render();
			
			D3D::Get()->SetBlender(D3D::BL_state::Off);
		}
		D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
		Rasterizer::Get()->SetFrontCullMode();


		normalMapShader->Render(testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		testcube->Render();



		D3DXPLANE clipPlane = lake->getwaterPlane();
		terrainShader->SetPlane(clipPlane);

		terrainShader->Render(landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView());
		landscape->Render();
		Rasterizer::Get()->SetOnCullMode();
	}

	//호수의 굴절 평면 그리기 
	//이건 굴절이 아니라 그냥 투시된 형태
	//TODO : 굴절을 표현하기 위해서 어떻게 해야할까?
	{
		lakeRefractionTexture->SetTarget();
		lakeRefractionTexture->Clear();

		//뷰, 프로젝션 받기
		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);
		vpBuffer->SetVPMatrix(view, projection);
		//testcube->Render();
		//for (int i = 0; i < 6; i++) {
		//	normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		//}


		//호수 아랫면을 랜더링하게 클립면을 바꿔준다
		D3DXPLANE clipPlane = lake->getwaterPlane();
		clipPlane *= -1;
		terrainShader->SetPlane(clipPlane);

		terrainShader->Render(landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView());
		landscape->Render();
		Rasterizer::Get()->SetSolid();
	}

	//메인 장면 렌더링(2번 텍스쳐에는 터레인의  깊이값 저장한다)
	{
		mainRendering->SetTarget();
		mainRendering->Clear();

		Camera::Get()->GetView(&view);
		D3D::Get()->GetProjection(&projection);
		vpBuffer->SetVPMatrix(view, projection);


		D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
		Rasterizer::Get()->SetOffCullMode();
		{
			skydomeShader->Render();
			skydome->Render();
			

			D3D::Get()->SetBlender(D3D::BL_state::Add);
			skyplaneShader->Render(cloud->getPerlinMap());
			cloud->Render();
			

			rainCone->Render();
			rainShader->Render(rainCone->getIndexCount(), rainCone->getWorld(), view, projection,
				*mainRendering->GetShadowResourceView(1), D3DXCOLOR(1, 1, 1, 1));
			D3D::Get()->SetBlender(D3D::BL_state::Off);
		}
		D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
		Rasterizer::Get()->SetOnCullMode();



		normalMapShader->Render(testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
		testcube->Render();



		D3DXPLANE clipPlane = lake->getwaterPlane();
		terrainShader->SetPlane(clipPlane);

		terrainShader->Render(landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView());
		landscape->Render();
		//terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView(), lake->getwaterPlane());
		//Rasterizer::Get()->SetSolid();


		lake->Render();
		waterShader->Render(lake->indexCount, lake->world,
			lake->getNormalTexture(), cloud->getPerlinMap(),
			*lakeReflectionTexture->GetShadowResourceView(),
			*lakeRefractionTexture->GetShadowResourceView());

		//grass->Render();
		//grassShader->Render(grass->getInstanceCount(), *noise->GetPerlinNoise());


		player->Render();
	}
	//{
	//	rainTexture->SetTarget();
	//	rainTexture->Clear();
	//
	//	D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
	//	D3D::Get()->SetBlender(D3D::BL_state::Off);
	//	//D3D::Get()->SetBlender(D3D::BL_state::Add);
	//	{
	//		rainCone->Render();
	//		//colorShader->Render(rainCone->getIndexCount(), rainCone->getWorld(), view, projection,
	//		//	D3DXCOLOR(1, 1, 1, 1));
	//		rainShader->Render(rainCone->getIndexCount(), rainCone->getWorld(), view, projection,
	//			*mainRendering->GetShadowResourceView(1), D3DXCOLOR(1, 1, 1, 1));
	//	}
	//	D3D::Get()->SetBlender(D3D::BL_state::Off);
	//	D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
	//}
	//{
	//	postRendering->SetTarget();
	//	postRendering->Clear(0,0,0,1);
	//
	//	Camera::Get()->GetDefaultView(&view);
	//	D3D::Get()->GetOrthoProjection(&projection);
	//	D3D::Get()->SetBlender(D3D::BL_state::Off);
	//	orthoWindow->Render();
	//
	//	textureShader->Render(orthoWindow->GetIndexCount(), orthoWindow->GetWorld(), view, projection, *mainRendering->GetShadowResourceView(0));
	//
	//	D3D::Get()->SetDepthStencilState(D3D::DS_state::offState);
	//	D3D::Get()->SetBlender(D3D::BL_state::Add);
	//	textureShader->Render(orthoWindow->GetIndexCount(), orthoWindow->GetWorld(), view, projection, *rainTexture->GetShadowResourceView(0));
	//	D3D::Get()->SetBlender(D3D::BL_state::Off);
	//	D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
	//
	//}
}

void GameMain::Render()
{
	D3DXMATRIX world, view, projection;


	Camera::Get()->GetDefaultView(&view);
	D3D::Get()->GetOrthoProjection(&projection);
	D3D::Get()->SetBlender(D3D::BL_state::Off);
	vpBuffer->SetVPMatrix(view, projection);
	

	textureShader->Render( *mainRendering->GetShadowResourceView(0));
	orthoWindow->Render();


	/*
	//WATER REFLECTION OLD ver.
	{

	//스텐실 버퍼 삭제
	//1.스텐실 버퍼를 초기화 한 후 그 위에다 거울면을 그린다(깊이 버퍼는 남겨 있기에 적절히 z버퍼로 가려짐)
	D3D::Get()->ClearDepthStencil(D3D11_CLEAR_STENCIL,0, 0);

	//그려야 할 곳(물)을 스텐실 버퍼에 기록
	D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorPlaneRenderState);
	D3D::Get()->SetBlender_Linear();
	lake->Render();
	//colorShader->Render(lake->indexCount, lake->world, view, projection, D3DXCOLOR(0, 1, 0, 1));
	waterShader->Render(lake->indexCount, lake->world, view, projection, nullptr, lake->getNormalTexture(), cloud->getPerlinMap());
	//호수 모양만 스텐실 버퍼가 기록되어있음


	//깊이버퍼를 삭제하여 다시 호수 위에 그릴수 있게 만듬
	D3D::Get()->ClearDepthStencil(D3D11_CLEAR_DEPTH, 1, 0);


	//반사된 세계는 현실 세계에 대해 뒤집혀있으므로 컬모드가 반대다
	Rasterizer::Get()->SetFrontCullMode();


	//반사된 뷰, 프로젝션 받기
	Camera::Get()->GetMirrorView(&view);
	D3D::Get()->GetProjection(&projection);


	//호수면에만 그리도록 뎁스 스텐실 변경해준다
	D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorSkyplaneState);



	//스카이뷰를 그릴때 카메라 위치도 다시 셋팅해줘야 한다.
	D3DXMATRIX world;
	D3DXVECTOR3 camPos;
	Camera::Get()->GetPosition(&camPos);

	camPos.y = -camPos.y + (lake->GetWaterHeigh() * 2.0f);

	D3DXMatrixTranslation(&world, camPos.x, camPos.y, camPos.z);


	cloud->Render();
	skyplaneShader->Render(cloud->getIndexCount(), world, view, projection, cloud->getDiffuseMap(), cloud->getPerlinMap());



	//물과의 색상혼합을 해줌
	D3D::Get()->SetBlender_AddBlend();

	D3D::Get()->SetDepthStencilState(D3D::DS_state::mirrorObjectRenderState);
	testcube->Render();
	for (int i = 0; i < 6; i++) {
	normalMapShader->Render(testcube->indexCount, testcube->world[i], view, projection, testcube->diffuseMap, testcube->normalMap, testcube->heightMap, *blurShadowTexture->GetShadowResourceView());
	}


	landscape->Render();
	terrainShader->Render(landscape->getIndexCount(), landscape->getWorld(), view, projection, landscape->getDiffuseMap(), landscape->getNormalMap(), *blurShadowTexture->GetShadowResourceView());



	}
	*/
	D3D::Get()->SetBlender(D3D::BL_state::Off);
	Rasterizer::Get()->SetOnCullMode();
	D3D::Get()->SetDepthStencilState(D3D::DS_state::onState);
}

//void GameMain::ControlCamera()
//{
//	if (Keyboard::Get()->KeyPress('W'))
//		Camera::Get()->MoveForward();
//	else if (Keyboard::Get()->KeyPress('S'))
//		Camera::Get()->MoveBackward();
//
//	if (Keyboard::Get()->KeyPress('A'))
//		Camera::Get()->MoveLeft();
//	else if (Keyboard::Get()->KeyPress('D'))
//		Camera::Get()->MoveRight();
//
//	if (Keyboard::Get()->KeyPress('E'))
//		Camera::Get()->MoveUp();
//	else if (Keyboard::Get()->KeyPress('Q'))
//		Camera::Get()->MoveDown();
//
//	if (Mouse::Get()->ButtonPress(1))
//	{
//		D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();
//
//		Camera::Get()->Rotate(D3DXVECTOR2(move.y, move.x));
//	}
//}

void GameMain::ControlCamera()
{
	if (Camera::Get()->Fallowing()) {
		if (Keyboard::Get()->KeyPress('W'))
			player->Move(0, 0.2f);
		else if (Keyboard::Get()->KeyPress('S'))
			player->Move(0, -0.2f);

		if (Keyboard::Get()->KeyPress('A'))
			player->Move(-0.2f, 0);
		else if (Keyboard::Get()->KeyPress('D'))
			player->Move(0.2f, 0);
	}
	else {
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
	}


	if (Mouse::Get()->ButtonPress(1))
	{
		D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

		Camera::Get()->Rotate(D3DXVECTOR2(move.y, move.x));
	}
}