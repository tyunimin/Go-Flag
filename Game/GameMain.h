//
// GameMain.h
//
#pragma once

#include <Keyboard.h>
#include <Mouse.h>
#include "StepTimer.h"

//�O���錾
class ILoadingScreen;

// �V�[���̗�
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

// �O���錾
class DebugCamera;

class IScene;

/// <summary>
/// Game�N���X����A���[�U���������𔲂��o�����N���X
/// </summary>
class GameMain
{
private:

    // �L�[�{�[�h
    std::unique_ptr<DirectX::Keyboard> m_keybord;
    
    // �}�E�X
    std::unique_ptr<DirectX::Mouse> m_mouse;

	// ���̃V�[���ԍ�
	GAME_SCENE m_nextScene;
 
    // �V�[��
    IScene* m_pScene;
    
    // ���[�h��� 
    std::unique_ptr<ILoadingScreen> m_loadingScreen;


public:
    GameMain();
    ~GameMain();

    void Initialize();
    void Update(const DX::StepTimer& timer);
    void Render();
    void Finalize();

	// �V�[���̍쐬
	void CreateScene();

	// �V�[���̍폜
	void DeleteScene();
private:
    void LoadResources(bool useLoadingScreen = true);
};
