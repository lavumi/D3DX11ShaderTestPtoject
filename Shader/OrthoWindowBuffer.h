#pragma once
#include "ShaderBuffer.h"

class OrthoWindowBuffer : public ShaderBuffer
{
public:
	OrthoWindowBuffer()
		: ShaderBuffer(sizeof(VS_DATA), 0,0,0, 0)
	{
		D3DInfo info;
		D3D::GetInfo(&info);

		vsData.screenSize.x = (float)info.screenWidth;
		vsData.screenSize.y = (float)info.screenHeight;
		
		UpdateVSBuffer(&vsData, sizeof(VS_DATA));
	}

	
	void Update()
	{
	}

	struct VS_DATA
	{
		D3DXVECTOR2 screenSize;
		D3DXVECTOR2 padding;
	};
	struct PS_DATA
	{

	};


private:
	
	VS_DATA vsData;
	PS_DATA psData;
};