#pragma once
#include"ATower.h"

class ATower9 : public ATower
{
public:
	ATower9() {}
	ATower9(class UGame* game, int updateOrder = 1);
	virtual ~ATower9() { Clear(); }

	ATower* Copy() override { return new ATower9(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};