#include "environmentMapper.h"
#include "dxstructures.h"
#include "dxDevice.h"

using namespace mini;
using namespace gk2;
using namespace DirectX;
using namespace std;

const int EnvironmentMapper::TEXTURE_SIZE = 256;

EnvironmentMapper::EnvironmentMapper(const DxDevice& device, float nearPlane, float farPlane, XMFLOAT3 position)
	: m_nearPlane(nearPlane), m_farPlane(farPlane),	m_position(position.x, position.y, position.z, 1.0f)
{
	Texture2DDescription texDesc;
	// TODO : 1.11 Setup texture width, height, mip levels and bind flags
	texDesc.Width = texDesc.Height = TEXTURE_SIZE;
	texDesc.MipLevels = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

	// TODO : 1.12 Uncomment following lines
	m_faceTexture = device.CreateTexture(texDesc); 
	m_renderTarget = device.CreateRenderTargetView(m_faceTexture);

	SIZE s;
	s.cx = s.cy = TEXTURE_SIZE;
	m_depthBuffer = device.CreateDepthStencilView(s);

	// TODO : 1.13 Create description for empty texture used as environment cube map, setup texture's width, height, mipLevels, bindflags, array size and miscFlags
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.ArraySize = 6;
	// TODO : 1.14 Uncomment following lines
	m_envTexture = device.CreateTexture(texDesc);
	m_envView = device.CreateShaderResourceView(m_envTexture);

	//Shaders
	auto vsCode = device.LoadByteCode(L"envMapVS.cso");
	m_envVS = device.CreateVertexShader(vsCode);
	auto psCode = device.LoadByteCode(L"envMapPS.cso");
	m_envPS = device.CreatePixelShader(psCode);
}

void EnvironmentMapper::Begin(const dx_ptr<ID3D11DeviceContext>& context) const
{
	context->VSSetShader(m_envVS.get(), nullptr, 0);
	auto tx_ptr = m_envView.get();
	context->PSSetShaderResources(0, 1, &tx_ptr);
	context->PSSetShader(m_envPS.get(), nullptr, 0);
}

DirectX::XMMATRIX mini::gk2::EnvironmentMapper::FaceViewMtx(D3D11_TEXTURECUBE_FACE face) const
{
	// TODO : 1.15 Setup view matrix
	XMFLOAT4 lookTo(0, 0, 0, 1);
	XMFLOAT4 up(0, 0, 0, 1);
	switch (face) {
		case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
		{
			lookTo.x = 1;
			up.y = 1;
			break;
		}
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
		{
			lookTo.x = -1;
			up.y = 1;
			break;
		}
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
		{
			lookTo.y = 1;
			up.z = -1;
			break;
		}
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
		{
			lookTo.y = -1;
			up.z = 1;
			break;
		}
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
		{
			lookTo.z = 1;
			up.y = 1;
			break;
		}
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
		{
			lookTo.z = -1;
			up.y = 1;
			break;
		}
	}
	XMMATRIX viewMtx = XMMatrixLookToLH(XMLoadFloat4(&m_position), XMLoadFloat4(&lookTo), XMLoadFloat4(&up));
	// TODO : 1.16 Replace with correct implementation
	return viewMtx;
}

DirectX::XMFLOAT4X4 mini::gk2::EnvironmentMapper::FaceProjMtx() const
{
	XMFLOAT4X4 proj;

	// TODO : 1.17 Replace with correct implementation
	XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(XM_PIDIV2, 1, m_nearPlane, m_farPlane));

	return proj;
}

void EnvironmentMapper::SetTarget(const dx_ptr<ID3D11DeviceContext>& context)
{
	D3D11_VIEWPORT viewport;

	// TODO : 1.18 Setup viewport
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.Width = viewport.Height = TEXTURE_SIZE;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* targets[1] = { m_renderTarget.get() };
	context->OMSetRenderTargets(1, targets, m_depthBuffer.get());
}

void mini::gk2::EnvironmentMapper::ClearTarget(const dx_ptr<ID3D11DeviceContext>& context)
{
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(m_renderTarget.get(), clearColor);
	context->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void EnvironmentMapper::SaveFace(const dx_ptr<ID3D11DeviceContext>& context, D3D11_TEXTURECUBE_FACE face)
{
	if (face < 0 || face > 5)
		return;
	// TODO : 1.19 Copy face to environment cube map
	context->CopySubresourceRegion(m_envTexture.get(), face, 0, 0, 0, m_faceTexture.get(), 0, 0);
}