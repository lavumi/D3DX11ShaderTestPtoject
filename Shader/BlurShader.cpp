#include "BlurShader.h"

BlurShader::BlurShader()
	:Shader(L"./Shader/FX/BlurShader.fx")
{
	CreateInputLayout(VertexTexture::desc, VertexTexture::count);

	CreateBuffers();
}

BlurShader::~BlurShader()
{
}

void BlurShader::Update()
{

}

void BlurShader::Render(UINT indexCount, D3DXMATRIX world1, ID3D11ShaderResourceView* diffuse)
{

	D3D::GetDeviceContext()->IASetInputLayout(layout);
	D3D::GetDeviceContext()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDeviceContext()->PSSetShader(pixelShader, NULL, 0);

	cameraBuffer->OrthoUpdate();
	cameraBuffer->SetVSBuffer(0);

	worldBuffer->SetWorld(world1);
	worldBuffer->SetVSBuffer(1);

	D3D::GetDeviceContext()->VSSetConstantBuffers(2, 1, &screenSizeBuffer);

	if(diffuse != nullptr)
		D3D::GetDeviceContext()->PSSetShaderResources(0, 1, &diffuse);

	D3D::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

void BlurShader::CreateBuffers()
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;
	HRESULT hr;

	D3DInfo info;
	D3D::GetInfo(&info);

	D3DXVECTOR4 screenSize;
	screenSize.x = (float)info.screenWidth;
	screenSize.y = (float)info.screenHeight;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(D3DXVECTOR4);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = screenSize;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &screenSizeBuffer);
	assert(SUCCEEDED(hr));
}