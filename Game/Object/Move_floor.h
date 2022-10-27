#pragma once
#include<SimpleMath.h>
#include<Model.h>

class Move_floor
{
private:
	//ÉtÉçÉAêî
	static const int FLOOR = 400;
	bool m_moveflag;
	float m_vel;
	//è∞èÓïÒ
	DirectX::SimpleMath::Vector3 m_floorPos[FLOOR];
	std::unique_ptr<DirectX::Model> m_floor;
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