#pragma once
#include "../Utility/BinaryInputOutputHandler.h"
#include "ModelEnums.h"

/********************************************************************************
 @breif
 Animation ������ �����ϴ� Class

 Animation Name�� KeyFrameRate(�ʴ� ������ ������ ��)�� ���� ��, �� ������ ����
********************************************************************************/


class ModelAnimation : public BinaryInputOutputHandler
{
public:
	ModelAnimation();
	ModelAnimation(wstring name, int keyFrameCount, float defaultFrameRate);
	~ModelAnimation();

	void SetName(wstring name) { this->name = name; }

	bool GetIsLoop() const { return isLoop; }
	void SetIsLoop(bool isLoop) { this->isLoop = isLoop; }

	bool GetIsEnd() const { return isEnd; }
	void SetIsEnd(bool isEnd) { this->isEnd = isEnd; }

	RootType GetRootType() { return rootType; }
	void SetRootType(RootType rootType) { this->rootType = rootType; }

	wstring GetName() const { return name; }
	int GetKeyFrameCount() const { return keyFrameCount; }

	float GetFrameRate() const { return frameRate; }
	void SetFrameRate(float frameRate) { this->frameRate = frameRate; }

	float GetDefaultFrameRate() const { return defaultFrameRate; }

	void ResetFrameRate() { frameRate = defaultFrameRate; }

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:
	wstring name;

	RootType rootType;

	int keyFrameCount; /// �� Frame ��
	float frameRate; /// �ʴ� Frame ��
	float defaultFrameRate;

	bool isLoop;
	bool isEnd;
};
