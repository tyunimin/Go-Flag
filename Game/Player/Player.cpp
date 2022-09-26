#include"pch.h"
#include"Player.h"
#include "DeviceResources.h"

// 名前空間の利用
using namespace DirectX;
//コンストラクタ
Player::Player():
	m_position(DirectX::SimpleMath::Vector3::Zero),
	m_rotation(0.0f),
	m_flag(false),
	m_jump_count(0),
	m_vel(0.0f,0.0f,0.0f)
{
}
//デストラクタ
Player::~Player()
{
	//本来はゲームが終了するまでFinalizeをしてはいけない
	//m_pAdx2->Finalize();
}
//初期化
void Player::Initialize()
{
	//ADX2のインスタンスを取得
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2の初期化
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.01f);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<CommonStates>(device);

	// エフェクトファクトリの作成
	EffectFactory* factory = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	factory->SetDirectory(L"Resources/Models");
	// ファイルを指定してモデルデータ読み込み
	m_pModel = Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Sky.cmo",
		*factory
	);
	
	delete factory;
}

//更新
void Player::Update()
{
	//キーボード情報
	Keyboard::State keyState = Keyboard::Get().GetState();
	m_keyTracker.Update(keyState);

	Jump(m_keyTracker);	//ジャンプ

	Rotation();		//回転
	Move(keyState);	//操作
}

//描画
void Player::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//	変換行列の宣言(ワールド)
	DirectX::SimpleMath::Matrix world;

	//	ワールド行列の初期化(単位行列)
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation / 180.0f * 3.14f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position.x, m_position.y, m_position.z);

	// モデル描画
	m_pModel->Draw(context, *m_commonState.get(), world, view, projection);
}
//終了
void Player::Finalize()
{

}
//ジャンプ
void Player::Jump(DirectX::Keyboard::KeyboardStateTracker KeyState)
{
	float Grav = 0.1f;	//重力
	m_vel.y -= Grav;
	m_position.y += m_vel.y;

	if (KeyState.IsKeyPressed(DirectX::Keyboard::Space)&&m_jump_count<2)
	{
		m_vel.y = 1.0f;
		m_pAdx2->Play(CRI_CUESHEET_0_SE_JUMP06);
	}

	if (m_vel.y < 0.0f)
	{
		m_vel.y = 0.0f;
	}
}
//回転
void Player::Rotation()
{
	if (m_flag == true)
	{
		m_rotation += 2.0f;
	}
	if (m_rotation >= 45.0f)
	{
		m_flag = false;
	}
	if (m_flag == false)
	{
		m_rotation -= 2.0f;
	}
	if (m_rotation <= -45.0f)
	{
		m_flag = true;
	}
}

//歩き
void Player::Move(DirectX::Keyboard::State KeyState)
{
	if (KeyState.W||KeyState.Up)
	{
		m_vel.z = 0.1f;
		m_position.z -= m_vel.z;
	}
	if (KeyState.S||KeyState.Down)
	{
		m_vel.z = 0.1f;
		m_position.z += m_vel.z;
	}
	if (KeyState.A||KeyState.Left)
	{
		m_vel.x = 0.1f;
		m_position.x -= m_vel.x;
	}
	if (KeyState.D||KeyState.Right)
	{
		m_vel.x = 0.1f;
		m_position.x += m_vel.x;
	}

}

SimpleMath::Vector3 Player::GetPosition()
{
	return m_position;
}

SimpleMath::Vector3 Player::SetPosition(SimpleMath::Vector3 position)
{
	return m_position = position;
}

int Player::JumpCountHeal()
{
	return m_jump_count = 0;
}

int Player::GetJumpCount(int x)
{
	return m_jump_count = x;
}