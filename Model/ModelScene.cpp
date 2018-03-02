#include "../stdafx.h"
#include "ModelScene.h"
#include "Model.h"
#include "ModelMaterial.h"
#include "ModelBuffer.h"
#include "ModelUtility.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelBoneWeights.h"

wstring ModelScene::TexturePath = L"";

ModelScene::ModelScene()
	: manager(NULL), scene(NULL), converter(NULL), importer(NULL)	, skeleton(NULL)
{

	//manager = FbxManager::Create();
	// TexturePath�� ����� ����
	WCHAR folder[255];
	GetCurrentDirectory(255, folder);
	
	TexturePath = folder;
	TexturePath += L"\\Contents\\Models\\Textures\\";
	
	modelBuffer = new ModelBuffer();
	animationController = new ModelAnimationController();

	D3DXMatrixScaling(&scaleBias, 0.01f, 0.01f, 0.01f);
}

ModelScene::~ModelScene()
{
	for each(ModelMaterial* temp in materials)
		SAFE_DELETE(temp);
	
	for each(Model* temp in models)
		SAFE_DELETE(temp);
	
	SAFE_DELETE(modelBuffer);
	
	if (scene != NULL)scene->Destroy();
	if (manager != NULL)manager->Destroy();
}

void ModelScene::SetAni(int index)
{
	animationController->SetCurrentAnimation(index);
}

/****************************************************************************************************
@brief
Fbx SDK �� �޸� ������ ���� Manager ���� 
Fbx File�� Impoter�� ���Ͽ� import��Ų �� initialize
File�� Scene����(Material, Skeleton, Mesh, Animation) �ε�
****************************************************************************************************/
void ModelScene::LoadFbx(wstring filePath)
{
	this->filePath = filePath;

	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);

	string tempFile = String::WStringToString(filePath);

	//SDK�� ������ ���´�.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);
	
	//������ ����
	importer = FbxImporter::Create(manager, "");
	bool status = importer->Initialize(tempFile.c_str(), -1, NULL);
	assert(status == true);
	
	status = importer->Import(scene);
	assert(status == true);

	converter = new FbxGeometryConverter(manager);
	{
		ProcessScene();
	}
	SAFE_DELETE(converter);

	importer->Destroy();
}

void ModelScene::LoadAniFbx(wstring filePath, wstring aniName)
{
	this->filePath = filePath;

	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);

	string tempFile = String::WStringToString(filePath);

	//SDK�� ������ ���´�.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

	//������ ����
	importer = FbxImporter::Create(manager, "");
	bool status = importer->Initialize(tempFile.c_str(), -1, NULL);
	assert(status == true);

	status = importer->Import(scene);
	assert(status == true);

	converter = new FbxGeometryConverter(manager);
	{
		ProcessAnimations(aniName);
	}
	SAFE_DELETE(converter);

	importer->Destroy();
}

void ModelScene::SaveBinary(wstring filePath, wstring fileName)
{
	wstring totalFileName = filePath + fileName;

	ExportMaterials(totalFileName + L"_material.bin");
	ExportSkeleton(totalFileName + L"_skeleton.bin");
	ExportModels(totalFileName + L"_models.bin");
	ExportAnimations(totalFileName + L"_animations.bin");
}

void ModelScene::LoadBinary(wstring filePath, wstring fileName)
{
	wstring totalFileName = filePath + fileName;

	ImportMaterials(totalFileName + L"_material.bin");
	ImportSkeleton(totalFileName + L"_skeleton.bin");
	ImportModels(totalFileName + L"_models.bin");
	ImportAnimations(totalFileName + L"_animations.bin");
}

ModelAnimation * ModelScene::GetAnimation(wstring animName)
{
	map<wstring, ModelAnimation*>::iterator iter;
	iter = animations.find(animName);
	if (iter != animations.end())
		return iter->second;

	return NULL;
}

