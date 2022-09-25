#pragma once

#include "../IScene.h"
#include "Libraries/MyLibraries/Camera.h"
#include"../Player/Player.h"

#include<random>
#include <Model.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include"Libraries/ADX2/Adx2.h"

#include"Resources/Sounds/Public/Helpers/CueSheet_0.h"
#include"Resources/Sounds/Public/Helpers/NewProject_acf.h"

class LongMode_HARD : public IScene
{
private:
	//フロア数
	static const int FLOOR = 400;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//プレイヤー
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Camera> m_pCamera;

	//奥の壁
	std::unique_ptr<DirectX::Model> m_pwall;
	DirectX::SimpleMath::Vector3 m_wall_back;
	//空中の床
	std::unique_ptr<DirectX::Model> m_pFloor;
	//床
	std::unique_ptr<DirectX::Model> m_pGround;
	//ゴールモデル
	std::unique_ptr<DirectX::Model> m_pGoal;
	DirectX::SimpleMath::Vector3 m_goalPos;
	//プレイヤーポジション
	DirectX::SimpleMath::Vector3 m_playerPos;
	//床情報
	DirectX::SimpleMath::Vector3 m_floorPos[FLOOR];
	//画像
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_endless_picture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fade_picture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jump_Icon;

	//フェードイン
	float m_blackalpha;
	bool m_cflag;
	bool m_Sceneflag;

	bool moveflag = false;
	float movecount = 0.0f;

	ADX2* m_pAdx2;
	int m_musicID;

	int m_playerJcount;
	DirectX::Keyboard::KeyboardStateTracker m_keyTracker;

public:
	LongMode_HARD();
	~LongMode_HARD();

	// 初期化
	void Initialize() override;
	// 更新
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	// 描画
	void Draw() override;
	// 終了処理
	void Finalize() override;
	// リソースの読み込み
	void LoadResources() override;
	//当たり判定
	bool HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z);
	//床を動かす処理
	void MoveFloor();
};