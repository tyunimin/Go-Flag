/*
ロード画面:インターフェース
*/
#pragma once

#include "StepTimer.h"


class IScene;


class ILoadingScreen
{
public:
	virtual ~ILoadingScreen() = default;

	// 初期化
	virtual void Initialize(IScene* targetScene) = 0;

	// 更新
	virtual void Update(const DX::StepTimer& timer) = 0;

	// 描画
	virtual void Draw() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	// 読み込み中かどうかを取得
	virtual bool IsLoading() = 0;

private:
	// リソースの読み込み
	virtual void LoadSceneResources(IScene* targetScene) = 0;
};
