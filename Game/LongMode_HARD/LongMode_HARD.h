#pragma once

#include "../IScene.h"
#include "Libraries/MyLibraries/Camera.h"
#include"../Player/Player.h"

#include<random>
#include <Model.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>

#include"Libraries/ADX2/Adx2.h"

#include"Resources/Sounds/Public/Helpers/CueSheet_0.h"
#include"Resources/Sounds/Public/Helpers/NewProject_acf.h"

class LongMode_HARD : public IScene
{
private:
	//�t���A��
	static const int FLOOR = 400;

	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//�v���C���[
	std::unique_ptr<Player> m_pPlayer;
	std::unique_ptr<Camera> m_pCamera;

	//���̕�
	std::unique_ptr<DirectX::Model> m_pwall;
	DirectX::SimpleMath::Vector3 m_wall_back;
	//�󒆂̏�
	std::unique_ptr<DirectX::Model> m_pFloor;
	//��
	std::unique_ptr<DirectX::Model> m_pGround;
	//�S�[�����f��
	std::unique_ptr<DirectX::Model> m_pGoal;
	DirectX::SimpleMath::Vector3 m_goalPos;
	//�v���C���[�|�W�V����
	DirectX::SimpleMath::Vector3 m_playerPos;
	//�����
	DirectX::SimpleMath::Vector3 m_floorPos[FLOOR];
	//�摜
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_endless_picture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_fade_picture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jump_Icon;

	//�t�F�[�h�C��
	float m_blackalpha;
	bool m_cflag;
	bool m_Sceneflag;

	bool moveflag = false;
	float movecount = 0.0f;

	ADX2* m_pAdx2;
	int m_musicID;

	int m_playerJcount;
	DirectX::Keyboard::KeyboardStateTracker m_keyTracker;

public:
	LongMode_HARD();
	~LongMode_HARD();

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
	//�����蔻��
	bool HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z);
	//���𓮂�������
	void MoveFloor();
};