#include"pch.h"
#include"Player.h"
#include "DeviceResources.h"

// ���O��Ԃ̗��p
using namespace DirectX;
//�R���X�g���N�^
Player::Player():
	m_position(DirectX::SimpleMath::Vector3::Zero),
	m_rotation(0.0f),
	m_flag(false),
	m_jump_count(0),
	m_vel(0.0f,0.0f,0.0f)
{
}
//�f�X�g���N�^
Player::~Player()
{
	//�{���̓Q�[�����I������܂�Finalize�����Ă͂����Ȃ�
	//m_pAdx2->Finalize();
}
//������
void Player::Initialize()
{
	//ADX2�̃C���X�^���X���擾
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2�̏�����
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.01f);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<CommonStates>(device);

	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* factory = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	factory->SetDirectory(L"Resources/Models");
	// �t�@�C�����w�肵�ă��f���f�[�^�ǂݍ���
	m_pModel = Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/Sky.cmo",
		*factory
	);
	
	delete factory;
}

//�X�V
void Player::Update()
{
	//�L�[�{�[�h���
	Keyboard::State keyState = Keyboard::Get().GetState();
	m_keyTracker.Update(keyState);

	Jump(m_keyTracker);	//�W�����v

	Rotation();		//��]
	Move(keyState);	//����
}

//�`��
void Player::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//	�ϊ��s��̐錾(���[���h)
	DirectX::SimpleMath::Matrix world;

	//	���[���h�s��̏�����(�P�ʍs��)
	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateRotationY(m_rotation / 180.0f * 3.14f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position.x, m_position.y, m_position.z);

	// ���f���`��
	m_pModel->Draw(context, *m_commonState.get(), world, view, projection);
}
//�I��
void Player::Finalize()
{

}
//�W�����v
void Player::Jump(DirectX::Keyboard::KeyboardStateTracker KeyState)
{
	float Grav = 0.1f;	//�d��
	m_vel.y -= Grav;
	m_position.y += m_vel.y;

	if (KeyState.IsKeyPressed(DirectX::Keyboard::Space)&&m_jump_count<2)
	{
		m_vel.y = 1.0f;
		m_pAdx2->Play(CRI_CUESHEET_0_SE_JUMP06);
	}

	if (m_vel.y < 0.0f)
	{
		m_vel.y = 0.0f;
	}
}
//��]
void Player::Rotation()
{
	if (m_flag == true)
	{
		m_rotation += 2.0f;
	}
	if (m_rotation >= 45.0f)
	{
		m_flag = false;
	}
	if (m_flag == false)
	{
		m_rotation -= 2.0f;
	}
	if (m_rotation <= -45.0f)
	{
		m_flag = true;
	}
}

//����
void Player::Move(DirectX::Keyboard::State KeyState)
{
	if (KeyState.W||KeyState.Up)
	{
		m_vel.z = 0.1f;
		m_position.z -= m_vel.z;
	}
	if (KeyState.S||KeyState.Down)
	{
		m_vel.z = 0.1f;
		m_position.z += m_vel.z;
	}
	if (KeyState.A||KeyState.Left)
	{
		m_vel.x = 0.1f;
		m_position.x -= m_vel.x;
	}
	if (KeyState.D||KeyState.Right)
	{
		m_vel.x = 0.1f;
		m_position.x += m_vel.x;
	}

}

SimpleMath::Vector3 Player::GetPosition()
{
	return m_position;
}

SimpleMath::Vector3 Player::SetPosition(SimpleMath::Vector3 position)
{
	return m_position = position;
}

int Player::JumpCountHeal()
{
	return m_jump_count = 0;
}

int Player::GetJumpCount(int x)
{
	return m_jump_count = x;
}