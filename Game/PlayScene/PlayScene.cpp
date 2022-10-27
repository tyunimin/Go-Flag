/*
	�v���C�V�[��
*/
#include "pch.h"

#include "DeviceResources.h"

#include "PlayScene.h"

#include <WICTextureLoader.h>
#include<codecvt>
#include<thread>

// ���O��Ԃ̗��p
using namespace DirectX;

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
PlayScene::PlayScene():
	m_position(0.0f),
	m_jump(0.0f),
	m_floorflag(false),
	m_moveflag(false),
	m_playerPos(DirectX::SimpleMath::Vector3::Zero),
	m_cflag(false),
	m_blackalpha(1.0f),
	m_Sceneflag(false),
	m_playerJcount(0)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	m_pDebugCamera = std::make_unique<Camera>();
	m_pPlayer = std::make_unique<Player>();

	//	�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// �X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
PlayScene::~PlayScene()
{
	//�{���̓Q�[�����I������܂�Finalize�����Ă͂����Ȃ�
	m_pAdx2->Finalize();
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void PlayScene::Initialize()
{
	//ADX2�̃C���X�^���X���擾
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2�̏�����
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_EREMOPHILA);
	//�v���C���[������
	m_pPlayer->Initialize();

	std::default_random_engine eng(m_rnd());
	std::uniform_int_distribution<int> distr(-10, 10);
	std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
	std::uniform_real_distribution<float> dis(-7.0f, 7.0f);
	std::uniform_real_distribution<float> di(-7.0f, 3.0f);

	m_groundPos = { 0.0f,-1.0f,0.0f };

	float y = 4.0f;
	for (int i = 0; i < FLOOR; i++)
	{
		std::default_random_engine eng(m_rnd());
		std::uniform_real_distribution<float> di(-5.0f, 5.0f);
		m_floorPos[i] = { di(eng),y,-10.0f };
		y += 4.0f;
	}
	
	m_goalPos = m_floorPos[FLOOR - 1];
	m_goalPos.y += 0.5f;
	m_wall_back = { 0.0f,9.0f,-12.0f };

	m_NumPos = { 400,10 };

	m_CountDown = std::make_unique<CountDown>();
	m_CountDown->Initialize(3000);
}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE PlayScene::Update(const DX::StepTimer& timer)
{
	// �L�[���͏����擾����
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();
	m_pPlayer->Update();
	m_playerPos = m_pPlayer->GetPosition();
	//���̓����蔻��
	if (HitCheck(m_playerPos, SimpleMath::Vector3(0.0f, 0.0f, 0.0f), 12.0f, 0.5f, 12.0f))
	{
		if (m_playerPos.y < -0.02f)
		{
			m_playerPos.y = 0.0f;
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		m_pPlayer->SetPosition(m_playerPos);
	}

	m_CountDown->Update(timer);

	//�S�Ă�AABB��ς���

	//�S�[���|�[��
	if (HitCheck(m_playerPos, m_goalPos, 0.8f, 0.8f, 1.0f))
	{
		m_Sceneflag = true;
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	//�n��
	if (HitCheck(m_playerPos, m_groundPos,12.0f,1.0f,12.0f))
	{
		if (m_playerPos.y < -0.02f)
		{
			m_playerPos.y = 0.0f;
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		m_pPlayer->SetPosition(m_playerPos);
	}
	//���̓����蔻��
	for (int i = 0; i < FLOOR; i++)
	{
		if (HitCheck(m_playerPos, m_floorPos[i], 1.0f, 1.0f, 1.0f))
		{
			if (m_playerPos.y > m_floorPos[i].y)
			{
				m_playerPos.y = m_floorPos[i].y + 1.0f;
				m_pPlayer->SetPosition(m_playerPos);
				m_pPlayer->JumpCountHeal();
				m_playerJcount = 0;
			}
			else
			{
				m_playerPos.y = m_floorPos[i].y - 1.0f;
				m_pPlayer->SetPosition(m_playerPos);
			}
		}
	}
	//���̕�
	if (HitCheck(m_playerPos,m_wall_back,25.0f,100.0f,1.0f))
	{
		m_playerPos.z = m_wall_back.z + 1.0f;
		m_pPlayer->SetPosition(m_playerPos);
	}

	DirectX::SimpleMath::Vector3 Pos = m_pPlayer->GetPosition();

	if (Pos.y < -1.0f)
	{
		m_blackalpha += 0.01f;
	}

	if(m_blackalpha>=0.0f&&m_cflag==false)
	m_blackalpha -= 0.01f;

	if (m_blackalpha < 0.0f&&m_cflag==false)
	{
		m_cflag = true;
		m_blackalpha = 0.0f;
	}
	if(m_Sceneflag==true)
		m_blackalpha += 0.01f;

	if (m_blackalpha >= 1.1f&&m_Sceneflag==false)
		return GAME_SCENE::TITLE;
	if (m_blackalpha >= 1.0f && m_Sceneflag == true)
		return GAME_SCENE::RESULT;

	//�L�[�{�[�h���
	m_keyTracker.Update(keyState);
	if (m_keyTracker.IsKeyPressed(Keyboard::Space))
		m_playerJcount++;
	if (m_playerJcount > 1)
		m_playerJcount = 2;

	m_pPlayer->GetJumpCount(m_playerJcount);

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
�`��
--------------------------------------------------*/
void PlayScene::Draw()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//	�E�B���h�E�T�C�Y�̎擾
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//	�ϊ��s��̐錾(���[���h�A�r���[�A�ˉe(�v���W�F�N�V����))
	DirectX::SimpleMath::Matrix world, view, projection;
	//	���[���h�s��̏�����(�P�ʍs��)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();	
	projection = m_pDebugCamera->GetProjectionMatrix();

	m_pDebugCamera->SetCameraTarget(m_playerPos);

	FXMVECTOR color = Colors::White;				//���F�Ɏw��F���|�����킹��
	float rotation = 0.0f;							//��]�p�x�����W�A���Ŏw�肷��
	DirectX::SimpleMath::Vector2 origin = { 0,0 };	//��]���钆�S�_�̎w��
	float scale = 2.0f;								//�g�嗦�̎w��
	SpriteEffects effects = SpriteEffects_None;		//�\�����]����
	float layerDepth = 0.0f;						//�\���[�x

	RECT rect = { 0,0,1280,720 };
	RECT rec = { 0,0,50, 600-m_playerPos.y/4.0f*(600/10)};

	//-----�S�[��----------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world = DirectX::SimpleMath::Matrix::CreateScale(0.001f, 0.001f, 0.001f);
	world = DirectX::SimpleMath::Matrix::CreateTranslation(m_goalPos.x, m_goalPos.y, m_goalPos.z);
	m_pGoal->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//���̕�---------------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(25.0f, 80.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_wall_back.x, m_wall_back.y, m_wall_back.z);
	m_pwall->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------

	//-----��--------------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world=DirectX::SimpleMath::Matrix::CreateTranslation(0.0f,-1.0f,0.0f);
	m_pGround->Draw(context, *m_commonState.get(), world, view, projection);
	//---------------------------------------------------------------------------------------
	//�󒆂̏�----------------------------------------------------------------
	for (int i = 0; i < FLOOR; i++)
	{
		world = DirectX::SimpleMath::Matrix::Identity;
		world *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
		world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_floorPos[i].x, m_floorPos[i].y, m_floorPos[i].z);
		m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);
	}
	//------------------------------------------------------------------------

	world = DirectX::SimpleMath::Matrix::Identity;
	m_pFloor->Draw(context, *m_commonState.get(), world, view, projection);

	m_pPlayer->Render(view,projection);

	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_texture_black.Get(), DirectX::SimpleMath::Vector2{ 0,0 }, &rect , 
		DirectX::SimpleMath::Vector4{1.0f,1.0f,1.0f,m_blackalpha}
		, rotation, origin,scale, effects, layerDepth);
	m_spriteBatch->Draw(m_shortmode_texture.Get(), SimpleMath::Vector2{ 0,0 },&rect);

	switch (m_playerJcount)
	{
	case 0:
		//2�\��
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	case 1:
		//1�\��														 
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	case 2:
		//0�\��														 
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(740, 550),
			&rect, SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f }, 0.8f);
		break;
	}
	m_spriteBatch->Draw(m_green_bar.Get(), SimpleMath::Vector2(1220, 60));
	m_spriteBatch->Draw(m_red_bar.Get(), SimpleMath::Vector2(1220, 60),&rec);
	m_spriteBatch->End();

	m_CountDown->Draw();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void PlayScene::Finalize()
{
}

bool PlayScene::HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z)
{
	//XYZ���W�Ōv�Z�����āA�I�u�W�F�N�g���m�̓����蔻������A�������Ă�����true��Ԃ�
	if (object1.x > object2.x - x && object1.x < object2.x + x && object1.y>object2.y - y && object1.y < object2.y + y && object1.z>object2.z - z && object1.z < object2.z + z)
	{
		return true;
	}
	return false;
}

void PlayScene::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();

	std::this_thread::sleep_for(std::chrono::seconds(0));

	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_texture_black.ReleaseAndGetAddressOf()
	);

	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Short.png",
		nullptr,
		m_shortmode_texture.ReleaseAndGetAddressOf()
	);
	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/JUMP_Icon.png",
		nullptr,
		m_jump_Icon.ReleaseAndGetAddressOf()
	);	
	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/green_bar.png",
		nullptr,
		m_green_bar.ReleaseAndGetAddressOf()
	);
	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/red_bar.png",
		nullptr,
		m_red_bar.ReleaseAndGetAddressOf()
	);

	
	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* Goalfac = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	Goalfac->SetDirectory(L"Resources/Models");
	m_pGoal = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Goalpoal.cmo",
		*Goalfac
	);
	delete Goalfac;

	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* Groundfac = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	Groundfac->SetDirectory(L"Resources/Models");
	m_pGround = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Ground.cmo",
		*Groundfac
	);
	delete Groundfac;

	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* Floorfac = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	Floorfac->SetDirectory(L"Resources/Models");
	m_pFloor = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/floor.cmo",
		*Floorfac
	);
	delete Floorfac;

	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* wallfac = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	wallfac->SetDirectory(L"Resources/Models");
	m_pwall = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/wall.cmo",
		*wallfac
	);
	delete wallfac;

}