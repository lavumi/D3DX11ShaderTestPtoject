#include "../stdafx.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"

ModelSkeleton::ModelSkeleton()
{
	skinTransforms = NULL;
	boneAnimationTransforms = NULL;

	boneCount = 0;
}

ModelSkeleton::ModelSkeleton(const ModelSkeleton & otherSkeleton)
{
	boneCount = otherSkeleton.boneCount;

	skinTransforms = NULL;
	boneAnimationTransforms = NULL;
	/*if (otherSkeleton.skinTransforms != NULL)
	{
		skinTransforms = new D3DXMATRIX[boneCount];
		memcpy(skinTransforms, otherSkeleton.skinTransforms, sizeof(D3DXMATRIX) * boneCount);
	}
	if (otherSkeleton.boneAnimationTransforms != NULL)
	{
		boneAnimationTransforms = new D3DXMATRIX[boneCount];
		memcpy(boneAnimationTransforms, otherSkeleton.boneAnimationTransforms, sizeof(D3DXMATRIX) * boneCount);
	}
*/
	size_t skeletonBoneCount = (size_t)boneCount;
	skeletonBones.reserve(skeletonBoneCount);
	for (size_t i = 0; i < skeletonBoneCount; i++)
	{
		wstring tempName = otherSkeleton.skeletonBones[i].first;
		ModelSkeletonBone tempSkeletonBone = *(otherSkeleton.skeletonBones[i].second);

		skeletonBones.push_back({ tempName, new ModelSkeletonBone(tempSkeletonBone) });
	}
}

ModelSkeleton::~ModelSkeleton()
{
	for each(Pair temp in skeletonBones)
		SAFE_DELETE(temp.second);

	SAFE_DELETE_ARRAY(boneAnimationTransforms);
	SAFE_DELETE_ARRAY(skinTransforms);
}

void ModelSkeleton::AddSkeletonBone(ModelSkeletonBone * skeletonBone)
{
	skeletonBones.push_back(Pair(skeletonBone->GetName(), skeletonBone));
	
	boneCount++;
}

ModelSkeletonBone * ModelSkeleton::FindBone(wstring boneName)
{
	for each(Pair temp in skeletonBones)
	{
		if (boneName == temp.first)
			return temp.second;
	}
	
	return NULL;
}

int ModelSkeleton::FindBoneIndex(wstring boneName)
{
	int index = 0;
	for each(Pair temp in skeletonBones)
	{
		if (boneName == temp.first)
			return index;

		index++;
	}

	return -1;
}

D3DXMATRIX & ModelSkeleton::GetBoneAnimationTransform(int boneIdx)
{
	return boneAnimationTransforms[boneIdx];
}

ModelSkeletonBone * ModelSkeleton::GetSkeletonBone(int index)
{
	return skeletonBones[index].second;
}

void ModelSkeleton::AddAnimationFromOtherSkeleton(wstring srcAnimName, wstring dstAnimName, ModelSkeleton & srcSkeleton)
{
	vector<Pair> srcSkeletonBones = srcSkeleton.skeletonBones;

	vector<Pair>::const_iterator it = srcSkeletonBones.begin();
	for (; it != srcSkeletonBones.end(); ++it)
	{
		ModelSkeletonBone* srcBone = it->second;
		ModelSkeletonBone* dstBone = NULL;

		wstring srcBoneName = it->first;
		dstBone = FindBone(srcBoneName);

		if (dstBone != NULL)
		{
			ModelAnimationKeyFrames* srcAnimKeyFrames = srcBone->GetAnimationKeyFrames(srcAnimName);
			ModelAnimationKeyFrames* dstAnimKeyFrames = new ModelAnimationKeyFrames(*srcAnimKeyFrames);
			dstAnimKeyFrames->SetAnimationName(dstAnimName);
			dstBone->AddAnimationKeyFrames(dstAnimKeyFrames);
		}
	}
}

/****************************************************************************************************
 @brief
 ���� Animation�� Final Transform ������ ������Ʈ
****************************************************************************************************/
void ModelSkeleton::UpdateAnimation(ModelAnimationController * animationController)
{
	ModelAnimation* currentAnimation = animationController->GetCurrentAnimation();
	int keyFrame = animationController->GetCurrentKeyFrame();

	if (currentAnimation == NULL)
		return;

	RootType rootType = currentAnimation->GetRootType();
	BuildBoneTransforms(animationController, currentAnimation->GetName(), rootType);
}

void ModelSkeleton::Export(BinaryWriter * bw)
{
	bw->Write(boneCount);

	UINT skeletonBonesCount = skeletonBones.size();
	bw->Write(skeletonBonesCount);
	for (size_t i = 0; i < skeletonBonesCount; i++)
	{
		bw->Write(skeletonBones[i].first);
		(skeletonBones[i].second)->Export(bw);
	}

	bw->Write(skinTransforms, sizeof(D3DXMATRIX), boneCount);
	bw->Write(boneAnimationTransforms, sizeof(D3DXMATRIX), boneCount);
}

void ModelSkeleton::Import(BinaryReader * br)
{
	boneCount = br->Int();

	UINT skeletonBonesCount = br->UInt();
	skeletonBones.clear();
	skeletonBones.reserve(skeletonBonesCount);
	for (size_t i = 0; i < skeletonBonesCount; i++)
	{
		wstring name = br->WString();
		ModelSkeletonBone* modelSkeletonBone = new ModelSkeletonBone();
		modelSkeletonBone->Import(br);
		skeletonBones.push_back(make_pair(name, modelSkeletonBone));
	}

	SAFE_DELETE_ARRAY(skinTransforms);
	skinTransforms = new D3DXMATRIX[boneCount];
	br->Read(skinTransforms, sizeof(D3DXMATRIX), boneCount);

	SAFE_DELETE_ARRAY(boneAnimationTransforms);
	boneAnimationTransforms = new D3DXMATRIX[boneCount];
	br->Read(boneAnimationTransforms, sizeof(D3DXMATRIX), boneCount);
}

