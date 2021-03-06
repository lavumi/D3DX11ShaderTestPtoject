#include "../stdafx.h"
#include "TerrainGrass.h"
#include "Landscape.h"
#include "../Render/Frustum.h"

#include "../Shader/GrassBuffer.h"
#include "../Shader/WorldBuffer.h"

TerrainGrass::TerrainGrass()
{
	vertexBuffer = 0;
	instanceBuffer = 0;

	buffer = new GrassBuffer();
//	wBuffer = new WorldBuffer();
//	D3DXMatrixIdentity(&world);
//	wBuffer->SetWorld(world);
}

TerrainGrass::~TerrainGrass()
{

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(instanceBuffer);

	SAFE_DELETE(buffer);
//	SAFE_DELETE(wBuffer);
}

void TerrainGrass::Initialize(Landscape* land)
{
	this->land = land;
	//this->frustum = frustum;


	int size = 30;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (abs(i - 15) + abs(j - 15) > 15)
				continue;
			D3DXVECTOR3 position;
			position.x = i * 2+160;
			position.z = j * 2+160;
			land->GetY(position);
			if (position.y <0 && position.y > -7)
				instanceData.push_back(D3DXVECTOR2(i * 2 + 160, j * 2 + 160));
		}
	}


	//CreateInstanceData();
	CreateBuffer();

	land->GetHeightMap(heightMap);
	
	buffer->SetGrassData(1.0f, 0.04f, D3DXVECTOR3(1,0,0));

	




	//D3D11_MAPPED_SUBRESOURCE subResource;
	//
	//HRESULT hr = D3D::GetDeviceContext()->Map
	//(
	//	instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	//);
	//
	//memcpy(subResource.pData, &instanceData[0], sizeof(D3DXVECTOR2) * instanceData.size());
	//
	//D3D::GetDeviceContext()->Unmap(instanceBuffer, 0);


}

void TerrainGrass::Update()
{





//
//
//
//D3DXMatrixTranslation(&world, position.x, 0, position.z);

//	wBuffer->SetWorld(world);
}

void TerrainGrass::Render()
{

	UINT stride[2];
	stride[0] = sizeof(Vertex);
	stride[1] = sizeof(D3DXVECTOR2);


	UINT offset[2];
	offset[0] = 0;
	offset[1] = 0;

	ID3D11Buffer* bufferPointers[2];
	bufferPointers[0] = vertexBuffer;
	bufferPointers[1] = instanceBuffer;

//	wBuffer->SetBuffer();
	buffer->SetBuffers();
	D3D::GetDeviceContext()->IASetVertexBuffers(0, 2, bufferPointers, stride, offset);
	D3D::GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	D3D::GetDeviceContext()->GSSetShaderResources(0, 1, &heightMap);

	D3D::GetDeviceContext()->DrawInstanced(vertexCount, instanceData.size(), 0, 0);
	//D3D::GetDeviceContext()->Draw(vertexCount, 0);
}

void TerrainGrass::SetCharacterPos(D3DXVECTOR3* pos)
{
	buffer->SetCharacter(D3DXVECTOR4(pos->x, pos->y, pos->z, 1));
}

void TerrainGrass::SetPosByFrustum(Frustum * frustum)
{
	return;
	instanceData.clear();
	D3DXVECTOR3 position, forward;
	Camera::Get()->GetPosition(&position, &forward);
	position.x = floor(position.x / 2)*2;
	position.z = floor(position.z / 2)*2;
	land->GetY(position);


	int size = 150;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			D3DXVECTOR3 temp;

			temp = position;
			temp.x += i*2 - size;
			temp.z += j*2 - size;
			if (frustum->CheckSphere(temp, 3)) {
				instanceData.push_back(D3DXVECTOR2(temp.x, temp.z));
			}
		}
	}


	D3D11_MAPPED_SUBRESOURCE subResource;

	HRESULT hr = D3D::GetDeviceContext()->Map
	(
		instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, &instanceData[0], sizeof(D3DXVECTOR2) * instanceData.size());

	D3D::GetDeviceContext()->Unmap(instanceBuffer, 0);
}





void TerrainGrass::CreateBuffer()
{
	HRESULT hr;
	int grassdense = 10;
	int densetile = 3;


	vertexCount =(UINT)( grassdense* grassdense*densetile * densetile);

	
	int rndx, rndy;
	Vertex* vertexData = new Vertex[vertexCount];
	int i = 0;
	for (int k = 0; k < densetile*densetile; k++) {
		for (int j = 0; j < grassdense*grassdense; j++) {
			rndx = rand() % 100;
			rndy = rand() % 100;
			vertexData[i++].position = D3DXVECTOR3(
				(float)(j / grassdense) / grassdense + (float)(k % densetile)  + (float)(rndx - 50) / 100.0f, 
				0.0f, 
				(float)(j % grassdense) / grassdense + (float)(k / densetile)  + (float)(rndy - 50) / 100.0f) ;
		}
	}




	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;


	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Vertex) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertexData);



	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(D3DXVECTOR2) * instanceData.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &instanceData[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;



	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
	assert(SUCCEEDED(hr));
}

void TerrainGrass::CreateBufferTemp()
{
	HRESULT hr;

	vertexCount = 1;
	Vertex* vertexData = new Vertex[vertexCount];
	int i = 0;
	vertexData[i++].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;


	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(Vertex) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertexData;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(vertexData);



	if (instanceData.size() == 0)
		return;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(D3DXMATRIX) * instanceData.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &instanceData[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;



	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &instanceBuffer);
	assert(SUCCEEDED(hr));
}

void TerrainGrass::CreateInstanceData()
{
	//land->GetGroundPos(instanceData);

}
