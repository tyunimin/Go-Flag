//
// GridFloor.cpp
//
#include "pch.h"
#include "GridFloor.h"

//-------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------
GridFloor::GridFloor(ID3D11Device1* device, ID3D11DeviceContext1* context, const int divsX, const int divsY)
	: m_divsX(divsX)
	, mDivsY(divsY)	
{
	// エフェクトの生成
	m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);

	// 頂点カラーの設定
	m_basicEffect->SetVertexColorEnabled(true);
	
	// プリミティブバッチの生成
	m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(context);
	
	// インプットレイアウトの設定
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, 
		byteCodeLength,
		m_inputLayout.GetAddressOf()
	);
}

//-------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------
GridFloor::~GridFloor()
{
}

//-------------------------------------------------------------------
// 描画
//-------------------------------------------------------------------
void GridFloor::Draw(ID3D11DeviceContext1* context, DirectX::CommonStates* states, const DirectX::SimpleMath::Matrix view, const DirectX::SimpleMath::Matrix proj, const DirectX::GXMVECTOR color)
{
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::Identity;

	context->IASetInputLayout(m_inputLayout.Get());
	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	m_basicEffect->SetWorld(world);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(proj);
	m_basicEffect->Apply(context);

	m_primitiveBatch->Begin();

	const DirectX::XMVECTORF32 axisX = { (float)m_divsX, 0.0f, 0.0f };
	const DirectX::XMVECTORF32 axisY = { 0.0f, 0.0f, (float)mDivsY };

	size_t divsX = std::max<size_t>(1, m_divsX);
	DirectX::FXMVECTOR origin = DirectX::g_XMZero;
	// 垂直線
	for (size_t i = 0; i <= divsX; ++i)
	{
		float scaleFactor = float(i) / float(divsX);
		scaleFactor = (scaleFactor * 1.0f) - 0.5f;
		DirectX::XMVECTOR scaleV = XMVectorScale(axisX, scaleFactor);
		scaleV = DirectX::XMVectorAdd(scaleV, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(scaleV, axisY * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(scaleV, axisY * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}
	// 水平線
	size_t divsY = std::max<size_t>(1, mDivsY);
	for (size_t i = 0; i <= divsY; i++)
	{
		float scaleFactor = float(i) / float(divsY);
		scaleFactor = (scaleFactor * 1.0f) - 0.5f;
		DirectX::XMVECTOR scaleV = XMVectorScale(axisY, scaleFactor);
		scaleV = DirectX::XMVectorAdd(scaleV, origin);

		DirectX::VertexPositionColor v1(DirectX::XMVectorSubtract(scaleV, axisX * 0.5f), color);
		DirectX::VertexPositionColor v2(DirectX::XMVectorAdd(scaleV, axisX * 0.5f), color);
		m_primitiveBatch->DrawLine(v1, v2);
	}

	m_primitiveBatch->End();
}
