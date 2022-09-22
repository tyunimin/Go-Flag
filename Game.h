//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Game/GameMain.h"

// �O���錾
class Scene;


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:
    // �萔 /////////////////////////////////////////////////////////
    static const wchar_t* TITLE;    // �Q�[���^�C�g��
    static const int SCREEN_W;      // ��ʃT�C�Y
    static const int SCREEN_H;

private:
    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // �ǉ����������o ///////////////////////////////////////////////
    // �V�[��
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
