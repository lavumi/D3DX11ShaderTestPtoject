#pragma once
#include "../Utility/BinaryInputOutputHandler.h"

/********************************************************************************
@breif
Material ������ �����ϴ� Class
Ambient(�ֺ�), Diffuse(�л�), Emissive(�߱�), Specular(�ݻ�), Shininess(����)�� ������ �����Ѵ�
********************************************************************************/
class ModelMaterial : public BinaryInputOutputHandler
{
public:
	ModelMaterial();
	ModelMaterial(int number, FbxSurfaceMaterial* material);
	~ModelMaterial();

	UINT GetIndex() { return number; }

	ID3D11ShaderResourceView* GetAmbientView() { return ambientView; }
	ID3D11ShaderResourceView* GetEmissiveView() { return emissiveView; }
	ID3D11ShaderResourceView* GetDiffuseView() { return diffuseView; }
	ID3D11ShaderResourceView* GetSpecularView() { return specularView; }
	ID3D11ShaderResourceView* GetNormalMapView() { return normalMapView; }

	virtual void Export(BinaryWriter* bw);
	virtual void Import(BinaryReader* br);

private:
	D3DXCOLOR GetProperty
	(
		FbxSurfaceMaterial* material
		, const char* name, const char* factorName
		, wstring* textureName
	);

	void GetProperty
	(
		FbxSurfaceMaterial* material
		, const char* name
		, wstring* textureName
	);

	void CreateView(wstring path, ID3D11ShaderResourceView** view);

	UINT number; /// Material Index
	D3DXCOLOR ambient; /// �ֺ���: ���� ������ ��ġ �ֺ����� ������ �ݻ�� ���� ��Ÿ���� �� ó�� ������ ��
	D3DXCOLOR emissive; /// �߱���: ������ ���� �߻��ϴ� ��
	D3DXCOLOR diffuse; /// �л��: ���ݻ�� ���� ���� ������� �����ڿ��� �����ϰ� �������� ��
	D3DXCOLOR specular; /// �ݻ��: �ֱ��� �ݻ�Ǿ� �����ڿ��� ���� �� ������ ��
	float shininess; /// ����: ������ ����

	/// �� ���� Texture
	wstring ambientFile;
	wstring emissiveFile;
	wstring diffuseFile;
	wstring specularFile;
	wstring normalMapFile;

	/// �� Texture�� ���� ������ ResourceView
	ID3D11ShaderResourceView* ambientView;
	ID3D11ShaderResourceView* emissiveView;
	ID3D11ShaderResourceView* diffuseView;
	ID3D11ShaderResourceView* specularView;
	ID3D11ShaderResourceView* normalMapView;
};