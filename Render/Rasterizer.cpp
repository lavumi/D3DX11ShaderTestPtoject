#include "../stdafx.h"
#include "Rasterizer.h"


Rasterizer::Rasterizer()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateRasterizerState(&rasterDesc, &solidState);
	assert(SUCCEEDED(hr));

	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	hr = D3D::GetDevice()->CreateRasterizerState(&rasterDesc, &wireframeState);
	assert(SUCCEEDED(hr));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	hr = D3D::GetDevice()->CreateRasterizerState(&rasterDesc, &offCullState);
	assert(SUCCEEDED(hr));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	hr = D3D::GetDevice()->CreateRasterizerState(&rasterDesc, &onCullState);
	assert(SUCCEEDED(hr));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	hr = D3D::GetDevice()->CreateRasterizerState(&rasterDesc, &frontCullState);
	assert(SUCCEEDED(hr));

	D3D::GetDeviceContext()->RSSetState(solidState);
}

Rasterizer::~Rasterizer()
{
	SAFE_RELEASE(solidState);
	SAFE_RELEASE(wireframeState);
}


void Rasterizer::SetSolid()
{
	D3D::GetDeviceContext()->RSSetState(solidState);
}

void Rasterizer::SetWireframe()
{
	D3D::GetDeviceContext()->RSSetState(wireframeState);
}

void Rasterizer::SetOnCullMode()
{
	D3D::GetDeviceContext()->RSSetState(onCullState);
}

void Rasterizer::SetOffCullMode()
{
	D3D::GetDeviceContext()->RSSetState(offCullState);
}

void Rasterizer::SetFrontCullMode()
{
	D3D::GetDeviceContext()->RSSetState(frontCullState);
}
