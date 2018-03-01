#pragma once
#include "../Utility/BinaryInputOutputHandler.h"
/********************************************************************************
* @brief
* KeyFrame�� ����� Quaternion ���� Class
*
* Quaternion(ȸ����, ȸ����), Translation(ȸ���� ��ġ)�� ����
********************************************************************************/
class ModelAnimationQuaternionKeyFrame : public BinaryInputOutputHandler
{
public:
	ModelAnimationQuaternionKeyFrame() {};
	ModelAnimationQuaternionKeyFrame(const D3DXMATRIX& transform)
	{
		// Trnasform Matrix�� Quaternion���� ��ȯ
		D3DXQuaternionRotationMatrix(&q, &transform);
		// Translation������ ���� ����
		translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
	}

	const D3DXQUATERNION& GetQuaternion() const { return q; }
	const D3DXVECTOR3& GetTranslation() const { return translation; }

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:
	D3DXQUATERNION q; /// ȸ����(Vector)�� ȸ������ ���� ������ ���� Quaternion
	D3DXVECTOR3 translation; /// ȸ������ ��ġ(Position) ����
};

/********************************************************************************
 @brief
 AnimationName�� �ش��ϴ� Animation�� ��� KeyFrmae ������ �����ϴ� Class
 
 (KeyFrmae ���� => Transform Matrix�� Quaternion�� Pair�� ����)
********************************************************************************/
class ModelAnimationKeyFrames : public BinaryInputOutputHandler
{
public:
	ModelAnimationKeyFrames() {};
	ModelAnimationKeyFrames(wstring animationName);
	ModelAnimationKeyFrames(ModelAnimationKeyFrames& other);
	~ModelAnimationKeyFrames();
	
	void AddKeyFrame(const D3DXMATRIX& transform);

	wstring GetAnimationName();
	void SetAnimationName(wstring animationName);

	const D3DXMATRIX& GetKeyFrameTransform(int keyFrame) const;
	const ModelAnimationQuaternionKeyFrame& GetQuaternionKeyFrame(int keyFrame) const;

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:
	wstring animationName;

	typedef pair<D3DXMATRIX, ModelAnimationQuaternionKeyFrame> Pair;
	vector<Pair> keyFrames; ///��� KeyFrame�� ������ �����ϴ� ����
};