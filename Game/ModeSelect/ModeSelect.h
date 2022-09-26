#pragma once
#include"Game/IScene.h"

#include <SpriteBatch.h>
#include <CommonStates.h>

#include"Libraries/ADX2/Adx2.h"

#include"Resources/Sounds/Public/Helpers/CueSheet_0.h"
#include"Resources/Sounds/Public/Helpers/NewProject_acf.h"

class ModeSelect :public IScene
{
private:
	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//�摜
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectmode;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shortmode;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_longmode;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_longmode_hard;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture_black;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_select;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_short_floor;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_long_floor;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_longH_floor;

	DirectX::Keyboard::KeyboardStateTracker m_keyTracker;

	float m_alpha;
	float m_blackalpha;
	bool m_flag;
	bool m_cflag;

	int m_SceneSelect;

	ADX2* m_pAdx2;
	int m_musicID;
public:
	ModeSelect();
	~ModeSelect();

	// ������
	void Initialize() override;
	// �X�V
	GAME_SCENE Update(const DX::StepTimer& timer) override;
	// �`��
	void Draw() override;
	// �I������
	void Finalize() override;
	// ���\�[�X�̓ǂݍ���
	void LoadResources() override;
};