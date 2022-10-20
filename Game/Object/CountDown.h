#pragma once

#include "StepTimer.h"

#include <CommonStates.h>
#include <SpriteBatch.h>

class CountDown
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Num;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CountDown;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_commonState;
	RECT First_rect;
	RECT Second_rect;
	RECT Third_rect;
	float m_count;
public:
	CountDown();
	~CountDown();

	void Initialize(int count);
	void Update(const DX::StepTimer& timer);
	void Draw();
	void Finalize();
	void LoadResouce();
	RECT Numfirst();
	RECT Numsecond();
	RECT Numthird();

};