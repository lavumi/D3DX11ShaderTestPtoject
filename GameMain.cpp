#include "stdafx.h"
#include "GameMain.h"

#include "../Environment/Environment.h"

#include "./Object/TestCube.h"
#include "../Object/Character.h"

#include "../Render/RenderingManager.h"
#include "../System/TextManager.h"

void GameMain::Initialize()
{
	renderManager = new RenderingManager();
	txtManager = new TextManager();


	char frametxt[5];
	int frame = Frames::Get()->getFrame();
	_itoa_s(frame, frametxt, 5);


	char text[16];
	strcpy_s(text, "FPS : ");
	strcat_s(text, frametxt);
	txtManager->AddText(D3DXVECTOR2(10, 10), text);

	Camera::Get();
	UserInterface::Get();






	environment = new Environment();

	player = new Character();
	testcube = new TestCube();



	renderManager->Initianlize(txtManager, environment);
	renderManager->AddCharacter(player);
	renderManager->Test(testcube);



	environment->Initialize();

	player->Initialize(environment);
	player->SetFallowCamera();

}

void GameMain::Destroy()
{
	SAFE_DELETE(renderManager);
	
	
	UserInterface::Delete();

	SAFE_DELETE(testcube);
	SAFE_DELETE(environment);
	SAFE_DELETE(player);
}

void GameMain::Update()
{
	environment->Update();
	testcube->Update();
	player->Update();


	if (Keyboard::Get()->KeyUp(VK_SPACE)) {

	}

	ControlCamera();
	Camera::Get()->Update();

	renderManager->Update();




	char frametxt[5];
	int frame = (int)Frames::Get()->getFrame();
	_itoa_s(frame, frametxt, 10);

	

	char text[16];
	strcpy_s(text, "FPS : ");
	strcat_s(text, frametxt);
	txtManager->ChangeText(0, text);
}



void GameMain::Render()
{
	renderManager->Render();
	/*
	//WATER REFLECTION OLD ver.
	{

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



	}
	*/
}


void GameMain::ControlCamera()
{
	if (Camera::Get()->Fallowing()) {

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


