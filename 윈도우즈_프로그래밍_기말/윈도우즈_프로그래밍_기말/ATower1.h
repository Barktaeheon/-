#pragma once
#include"ATower.h"

class ATower1 : public ATower
{
public:
	ATower1(){}
	ATower1(class UGame* game, int updateOrder = 1);
	virtual ~ATower1() { Clear(); }

	ATower* Copy() override { return new ATower1(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};

