/*
�^�C�g���V�[��
*/
#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>


#include "../GameMain.h"
#include "DeviceResources.h"

#include "TitleScene.h"

using namespace DirectX;

/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
TitleScene::TitleScene():
	m_alpha(1.0f),
	m_blackalpha(1.0f),
	m_flag(false),
	m_cflag(false)
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
TitleScene::~TitleScene()
{
	//�{���̓Q�[�����I������܂�Finalize�����Ă͂����Ȃ�
	m_pAdx2->Finalize();
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void TitleScene::Initialize()
{
	//ADX2�̃C���X�^���X���擾
	m_pAdx2 = &ADX2::GetInstance();
	//ADX2�̏�����
	m_pAdx2->Initialize("Resources/Sounds/Public/NewProject.acf",
		"Resources/Sounds/Public/CueSheet_0.acb");
	m_pAdx2->SetVolume(0.1f);
	//BGM�̍Đ�
	m_musicID = m_pAdx2->Play(CRI_CUESHEET_0_C0_01005);

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	// �R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);

	// �X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(device, L"Resources/Fonts/SegoeUI_18.spritefont");
}

/*--------------------------------------------------
�X�V
�߂�l	:���̃V�[���ԍ�
--------------------------------------------------*/
GAME_SCENE TitleScene::Update(const DX::StepTimer& timer)
{
	// �L�[���͏����擾����
	DirectX::Keyboard::State keyState = DirectX::Keyboard::Get().GetState();

	// �}�E�X���͏����擾����
	DirectX::Mouse::State mouseState = DirectX::Mouse::Get().GetState();

	if (m_keyTracker.IsKeyPressed(Keyboard::Space))
	{
		m_cflag = true;
		//���ʉ��Đ�
		m_pAdx2->Play(CRI_CUESHEET_0_A5_02036);
	}

	if (!m_cflag)
		m_blackalpha -= 0.01f;
	if (m_blackalpha <= 0.0f&&!m_cflag)
		m_blackalpha = 0.0f;

	if(m_cflag)
	{
		m_blackalpha += 0.01f;
	}

	if (!m_flag)
		m_alpha += 0.01f;
	if (m_alpha >= 1.0f)
		m_flag = true;
	if (m_flag)
		m_alpha -= 0.01f;
	if (m_alpha <= 0.0f)
		m_flag = false;


	if (m_blackalpha >= 1.1f)
	{
		return GAME_SCENE::MODE_SELECT;
	}

	return GAME_SCENE::NONE;
}

/*--------------------------------------------------
�`��
--------------------------------------------------*/
void TitleScene::Draw()
{
	RECT rect = { 0,0,600,100 };
	RECT rec={ 0,0,1280,1020 };
	FXMVECTOR color = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_alpha };//���F�Ɏw��F���|�����킹��
	FXMVECTOR black = DirectX::SimpleMath::Vector4{ 1.0f,1.0f,1.0f,m_blackalpha };
	float rotation = 0.0f;							//��]�p�x�����W�A���Ŏw�肷��
	DirectX::SimpleMath::Vector2 origin = { 0,0 };	//��]���钆�S�_�̎w��
	float scale = 1.0f;								//�g�嗦�̎w��
	SpriteEffects effects = SpriteEffects_None;		//�\�����]����
	float layerDepth = 1.0f;						//�\���[�x

	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());

	SimpleMath::Vector2 pos(0, 0);
	SimpleMath::Vector2 pos_Z(400, 500);

	m_spriteBatch->Draw(m_texture.Get(), pos);

	m_spriteBatch->Draw(m_texture_Space.Get(), pos_Z, &rec, color, rotation, origin, scale, effects, layerDepth);
	m_spriteBatch->Draw(m_criware_texture.Get(), SimpleMath::Vector2{ 1180,620 },&rec, FXMVECTOR{ 1.0f,1.0f,1.0f,1.0f }, rotation, origin, 0.1f, effects, layerDepth);
	m_spriteBatch->Draw(m_texture_black.Get(), pos, &rec, black, rotation, origin, scale, effects, layerDepth);

	m_spriteBatch->End();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void TitleScene::Finalize()
{

}

void TitleScene::LoadResources()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();

	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Title.png",
		nullptr,
		m_texture.ReleaseAndGetAddressOf()
	);

	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Push_Space.png",
		nullptr,
		m_texture_Space.ReleaseAndGetAddressOf()
	);

	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/black.png",
		nullptr,
		m_texture_black.ReleaseAndGetAddressOf()
	);
	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/CRIWARELOGO_1.png",
		nullptr,
		m_criware_texture.ReleaseAndGetAddressOf()
	);

}