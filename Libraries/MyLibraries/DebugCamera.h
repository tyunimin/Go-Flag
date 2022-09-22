//
// DebugCamera.h
//
#pragma once

#include <Mouse.h>

#include "Camera.h"

/// <summary>
/// デバッグ用カメラクラス
/// </summary>
class DebugCamera : public Camera
{
public:
	static const float DEFAULT_CAMERA_DISTANCE;	// カメラの距離

private:
	float m_angleX;								// 縦回転角
	float m_angleY;								// 横回転角
	int m_prevX, m_prevY;						// 前回のマウス座標(X,Y)
	int m_scrollWheelValue;						// マウスホイールのスクロール値

public:
	// コンストラクタ
	DebugCamera();
	// デストラクタ
	~DebugCamera();

	// 更新処理
	void Update();

private:
	// マウスポインタのドラッグ開始位置からの変位(相対値)
	void DraggedDistance(int x, int y);

	// ビュー行列の算出
	void CalculateViewMatrix() override;
};
