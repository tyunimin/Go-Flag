/*
	プレイシーン
*/
#include "pch.h"

#include "DeviceResources.h"

#include "PlayScene.h"

#include <WICTextureLoader.h>
#include<codecvt>
#include<thread>

// 名前空間の利用
using namespace DirectX;

/*--------------------------------------------------
コンストラクタ
--------------------------------------------------*/
PlayScene::PlayScene():
	m_position(0.0f),
	m_jump(0.0f),
	m_floorflag(false),
	m_moveflag(false),
	m_playerPos(DirectX::SimpleMath::Vector3::Zero),
	m_cflag(false),
	m_blackalpha(1.0f),
	m_Sceneflag(false),
	m_playerJcount(0)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	m_pDebugCamera = std::make_unique<Camera>();
	m_pPlayer = std::make_unique<Player>();
	m_Mfloor = std::make_unique<Move_floor>();

	//	コモンステート::D3Dレンダリング状態オブジェクト
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// スプライトバッチ::デバッグ情報の表示に必要
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");
}

/*--------------------------------------------------
デストラクタ
--------------------------------------------------*/
PlayScene::~PlayScene()
{
	//本来はゲームが終了するまでFinalizeをしてはいけない
	m_pAdx2->Finalize();
}

/*--------------------------------------------------
初期化
--------------------------------------------------*/
void PlayScene::Initialize()
{
	//ADX2のインスタンスを取得
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2の初期化
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_EREMOPHILA);
	//プレイヤー初期化
	m_pPlayer->Initialize();

	std::default_random_engine eng(m_rnd());
	std::uniform_int_distribution<int> distr(-10, 10);
	std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
	std::uniform_real_distribution<float> dis(-7.0f, 7.0f);
	std::uniform_real_distribution<float> di(-7.0f, 3.0f);

	m_floorPos.x = distr(eng);

	m_goalPos = { 5.0f,1.6f,-10.0f };
	m_floorPos = { 0.0f,40.0f,-10.0f };
	m_skyfloorPos = { 2.0f,2.5f,-10.0f };
	m_skyfloorPos2 = { 2.0f,7.0f,-10.0f };
	m_skyfloorPos3 = {dist(eng),11.0f,-10.0f };
	m_skyfloorPos4 = {dis(eng),15.0f,-11.0f };
	m_skyfloorPos5 = {dist(eng),19.0f,-10.0f };
	m_skyfloorPos6 = { di(eng),21.0f,-10.0f };
	m_skyfloorPos7 = { di(eng),25.0f,-10.0f };
	m_skyfloorPos8 = { di(eng),30.0f,-10.0f };
	m_skyfloorPos9 = { di(eng),35.0f,-10.0f };

	m_groundPos = { 0.0f,-1.0f,0.0f };

	m_goalPos.y = m_floorPos.y + 0.5f;
	m_goalPos.x = m_floorPos.x;
	m_goalPos.z = m_floorPos.z;

	m_wall_back = { 0.0f,9.0f,-12.0f };

	m_NumPos = { 400,10 };

	m_Mfloor->Initialize(SimpleMath::Vector3(0.0f, 4.0f, 0.0f), 1);

	m_CountDown = std::make_unique<CountDown>();
	m_CountDown->Initialize(3000);
}

