#include"pch.h"
#include"Move_floor.h"


Move_floor::Move_floor()
{

}

Move_floor::~Move_floor()
{

}

void Move_floor::Initialize(DirectX::SimpleMath::Vector3 Pos)
{
}

void Move_floor::Update()
{
	Move();
}

void Move_floor::Render()
{

}

void Move_floor::Finalize()
{

}

DirectX::SimpleMath::Vector3 Move_floor::GetPos()
{
	for(int i=0;i<FLOOR;i++)
	return m_floorPos[i];
}

void Move_floor::Move()
{
	for (int i = 0; i < FLOOR; i++)
	{
		if (moveflag == false)
		{
			movecount += 0.005f;
		
		}
		if (movecount > 0.1f)
			moveflag = true;
		if (moveflag == true)
		{
			movecount -= 0.005f;
		}
		if (movecount < -0.1f)
			moveflag = false;
	}
}