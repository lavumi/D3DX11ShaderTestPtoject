#pragma once
#include "Shader.h"

class TextureShader : public Shader {
public:
	TextureShader();
	~TextureShader();


	virtual void Update();
	virtual void Render(UINT indexCount,
		D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, ID3D11ShaderResourceView* diffuse);



private:
	void CreateBuffers();





};