void ModelScene::Update(D3DXMATRIX* worldPos)
{
	bool isAnimated = false;

	//if (animationController != NULL)
	{
		isAnimated = animationController->GetAnimationCount() > 0;

		if (isAnimated == true)
		{
			animationController->Update();

			// Skeleton ���� ����
			if (skeleton != NULL)
			{
				skeleton->UpdateAnimation(animationController);
			}

			// Skeleton ������ ���� Mesh���� ����
			for each(Model* model in models)
				model->UpdateAnimation(animationController);
		}
	}

	if (skeleton != NULL)
	{
		D3DXMATRIX* bonesArray = skeleton->GetSkinTransforms();
		if (bonesArray != NULL)
		{
			UINT boneCount = skeleton->GetBoneCount();
			boneCount = boneCount < ModelBoneWeights::MaxBonesPerMax ? boneCount : ModelBoneWeights::MaxBonesPerMax;

			modelBuffer->SetBoneArray(bonesArray, boneCount);
		}
	}

	modelBuffer->SetSkinning(isAnimated);
	modelBuffer->Update();

	for each(Model* model in models)
		model->Update(isAnimated, worldPos);
}

void ModelScene::Render(FBXModelShader* shader)
{
	modelBuffer->SetVSBuffer(2);

	for each(Model* model in models)
		model->Render(shader);
}



/****************************************************************************************************
@brief
Material, Skelton, Mesh, Animation ������ Top->Down ����� ����� ��ȸ�� �о����
****************************************************************************************************/
void ModelScene::ProcessScene()
{
	ProcessMaterial();
	ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eSkeleton);
	ProcessNode(scene->GetRootNode(), FbxNodeAttribute::eMesh);
	ProcessAnimations();
	
}

/****************************************************************************************************
@brief
Scene���� ��� Material ������ �о�, ������ Material ������ ModelMaterial Class�� ����
****************************************************************************************************/
void ModelScene::ProcessMaterial()
{
	for (int i = 0; i < scene->GetMaterialCount(); i++)
	{
		FbxSurfaceMaterial* temp = scene->GetMaterial(i);
		
		bool isExist = false;
		for (size_t i = 0; i < fbxMaterials.size(); i++)
		{
			if (*temp == *fbxMaterials[i]) isExist = true;
		}

		if (isExist == false)
		{
			ModelMaterial* material = new ModelMaterial(i, temp);

			materials.push_back(material);
			fbxMaterials.push_back(temp);
		}
	}
}

/****************************************************************************************************
@brief
Node�� ������ �ε�
�� Node�� �Ӽ�(Skeelton, Mesh ���� ������ �Ӽ��� ���ʿ��ϹǷ� �ε����� ����)���� ��ȸ
�Լ� ������ ��������� ��� �ڽ� Node�� ���Ͽ� ����

@param FbxNode* node ���� ����� Root Node�� �ѹ� ����, ���� ��������� �ڽ� ��带 ����
@param FbxNodeAttribute::EType attribute ����� �Ӽ�(Skeleton, Mesh...)
****************************************************************************************************/
void ModelScene::ProcessNode(FbxNode * node, FbxNodeAttribute::EType attribute)
{	
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == attribute)
		{
			switch (nodeAttribute->GetAttributeType())
			{
				case FbxNodeAttribute::eSkeleton:
				{
					ProcessSkeleton(node);
				}	
				break;

				case FbxNodeAttribute::eMesh:
				{
					ProcessMesh(node);
				}	
				break;

				case FbxNodeAttribute::eMarker:
				case FbxNodeAttribute::eNurbs:
				case FbxNodeAttribute::ePatch:
				case FbxNodeAttribute::eCamera:
				case FbxNodeAttribute::eLight:
				case FbxNodeAttribute::eNull:
					break;
			}//switch
		}//if
	}//if(nodeAttribute)

	for (int i = 0; i < node->GetChildCount(); i++)
		ProcessNode(node->GetChild(i), attribute);
}

