/*
	�G���h���X�V�[��
*/
#include"pch.h"

#include "DeviceResources.h"
#include"LongMode_HARD.h"

#include <WICTextureLoader.h>
#include<thread>
#include<codecvt>
#include<vector>

// ���O��Ԃ̗��p
using namespace DirectX;

LongMode_HARD::LongMode_HARD():
	m_playerPos{SimpleMath::Vector3::Zero},
	m_wall_back{SimpleMath::Vector3::Zero},
	m_floorPos{SimpleMath::Vector3::Zero},
	m_goalPos{SimpleMath::Vector3::Zero},
	m_cflag(false),
	m_blackalpha(1.0f),
	m_Sceneflag(false),
	movecount(0.0f),
	moveflag(false),
	m_playerJcount(0)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();
	//	�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	m_pPlayer = std::make_unique<Player>();
	m_pCamera = std::make_unique<Camera>();
	// �X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

LongMode_HARD::~LongMode_HARD()
{
	//�{���̓Q�[�����I������܂�Finalize�����Ă͂����Ȃ�
	m_pAdx2->Finalize();
}

void LongMode_HARD::Initialize()
{
	//ADX2�̃C���X�^���X���擾
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2�̏�����
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_EREMOPHILA);

	m_pPlayer->Initialize();
	//���̕�(�|�W�V����)
	m_wall_back = { 0.0f,10.0f,-11.5f };
	float y = 4.0f;

	std::random_device rnd;
	for (int i = 0; i < FLOOR; i++)
	{
		std::default_random_engine eng(rnd());
		std::uniform_real_distribution<float> di(-5.0f, 5.0f);
		m_floorPos[i] = { di(eng),y,-10.0f };
		y += 4.0f;
	}
	m_goalPos = m_floorPos[FLOOR-1];
	m_goalPos.y += 0.5f;
}

