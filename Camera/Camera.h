#pragma once

class iFallowCamera;
class Camera
{
public:
	Camera();
	~Camera();

	static Camera* Get();
	static void Delete();




	void GetPosition(D3DXVECTOR3* position, D3DXVECTOR3* forward = nullptr)
	{
		*position = this->position;
		if(forward != nullptr)
			*forward = this->forward;
	}


	void GetCameraSpaceMatrix(D3DXMATRIX& matrix);


	void SetFallowCamera(iFallowCamera* target);

	void UpdateRotationMatrix();
	//void UpdateProjectionMatrix();
	//void UpdateProjection2Orth();
	void UpdateViewMatrix();
	
	
	void Update();



	//Camera Control
	void Move(D3DXVECTOR3 translation);
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void MoveForward();
	void MoveBackward();

	void Rotate(D3DXVECTOR2 rotate);
	void RotateUp();
	void RotateDown();
	void RotateLeft();
	void RotateRight();



	
	void GetCamAngle(float*& angle) {
		angle = &angle_theta;
	}
	

	void GetView(D3DXMATRIX* view)
	{
		memcpy(view, this->view, sizeof(D3DXMATRIX));
	}

	void GetMirrorView(D3DXMATRIX* view) {
		memcpy(view, this->mirrorView, sizeof(D3DXMATRIX));
	}

	void GetDefaultView(D3DXMATRIX* view) {
		memcpy(view, this->defaultView, sizeof(D3DXMATRIX));
	}


	void GetRay(D3DXVECTOR3* origin, D3DXVECTOR3* direction, D3DXMATRIX projection);

	const static float screenNear; //�Ÿ�
	const static float screenDepth; //����

	float fieldOfView; //�þ߰�

	bool Fallowing() {
		if (targetpos == nullptr)
			return false;
		else
			return true;
	}
private:
	static Camera* instance;



	D3DXVECTOR3 position; //��ġ
	D3DXVECTOR3 forward; //����
	D3DXVECTOR3 right; //����
	D3DXVECTOR3 up; //����
	D3DXVECTOR2 rotate; //ȸ��

	D3DXMATRIX view;
	D3DXMATRIX mirrorView;
	D3DXMATRIX rotation;

	D3DXMATRIX defaultView;


	D3DXMATRIX cameraSpace;

	float translationSpeed;
	float rotationSpeed;



	//FallowCamera
	D3DXVECTOR3* targetpos;
	float targetDistance;
	float targetHeightOffset;

	//fallowcamera�� ȸ�� ����
	float angle_pie, angle_theta;

};


