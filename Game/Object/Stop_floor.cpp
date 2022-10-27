#include"pch.h"
#include"Stop_floor.h"
#include "DeviceResources.h"

using namespace DirectX;

Stop_floor::Stop_floor()
{
	m_pDebugCamera = std::make_unique<Camera>();
}

Stop_floor::~Stop_floor()
{

}

void Stop_floor::Initialize(DirectX::SimpleMath::Vector3 Pos, int i)
{
	
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	//	コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// エフェクトファクトリの作成
	EffectFactory* Floorfac = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	Floorfac->SetDirectory(L"Resources/Models");
	m_floor = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/floor.cmo",
		*Floorfac
	);
	delete Floorfac;

	m_floorPos = Pos;
}

void Stop_floor::Update(int i)
{
}

void Stop_floor::Render(DirectX::SimpleMath::Vector3 Pos)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();	
	//	変換行列の宣言(ワールド、ビュー、射影(プロジェクション))
	DirectX::SimpleMath::Matrix world, view, projection;
	//	ワールド行列の初期化(単位行列)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();
	projection = m_pDebugCamera->GetProjectionMatrix();

	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_floorPos.x, m_floorPos.y, m_floorPos.z);
	m_floor->Draw(context, *m_commonState.get(), world, view, projection);
}

void Stop_floor::Finalize()
{
}
