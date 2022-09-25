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
	m_SceneSelect(0)
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

	// マウス入力情報を取得する
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	if (keyState.Z)
	{
		m_SceneSelect = 1;
		m_cflag = true;
		//効果音再生
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	if (keyState.X)
	{
		m_SceneSelect = 2;
		m_cflag = true;
		//効果音再生
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	if (keyState.C)
	{
		m_SceneSelect = 3;
		m_cflag = true;
		//効果音再生
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
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
	FXMVECTOR color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_alpha };//原色に指定色を掛け合わせる
	FXMVECTOR black = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_blackalpha };
	RECT rect={ 0,0,1280,720 };
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());
	m_spriteBatch->Draw(m_texture_black.Get(), SimpleMath::Vector3{ 0.0f,0.0f,0.0f }, &rect,SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_selectmode.Get(), SimpleMath::Vector3{ 0.0f,0.0f,0.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_pushZ.Get(), SimpleMath::Vector3{ 605.0f,225.0f,0.0f }, &rect, color);
	m_spriteBatch->Draw(m_pushX.Get(), SimpleMath::Vector3{ 640.0f,370.0f,0.0f }, &rect, color);
	m_spriteBatch->Draw(m_pushC.Get(), SimpleMath::Vector3{ 640.0f,555.0f,0.0f }, &rect, color);
	m_spriteBatch->Draw(m_shortmode.Get(), SimpleMath::Vector3{ 100.0f,185.0f,0.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_longmode.Get(), SimpleMath::Vector3{ 100.0f,370.0f,0.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_longmode_hard.Get(), SimpleMath::Vector3{ 100.0f,555.0f,0.0f }, &rect, SimpleMath::Vector4{ 1.0f,1.0f,1.0f,1.0f });
	m_spriteBatch->Draw(m_texture_black.Get(), SimpleMath::Vector3{ 0.0f,0.0f,0.0f }, &rect, black);
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
		L"Resources/Textures/Push_Z.png",
		nullptr,
		m_pushZ.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/PushXButton.png",
		nullptr,
		m_pushX.ReleaseAndGetAddressOf()
	);
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/PushCButton.png",
		nullptr,
		m_pushC.ReleaseAndGetAddressOf()
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
}