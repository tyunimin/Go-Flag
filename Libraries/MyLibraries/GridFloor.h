//
// GridFloor.h
//
#pragma once

#include <CommonStates.h>
#include <PrimitiveBatch.h>
#include <Effects.h>
#include <VertexTypes.h>
#include <SimpleMath.h>

/// <summary>
/// デバッグ表示用のグリッド床を表示するクラス
/// </summary>
class GridFloor
{
	// エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;

	// インプットレイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// 床の一辺のサイズ
	float m_sizeX, m_sizeY;

	// 分割数
	int m_divsX, mDivsY;

public:
	// コンストラクタ
	GridFloor(
		ID3D11Device1* device, 
		ID3D11DeviceContext1* context, 
		const int divsX,	// 分割数X
		const int divsY		// 分割数Y
	);

	// デストラクタ
	~GridFloor();

	// 描画処理
	void Draw(
		ID3D11DeviceContext1* context, 
		DirectX::CommonStates* states,			// D3Dレンダリング状態オブジェクト
		const DirectX::SimpleMath::Matrix view,	// ビュー行列
		const DirectX::SimpleMath::Matrix proj,	// 射影行列
		const DirectX::GXMVECTOR color = DirectX::Colors::Gray
	);
};
