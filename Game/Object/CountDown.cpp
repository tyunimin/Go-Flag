/// <summary>
/// �J�E���g�_�E���N���X
/// </summary>
#include "pch.h"
#include "CountDown.h"
#include "DeviceResources.h"

#include <WICTextureLoader.h>
#include <SimpleMath.h>

// ���O��Ԃ̗��p
using namespace DirectX;

CountDown::CountDown():
	First_rect{0,0,0,0}
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();
	//�X�v���C�g�o�b�`::�f�o�b�O���̕\���ɕK�v
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	//	�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
}

CountDown::~CountDown()
{

}

void CountDown::Initialize(int count)
{
	m_count = count;
	LoadResouce();
}

void CountDown::Update(const DX::StepTimer& timer)
{
	float time=timer.GetElapsedSeconds();
	time *= 60.0;
	m_count -= time;
	if (m_count <= 0)
	{
		m_count = 0;
	}
}

void CountDown::Draw()
{
	First_rect = Numfirst();
	Second_rect = Numsecond();
	Third_rect = Numthird();
	
	RECT rect = { 0,0,200,50 };

	m_spriteBatch->Begin();
	m_spriteBatch->Draw(m_Num.Get(),
		SimpleMath::Vector2(100, 600),
		&First_rect,
		SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		0.0f,
		SimpleMath::Vector2{ 0.0f,0.0f },
		0.8f);
	m_spriteBatch->Draw(m_Num.Get(),
		SimpleMath::Vector2(50, 600),
		&Second_rect,
		SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		0.0f,
		SimpleMath::Vector2{ 0.0f,0.0f },
		0.8f);
	m_spriteBatch->Draw(m_Num.Get(),
		SimpleMath::Vector2(0, 600),
		&Third_rect,
		SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		0.0f,
		SimpleMath::Vector2{ 0.0f,0.0f },
		0.8f);
	m_spriteBatch->Draw(m_CountDown.Get(),
		SimpleMath::Vector2(0, 550),
		&rect,
		SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		0.0f,
		SimpleMath::Vector2{ 0.0f,0.0f },
		0.8f);
	m_spriteBatch->End();
}

void CountDown::Finalize()
{

}

void CountDown::LoadResouce()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// D3D�f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̎擾
	ID3D11Device1* device = pDR->GetD3DDevice();

	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/FantasyNum.png",
		nullptr,
		m_Num.ReleaseAndGetAddressOf()
	);
	// �摜�̓ǂݍ���
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/CountDown.png",
		nullptr,
		m_CountDown.ReleaseAndGetAddressOf()
	);
}

RECT CountDown::Numfirst()
{
	RECT rect = { 0,0,0,0 };
	switch (static_cast<int>(m_count) /100% 10)
	{
	case 1:
		rect = { 0,0,50,70 };
		break;
	case 2:
		rect = { 50,0,100,70 };
		break;
	case 3:
		rect = { 100,0,150,70 };
		break;
	case 4:
		rect = { 150,0,200,70 };
		break;
	case 5:
		rect = { 200,0,250,70 };
		break;
	case 6:
		rect = { 250,0,300,70 };
		break;
	case 7:
		rect = { 300,0,350,70 };
		break;
	case 8:
		rect = { 350,0,400,70 };
		break;
	case 9:
		rect = { 400,0,450,70 };
		break;
	case 0:
		rect = { 450,0,500,70 };
		break;
	}

	return rect;
}
RECT CountDown::Numsecond()
{
	RECT rect = { 0,0,0,0 };
	switch ((static_cast<int>(m_count) / 1000 % 10))
	{
	case 1:
		rect = { 0,0,50,70 };
		break;
	case 2:
		rect = { 50,0,100,70 };
		break;
	case 3:
		rect = { 100,0,150,70 };
		break;
	case 4:
		rect = { 150,0,200,70 };
		break;
	case 5:
		rect = { 200,0,250,70 };
		break;
	case 6:
		rect = { 250,0,300,70 };
		break;
	case 7:
		rect = { 300,0,350,70 };
		break;
	case 8:
		rect = { 350,0,400,70 };
		break;
	case 9:
		rect = { 400,0,450,70 };
		break;
	case 0:
		rect = { 450,0,500,70 };
		break;
	}

	return rect;
}
RECT CountDown::Numthird()
{
	RECT rect = { 0,0,0,0 };
	switch (static_cast<int>(m_count) / 10000)
	{
	case 1:
		rect = { 0,0,50,70 };
		break;
	case 2:
		rect = { 50,0,100,70 };
		break;
	case 3:
		rect = { 100,0,150,70 };
		break;
	case 4:
		rect = { 150,0,200,70 };
		break;
	case 5:
		rect = { 200,0,250,70 };
		break;
	case 6:
		rect = { 250,0,300,70 };
		break;
	case 7:
		rect = { 300,0,350,70 };
		break;
	case 8:
		rect = { 350,0,400,70 };
		break;
	case 9:
		rect = { 400,0,450,70 };
		break;
	case 0:
		rect = { 450,0,500,70 };
		break;
	}
	return rect;
}
