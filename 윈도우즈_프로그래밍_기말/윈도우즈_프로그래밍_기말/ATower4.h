#pragma once
#include"ATower.h"

class ATower4 : public ATower
{
public:
	ATower4() {}
	ATower4(class UGame* game, int updateOrder = 1);
	virtual ~ATower4() { Clear(); }

	ATower* Copy() override { return new ATower4(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};