/*--------------------------------------------------
更新
戻り値	:次のシーン番号
--------------------------------------------------*/
GAME_SCENE PlayScene::Update(const DX::StepTimer& timer)
{
	// キー入力情報を取得する
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();
	m_pPlayer->Update();
	m_playerPos = m_pPlayer->GetPosition();
	m_Mfloor->Update(1);
	m_Movefloor = m_Mfloor->GetPos(1);
	float vel = m_Mfloor->Move(1);
	if (HitCheck(m_playerPos, m_Movefloor, 1.0f, 1.0f, 1.0f))
	{
		if (m_playerPos.y > m_Movefloor.y)
		{
			m_playerPos.y = m_Movefloor.y + 1.0f;
			m_playerPos.x += vel+vel;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_Movefloor.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	m_CountDown->Update(timer);

	//全てのAABBを変える

	//ゴールポール
	if (HitCheck(m_playerPos, m_goalPos, 0.8f, 0.8f, 1.0f))
	{
		m_Sceneflag = true;
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	//空中床
	if(HitCheck(m_playerPos,m_floorPos,1.0f,1.0f,1.0f))
	{
			m_playerPos.y = m_floorPos.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
	}

	//一番下空中床
	if (HitCheck(m_playerPos, m_skyfloorPos, 1.0f, 1.0f, 2.0f))
	{
		if (m_playerPos.y > m_skyfloorPos.y)
		{
			m_playerPos.y = m_skyfloorPos.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から2番目空中床
	if(HitCheck(m_playerPos,m_skyfloorPos2,2.8f,1.0f,1.3f))
	{
		if (m_playerPos.y > m_skyfloorPos2.y)
		{
			m_playerPos.y = m_skyfloorPos2.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos2.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から3番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos3, 0.8f, 1.0f, 2.5f))
	{
		if (m_playerPos.y > m_skyfloorPos3.y)
		{
			m_playerPos.y = m_skyfloorPos3.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos3.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}
	
	//下から4番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos4, 1.6f, 1.0f, 2.3f)) 
	{
		if (m_playerPos.y > m_skyfloorPos4.y)
		{
			m_playerPos.y = m_skyfloorPos4.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos4.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}
	

	//下から5番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos5, 1.6f, 1.0f, 2.3f))
	{
		if (m_playerPos.y > m_skyfloorPos5.y)
		{
			m_playerPos.y = m_skyfloorPos5.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos5.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から5番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos6, 1.6f, 1.0f, 2.3f))
	{
		if (m_playerPos.y > m_skyfloorPos6.y)
		{
			m_playerPos.y = m_skyfloorPos6.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos6.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から6番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos7, 1.6f, 1.0f, 2.3f))
	{
		if (m_playerPos.y > m_skyfloorPos7.y)
		{
			m_playerPos.y = m_skyfloorPos7.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos7.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から7番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos8, 1.6f, 1.0f, 2.3f))
	{
		if (m_playerPos.y > m_skyfloorPos8.y)
		{
			m_playerPos.y = m_skyfloorPos8.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos8.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//下から8番目空中床
	if (HitCheck(m_playerPos, m_skyfloorPos9, 1.6f, 1.0f, 2.3f))
	{
		if (m_playerPos.y > m_skyfloorPos9.y)
		{
			m_playerPos.y = m_skyfloorPos9.y + 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		else
		{
			m_playerPos.y = m_skyfloorPos9.y - 1.0f;
			m_pPlayer->SetPosition(m_playerPos);
		}
	}

	//地面
	if (HitCheck(m_playerPos, m_groundPos,12.0f,1.0f,12.0f))
	{
		if (m_playerPos.y < -0.02f)
		{
			m_playerPos.y = 0.0f;
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		m_pPlayer->SetPosition(m_playerPos);
	}

	//奥の壁
	if (HitCheck(m_playerPos,m_wall_back,25.0f,100.0f,1.0f))
	{
		m_playerPos.z = m_wall_back.z + 1.0f;
		m_pPlayer->SetPosition(m_playerPos);
	}

	DirectX::SimpleMath::Vector3 Pos = m_pPlayer->GetPosition();

	if (Pos.y < -1.0f)
	{
		m_blackalpha += 0.01f;
	}

	if(m_blackalpha>=0.0f&&m_cflag==false)
	m_blackalpha -= 0.01f;

	if (m_blackalpha < 0.0f&&m_cflag==false)
	{
		m_cflag = true;
		m_blackalpha = 0.0f;
	}
	if(m_Sceneflag==true)
		m_blackalpha += 0.01f;

	if (m_blackalpha >= 1.1f&&m_Sceneflag==false)
		return GAME_SCENE::TITLE;
	if (m_blackalpha >= 1.0f && m_Sceneflag == true)
		return GAME_SCENE::RESULT;

	//キーボード情報
	m_keyTracker.Update(keyState);
	if (m_keyTracker.IsKeyPressed(Keyboard::Space))
		m_playerJcount++;
	if (m_playerJcount > 1)
		m_playerJcount = 2;

	m_pPlayer->GetJumpCount(m_playerJcount);

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
描画
--------------------------------------------------*/
void PlayScene::Draw()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//	ウィンドウサイズの取得
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//	変換行列の宣言(ワールド、ビュー、射影(プロジェクション))
	DirectX::SimpleMath::Matrix world, view, projection;
	//	ワールド行列の初期化(単位行列)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();	
	projection = m_pDebugCamera->GetProjectionMatrix();

	m_pDebugCamera->SetCameraTarget(m_playerPos);

	FXMVECTOR color = Colors::White;				//原色に指定色を掛け合わせる
	float rotation = 0.0f;							//回転角度をラジアンで指定する
	DirectX::SimpleMath::Vector2 origin = { 0,0 };	//回転する中心点の指定
	float scale = 2.0f;								//拡大率の指定
	SpriteEffects effects = SpriteEffects_None;		//表示反転効果
	float layerDepth = 0.0f;						//表示深度

	RECT rect = { 0,0,1280,720 };
	RECT rec = { 0,0,50, 600-m_playerPos.y/4.0f*(600/10)};

	//-----ゴール----------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world = DirectX::SimpleMath::Matrix::CreateScale(0.001f, 0.001f, 0.001f);
	world = DirectX::SimpleMath::Matrix::CreateTranslation(m_goalPos.x, m_goalPos.y, m_goalPos.z);
	m_pGoal->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//奥の壁---------------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(25.0f, 80.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_wall_back.x, m_wall_back.y, m_wall_back.z);
	m_pwall->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//-----床--------------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world=DirectX::SimpleMath::Matrix::CreateTranslation(0.0f,-1.0f,0.0f);
	m_pGround->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---空中の床----------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_floorPos.x, m_floorPos.y, m_floorPos.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---一番下の空中の床--------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 3.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos.x, m_skyfloorPos.y, m_skyfloorPos.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から2番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(5.0f, 1.0f, 2.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos2.x, m_skyfloorPos2.y, m_skyfloorPos2.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から3番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos3.x, m_skyfloorPos3.y, m_skyfloorPos3.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から3番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(3.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos4.x, m_skyfloorPos4.y, m_skyfloorPos4.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から4番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos5.x, m_skyfloorPos5.y, m_skyfloorPos5.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から5番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos6.x, m_skyfloorPos6.y, m_skyfloorPos6.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から6番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos7.x, m_skyfloorPos7.y, m_skyfloorPos7.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から6番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(3.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos8.x, m_skyfloorPos8.y, m_skyfloorPos8.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//---下から6番目の空中の床---------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(2.0f, 1.0f, 4.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_skyfloorPos9.x, m_skyfloorPos9.y, m_skyfloorPos9.z);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	world = DirectX::SimpleMath::Matrix::Identity;
	world *= SimpleMath::Matrix::CreateTranslation(m_Movefloor);
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);

	m_pPlayer->Render(view,projection);

	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_texture_black.Get(), DirectX::SimpleMath::Vector2{ 0,0 }, &rect , 
		DirectX::SimpleMath::Vector4{1.0f,1.0f,1.0f,m_blackalpha}
		, rotation, origin,scale, effects, layerDepth);
	m_spriteBatch->Draw(m_shortmode_texture.Get(), SimpleMath::Vector2{ 0,0 },&rect);

	switch (m_playerJcount)
	{
	case 0:
		//2個表示
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	case 1:
		//1個表示														 
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	case 2:
		//0個表示														 
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	}
	m_spriteBatch->Draw(m_green_bar.Get(), SimpleMath::Vector2(1220, 60));
	m_spriteBatch->Draw(m_red_bar.Get(), SimpleMath::Vector2(1220, 60),&rec);
	m_spriteBatch->End();

	//m_spriteBatch->Begin
	//(DirectX::SpriteSortMode_Deferred,
	//	m_commonState->NonPremultiplied());
	//std::string strnum = std::to_string(m_playerPos.y);
	//std::wstring_convert
	//	<std::codecvt_utf8<wchar_t>, wchar_t> cv;
	//std::wstring wsnum = cv.from_bytes(strnum);
	//m_spriteFont->DrawString
	//(m_spriteBatch.get(),wsnum.c_str(),
	//	DirectX::SimpleMath::Vector2(0, 100));

	//m_spriteBatch->End();

	m_CountDown->Draw();
}

/*--------------------------------------------------
終了処理
--------------------------------------------------*/
void PlayScene::Finalize()
{
}

bool PlayScene::HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z)
{
	//XYZ座標で計算をして、オブジェクト同士の当たり判定を取り、当たっていたらtrueを返す
	if (object1.x > object2.x - x && object1.x < object2.x + x && object1.y>object2.y - y && object1.y < object2.y + y && object1.z>object2.z - z && object1.z < object2.z + z)
	{
		return true;
	}
	return false;
}

void PlayScene::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3Dデバイスとデバイスコンテキストの取得
	ID3D11Device1* device = pDR->GetD3DDevice();

	std::this_thread::sleep_for(std::chrono::seconds(0));

	// 画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_texture_black.ReleaseAndGetAddressOf()
	);

	// 画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Short.png",
		nullptr,
		m_shortmode_texture.ReleaseAndGetAddressOf()
	);
	// 画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/JUMP_Icon.png",
		nullptr,
		m_jump_Icon.ReleaseAndGetAddressOf()
	);	
	// 画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/green_bar.png",
		nullptr,
		m_green_bar.ReleaseAndGetAddressOf()
	);
	// 画像の読み込み
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/red_bar.png",
		nullptr,
		m_red_bar.ReleaseAndGetAddressOf()
	);

	// エフェクトファクトリの作成
	EffectFactory* Goalfac = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	Goalfac->SetDirectory(L"Resources/Models");
	m_pGoal = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Goalpoal.cmo",
		*Goalfac
	);
	delete Goalfac;

	// エフェクトファクトリの作成
	EffectFactory* Groundfac = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	Groundfac->SetDirectory(L"Resources/Models");
	m_pGround = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Ground.cmo",
		*Groundfac
	);
	delete Groundfac;

	// エフェクトファクトリの作成
	EffectFactory* Floorfac = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	Floorfac->SetDirectory(L"Resources/Models");
	m_pFloor = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/floor.cmo",
		*Floorfac
	);
	delete Floorfac;

	// エフェクトファクトリの作成
	EffectFactory* wallfac = new EffectFactory(pDR->GetD3DDevice());
	// テクスチャの読み込みパス指定
	wallfac->SetDirectory(L"Resources/Models");
	m_pwall = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/wall.cmo",
		*wallfac
	);
	delete wallfac;

}