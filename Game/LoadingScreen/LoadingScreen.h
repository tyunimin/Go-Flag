/*
���[�h���
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

	// ���\�[�X�ǂݍ��ݗp
	
	std::thread m_loadingThread;
	bool m_isLoading;
	std::mutex m_loadingMutex;


	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �X�v���C�g�t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;

	// �w�i
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_blackTexture;
	// Loading
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_loadingTexture;
	// �h�b�g
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dotTexture;

	bool m_flag_left;
	bool m_flag_center;
	bool m_flag_right;

	int m_count;
	float m_counter;
public:

	// �R���X�g���N�^
	LoadingScreen();

	// �f�X�g���N�^
	~LoadingScreen();

	// ������
	void Initialize(IScene* targetScene) override;

	// �X�V
	void Update(const DX::StepTimer& timer) override;

	// �`��
	void Draw() override;

	// �I������
	void Finalize() override;

	// �ǂݍ��ݒ����ǂ����̎擾
	bool IsLoading();

private:
	// �ΏۃV�[���̃��\�[�X�̓ǂݍ���
	void LoadSceneResources(IScene* targetScene) override;

};