/****************************************************************************************************
@brief
Bone�� ���������� ������ Skeleton ������ �ε��Ͽ� ModelSkeleton Class�� ����
****************************************************************************************************/
void ModelScene::ProcessSkeleton(FbxNode * node)
{
	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == NULL)
		return;

	if (skeleton == NULL)
		skeleton = new ModelSkeleton();


	int parentBoneIndex = -1;
	FbxNode* parentNode = node->GetParent();
	if (parentNode != NULL)
	{
		wstring boneName = String::StringToWString(parentNode->GetName());
		parentBoneIndex = skeleton->FindBoneIndex(boneName);
	}

	// ModelSkeleton Class�� �� Bone�� ������ ModelSkeletonBone Class�� ����
	wstring boneName = String::StringToWString(node->GetName());
	ModelSkeletonBone* skeletonBone = new ModelSkeletonBone(boneName, parentBoneIndex);
	skeleton->AddSkeletonBone(skeletonBone);
}

/****************************************************************************************************
@brief
Mesh ������ �ε��Ͽ� Model Class�� ����
�� Mesh ����(Global Transform, Geomatric Offset)�� Model Class��
�� Vertex�� ����(Pos, Normal, Uv, BoneWeight)�� Material ���� ������ Model Class���� Part Class�� ����
****************************************************************************************************/
void ModelScene::ProcessMesh(FbxNode * node)
{
	// Mesh ������ �о�� ��, �⺻ ���� ������ �ﰢ������ �б�
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	converter->Triangulate(attribute, true, true);

	FbxMesh* mesh = node->GetMesh();
	assert(mesh != NULL);

	int vertexCount = mesh->GetControlPointsCount();
	assert(vertexCount > 0);

	// Vertex�� ����ŭ ModelBoneWeights Class�� �Ҵ�
	vector<ModelBoneWeights> boneWeights(vertexCount, ModelBoneWeights());
	ProcessBoneWeights(mesh, boneWeights);

	// BoneWeights�� ������ ����ִ� ModelBuffer�� Model -> Part���� ����
	Model* model = new Model(this, String::StringToWString(node->GetName()));

	// Control Point�� Vertex�� �������� ����, (i.e. ����ü�� ��� 4��)
	FbxVector4* controlPoints = mesh->GetControlPoints();

	// Mesh�� Polygon(�ﰢ��)����ŭ �ݺ�
	for (int i = 0; i < mesh->GetPolygonCount(); i++)
	{
		ModelMaterial* material = NULL;

		// Polygon�� �� ��
		for (int vi = 0; vi < 3; vi++)
		{
			// i��° Polygon�� vi(0~2)��° Vertex�� Vertex Index�� ����
			int vertexIndex = mesh->GetPolygonVertex(i, vi);
			if (vertexIndex < 0 || vertexIndex >= vertexCount)
				continue;

			if (material == NULL)
				material = LinkMaterialWithPolygon(mesh, 0, i, 0, vertexIndex);

			FbxVector4 fbxPos = controlPoints[vertexIndex];
			FbxVector4 fbxNormal;
			mesh->GetPolygonVertexNormal(i, vi, fbxNormal);
			fbxNormal.Normalize();

			D3DXVECTOR3 vecPos = ModelUtility::ToVector3(fbxPos);
			D3DXVECTOR3 vecNormal = ModelUtility::ToVector3(fbxNormal);

			D3DXVECTOR2 vecUv = GetUV(mesh, 0, i, vi, vertexIndex);

			// Model �� Vertex������ �߰�(Model���� Material���� ������ �� Part�� �߰�)
			model->AddVertex(material, vecPos, vecNormal, vecUv, boneWeights[vertexIndex]);
		}//for(vi)
	}//for(i)

	 // Mesh�� Global TransForm(��Ʈ->��) �� AbsoluteTransform�� ����
	D3DXMATRIX temp = GetAbsoluteTransformFromCurrentTake(node, FbxTime(0));
	model->SetAnimationTransform(temp);
	// Mesh�� GeobetrixOffset�� �߰�(��κ��� ��� Identity Matrix)
	model->SetGeometricOffset(scaleBias * GetGeometricOffset(node));

	model->CreateData();
	model->CreateBuffer();

	models.push_back(model);
}

