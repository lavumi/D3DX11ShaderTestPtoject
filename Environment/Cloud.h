#pragma once

class PerlinNoise;
class Cloud{
public:
	Cloud();
	~Cloud();


	void Initialize();
	void Update();
	void Render();

	void MakeCloudPerlin();

	UINT getIndexCount() {
		return indexCount;
	}

	ID3D11ShaderResourceView* getPerlinMap() {
		return perlin;
	}

	D3DXMATRIX getWorld() {
		return world;
	}
	
	
private:

	void CreateBuffer();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;


	UINT vertexCount;
	UINT indexCount;







	PerlinNoise* perlinNoise;
	ID3D11ShaderResourceView* perlin;
	D3DXMATRIX world, scale;


	//PerlinNoise* noise;
};