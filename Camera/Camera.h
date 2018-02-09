#pragma once

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


	void UpdateRotationMatrix();
	//void UpdateProjectionMatrix();
	//void UpdateProjection2Orth();
	void UpdateViewMatrix();
	//void Update();

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
	//void GetProjection(D3DXMATRIX* projection)
	//{
	//	memcpy(projection, this->projection, sizeof(D3DXMATRIX));
	//}
	//
	//void GetOrthProjection(D3DXMATRIX* projection)
	//{
	//	memcpy(projection, this->orthoProjection, sizeof(D3DXMATRIX));
	//}


	void GetRay(D3DXVECTOR3* origin, D3DXVECTOR3* direction, D3DXMATRIX projection);

	const static float screenNear; //�Ÿ�
	const static float screenDepth; //����

	float fieldOfView; //�þ߰�

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


	//D3D11_VIEWPORT viewport;
	//
	//float screenAspect; //ȭ�����
	//D3DXMATRIX projection;
	//D3DXMATRIX orthoProjection;
};