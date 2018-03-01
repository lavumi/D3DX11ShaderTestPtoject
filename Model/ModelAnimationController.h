#pragma once

enum class AnimationMode
{
	Play = 0, Pause, Stop,
};

/********************************************************************************
 @breif
 Time�� ���� Animatiion�� Frame�� ����ϴ� Class
 
 ���� ���Ǵ� Animation �� �����ϰ�, �� Animation�� ���� Time�� �ش��ϴ� Key Frame(current, next)�� ���
 useQuaternionKeyFrames�� Ture�϶� Time�� ���� Frame �������� ���
********************************************************************************/
class ModelAnimation;
class ModelAnimationController
{
public:
	ModelAnimationController();
	~ModelAnimationController();

	void AddAnimation(ModelAnimation* animation);

	void SetIsLoop(bool isLoop) { this->isLoop = isLoop; }

	ModelAnimation* GetCurrentAnimation();
	void SetCurrentAnimation(wstring animationName);
	void SetCurrentAnimation(int index);

	UINT GetAnimationCount();
	ModelAnimation* GetAnimation(int index);
	ModelAnimation* GetAnimation(wstring animName);

	int GetCurrentKeyFrame() { return currentKeyFrame; }
	void SetCurrentKeyFrame(int keyFrame);
	
	int GetNextKeyFrame() { return nextKeyFrame; }
	float GetKeyFrameFactor() { return keyFrameFactor; }

	AnimationMode GetAnimationMode() { return animationMode; }

	void UseQuaternionKeyFrames(bool use) { useQuaternionKeyFrames = use; }
	bool UseQuaternionKeyFrames() { return useQuaternionKeyFrames; }

	void Play();
	void Pause();
	void Stop();

	void Update();
private:
	AnimationMode animationMode; /// ��� ���
	ModelAnimation* currentAnimation; /// ���� Animation
	ModelAnimation* prevAnimation; /// ���� Animation

	typedef pair<wstring, ModelAnimation *> Pair;
	vector<Pair> animations; /// Animation ����
	
	float frameTimer; /// ���� KeyFrame���� ���� �� ��� �ð�

	int currentKeyFrame; /// ���� KeyFrame
	int nextKeyFrame; /// ���� KeyFrame
	float keyFrameFactor; /// ���� KeyFrame�� ���� KeyFrame���̿����� ���� ��
	bool useQuaternionKeyFrames; /// ���� ��� ���� flag

	bool isLoop;
};