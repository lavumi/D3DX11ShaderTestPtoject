#pragma once
#include "../Utility/BinaryInputOutputHandler.h"

/********************************************************************************
 @brief
 Skeleton Bone�� ������ �����ϴ� Class

 Bone�� Name, Index�� ����
 Bone�� Animation �� ��� ModelAnimationKeyFrames�� ����

 BindPoseTransform 
	=> Root Space(Binding Space or Local Space) ��ǥ�迡�� Bone�� Tranfrom

 BoneReferenceTransform =>
	=> Bone Space ��ǥ�� ������ Cluster�� Transfrom
********************************************************************************/
class ModelAnimationKeyFrames;
class ModelSkeletonBone : public BinaryInputOutputHandler
{
public:
	ModelSkeletonBone();
	ModelSkeletonBone(wstring name, int parentBoneIndex);
	ModelSkeletonBone(const ModelSkeletonBone& otherSkeletonBone);
	~ModelSkeletonBone();

	void AddAnimationKeyFrames(ModelAnimationKeyFrames* keyFrames);
	
	void SetBindPoseTransform(const D3DXMATRIX& matBindPoseTransform);
	void SetBoneReferenceTransform(const D3DXMATRIX& matBoneReferenceTransform);

	ModelAnimationKeyFrames* GetAnimationKeyFrames(wstring animationName);

	const D3DXMATRIX& GetBindPoseTransform() 
	{ 
		return matBindPoseTransform; 
	}

	const D3DXMATRIX& GetInvBindPoseTransform() 
	{ 
		return matInvBindPoseTransform; 
	}

	const D3DXMATRIX& GetBoneReferenceTransform() 
	{ 
		return matBoneReferenceTransform; 
	}

	const D3DXMATRIX& GetInvBoneReferenceTransform() 
	{ 
		return matInvBoneReferenceTransform; 
	}

	wstring GetName() { return name; }
	int GetParentBoneIndex() { return parentBoneIndex; }

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:

	wstring name;

	D3DXMATRIX matBindPoseTransform;
	D3DXMATRIX matInvBindPoseTransform;
	D3DXMATRIX matBoneReferenceTransform;
	D3DXMATRIX matInvBoneReferenceTransform;

	int parentBoneIndex;

	map<wstring, ModelAnimationKeyFrames *> animationKeyFrames;
};