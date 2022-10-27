#include"pch.h"
#include"Stop_floor.h"
#include "DeviceResources.h"

using namespace DirectX;

Stop_floor::Stop_floor()
{
	m_pDebugCamera = std::make_unique<Camera>();
}

Stop_floor::~Stop_floor()
{

}

void Stop_floor::Initialize(DirectX::SimpleMath::Vector3 Pos, int i)
{
	
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11Device1* device = pDR->GetD3DDevice();
	//	�R�����X�e�[�g::D3D�����_�����O��ԃI�u�W�F�N�g
	m_commonState = std::make_unique<DirectX::CommonStates>(device);
	// �G�t�F�N�g�t�@�N�g���̍쐬
	EffectFactory* Floorfac = new EffectFactory(pDR->GetD3DDevice());
	// �e�N�X�`���̓ǂݍ��݃p�X�w��
	Floorfac->SetDirectory(L"Resources/Models");
	m_floor = DirectX::Model::CreateFromCMO(
		pDR->GetD3DDevice(),
		L"Resources/Models/floor.cmo",
		*Floorfac
	);
	delete Floorfac;

	m_floorPos = Pos;
}

void Stop_floor::Update(int i)
{
}

void Stop_floor::Render(DirectX::SimpleMath::Vector3 Pos)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();	
	//	�ϊ��s��̐錾(���[���h�A�r���[�A�ˉe(�v���W�F�N�V����))
	DirectX::SimpleMath::Matrix world, view, projection;
	//	���[���h�s��̏�����(�P�ʍs��)
	world = DirectX::SimpleMath::Matrix::Identity;
	view = m_pDebugCamera->GetViewMatrix();
	projection = m_pDebugCamera->GetProjectionMatrix();

	world = DirectX::SimpleMath::Matrix::Identity;
	world *= DirectX::SimpleMath::Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_floorPos.x, m_floorPos.y, m_floorPos.z);
	m_floor->Draw(context, *m_commonState.get(), world, view, projection);
}

void Stop_floor::Finalize()
{
}
