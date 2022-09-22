/*
���[�h���
*/

#include "pch.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

#include "LoadingScreen.h"

#include "DeviceResources.h"
#include "../IScene.h"

using namespace DirectX;


/*--------------------------------------------------
�R���X�g���N�^
--------------------------------------------------*/
LoadingScreen::LoadingScreen() :
	m_isLoading{ false },
	m_flag_left{false},
	m_flag_center{false},
	m_flag_right{false},
	m_count{0},
	m_counter{0.0f}
{
}

/*--------------------------------------------------
�f�X�g���N�^
--------------------------------------------------*/
LoadingScreen::~LoadingScreen()
{
	//���[�h�����烍�[�h�҂�
	if (m_loadingThread.joinable())
	{
		m_loadingThread.join();
	}
}

/*--------------------------------------------------
������
--------------------------------------------------*/
void LoadingScreen::Initialize(IScene* targetScene)
{
	// �ΏۃV�[���̃��\�[�X�̓ǂݍ���
	LoadSceneResources(targetScene);


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
void LoadingScreen::Update(const DX::StepTimer& timer)
{
	float time = timer.GetElapsedSeconds();

	m_counter++;
	if (m_counter > 13.0f)
		m_counter = 0.0f;

	if (m_counter >= 12.5f){ m_count++; }

	switch (m_count)
	{
	case 1:
		m_flag_left = true;
		break;
	case 2:
		m_flag_center = true;
		break;
	case 3:
		m_flag_right = true;
		break;
	case 4:
		m_flag_left = false;
		m_flag_center = false;
		m_flag_right = false;
		m_count = 0;
	}
}

/*--------------------------------------------------
�`��
--------------------------------------------------*/
void LoadingScreen::Draw()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonState->NonPremultiplied());

	//�w�i
	DirectX::SimpleMath::Vector2 pos(0.0f, 0.0f);
	m_spriteBatch->Draw(m_blackTexture.Get(), pos);
	//loading
	DirectX::SimpleMath::Vector2 position(600.0f, 520.0f);
	m_spriteBatch->Draw(m_loadingTexture.Get(), position);

	if (m_flag_left)
	{
		//�h�b�g	��
		DirectX::SimpleMath::Vector2 point(1130.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), point);
	}
	if (m_flag_center)
	{
		//�h�b�g	����
		DirectX::SimpleMath::Vector2 posi(1160.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), posi);
	}
	if (m_flag_right)
	{
		//�h�b�g	�E
		DirectX::SimpleMath::Vector2 post(1190.0f, 640.0f);
		m_spriteBatch->Draw(m_dotTexture.Get(), post);
	}

	m_spriteBatch->End();
}

/*--------------------------------------------------
�I������
--------------------------------------------------*/
void LoadingScreen::Finalize()
{
}

/*--------------------------------------------------
	// �ǂݍ��ݒ����ǂ����̎擾
--------------------------------------------------*/
bool LoadingScreen::IsLoading()
{
	m_loadingMutex.lock();
	bool isLoading = m_isLoading;
	m_loadingMutex.unlock();
	return isLoading;
}

/*--------------------------------------------------
�V�[���̃��\�[�X�̓ǂݍ���
--------------------------------------------------*/
void LoadingScreen::LoadSceneResources(IScene* targetScene)
{
	if (m_loadingThread.joinable())
	{
		m_loadingThread.join();
	}

	m_isLoading = true;
	m_loadingThread = std::thread(
		[=]
		{
			targetScene->LoadResources();

			m_loadingMutex.lock();
			m_isLoading = false;
			m_loadingMutex.unlock();
		});

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();

	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Black.png",
		nullptr,
		m_blackTexture.ReleaseAndGetAddressOf()
	);
	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Loading.png",
		nullptr,
		m_loadingTexture.ReleaseAndGetAddressOf()
	);
	// �e�N�X�`���̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Dot.png",
		nullptr,
		m_dotTexture.ReleaseAndGetAddressOf()
	);
}