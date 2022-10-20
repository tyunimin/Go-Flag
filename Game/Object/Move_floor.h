#pragma once
#include<SimpleMath.h>

class Move_floor
{
private:
	//フロア数
	static const int FLOOR = 400;
	bool m_moveflag;
	float m_vel;
	//床情報
	DirectX::SimpleMath::Vector3 m_floorPos[FLOOR];
public:
	Move_floor();
	~Move_floor();

	void Initialize(DirectX::SimpleMath::Vector3 Pos,int i);
	void Update(int i);
	void Render();
	void Finalize();
	DirectX::SimpleMath::Vector3 GetPos(int i);
	float Move(int i);
};