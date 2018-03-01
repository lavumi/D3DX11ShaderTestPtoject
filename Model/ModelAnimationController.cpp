#include "../stdafx.h"
#include "ModelAnimationController.h"
#include "ModelAnimation.h"

ModelAnimationController::ModelAnimationController()
	: currentAnimation(NULL), prevAnimation(NULL)
	, currentKeyFrame(0), nextKeyFrame(0), keyFrameFactor(0.0f), frameTimer(0.0f)
	, isLoop(false)
{
	useQuaternionKeyFrames = true;

	animationMode = AnimationMode::Stop;
}

ModelAnimationController::~ModelAnimationController()
{
	for each(Pair temp in animations)
		SAFE_DELETE(temp.second);
	
	currentAnimation = NULL;
}

/****************************************************************************************************
 @brief
 animations�� ModelAnimation�� Animation �̸� ���� �߰�
****************************************************************************************************/
void ModelAnimationController::AddAnimation(ModelAnimation * animation)
{
	bool isExist = false;
	for (size_t i = 0; i < animations.size(); i++)
	{
		// �ߺ� Ȯ��
		if (animations[i].first == animation->GetName())
		{
			isExist = true;
			
			break;
		}
	}
	assert(isExist == false);

	animations.push_back(Pair(animation->GetName(), animation));
}

ModelAnimation * ModelAnimationController::GetCurrentAnimation()
{
	if(currentAnimation != NULL)
		return currentAnimation;

	return NULL;
}

/****************************************************************************************************
 @brief
 Animation�� �̸����� ���� Animation�� ����
****************************************************************************************************/
void ModelAnimationController::SetCurrentAnimation(wstring animationName)
{
	UINT count = -1;
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].first == animationName)
		{
			count = i;
			break;
		}
	}
	
	if (count != -1)
	{
		if (currentAnimation == animations[count].second)
			return;

		// ���� �ִϸ��̼� ��ü, ���� �ִϸ��̼� ����
		prevAnimation = currentAnimation;
		currentAnimation = animations[count].second;
	}
	else
		return;

	// ���� Animation�� �ٲ�� Animation ���� ����
	Stop();
}

/****************************************************************************************************
 @brief
 Animation�� Index�� ���� Animation�� ����
****************************************************************************************************/
void ModelAnimationController::SetCurrentAnimation(int index)
{
	if( index < 0 || index >= (int)animations.size() )
		currentAnimation = NULL;
	else
	{
		prevAnimation = currentAnimation;
		currentAnimation = animations[index].second;
	}
	
	Stop();
}

UINT ModelAnimationController::GetAnimationCount()
{
	return animations.size();
}

ModelAnimation * ModelAnimationController::GetAnimation(int index)
{
	if( index < 0 || index >= (int)animations.size() )
		return NULL;
	else
		return animations[index].second;
}

ModelAnimation * ModelAnimationController::GetAnimation(wstring animName)
{
	UINT count = -1;
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].first == animName)
		{
			count = i;
			break;
		}
	}

	if (count != -1)
	{
		return animations[count].second;
	}
	else
		return NULL;
}

void ModelAnimationController::Play()
{
	currentAnimation->SetIsEnd(false);
	animationMode = AnimationMode::Play;
}

void ModelAnimationController::Pause()
{
	animationMode = AnimationMode::Pause;
}

void ModelAnimationController::Stop()
{
	animationMode = AnimationMode::Stop;
	frameTimer = 0.0f;

	SetCurrentKeyFrame(0);
}

/****************************************************************************************************
 @brief
 ���� KeyFrame, ���� KeyFrame�� ����
****************************************************************************************************/
void ModelAnimationController::SetCurrentKeyFrame(int keyFrame)
{
	currentKeyFrame = (keyFrame) % currentAnimation->GetKeyFrameCount();
	nextKeyFrame = (currentKeyFrame + 1) % currentAnimation->GetKeyFrameCount();
	
	keyFrameFactor = 0.0f;
}

/****************************************************************************************************
 @brief
 frameTimer�� ������Ű�� �׿� ���� ���� KeyFrame�� ���� KeyFrame ������ ����
 frameTimer�� �� Frame�ð����� ������� KyeFrame�� �������� �̵��ϰ� frameTimer�� �ʱ�ȭ
 KeyFrame�� Animation�� ��ü Frame ���� ���� �� �������� ����
 ������ ����, �ð� ����� ���� ���� KeyFrame�� ���� KeyFrame�� ����� ��ġ�� ��Ÿ���� FrameFactor�� ���
****************************************************************************************************/
void ModelAnimationController::Update()
{
	if (currentAnimation == NULL || animationMode != AnimationMode::Play)
		return;

	frameTimer += Frames::TimeElapsed(); //���� Frmae�� ���� �� �� ��� �ð�

	float secPerFrame = 1.0f / currentAnimation->GetFrameRate(); // Frame �ϳ��� �ҿ� �ð�

	if (frameTimer > secPerFrame)
	{
		currentKeyFrame = nextKeyFrame;

		int frameCount = 0;
		while (frameTimer > secPerFrame)
		{
			//currentKeyFrame = (currentKeyFrame + 1) % currentAnimation->GetKeyFrameCount();
			nextKeyFrame = (nextKeyFrame + 1) % currentAnimation->GetKeyFrameCount();

			frameTimer -= secPerFrame;
			frameCount++;

			// Loop�� �ƴ� Animation�� ����� ��
			if (currentAnimation->GetIsLoop() == false && nextKeyFrame == 0)
			{
				currentAnimation->SetIsEnd(true);

				/*if (prevAnimation != NULL)
				{
				currentAnimation = prevAnimation;
				prevAnimation = NULL;
				currentKeyFrame = 0;
				}*/
				animationMode = AnimationMode::Stop;

				currentKeyFrame = currentAnimation->GetKeyFrameCount() - 1;
				nextKeyFrame = currentKeyFrame;
			}
		}
		keyFrameFactor = (frameTimer + frameCount * secPerFrame) / ((frameCount + 1) *secPerFrame);
	}
	else
	{
		// KeyFrame�� �������� ��� [0,1]
		// ���� Frame�� ���� Frame ���̿��� ��� ���� �ð��� ����Ǿ������� ��Ÿ��
		keyFrameFactor = frameTimer / secPerFrame;
	}
}
