#pragma once

class Landscape;
class GrassBuffer;
class WorldBuffer;
class Character;
class TerrainGrass
{
public:
	TerrainGrass();
	~TerrainGrass();

	void Initialize(Landscape* land);

	void Update();
	void Render();

	void SetCharacterPos(D3DXVECTOR3* pos);




	
	
private:

	void CreateBuffer();
	void CreateBufferTemp();
	void CreateInstanceData();

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* instanceBuffer;


	ID3D11ShaderResourceView* heightMap;

	UINT vertexCount;

	//VertexTexture* vertexData;
	vector<D3DXMATRIX> instanceData;

	Landscape* land;
	D3DXMATRIX world;
	WorldBuffer* wBuffer;
	GrassBuffer* buffer;
	Character* player;
};