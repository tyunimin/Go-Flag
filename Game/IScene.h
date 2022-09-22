/*
�V�[���C���^�[�t�F�[�X
*/
#pragma once

#include "StepTimer.h"
#include "GameMain.h"


class IScene
{
public:

	virtual ~IScene() = default;

	// ������
	virtual void Initialize() = 0;

	// �X�V
	virtual GAME_SCENE Update(const DX::StepTimer& timer) = 0;

	// �`��
	virtual void Draw() = 0;

	// �I������
	virtual void Finalize() = 0;

	//���\�[�X�̓ǂݍ��݊֐�
	virtual void LoadResources() = 0;
};