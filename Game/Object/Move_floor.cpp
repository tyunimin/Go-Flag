#include"pch.h"
#include"Move_floor.h"

using namespace DirectX;

Move_floor::Move_floor():
	m_vel(0.0f),
	m_moveflag(false)
{
	for (int i = 0; i < FLOOR; i++)
		m_floorPos[i] = SimpleMath::Vector3::Zero;
}

Move_floor::~Move_floor()
{

}

void Move_floor::Initialize(DirectX::SimpleMath::Vector3 Pos,int i)
{
	m_floorPos[i] = Pos;
}

void Move_floor::Update(int i)
{
	Move(i);
}

void Move_floor::Render()
{

}

void Move_floor::Finalize()
{

}

DirectX::SimpleMath::Vector3 Move_floor::GetPos(int i)
{
	return m_floorPos[i];
}

float Move_floor::Move(int i)
{
	if (m_moveflag == false)
	{
		m_vel += 0.005f;
		m_floorPos[i].x += m_vel;
	}
	if (m_vel > 0.1f)
		m_moveflag = true;
	if (m_moveflag == true)
	{
		m_vel -= 0.005f;
		m_floorPos[i].x += m_vel;
	}
	if (m_vel < -0.1f)
		m_moveflag = false;
	return m_vel;
}