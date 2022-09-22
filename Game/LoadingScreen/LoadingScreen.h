/*
ロード画面
*/
#pragma once

#include<thread>
#include<mutex>

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include "../ILoadingScreen.h"


class LoadingScreen : public ILoadingScreen
{
private:

	// リソース読み込み用
	
	std::thread m_loadingThread;
	bool m_isLoading;
	std::mutex m_loadingMutex;


	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// スプライトフォント
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// 背景
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_blackTexture;
	// Loading
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_loadingTexture;
	// ドット
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dotTexture;

	bool m_flag_left;
	bool m_flag_center;
	bool m_flag_right;

	int m_count;
	float m_counter;
public:

	// コンストラクタ
	LoadingScreen();

	// デストラクタ
	~LoadingScreen();

	// 初期化
	void Initialize(IScene* targetScene) override;

	// 更新
	void Update(const DX::StepTimer& timer) override;

	// 描画
	void Draw() override;

	// 終了処理
	void Finalize() override;

	// 読み込み中かどうかの取得
	bool IsLoading();

private:
	// 対象シーンのリソースの読み込み
	void LoadSceneResources(IScene* targetScene) override;

};