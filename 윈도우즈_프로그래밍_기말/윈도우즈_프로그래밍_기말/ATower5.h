#pragma once
#include"ATower.h"

class ATower5 : public ATower
{
public:
	ATower5() {}
	ATower5(class UGame* game, int updateOrder = 1);
	virtual ~ATower5() { Clear(); }

	ATower* Copy() override { return new ATower5(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};