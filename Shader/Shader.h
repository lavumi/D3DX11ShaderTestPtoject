#pragma once

#include "WorldBuffer.h"

class Shader
{
public:
	void SetShader();
protected:
	friend class ShaderManager;
	Shader(wstring vertexshaderFile);
	~Shader();

	bool CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* desc, UINT count);

	



	ID3D11InputLayout* layout;


	bool CreateVertexShader();
	bool CreatePixelShader();
	bool CreateGeometryShader();
	bool CreateHullShader();
	bool CreateDomainShader();
	bool CheckShaderError(HRESULT hr, ID3D10Blob* error);

	wstring shaderFile;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geoShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;


	ID3D10Blob* vertexBlob;
	ID3D10Blob* pixelBlob;
	ID3D10Blob* geoBlob;
	ID3D10Blob* hullBlob;
	ID3D10Blob* domainBlob;



};