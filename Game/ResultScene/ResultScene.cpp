/*
タイトルシーン
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "../GameMain.h"
#include "DeviceResources.h"

#include "ResultScene.h"

using namespace DirectX;

/*--------------------------------------------------
コンストラクタ
--------------------------------------------------*/
ResultScene::ResultScene():
	m_blackalpha(1.0f),
	m_pushX(0.0f),
	m_cflag(false),
	m_pushflag(false),
	m_pushXflag(false)
{
}

/*--------------------------------------------------
デストラクタ
--------------------------------------------------*/
ResultScene::~ResultScene()
{
	//本来はゲームが終了するまでFinalizeをしてはいけない
	m_pAdx2->Finalize();
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void ResultScene::Initialize()
{
	//ADX2のインスタンスを取得
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2の初期化
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_EREMOPHILA);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	// コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	// スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");
}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE ResultScene::Update(const DX::StepTimer& timer)
{
	// キー入力情報を取得する
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();

	// マウス入力情報を取得する
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	if (keyState.X)
	{
		m_cflag = true;
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}
	if (m_cflag==false)
		m_blackalpha -= 0.01f;
	if (m_blackalpha <= 0.0f && m_cflag == false)
	{
		m_blackalpha = 0.0f;
	}


	if(!m_pushXflag)
		m_pushX += 0.01f;
	if (m_pushX >= 1.0f)
		m_pushXflag = true;
	if(m_pushXflag)
		m_pushX -= 0.01f;
	if (m_pushX <= 0.0f)
		m_pushXflag = false;

	if (m_cflag)
		m_blackalpha += 0.01f;

	if (m_blackalpha >= 1.1f)
		return GAME_SCENE::TITLE;


	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
描画
--------------------------------------------------*/
void ResultScene::Draw()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());

	SimpleMath::Vector2 pos(0, 0);
	SimpleMath::Vector2 position(600, 600);

	m_spriteBatch->Draw(m_texture.Get(), pos);

	m_spriteBatch->Draw(m_texture_pushX.Get(), position, DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_pushX });
	m_spriteBatch->Draw(m_texture_black.Get(), pos, DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_blackalpha });
	
	m_spriteBatch->End();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void ResultScene::Finalize()
{
}

void ResultScene::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();

	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Clear.png",
		nullptr,
		m_texture.ReleaseAndGetAddressOf()
	);

	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_texture_black.ReleaseAndGetAddressOf()
	);

	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/PushXButton.png",
		nullptr,
		m_texture_pushX.ReleaseAndGetAddressOf()
	);
}