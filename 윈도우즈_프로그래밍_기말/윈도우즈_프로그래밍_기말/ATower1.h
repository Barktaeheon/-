#pragma once
#include"ATower.h"

class ATower1 : public ATower
{
public:
	ATower1(){}
	ATower1(class UGame* game, int updateOrder = 1);
	virtual ~ATower1() { Clear(); }

	ATower* Copy() override { return new ATower1(uGame); }
	// 초기화 함수
	void Initialize() override;
	// 그리는 함수
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};

