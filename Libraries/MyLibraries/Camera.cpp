//
// Camera.cpp
//
#include "pch.h"
#include "Camera.h"
#include<memory>

#include "DeviceResources.h"


using namespace DirectX;

// 定数の初期化
const DirectX::SimpleMath::Vector3 Camera::TARGET_TO_EYE_VEC =
DirectX::SimpleMath::Vector3(0.0f, 10.0f, 10.0f);

//-------------------------------------------------------------------
// コンストラクタ
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
// デストラクタ
//-------------------------------------------------------------------
Camera::~Camera()
{
}

//-------------------------------------------------------------------
// ビュー行列の算出
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
// プロジェクション行列の算出
//-------------------------------------------------------------------
void Camera::CalculateProjectionMatrix()
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	// ウィンドウサイズの取得
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	constexpr float fieldOfView = XMConvertToRadians(45.0f);    // 画角
	float aspectRatio = width / height;							// 画面縦横比
	float nearPlane = 1.0f;                                     // カメラから一番近い投影面
	float farPlane = 100.0f;                                    // カメラから一番遠い投影面
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