/****************************************************************************************************
@brief
Animation ������ �ε��Ͽ� ModelAnimationController Class �� ����
****************************************************************************************************/
void ModelScene::ProcessAnimations(wstring aniName)
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == NULL) return;

	// Fbx File ���� ������ Frame Rate(�ʴ� ������ Frame��)�� �ε�
	float frameRate = (float)FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	// animationArray�� Animation �̸��� �ε��Ͽ� ����
	FbxArray<FbxString *> animationArray;
	FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	if (document != NULL)
		document->FillAnimStackNameArray(animationArray);

	UINT animationCount = importer->GetAnimStackCount();
	// importer�� ���� FBX�� ����� ��� Animation ������ �ε�
	for (UINT i = 0; i < animationCount; i++)
	{
		FbxTakeInfo* animationInfo = importer->GetTakeInfo(i);
		wstring animationName;
		if (aniName == L"")
			animationName = String::StringToWString(animationInfo->mImportName.Buffer());
		else
			animationName = aniName;

		// FbxTimeSpan: ���� �ð��� �����ϴ� Class
		FbxTimeSpan span = animationInfo->mLocalTimeSpan;
	
		// �� ������ ����, �� �ð��� �ε��Ͽ� �Ҵ�
		double startTime = span.GetStart().GetSecondDouble();
		double endTime = span.GetStop().GetSecondDouble();

		if (startTime < endTime)
		{
			// KeyFrames�� �� = Animation ���� �ð�(��) * �ʴ� Frmae ��
			int keyFrameCount = (int)((endTime - startTime) * (double)frameRate);

			// AnimationController�� ModelAnimation Class�� ������ ����
			ModelAnimation* animation = new ModelAnimation(animationName, keyFrameCount, frameRate);
			AddAnimation(animation);

			animationController->AddAnimation(animation);
			
			// �⺻ Animation ������ �̿��Ͽ�, Skeleton, Mesh�� Animation ���� ���
			ProcessAnimation(rootNode, animationName, frameRate, (float)startTime, (float)endTime);
		}
	}
	animationArray.Clear();

	// �������� �߰��� �ִϸ��̼��� ���� �ִϸ��̼����� ���� �� �÷���
	animationController->SetCurrentAnimation(animationCount-1);
	animationController->Play();

	//TODO: defualt ���� ����
}


