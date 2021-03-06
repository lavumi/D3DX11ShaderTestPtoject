#include "../stdafx.h"
#include "ShaderManager.h"
#include "Shader.h"
ShaderManager::ShaderManager()
{		



	assert(CreateShader(L"BlurShader", VertexTexture::desc, VertexTexture::count)											);
	assert(CreateShader(L"ColorShader", VertexColor::desc, VertexColor::count)												);
	assert(CreateShader(L"FBXModelShader", VertexTextureNormalTangentBlend::desc, VertexTextureNormalTangentBlend::count)	);
	//assert(CreateShader(L"GrassLightViewShader", VertexInstance::desc, VertexInstance::count)										);

	assert(CreateShader(L"GrassShader", VertexGrass::desc, VertexGrass::count));
	assert(CreateShader(L"GrassShadowShader", VertexGrass::desc, VertexGrass::count));

	assert(CreateShader(L"LightViewShader", VertexTextureNormalTangent::desc, VertexTextureNormalTangent::count)			);
	assert(CreateShader(L"FBXLightViewShader", VertexTextureNormalTangentBlend::desc, VertexTextureNormalTangentBlend::count));
	assert(CreateShader(L"NormalMapShader", VertexTextureNormalTangent::desc, VertexTextureNormalTangent::count)			);
	assert(CreateShader(L"RainShader", VertexTexture3::desc, VertexTexture3::count)											);
	assert(CreateShader(L"ShadowShader", VertexTextureNormalTangent::desc, VertexTextureNormalTangent::count)				);
	assert(CreateShader(L"FBXShadowShader", VertexTextureNormalTangentBlend::desc, VertexTextureNormalTangentBlend::count));
	assert(CreateShader(L"SkydomeShader", VertexTextureNormal::desc, VertexTextureNormal::count)							);
	assert(CreateShader(L"SkyplaneShader", VertexTexture::desc, VertexTexture::count)										);
	assert(CreateShader(L"TerrainShader", VertexTexture::desc, VertexTexture::count)										);
	assert(CreateShader(L"TextureShader", VertexTexture::desc, VertexTexture::count)										);
	assert(CreateShader(L"WaterShader", VertexTextureNormalTangent::desc, VertexTextureNormalTangent::count)				);
	assert(CreateShader(L"InstanceTextureShader", VertexTextureInstance::desc, VertexTextureInstance::count));
	assert(CreateShader(L"InstanceTextureLightViewShader", VertexTextureInstance::desc, VertexTextureInstance::count));
	assert(CreateShader(L"InstanceTextureShadowShader", VertexTextureInstance::desc, VertexTextureInstance::count));
	assert(CreateShader(L"FontShader", VertexTexture::desc, VertexTexture::count));
	assert(CreateShader(L"DepthFogShader", VertexTexture::desc, VertexTexture::count));
	}

ShaderManager::~ShaderManager()
{
	auto iter = shaders.begin();
	for (; iter != shaders.end();iter++) {
		SAFE_DELETE(iter->second);
	}
}

bool ShaderManager::CreateShader(wstring fxName, D3D11_INPUT_ELEMENT_DESC * desc, UINT count)
{
	wstring fullfxName = L"./FX/" + fxName + L".fx";

	Shader* shader = new Shader(fullfxName);
	bool success[6];

	success[0] = shader->CreateVertexShader();
	success[1] = shader->CreateHullShader();
	success[2] = shader->CreateDomainShader();
	success[3] = shader->CreateGeometryShader();
	success[4] = shader->CreatePixelShader();
	success[5] = shader->CreateInputLayout(desc, count);
	
	if (shaders.find(fxName) == shaders.end()) {
		shaders[fxName] = shader;
		return true;
	}
	else
		return false;
}

Shader * ShaderManager::GetShader(wstring fxName)
{
	if (shaders.find(fxName) != shaders.end())
		return shaders[fxName];
	else
		return nullptr;
}

bool ShaderManager::SetShader(wstring fxName)
{
	if (shaders.find(fxName) != shaders.end()) {
		shaders[fxName]->SetShader();
		return true;
	}
	else
		return false;
}


void ShaderManager::ReadFolder(wstring path)
{
	WIN32_FIND_DATA findData;
	wstring new_path = path + L"*.*";

	HANDLE handle = FindFirstFile(new_path.c_str(), &findData);
	assert(handle != NULL);

	//static int number = 0;
	do {
		wstring fileName = findData.cFileName;




		//if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
		//	if (wcscmp(findData.cFileName, L".") & wcscmp(findData.cFileName, L"..")) {
		//		ReadFolder(path + L"/" + findData.cFileName + L"/");
		//	}
		//}
		//else {
		//	vector<wstring> tempvec;
		//	String::SplitString(&tempvec, fileName, L".");
		//	if (tempvec[1] != L"fx") continue;
		//	wstring* temp = new wstring();
		//	*temp = path + fileName;
		//
		//	String::SplitString(&tempvec, path, L"/");
		//
		//	auto iter = tempvec.end();
		//	String::WStringToString(*(iter - 1));
		//}

	} while (FindNextFile(handle, &findData));


}