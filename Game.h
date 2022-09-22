//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Game/GameMain.h"

// 前方宣言
class Scene;


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
    // 定数 /////////////////////////////////////////////////////////
    static const wchar_t* TITLE;    // ゲームタイトル
    static const int SCREEN_W;      // 画面サイズ
    static const int SCREEN_H;

private:
    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // 追加したメンバ ///////////////////////////////////////////////
    // シーン
    std::unique_ptr<GameMain>               m_gameMain;

public:

private:
    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

public:
    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
};
