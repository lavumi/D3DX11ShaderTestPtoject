#pragma once
#include "../Shader/ShaderBuffer.h"

/********************************************************************************
 @brief
 ��� Bone�� ���� Time������ Transform Matrix�� �����ϴ� ����
 ��� Vertex���� ���� ���� ����ϹǷ� ModelClass���� �� �� ������Ʈ
********************************************************************************/
class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer()
		: ShaderBuffer(sizeof(Data))
	{
		D3DXMatrixIdentity(&data.boneScale);
		
		for(int i = 0; i < 100; i++)
			D3DXMatrixIdentity(&data.boneArray[i]);

		data.isSkinning = false;
	}

	void Update()
	{
		UpdateBuffer(&data, sizeof(data));
	}

	void SetSkinning(bool isSkinning)
	{
		data.isSkinning = (UINT)isSkinning;
	}

	void SetBoneArray(D3DXMATRIX* matrix, UINT count)
	{
		memcpy(data.boneArray, matrix, count * sizeof(D3DXMATRIX));

		for(UINT i = 0; i < count; i++)
			D3DXMatrixTranspose(&data.boneArray[i], &data.boneArray[i]);
	}


	struct Data
	{
		D3DXMATRIX boneScale;
		D3DXMATRIX boneArray[100];
		UINT isSkinning;
		D3DXVECTOR3 padding;
	};

private:
	Data data;
};