/****************************************************************************************************
@brief
�ش� Animation�� �� Bone�� Mesh�� ��� KeyFrame ������ ����Ͽ� animationKeyFrames�� �Ҵ��Ͽ�
�� Bone�� Mesh�� �ش��ϴ� ModelSkeletonBone, Model Class�� ����
****************************************************************************************************/
void ModelScene::ProcessAnimation(FbxNode * node, wstring animationName, float frameRate, float start, float stop)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			if (skeleton != NULL)
			{
				wstring boneName = String::StringToWString(node->GetName());
				ModelSkeletonBone* bone = skeleton->FindBone(boneName);

				if (bone != NULL)
				{
					ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(animationName);

					double time = 0;
					// Animation�� ���� �ð� �ȿ���, Frame Time ���� KeyFrame�� �߰�
					// �� KeyFrame Class�� �ش� Bone�� �ش� �ð������� Local Transform Matrix(�θ� ��-> �ڽ� ��)�� �ε��Ͽ� ����

					while (time <= (double)stop)
					{
						FbxTime animationTime;
						animationTime.SetSecondDouble(time);
						
						//// Bone �ڽ��� Global Transform(�ڽ� �� -> ��Ʈ)�� ����
						//D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, animationTime);
						//// �θ� Bone �� Global Transform(�θ� �� -> ��Ʈ)�� ����
						//D3DXMATRIX matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node->GetParent(), animationTime);
						//
						//// �θ� Bone �� Global Transform(�θ� �� -> ��Ʈ)�� Inverse Transform(��Ʈ -> �θ� ��)�� ����
						//D3DXMATRIX matInvParentAbsoluteTransform;
						//D3DXMatrixInverse(&matInvParentAbsoluteTransform, NULL, &matParentAbsoluteTransform);
						//
						//// matTransform = (�ڽ� �� -> ��Ʈ) * (��Ʈ -> �θ� ��) == (�ڽ� �� -> �θ� ��)
						//// �θ� Bone ��ǥ�迡���� Bone�� Transform Matrix
						//D3DXMATRIX localTransform = matAbsoluteTransform * matInvParentAbsoluteTransform;
						

						// Local Transform = �θ� Bone�� Global Trasforn �� Inverse Transform * �ڽ� Bone�� Global Transform
						D3DXMATRIX localTransform = GetLocalTransformFromCurrentTake(node, animationTime);
						animationKeyFrames->AddKeyFrame(localTransform);

						time += 1.0f / frameRate;
					}

					bone->AddAnimationKeyFrames(animationKeyFrames);
				}//if(bone)
			}//if(skeleton)
		}
		else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			Model* model = NULL;
			for each(Model* tempModel in models)
			{
				wstring modelName = String::StringToWString(node->GetName());
				if (modelName == tempModel->GetName())
				{
					model = tempModel;

					break;
				}
			}

			if (model != NULL)
			{
				ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(animationName);

				double time = 0;
				while (time <= (double)stop)
				{
					FbxTime animationTime;
					animationTime.SetSecondDouble(time);

					// Mesh�� ��� GlobalTransform Matrix(��Ʈ->�Ž�) ������ ����
					D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, animationTime);
					animationKeyFrames->AddKeyFrame(matAbsoluteTransform);

					time += 1.0f / frameRate;
				}

				model->AddAnimationKeyFrames(animationKeyFrames);
			}
		}//if(nodeAttribute->GetAttributeType())
	}//if(nodeAttribute)

	// �ڽ� node(Bone)�� ��������� ��ȸ
	for (int i = 0; i < node->GetChildCount(); ++i)
		ProcessAnimation(node->GetChild(i), animationName, frameRate, start, stop);
}

/****************************************************************************************************
@brief
Mesh�� Skin ������ �ε��Ͽ�, �ش� Mesh�� �� Bone�� ���Ͽ� ��ŭ�� ������ �޴���(Weight)�� �ε�
****************************************************************************************************/
void ModelScene::ProcessBoneWeights(FbxMesh * mesh, vector<ModelBoneWeights>& meshBoneWeights)
{
	if (skeleton == NULL)
		return;

	// Deformer�� �ַ� Scene���� �ϳ��� ����
	for (int i = 0; i < mesh->GetDeformerCount(); ++i)
	{
		FbxDeformer* deformer = mesh->GetDeformer(i);

		if (deformer == NULL)
			continue;

		// Deformer�� Ÿ���� Skin�� ���, �� Skinned Mesh�� ���
		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
		{
			
			FbxSkin* skin = (FbxSkin *)deformer;

			if (skin == NULL)
				continue;

			ProcessBoneWeights(skin, meshBoneWeights);
		}
	}
}