GAME_SCENE LongMode_HARD::Update(const DX::StepTimer& timer)
{
	m_pPlayer->Update();
	m_playerPos = m_pPlayer->GetPosition();	
	//���𓮂�������
	MoveFloor();
	//���̓����蔻��
	if (HitCheck(m_playerPos, SimpleMath::Vector3(0.0f, -1.0f, 0.0f), 12.0f, 1.0f, 12.0f))
	{
		if (m_playerPos.y < -0.02f)
		{
			m_playerPos.y = 0.0f;
			m_pPlayer->JumpCountHeal();
			m_playerJcount = 0;
		}
		m_pPlayer->SetPosition(m_playerPos);
	}
	//���̕ǂ̓����蔻��
	if (HitCheck(m_playerPos, m_wall_back, 25.0f, 2000.0f, 1.0f))
	{
		m_playerPos.z = m_wall_back.z + 1.0f;
		m_pPlayer->SetPosition(m_playerPos);
	}

	//�����Ă鏰�̓����蔻��
	for (int i = 0; i < FLOOR; i++)
	{
		if (HitCheck(m_playerPos, m_floorPos[i], 1.0f, 1.0f, 1.0f))
		{
			if (m_playerPos.y > m_floorPos[i].y)
			{
				m_pPlayer->JumpCountHeal();
				m_playerJcount = 0;
				m_playerPos.y = m_floorPos[i].y + 1.0f;
				m_pPlayer->SetPosition(m_playerPos);
			}
			else
			{
				m_playerPos.y = m_floorPos[i].y - 1.0f;
				m_pPlayer->SetPosition(m_playerPos);
			}
		}
	}
	//�S�[���|�[���̓����蔻��
	if (HitCheck(m_playerPos, m_goalPos, 0.8f, 0.8f, 1.0f))
	{
		m_Sceneflag = true;
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	//�t�F�[�h�֌W--------------------------------------------------
	SimpleMath::Vector3 Pos = m_playerPos;
	if (Pos.y < -1.0f)
		{m_blackalpha += 0.01f;}
	if (m_Sceneflag == true)
		m_blackalpha += 0.01f;
	if (m_blackalpha >= 0.0f && m_cflag == false)
		{m_blackalpha -= 0.01f;}
	if (m_blackalpha < 0.0f && m_cflag == false)
	{
		m_cflag = true;
		m_blackalpha = 0.0f;
	}
	if (m_blackalpha >= 1.1f && m_Sceneflag == false)
		return GAME_SCENE::TITLE;
	if (m_blackalpha >= 1.0f && m_Sceneflag == true)
		return GAME_SCENE::RESULT;
	//--------------------------------------------------------------
		
	//�L�[�{�[�h���
	Keyboard::State keyState = Keyboard::Get().GetState();
	m_keyTracker.Update(keyState);

	if (m_keyTracker.IsKeyPressed(Keyboard::Space))
		m_playerJcount++;
	if (m_playerJcount > 1)
		m_playerJcount = 2;

	m_pPlayer->GetJumpCount(m_playerJcount);

	return GAME_SCENE::NONE;
}

void LongMode_HARD::Draw()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Matrix world, view, projection;
	//	���[���h�s��̏�����(�P�ʍs��)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pCamera->GetViewMatrix();
	projection = m_pCamera->GetProjectionMatrix();
	m_pCamera->SetCameraTarget(m_playerPos);
	RECT rec = { 0,0,50, 600 - m_playerPos.y / 4.0f * (600 / FLOOR) };

	//�S�[���|�[��-----------------------------------------------------------
	world = SimpleMath::Matrix::Identity;
	world = SimpleMath::Matrix::CreateTranslation(m_goalPos);
	m_pGoal->Draw(context, *m_commonState.get(), world, view, projection);
	//-----------------------------------------------------------------------

	//��----------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, -1.0f, 0.0f);
	m_pGround->Draw(context, *m_commonState.get(), world, view, projection);
	//------------------------------------------------------------------------

	//���̕�---------------------------------------------------------------------------------
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(25.0f, 2000.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_wall_back.x, m_wall_back.y+980.0f, m_wall_back.z);
	m_pwall->Draw(context, *m_commonState.get(), world, view, projection);
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

	m_pPlayer->Render(view, projection);

	RECT rect = { 0,0,1280,720 };
	m_spriteBatch->Begin();
	switch (m_playerJcount)
	{
	case 0:
		//2�\��
		m_spriteBatch->Draw(m_jump_Icon.Get(), SimpleMath::Vector2(540, 550), 
			&rect, SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, SimpleMath::Vector2{ 0.0f,0.0f },0.8f);
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

	//���[�h�\��
	m_spriteBatch->Draw(m_endless_picture.Get(), SimpleMath::Vector2(0, 0));
	m_spriteBatch->Draw(m_green_bar.Get(), SimpleMath::Vector2(1220, 60));
	m_spriteBatch->Draw(m_red_bar.Get(), SimpleMath::Vector2(1220, 60), &rec);
	m_spriteBatch->Draw(m_fade_picture.Get(), SimpleMath::Vector2{ 0,0 }, &rect, 
		SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_blackalpha });
	m_spriteBatch->End();
}

void LongMode_HARD::Finalize()
{
}

void LongMode_HARD::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	//�e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/LONGMODE_HARD.png",
		nullptr,
		m_endless_picture.ReleaseAndGetAddressOf()
	);
	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_fade_picture.ReleaseAndGetAddressOf()
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
//���𓮂�������
void LongMode_HARD::MoveFloor()
{
	for (int i = 0; i < FLOOR; i++)
	{
		if (moveflag == false)
		{
			movecount += 0.001f;
			m_floorPos[i].x += movecount;
			if (HitCheck(m_playerPos, m_floorPos[i], 1.0f, 1.0f, 1.0f))
			{
				m_playerPos.x += movecount;
			}
		}
		if (movecount > 0.1f)
			moveflag = true;
		if (moveflag == true)
		{
			movecount -= 0.001f;
			m_floorPos[i].x += movecount;
			if (HitCheck(m_playerPos, m_floorPos[i], 1.0f, 1.0f, 1.0f))
			{
				m_playerPos.x += movecount;
			}
		}
		if (movecount < -0.1f)
			moveflag = false;
	}
	m_goalPos.x = m_floorPos[FLOOR-1].x;
}

//�����蔻��
bool LongMode_HARD::HitCheck(DirectX::SimpleMath::Vector3 object1, DirectX::SimpleMath::Vector3 object2, float x, float y, float z)
{
	//XYZ���W�Ōv�Z�����āA�I�u�W�F�N�g���m�̓����蔻������A�������Ă�����true��Ԃ�
	if (object1.x > object2.x - x && object1.x < object2.x + x && object1.y>object2.y - y && object1.y < object2.y + y && object1.z>object2.z - z && object1.z < object2.z + z)
	{
		return true;
	}
	return false;
}
