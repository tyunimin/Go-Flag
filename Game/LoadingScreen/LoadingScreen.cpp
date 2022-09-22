/*
ロード画面
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "LoadingScreen.h"

#include "DeviceResources.h"
#include "../IScene.h"

using namespace DirectX;


/*--------------------------------------------------
コンストラクタ
--------------------------------------------------*/
LoadingScreen::LoadingScreen() :
	m_isLoading{ false },
	m_flag_left{false},
	m_flag_center{false},
	m_flag_right{false},
	m_count{0},
	m_counter{0.0f}
{
}

/*--------------------------------------------------
デストラクタ
--------------------------------------------------*/
LoadingScreen::~LoadingScreen()
{
	//ロード中からロード待ち
	if (m_loadingThread.joinable())
	{
		m_loadingThread.join();
	}
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void LoadingScreen::Initialize(IScene* targetScene)
{
	// 対象シーンのリソースの読み込み
	LoadSceneResources(targetScene);


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
void LoadingScreen::Update(const DX::StepTimer& timer)
{
	float time = timer.GetElapsedSeconds();

	m_counter++;
	if (m_counter > 13.0f)
		m_counter = 0.0f;

	if (m_counter >= 12.5f){ m_count++; }

	switch (m_count)
	{
	case 1:
		m_flag_left = true;
		break;
	case 2:
		m_flag_center = true;
		break;
	case 3:
		m_flag_right = true;
		break;
	case 4:
		m_flag_left = false;
		m_flag_center = false;
		m_flag_right = false;
		m_count = 0;
	}
}

/*--------------------------------------------------
描画
--------------------------------------------------*/
void LoadingScreen::Draw()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());

	//背景
	DirectX::SimpleMath::Vector2 pos(0.0f, 0.0f);
	m_spriteBatch->Draw(m_blackTexture.Get(), pos);
	//loading
	DirectX::SimpleMath::Vector2 position(600.0f, 520.0f);
	m_spriteBatch->Draw(m_loadingTexture.Get(), position);

	if (m_flag_left)
	{
		//ドット	左
		DirectX::SimpleMath::Vector2 point(1130.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), point);
	}
	if (m_flag_center)
	{
		//ドット	中央
		DirectX::SimpleMath::Vector2 posi(1160.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), posi);
	}
	if (m_flag_right)
	{
		//ドット	右
		DirectX::SimpleMath::Vector2 post(1190.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), post);
	}

	m_spriteBatch->End();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void LoadingScreen::Finalize()
{
}

/*--------------------------------------------------
	// 読み込み中かどうかの取得
--------------------------------------------------*/
bool LoadingScreen::IsLoading()
{
	m_loadingMutex.lock();
	bool isLoading = m_isLoading;
	m_loadingMutex.unlock();
	return isLoading;
}

/*--------------------------------------------------
シーンのリソースの読み込み
--------------------------------------------------*/
void LoadingScreen::LoadSceneResources(IScene* targetScene)
{
	if (m_loadingThread.joinable())
	{
		m_loadingThread.join();
	}

	m_isLoading = true;
	m_loadingThread = std::thread(
		[=]
		{
			targetScene->LoadResources();

			m_loadingMutex.lock();
			m_isLoading = false;
			m_loadingMutex.unlock();
		});

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();

	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Black.png",
		nullptr,
		m_blackTexture.ReleaseAndGetAddressOf()
	);
	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Loading.png",
		nullptr,
		m_loadingTexture.ReleaseAndGetAddressOf()
	);
	// テクスチャの読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Dot.png",
		nullptr,
		m_dotTexture.ReleaseAndGetAddressOf()
	);
}