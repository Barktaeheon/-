#pragma once
#include"ATower.h"

class ATower10 : public ATower
{
public:
	ATower10() {}
	ATower10(class UGame* game, int updateOrder = 1);
	virtual ~ATower10() { Clear(); }

	ATower* Copy() override { return new ATower10(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};

