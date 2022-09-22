//
// DebugCamera.cpp
//
#include "pch.h"
#include "DebugCamera.h"

// 定数の初期化
const float DebugCamera::DEFAULT_CAMERA_DISTANCE = 5.0f;

// コンストラクタ
DebugCamera::DebugCamera()
	: Camera()
	, m_angleX(0.0f)
	, m_angleY(0.0f)
	, m_prevX(0)
	, m_prevY(0)
	, m_scrollWheelValue(0)
{
}

// デストラクタ
DebugCamera::~DebugCamera()
{
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void DebugCamera::Update()
{
	// どこかでMouseインスタンスが作られていれば、マウスの状態が取得できる(参考：Impl)
	DirectX::Mouse::State state = DirectX::Mouse::Get().GetState();

	// マウスの左クリック＆ドラッグでカメラ座標を更新する
	if (state.leftButton)
	{
		DraggedDistance(state.x, state.y);
	}

	// マウスの座標を前回の値として保存
	m_prevX = state.x;
	m_prevY = state.y;

	// マウスホイールのスクロール値を取得
	m_scrollWheelValue = state.scrollWheelValue;
	if (m_scrollWheelValue > 0)
	{
		m_scrollWheelValue = 0;
		DirectX::Mouse::Get().ResetScrollWheelValue();
	}

	// ビュー行列の算出
	CalculateViewMatrix();
}

//-------------------------------------------------------------------
// マウスポインタのドラッグ開始位置からの変位(相対値)
//-------------------------------------------------------------------
void DebugCamera::DraggedDistance(int x, int y)
{
	// マウスポインタの前回からの変位
	// なお、0.2fは適当な補正値 -> ドラッグの移動量を調整する
	float dx = static_cast<float>(x - m_prevX) * 0.2f;
	float dy = static_cast<float>(y - m_prevY) * 0.2f;

	if (dx != 0.0f || dy != 0.0f)
	{
		// マウスポインタの変位を元に、Ｘ軸Ｙ軸の回転角を求める
		// XとYの関係に注意！！
		float angleX = dy * DirectX::XM_PI / 180.0f;
		float angleY = dx * DirectX::XM_PI / 180.0f;

		// 角度の更新
		m_angleX += angleX;
		m_angleY += angleY;
	}
}

//-------------------------------------------------------------------
// ビュー行列の算出と、カメラ座標・注視点の取得
//-------------------------------------------------------------------
void DebugCamera::CalculateViewMatrix()
{
	// ビュー行列を算出する
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(m_angleY);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_angleX);

	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	DirectX::SimpleMath::Vector3    eye(0.0f, 1.0f, 5.0f);
	DirectX::SimpleMath::Vector3 target(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3     up(0.0f, 1.0f, 0.0f);

	eye = DirectX::SimpleMath::Vector3::Transform(eye, rt.Invert());
	eye *= (DEFAULT_CAMERA_DISTANCE - m_scrollWheelValue / 100);
	up = DirectX::SimpleMath::Vector3::Transform(up, rt.Invert());

	SetEyePosition(eye);
	SetTargetPosition(target);
	SetUpVector(up);

	SetViewMatrix(DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up));
}
