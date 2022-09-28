#include"pch.h"
#include"ModeSelect.h"

#include "DeviceResources.h"
#include <WICTextureLoader.h>
#include<codecvt>
#include<SimpleMath.h>
#include<thread>

using namespace DirectX;

ModeSelect::ModeSelect():
	m_alpha(1.0f),
	m_blackalpha(1.0f),
	m_flag(false),
	m_cflag(false),
	m_SceneSelect(1)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();
	// コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	// スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

ModeSelect::~ModeSelect()
{
	m_pAdx2->Finalize();
}

void ModeSelect::Initialize()
{
	//ADX2のインスタンスを取得
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2の初期化
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	//BGMの再生
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_C0_01005);
}

GAME_SCENE ModeSelect::Update(const DX::StepTimer& timer)
{
	// キー入力情報を取得する
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();

	m_keyTracker.Update(keyState);

	if (m_keyTracker.IsKeyPressed(Keyboard::Space))
	{
		m_cflag = true;
		//効果音再生
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	if (m_keyTracker.IsKeyPressed(Keyboard::Down)||m_keyTracker.IsKeyPressed(Keyboard::S))
	{
		m_SceneSelect++;
		if (m_SceneSelect >= 3)
			m_SceneSelect = 3;
	}
	if (m_keyTracker.IsKeyPressed(Keyboard::Up) || m_keyTracker.IsKeyPressed(Keyboard::W))
	{
		m_SceneSelect--;
		if (m_SceneSelect <=1)
			m_SceneSelect = 1;
	}

	if (!m_cflag)
		m_blackalpha -= 0.01f;
	if (m_blackalpha <= 0.0f && !m_cflag)
		m_blackalpha = 0.0f;

	if (m_cflag)
	{
		m_blackalpha += 0.01f;
	}

	if (!m_flag)
		m_alpha += 0.01f;
	if (m_alpha >= 1.0f)
		m_flag = true;
	if (m_flag)
		m_alpha -= 0.01f;
	if (m_alpha <= 0.0f)
		m_flag = false;


	if (m_blackalpha >= 1.1f)
	{
		switch (m_SceneSelect)
		{
		case 1:
			return GAME_SCENE::PLAY;
			break;
		case 2:
			return GAME_SCENE::LONGMODE;
			break;
		case 3:
			return GAME_SCENE::LONGMODE_HARD;
			break;
		}
	}

	return GAME_SCENE::NONE;
}

void ModeSelect::Draw()
{
	float rotation = 0.0f;							//回転角度をラジアンで指定する
	DirectX::SimpleMath::Vector2 origin = { 0,0 };	//回転する中心点の指定
	float scale = 1.0f;								//拡大率の指定
	SpriteEffects effects = SpriteEffects_None;		//表示反転効果
	float layerDepth = 1.0f;						//表示深度
	FXMVECTOR black = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_blackalpha };
	RECT rect={ 0,0,1280,720 };
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	m_spriteBatch->Draw(m_texture_black.Get(), SimpleMath::Vector2{ 0.0f,0.0f }, &rect,SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_selectmode.Get(), SimpleMath::Vector2{ 0.0f,0.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_shortmode.Get(), SimpleMath::Vector2{ 100.0f,155.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_short_floor.Get(), SimpleMath::Vector2{ 640.0f,155.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_longmode.Get(), SimpleMath::Vector2{ 100.0f,345.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_long_floor.Get(), SimpleMath::Vector2{ 640.0f,345.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_longmode_hard.Get(), SimpleMath::Vector2{ 100.0f,515.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_longH_floor.Get(), SimpleMath::Vector2{ 640.0f,515.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });

	switch (m_SceneSelect)
	{
	case 1:
		m_spriteBatch->Draw(m_select.Get(), SimpleMath::Vector2{ 0.0f,185.0f });
		break;
	case 2:
		m_spriteBatch->Draw(m_select.Get(), SimpleMath::Vector2{ 0.0f,370.0f });
		break;
	case 3:
		m_spriteBatch->Draw(m_select.Get(), SimpleMath::Vector2{ 0.0f,555.0f });
		break;
	}
	m_spriteBatch->Draw(m_texture_black.Get(), SimpleMath::Vector2{ 0.0f,0.0f }, & rect, black);
	m_spriteBatch->End();
}

void ModeSelect::Finalize()
{

}


void ModeSelect::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();

	//画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_texture_black.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/LongModeHARD.png",
		nullptr,
		m_longmode_hard.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/LongMode.png",
		nullptr,
		m_longmode.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/ShortMode.png",
		nullptr,
		m_shortmode.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/SelectMode.png",
		nullptr,
		m_selectmode.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile
	(
		device,
		L"Resources/Textures/Select.png",
		nullptr,
		m_select.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile
	(
		device,
		L"Resources/Textures/short_floor.png",
		nullptr,
		m_short_floor.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile
	(
		device,
		L"Resources/Textures/long_floor.png",
		nullptr,
		m_long_floor.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile
	(
		device,
		L"Resources/Textures/longH_floor.png",
		nullptr,
		m_longH_floor.ReleaseAndGetAddressOf()
	);

}