/****************************************************************************************************
 @brief
 �ش� Animation�� ���� Time������ KeyFrame�� ����(���� KeyFrame, ���� KeyFrmae, KeyFrame Factor)�� ���� 
 �� Bone�� ������ Tranform�� ����Ͽ�, �׷κ��� FinalTransform �� ����
****************************************************************************************************/
void ModelSkeleton::BuildBoneTransforms(ModelAnimationController* animationController, wstring animationName, RootType rootType)
{
	if (skinTransforms == NULL)
		skinTransforms = new D3DXMATRIX[boneCount];

	if (boneAnimationTransforms == NULL)	
		boneAnimationTransforms = new D3DXMATRIX[boneCount];


	int keyFrame = animationController->GetCurrentKeyFrame();
	int nextKeyFrame = animationController->GetNextKeyFrame();
	float keyframeFactor = animationController->GetKeyFrameFactor();
	bool useQuaterniokeyFrames = animationController->UseQuaternionKeyFrames(); // ���� ���� ����

	int index = 0;
	vector<Pair>::const_iterator it = skeletonBones.begin();
	for (; it != skeletonBones.end(); ++it)
	{
		ModelSkeletonBone* bone = it->second;
				
		D3DXMATRIX matInvBindPose = bone->GetInvBindPoseTransform(); // ��Ʈ->��
		D3DXMATRIX matBoneReference = bone->GetBoneReferenceTransform(); // ???
		D3DXMATRIX matAnimation; // Local Transform(�θ� ��->�ڽ� ��)�� ������ Transform
		D3DXMATRIX matParentAnimation; // �θ� ���� Local Transform(�θ��� �θ� ��->�θ� ��)�� ������ Transform

		D3DXMATRIX matFirstRootTransform;

		ModelAnimationKeyFrames* animatiokeyFrames = bone->GetAnimationKeyFrames(animationName);

		if (useQuaterniokeyFrames == true)
		{
			// ���� KeyFrame�� ���� KeyFrame�� Quaternion �� �ε�
			const ModelAnimationQuaternionKeyFrame& qKeyFrame1 = animatiokeyFrames->GetQuaternionKeyFrame(keyFrame);
			const ModelAnimationQuaternionKeyFrame& qKeyFrame2 = animatiokeyFrames->GetQuaternionKeyFrame(nextKeyFrame);
			const D3DXQUATERNION& q1 = qKeyFrame1.GetQuaternion();
			const D3DXQUATERNION& q2 = qKeyFrame2.GetQuaternion();
			const D3DXQUATERNION& minus_q2 = -q2;

			D3DXQUATERNION q; // ������ Quaternion

			if(D3DXQuaternionLengthSq(&(q1-q2)) > D3DXQuaternionLengthSq(&(q1+q2)))
				D3DXQuaternionSlerp(&q, &q1, &minus_q2, keyframeFactor); // Quaternion(ȸ����, ȸ����) ���� ���� ����
			else
				D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor); // Quaternion(ȸ����, ȸ����) ���� ���� ����

			D3DXMatrixRotationQuaternion(&matAnimation, &q); // Quaternion�� Trnsform Matrix�� ��ȯ

			D3DXVECTOR3 t1 = qKeyFrame1.GetTranslation();
			D3DXVECTOR3 t2 = qKeyFrame2.GetTranslation();
			
			
			D3DXVECTOR3 vTranslation; // ������ ���� ��ġ
			D3DXVec3Lerp(&vTranslation, &t1, &t2, keyframeFactor); // ��ġ�� ���� ����

			// ȸ�� ��, ȸ������ ���麸���� Matrix�� ���� ������ ��ġ�� �߰�
			matAnimation._41 = vTranslation.x;
			matAnimation._42 = vTranslation.y;
			matAnimation._43 = vTranslation.z;
		}
		else
		{
			// ���� ���� ������� �׳� KeyFrmae Trnsform�� ���
			matAnimation = animatiokeyFrames->GetKeyFrameTransform(keyFrame);
		}


		int parentBoneIndex = bone->GetParentBoneIndex();

		if (parentBoneIndex < 0)
		{
			// ��Ʈ�� ��� �θ� �� TrsnformAnimation�� Idenetity Transform���� ����
			D3DXMatrixIdentity(&matParentAnimation);

			// ��Ʈ ����� ù��° ������ ������ ����
			if (rootType == RootType::RootType_FirstFrame) matAnimation = animatiokeyFrames->GetKeyFrameTransform(0);
			// ��Ʈ ����� �������� ����
			else if (rootType == RootType::RootType_Fixed) matAnimation = bone->GetBindPoseTransform();
		}
		else
			matParentAnimation = boneAnimationTransforms[parentBoneIndex];

		// boneAnimationTransforms = (�ڽ� ��->�θ� ��) * (�θ� ��-�θ��� >�θ� ��) * ... (�θ��� ������...�θ� ��->��Ʈ ��) = (�ڽ� ��->��Ʈ��)
		boneAnimationTransforms[index] = matAnimation * matParentAnimation;

		// Mesh���� Vertex�� Root ������ ���� �ϹǷ�, matInvBindPose(Bone�� �ʱ� ��->��Ʈ)�� ���� ��
		// boneAnimationTransforms(��Ʈ ��-> �ڽ� ��) ���� Animation �� Bone�� ��ġ�� ����
		skinTransforms[index] = matInvBindPose *  boneAnimationTransforms[index];

		index++;
	}
}
