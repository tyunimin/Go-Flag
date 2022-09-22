//
// Camera.cpp
//
#include "pch.h"
#include "Camera.h"
#include<memory>

#include "DeviceResources.h"


using namespace DirectX;

// �萔�̏�����
const DirectX::SimpleMath::Vector3 Camera::TARGET_TO_EYE_VEC =
DirectX::SimpleMath::Vector3(0.0f, 10.0f, 10.0f);

//-------------------------------------------------------------------
// �R���X�g���N�^
//-------------------------------------------------------------------
Camera::Camera()
	: m_view(DirectX::SimpleMath::Matrix::Identity)
	, m_eye(0.0f, 3.0f, 15.0f)
	, m_target(0.0f, 0.0f, 0.0f)
	, m_up(0.0f, 1.0f, 0.0f)
	,m_nearPlane(1.0f)
	,m_farPlane(1000.0f)
	,m_aspectRatio(16.0f / 9.0f)
	,m_fov(45.0f)
{
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}


//-------------------------------------------------------------------
// �f�X�g���N�^
//-------------------------------------------------------------------
Camera::~Camera()
{
}

//-------------------------------------------------------------------
// �r���[�s��̎Z�o
//-------------------------------------------------------------------
void Camera::CalculateViewMatrix()
{
}

void Camera::SetCameraTarget(DirectX::SimpleMath::Vector3 target)
{
	m_target = target;

	m_eye = m_target + TARGET_TO_EYE_VEC;
}

//-------------------------------------------------------------------
// �v���W�F�N�V�����s��̎Z�o
//-------------------------------------------------------------------
void Camera::CalculateProjectionMatrix()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// �E�B���h�E�T�C�Y�̎擾
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	constexpr float fieldOfView = XMConvertToRadians(45.0f);    // ��p
	float aspectRatio = width / height;							// ��ʏc����
	float nearPlane = 1.0f;                                     // �J���������ԋ߂����e��
	float farPlane = 100.0f;                                    // �J���������ԉ������e��
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);
}

DirectX::SimpleMath::Matrix Camera::GetViewMatrix()
{
	SetUpVector(m_up);
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_eye, m_target, m_up);

	return m_view;
}

DirectX::SimpleMath::Matrix Camera::GetProjectionMatrix()
{
	float fieldOfView = DirectX::XMConvertToRadians(m_fov);

	m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		fieldOfView,
		m_aspectRatio,
		m_nearPlane,
		m_farPlane);

	return m_projection;
}