//
// GameMain.h
//
#pragma once

#include <Keyboard.h>
#include <Mouse.h>
#include "StepTimer.h"

//前方宣言
class ILoadingScreen;

// シーンの列挙
enum class GAME_SCENE : int
{
    NONE,

    TITLE,
    PLAY,
    LONGMODE,
    LONGMODE_HARD,
    MODE_SELECT,
    RESULT,
};

// 前方宣言
class DebugCamera;

class IScene;

/// <summary>
/// Gameクラスから、ユーザ処理部分を抜き出したクラス
/// </summary>
class GameMain
{
private:

    // キーボード
    std::unique_ptr<DirectX::Keyboard> m_keybord;
    
    // マウス
    std::unique_ptr<DirectX::Mouse> m_mouse;

	// 次のシーン番号
	GAME_SCENE m_nextScene;
 
    // シーン
    IScene* m_pScene;
    
    // ロード画面 
    std::unique_ptr<ILoadingScreen> m_loadingScreen;


public:
    GameMain();
    ~GameMain();

    void Initialize();
    void Update(const DX::StepTimer& timer);
    void Render();
    void Finalize();

	// シーンの作成
	void CreateScene();

	// シーンの削除
	void DeleteScene();
private:
    void LoadResources(bool useLoadingScreen = true);
};
