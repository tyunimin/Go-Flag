/*
�v���C�V�[��
*/
#pragma once

#include <CommonStates.h>

#include "../IScene.h"
#include"../Player/Player.h"
#include "Libraries/MyLibraries/Camera.h"
#include "../Object/CountDown.h"

#include <Model.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include<random>

#include"Game/Object/Move_floor.h"

#include"Libraries/ADX2/Adx2.h"

#include"Resources/Sounds/Public/Helpers/CueSheet_0.h"
#include"Resources/Sounds/Public/Helpers/NewProject_acf.h"

class PlayScene : public IScene
{
private:
	// �R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �X�v���C�g�t�H���g
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	//�J����
	std::unique_ptr<Camera> m_pDebugCamera;

	std::unique_ptr<Player> m_pPlayer;
	DirectX::SimpleMath::Vector3 m_playerPos;
	
	std::unique_ptr<DirectX::Model> m_pGoal;
	DirectX::SimpleMath::Vector3 m_goalPos;

	std::unique_ptr<DirectX::Model> m_pGround;
	DirectX::SimpleMath::Vector3 m_groundPos;

	std::unique_ptr<DirectX::Model> m_pFloor;
	DirectX::SimpleMath::Vector3 m_floorPos;
	DirectX::SimpleMath::Vector3 m_skyfloorPos;
	DirectX::SimpleMath::Vector3 m_skyfloorPos2;
	DirectX::SimpleMath::Vector3 m_skyfloorPos3;
	DirectX::SimpleMath::Vector3 m_skyfloorPos4;
	DirectX::SimpleMath::Vector3 m_skyfloorPos5;
	DirectX::SimpleMath::Vector3 m_skyfloorPos6;
	DirectX::SimpleMath::Vector3 m_skyfloorPos7;
	DirectX::SimpleMath::Vector3 m_skyfloorPos8;
	DirectX::SimpleMath::Vector3 m_skyfloorPos9;

	std::unique_ptr<Move_floor> m_Mfloor;
	DirectX::SimpleMath::Vector3 m_Movefloor;

	std::unique_ptr<DirectX::Model> m_pStairs;

	std::unique_ptr<DirectX::Model> m_pwall;

	DirectX::SimpleMath::Vector3 m_wall_back;
	DirectX::SimpleMath::Vector3 m_moveWall;

	DirectX::SimpleMath::Vector3 m_position;

	bool m_floorflag;
	bool m_moveflag;

	float m_jump;

	std::random_device m_rnd;

	DX::StepTimer m_count;
	DirectX::SimpleMath::Vector2 m_NumPos;

	float m_blackalpha;
	bool m_cflag;
	bool m_Sceneflag;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture_black;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shortmode_texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_jump_Icon;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_green_bar;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_red_bar;

	ADX2* m_pAdx2;
	int m_musicID;

	int m_playerJcount;
	DirectX::Keyboard::KeyboardStateTracker m_keyTracker;

	std::unique_ptr<CountDown> m_CountDown;
public:
	// �R���X�g���N�^
	PlayScene();

	// �f�X�g���N�^
	~PlayScene();

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

	bool HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z);
};