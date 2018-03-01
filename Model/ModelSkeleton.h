#pragma once
#include "../Utility/BinaryInputOutputHandler.h"
#include "ModelEnums.h"

/********************************************************************************
 @brief
 ��� Skeleton Bone�� ������ ����Ͽ� �����ϴ� Class

 �� Bone�� (Name, Index)�� �����Ͽ� �����

********************************************************************************/

class ModelSkeletonBone;
class ModelAnimationController;
class ModelSkeleton : public BinaryInputOutputHandler
{
public:
	ModelSkeleton();
	ModelSkeleton(const ModelSkeleton& otherSkeleton);
	~ModelSkeleton();

	void AddSkeletonBone(ModelSkeletonBone* skeletonBone);
	ModelSkeletonBone* FindBone(wstring boneName);
	int FindBoneIndex(wstring boneName);

	D3DXMATRIX& GetBoneAnimationTransform(int boneIdx);

	ModelSkeletonBone* GetSkeletonBone(int index);
	const vector<pair<wstring, ModelSkeletonBone *>>& GetSkeletonBones() { return skeletonBones; }
	
	D3DXMATRIX* GetSkinTransforms() { return skinTransforms; }

	int GetBoneCount() { return boneCount; }

	void AddAnimationFromOtherSkeleton(wstring srcAnimName, wstring dstAnimName, ModelSkeleton& srcSkeleton);

	void UpdateAnimation(ModelAnimationController* animationController); ///�� �����Ӹ��� ��� ���� Transform ������ ����

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:
	void BuildBoneTransforms(ModelAnimationController* animationController, wstring animationName, RootType rootType);

	int boneCount;

	typedef pair<wstring, ModelSkeletonBone *> Pair;
	vector<Pair> skeletonBones;

	D3DXMATRIX* skinTransforms; /// ���������� Vertex�� ����� Transform Matrix �迭
	D3DXMATRIX* boneAnimationTransforms; /// �� Bone�� Transform Matrix �迭
	 
};
