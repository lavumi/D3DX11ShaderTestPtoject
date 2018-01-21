#include "NormalMapShader.h"

NormalMapShader::NormalMapShader()
	:Shader(L"./Shader/FX/NormalMap.fx")
{
	CreateInputLayout(VertexTextureNormalTangent::desc, VertexTextureNormalTangent::count);

	CreateBuffers();
	parallexData.scale = 0.1f;
	parallexData.layer = 32;


	material.ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	material.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.shininess = 70;
	material.globalAmbient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
}

NormalMapShader::~NormalMapShader()
{
}

void NormalMapShader::Update()
{

}

void NormalMapShader::Render(UINT indexCount, D3DXMATRIX world, 
	ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * heightMap, ID3D11ShaderResourceView* lightMap)
{
	D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
	D3DXMATRIX invWorld;
	D3DXMatrixInverse(&invWorld, nullptr, &world);

	D3DXMatrixTranspose(&extraData.invTransWorld, &invWorld);
	extraData.shadowBias = 0.0001f;


	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	D3D::GetDeviceContext()->Map
	(
		ExtraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, &extraData, sizeof(ExtraData));
	D3D::GetDeviceContext()->Unmap(ExtraBuffer, 0);

	

	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	D3D::GetDeviceContext()->Map
	(
		parallaxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, &parallexData, sizeof(D3DXVECTOR4));

	D3D::GetDeviceContext()->Unmap(parallaxBuffer, 0);




	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	D3D::GetDeviceContext()->Map
	(
		MaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, &material, sizeof(MaterialData));

	D3D::GetDeviceContext()->Unmap(MaterialBuffer, 0);



	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	D3D::GetDeviceContext()->Map
	(
		LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, LightManager::Get()->GetLightData(), sizeof(LightManager::LightData));

	D3D::GetDeviceContext()->Unmap(LightBuffer, 0);




	worldBuffer->SetWorld(world);
	D3D::GetDeviceContext()->PSSetShaderResources(0, 1, &diffuseMap);
	D3D::GetDeviceContext()->PSSetShaderResources(1, 1, &normalMap);
	D3D::GetDeviceContext()->PSSetShaderResources(2, 1, &heightMap);
	D3D::GetDeviceContext()->PSSetShaderResources(3, 1, &lightMap);


	D3D::GetDeviceContext()->VSSetConstantBuffers(2, 1, &LightBuffer);
	D3D::GetDeviceContext()->VSSetConstantBuffers(3, 1, &ExtraBuffer);
	D3D::GetDeviceContext()->PSSetConstantBuffers(0, 1, &MaterialBuffer);
	D3D::GetDeviceContext()->PSSetConstantBuffers(1, 1, &parallaxBuffer);
	
	D3D::GetDeviceContext()->IASetInputLayout(layout);
	D3D::GetDeviceContext()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDeviceContext()->PSSetShader(pixelShader, NULL, 0);

	cameraBuffer->SetVSBuffer(0);
	worldBuffer->SetVSBuffer(1);

	D3D::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
	Sampler::Get()->SetDefault();

}

void NormalMapShader::CreateBuffers()
{
	D3D11_BUFFER_DESC desc;
	HRESULT hr;
	
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(ExtraData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &ExtraBuffer);
	assert(SUCCEEDED(hr));



	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(MaterialData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &MaterialBuffer);
	assert(SUCCEEDED(hr));


	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(ParallexData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &parallaxBuffer);
	assert(SUCCEEDED(hr));



	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(LightManager::LightData);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &LightBuffer);
	assert(SUCCEEDED(hr));


	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(D3DXMATRIX);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &mirrorViewBuffer);
	assert(SUCCEEDED(hr));

}