#include "../stdafx.h"
#include "Skydome.h"


Skydome::Skydome()
{
	CreateBuffer();
	D3DXMatrixIdentity(&world);
	wBuffer = new WorldBuffer();
	wBuffer->SetWorld(world);
}

Skydome::~Skydome()
{


	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(wBuffer);
}

void Skydome::Update()
{
	
	D3DXVECTOR3 position;
	Camera::Get()->GetPosition(&position);



	D3DXMatrixTranslation(&world, position.x, position.y, position.z);
	wBuffer->SetWorld(world);

}

void Skydome::Render()
{
	wBuffer->SetBuffer();
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3D::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}



void Skydome::CreateBuffer()
{
	HRESULT hr;



	wstring file = L"./Contents/Meshes/Sphere.data";


	ifstream ifs(file.c_str(), ios::binary);

	ifs.read((char*)&vertexCount, sizeof(UINT));
	VertexTextureNormal* vertexData = new VertexTextureNormal[vertexCount];
	ifs.read((char*)vertexData, sizeof(VertexTextureNormal)*vertexCount);

	ifs.read((char*)&indexCount, sizeof(UINT));
	UINT* indexData = new UINT[indexCount];
	ifs.read((char*)indexData, sizeof(UINT)*indexCount);

	ifs.close();
	

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;


	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexTextureNormal) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));



	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));



	

}
