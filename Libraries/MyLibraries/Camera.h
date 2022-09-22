//
// Camera.h
//
#pragma once
#include <SimpleMath.h>

// �f�o�b�O�p�J�����N���X

class Camera
{

	//�萔
private:
	static const DirectX::SimpleMath::Vector3 TARGET_TO_EYE_VEC;
private:

	// �r���[�s��
	DirectX::SimpleMath::Matrix m_view;
	// �v���W�F�N�V�����s��
	DirectX::SimpleMath::Matrix m_projection;

	// �J�������W
	DirectX::SimpleMath::Vector3 m_eye;
	// �����_
	DirectX::SimpleMath::Vector3 m_target;
	// ������x�N�g��
	DirectX::SimpleMath::Vector3 m_up;

	//����ȊO�̃J�������
	float m_nearPlane;
	float m_farPlane;
	float m_aspectRatio;
	float m_fov;

public:
	// �R���X�g���N�^
	Camera();

	// �f�X�g���N�^
	~Camera();

	void SetCameraTarget(DirectX::SimpleMath::Vector3 target);

	// �r���[�s��̃A�N�Z�T
	void SetViewMatrix(DirectX::SimpleMath::Matrix view) { m_view = view; }
	DirectX::SimpleMath::Matrix GetViewMatrix();

	// �v���W�F�N�V�����s��̃A�N�Z�T
	void SetProjectionMatrix(DirectX::SimpleMath::Matrix projection) { m_projection = projection; }
	DirectX::SimpleMath::Matrix GetProjectionMatrix();

	// �J�������W�̃A�N�Z�T
	void SetEyePosition(DirectX::SimpleMath::Vector3 eye) { m_eye = eye; }
	DirectX::SimpleMath::Vector3 GetEyePosition() const { return m_eye; }

	// �����_���W�̃A�N�Z�T
	void SetTargetPosition(DirectX::SimpleMath::Vector3 target) { m_target = target; }
	DirectX::SimpleMath::Vector3 GetTargetPosition() const { return m_target; }

	// ������x�N�g���̃A�N�Z�T
	void SetUpVector(DirectX::SimpleMath::Vector3 up) { m_up = up; }
	DirectX::SimpleMath::Vector3 GetUpVector() const { return m_up; }

protected:

	// �r���[�s��̎Z�o
	virtual void CalculateViewMatrix();

	// �v���W�F�N�V�����s��̎Z�o
	virtual void CalculateProjectionMatrix();
};
