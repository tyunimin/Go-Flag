#pragma once
#include<SimpleMath.h>

class Move_floor
{
private:
	//�t���A��
	static const int FLOOR = 400;
	bool moveflag = false;
	float movecount = 0.0f;
	//�����
	DirectX::SimpleMath::Vector3 m_floorPos[FLOOR];
public:
	Move_floor();
	~Move_floor();

	void Initialize(DirectX::SimpleMath::Vector3 Pos);
	void Update();
	void Render();
	void Finalize();
	void Move();
	DirectX::SimpleMath::Vector3 GetPos();
};