#pragma once
#include"ATower.h"

class ATower8 : public ATower
{
public:
	ATower8() {}
	ATower8(class UGame* game, int updateOrder = 1);
	virtual ~ATower8() { Clear(); }

	ATower* Copy() override { return new ATower8(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};