#pragma once
#include<Keyboard.h>
#include <CommonStates.h>
#include<SimpleMath.h>
#include<Model.h>
#include<Effects.h>

#include"Libraries/ADX2/Adx2.h"

#include"Resources/Sounds/Public/Helpers/CueSheet_0.h"
#include"Resources/Sounds/Public/Helpers/NewProject_acf.h"

class Player
{
private:
	// コモンステート
	std::unique_ptr<DirectX::CommonStates>     m_commonState;

	std::unique_ptr<DirectX::Model> m_pModel;

	float m_rotation;

	bool m_flag;

	int m_jump_count;

	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_vel;

	DirectX::Keyboard::KeyboardStateTracker m_keyTracker;

	ADX2* m_pAdx2;
public:
	Player();
	~Player();

	void Initialize();
	void Update();
	void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection);
	void Finalize();

	void Jump(DirectX::Keyboard::KeyboardStateTracker KeyState);
	void Rotation();
	void Move(DirectX::Keyboard::State KeyState);

	int JumpCountHeal();

	int GetJumpCount(int x);

	DirectX::SimpleMath::Vector3 GetPosition();
	DirectX::SimpleMath::Vector3 SetPosition(DirectX::SimpleMath::Vector3 position);
};