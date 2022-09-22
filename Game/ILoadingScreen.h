/*
���[�h���:�C���^�[�t�F�[�X
*/
#pragma once

#include "StepTimer.h"


class IScene;


class ILoadingScreen
{
public:
	virtual ~ILoadingScreen() = default;

	// ������
	virtual void Initialize(IScene* targetScene) = 0;

	// �X�V
	virtual void Update(const DX::StepTimer& timer) = 0;

	// �`��
	virtual void Draw() = 0;

	// �I������
	virtual void Finalize() = 0;

	// �ǂݍ��ݒ����ǂ������擾
	virtual bool IsLoading() = 0;

private:
	// ���\�[�X�̓ǂݍ���
	virtual void LoadSceneResources(IScene* targetScene) = 0;
};