/****************************************************************************************************
 @brief
 Skin ������ �ε��Ͽ�, Mesh�� �ش� Vertex�� �� Bone�� ���Ͽ� ��ŭ�� ������ �޴���(Weight)��
 ModelBoneWeights Class�� Vector�� ����(ũ��: VertexCount, Index: VertexIndex)
****************************************************************************************************/
void ModelScene::ProcessBoneWeights(FbxSkin * skin, vector<ModelBoneWeights>& meshBoneWeights)
{
	FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;

	int clusterCount = skin->GetClusterCount();
	vector<ModelBoneWeights> skinBoneWeights(meshBoneWeights.size(), ModelBoneWeights());
	for (int i = 0; i < clusterCount; i++)
	{
		FbxCluster* cluster = skin->GetCluster(i);
		if (cluster == NULL)
			continue;

		linkMode = cluster->GetLinkMode();
		FbxNode* pLinkNode = cluster->GetLink();

		if (pLinkNode == NULL)
			continue;

		wstring lineNodeName = String::StringToWString(pLinkNode->GetName());
		int nBoneIndex = skeleton->FindBoneIndex(lineNodeName);
		if (nBoneIndex < 0)
			continue;

		ModelSkeletonBone* skeletonBone = skeleton->GetSkeletonBone(nBoneIndex);

		FbxAMatrix matClusterTransformMatrix;
		FbxAMatrix matClusterLinkTransformMatrix;
		cluster->GetTransformMatrix(matClusterTransformMatrix); // �Ž��� Transform Matrix
		cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix); // ���� Transform Matrix

																		//TODO: üũ
		skeletonBone->SetBindPoseTransform(ModelUtility::ToMatrix(matClusterLinkTransformMatrix)); // ��->��Ʈ
		skeletonBone->SetBoneReferenceTransform(ModelUtility::ToMatrix(matClusterTransformMatrix)); // �Ž�->��Ʈ

		int* indices = cluster->GetControlPointIndices(); //�ش� Bone�� ������ �޴� ��� Vertex Index���� �迭
		double* weights = cluster->GetControlPointWeights(); // �� Vertex Index�� ���� Weight���� �迭

															 // indices[j]�� Index�� ���� Vertex�� ������ �ִ� Bone�� Idx��, �� ��ġ�� Weights��
															 // ModelBoneWeights Class�� �������� �߰�
		for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
			skinBoneWeights[indices[j]].AddBoneWeight(nBoneIndex, (float)weights[j]);
	}

	// Weight�� ���
	switch (linkMode)
	{
		case FbxCluster::eNormalize: // ��� Weight�� ���� 1 �� ����ȭ
			{
				for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
					skinBoneWeights[i].Normalize();
			}
			break;

		case FbxCluster::eAdditive: // �״�� �ջ�
			break;

		case FbxCluster::eTotalOne: // ��� Weight�� ���� 1 �̾�� ��(����ȭ ���� ����)
			break;
	}

	for (size_t i = 0; i < meshBoneWeights.size(); i++)
		meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
}

/****************************************************************************************************
 @biref
 �ش� Vertex Index�� Material ������ ModelMaterial Class �������� ��ȯ
****************************************************************************************************/
ModelMaterial * ModelScene::LinkMaterialWithPolygon(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	if (layerIndex < 0 || layerIndex > mesh->GetLayerCount())
		return NULL;

	
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return NULL;

	FbxLayerElementMaterial* fbxMaterial = mesh->GetLayer(layerIndex)->GetMaterials();
	if (fbxMaterial == NULL) return NULL;

	int mappingIndex = GetMappingIndex
	(
		fbxMaterial->GetMappingMode()
		, polygonIndex
		, 0
		, vertexIndex
	);
	if (mappingIndex < 0) return NULL;

	// Material�� Vertex Index�� ����� ��带 �ε��� ��,
	// �̿� �µ��� mappingIndex�� ��� ��, GetMaterial�� �ش� Vertex�� Material �� ��ȯ
	FbxLayerElement::EReferenceMode refMode = fbxMaterial->GetReferenceMode();
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			return GetMaterial(node->GetMaterial(mappingIndex));
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = fbxMaterial->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
				return GetMaterial(node->GetMaterial(tempIndex));
		}//if(mappingIndex)
	}//if(refMode)

	return NULL;
}

