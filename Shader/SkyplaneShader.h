#pragma once
#include "Shader.h"

class SkyplaneBuffer;
class SkyplaneShader : public Shader {
public:
	SkyplaneShader();
	~SkyplaneShader();


	virtual void Update();
	virtual void Render( 
		ID3D11ShaderResourceView* perlin);



private:
	SkyplaneBuffer * buffer;


	float speed;

};