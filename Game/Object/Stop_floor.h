#pragma once
#include<SimpleMath.h>
#include<Model.h>
#include<Effects.h>
#include <CommonStates.h>

#include "Libraries/MyLibraries/Camera.h"

class Stop_floor
{
private:
	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	//床情報
	DirectX::SimpleMath::Vector3 m_floorPos;
	//カメラ
	std::unique_ptr<Camera> m_pDebugCamera;
	std::unique_ptr<DirectX::Model> m_floor;
public:
	Stop_floor();
	~Stop_floor();

	void Initialize(DirectX::SimpleMath::Vector3 Pos, int i);
	void Update(int i);
	void Render(DirectX::SimpleMath::Vector3 Pos);
	void Finalize();
};