/****************************************************************************************************
@biref
MappingMode�� ���� Index�� ����Ͽ� ��ȯ
****************************************************************************************************/
int ModelScene::GetMappingIndex(FbxLayerElement::EMappingMode mode, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	switch (mode)
	{
		case FbxLayerElement::eAllSame: 
			return 0;
		case FbxLayerElement::eByControlPoint: 
			return vertexIndex;
		case FbxLayerElement::eByPolygonVertex: 
			return polygonIndex * 3 + polygonVertexIndex;
		case FbxLayerElement::eByPolygon:
			return polygonIndex;
	}
	
	return -1;
}

/****************************************************************************************************
@biref
�ش� Vertex Index�� UV���� �ε��Ͽ� ��ȯ
****************************************************************************************************/
D3DXVECTOR2 ModelScene::GetUV(FbxMesh * mesh, int layerIndex, int polygonIndex, int polygonVertexIndex, int vertexIndex)
{
	int layerCount = mesh->GetLayerCount();
	if (layerIndex >= layerCount) return D3DXVECTOR2();

	FbxLayer* layer = mesh->GetLayer(layerIndex);
	if(layer == NULL) return D3DXVECTOR2();

	// Diffuse Texture�� Uv�ε�
	FbxLayerElementUV* uv = layer->GetUVs(FbxLayerElement::eTextureDiffuse);
	if(uv == NULL) return D3DXVECTOR2();


	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uv->GetDirectArray();
	const FbxLayerElementArrayTemplate<int>& uvIndexArray = uv->GetIndexArray();

	int mappingIndex = 0;
	if (mappingMode == FbxLayerElement::eByControlPoint)
	{
		mappingIndex = vertexIndex;

		if (refMode == FbxLayerElement::eDirect)
		{
			if(mappingIndex < uvArray.GetCount())
				return ModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}
		else if (refMode == FbxLayerElement::eIndexToDirect)
		{
			if (mappingIndex < uvArray.GetCount())
			{
				int tempIndex = uvIndexArray.GetAt(mappingIndex);

				if(tempIndex < uvIndexArray.GetCount())
					return ModelUtility::ToUv(uvArray.GetAt(tempIndex));
			}
		}//if(refMode)
	}
	else if (mappingMode == FbxLayerElement::eByPolygonVertex)
	{
		mappingIndex = mesh->GetTextureUVIndex
		(
			polygonIndex
			, polygonVertexIndex
			, FbxLayerElement::eTextureDiffuse
		);

		switch (refMode)
		{
			case FbxLayerElement::eDirect:
			case FbxLayerElement::eIndexToDirect:
				if (mappingIndex < uvArray.GetCount())
					return ModelUtility::ToUv(uvArray.GetAt(mappingIndex));
		}//switch(refMode)
	}//if(mappingMode)

	return D3DXVECTOR2();
}

/****************************************************************************************************
 @biref
 FbxSurfaceMaterial ���� ����� ���� ModelMaterial Class�� ��ȯ
****************************************************************************************************/
ModelMaterial * ModelScene::GetMaterial(FbxSurfaceMaterial * fbxMaterial)
{
	for (size_t i = 0; i < fbxMaterials.size(); i++)
	{
		if (fbxMaterials[i] == fbxMaterial)
			return materials[i];
	}

	return NULL;
}

/****************************************************************************************************
 @biref
 Node�� Offset Transform Matrix�� ��ȯ
****************************************************************************************************/
D3DXMATRIX ModelScene::GetGeometricOffset(FbxNode * node)
{
	if (node == NULL)
	{
		D3DXMATRIX matrix;
		D3DXMatrixIdentity(&matrix);

		return matrix;
	}

	FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxAMatrix offset = FbxAMatrix(T, R, S);

	return ModelUtility::ToMatrix(offset);
}

/****************************************************************************************************
 @biref
 ������ time���� node�� Global Transform Matrix(���->��Ʈ)�� �ε��Ͽ� ��ȯ
****************************************************************************************************/
D3DXMATRIX ModelScene::GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time)
{
	if (node == NULL)
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		return mat;
	}
	
	FbxAMatrix matrix = node->EvaluateGlobalTransform(time);
	
	return ModelUtility::ToMatrix(matrix);
}

/****************************************************************************************************
 @biref
 ������ time���� node�� Local Transform Matrix(�ڽ�->�θ�)�� �ε��Ͽ� ��ȯ
****************************************************************************************************/
D3DXMATRIX ModelScene::GetLocalTransformFromCurrentTake(FbxNode * node, FbxTime time)
{
	if( node == NULL )
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		return mat;
	}

	FbxAMatrix matrix = node->EvaluateLocalTransform(time);

	return ModelUtility::ToMatrix(matrix);
}

void ModelScene::AddAnimation(ModelAnimation * animation)
{
	wstring animName = animation->GetName();
	if (animations.count(animName) > 0)
		return;
	else
		animations.insert({ animName, animation });
}


// Binary Export ����
void ModelScene::ExportMaterials(wstring fileName)
{
	BinaryWriter bMaterials = BinaryWriter();
	bMaterials.Open(fileName);
	{
		bMaterials.Write(materials);
	}
	bMaterials.Close();
}

void ModelScene::ExportSkeleton(wstring fileName)
{
	BinaryWriter bSkeleton = BinaryWriter();
	bSkeleton.Open(fileName);
	{
		bSkeleton.Write(skeleton);
	}
	bSkeleton.Close();
}

void ModelScene::ExportModels(wstring fileName)
{
	BinaryWriter bModels = BinaryWriter();
	bModels.Open(fileName);
	{
		bModels.Write(models);
	}
	bModels.Close();
}

void ModelScene::ExportAnimations(wstring fileName)
{
	BinaryWriter bAnimations = BinaryWriter();
	bAnimations.Open(fileName);
	{
		bAnimations.Write(animations);
	}
	bAnimations.Close();
}

void ModelScene::ImportMaterials(wstring fileName)
{
	BinaryReader bMaterials = BinaryReader();
	bMaterials.Open(fileName);
	{
		UINT materialsCount = bMaterials.UInt();
		materials.clear();
		materials.reserve(materialsCount);
		for (size_t i = 0; i < materialsCount; i++)
		{
			ModelMaterial* material = new ModelMaterial();
			material->Import(&bMaterials);
			materials.push_back(material);
		}
	}
	bMaterials.Close();
}

void ModelScene::ImportSkeleton(wstring fileName)
{
	BinaryReader bSkeleton = BinaryReader();
	bSkeleton.Open(fileName);
	{
		skeleton = new ModelSkeleton();
		skeleton->Import(&bSkeleton);
	}
	bSkeleton.Close();
}

void ModelScene::ImportModels(wstring fileName)
{
	BinaryReader bModels = BinaryReader();
	bModels.Open(fileName);
	{
		UINT modelsCount = bModels.UInt();
		models.clear();
		models.reserve(modelsCount);
		for (size_t i = 0; i < modelsCount; i++)
		{
			Model* model = new Model(this);
			model->Import(&bModels);
			models.push_back(model);
		}
	}
	bModels.Close();
}

void ModelScene::ImportAnimations(wstring fileName)
{
	BinaryReader bAnimations = BinaryReader();
	bAnimations.Open(fileName);
	{
		animations.clear();
		UINT animationKeyFramesCount = bAnimations.UInt();
		
		if (animationKeyFramesCount > 0 && animationController == NULL)
			animationController = new ModelAnimationController();

		for (size_t i = 0; i < animationKeyFramesCount; i++)
		{
			wstring name = bAnimations.WString();
			ModelAnimation* modelAnimation = new ModelAnimation();
			modelAnimation->Import(&bAnimations);
			animations.insert(make_pair(name, modelAnimation));

			animationController->AddAnimation(modelAnimation);
		}

		animationController->SetCurrentAnimation(animationKeyFramesCount-1);
		animationController->Play();
	}
	bAnimations.Close();
}
