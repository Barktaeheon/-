#pragma once
#include"ATower.h"

class ATower2 : public ATower
{
public:
	ATower2() {}
	ATower2(class UGame* game, int updateOrder = 1);
	virtual ~ATower2() { Clear(); }

	ATower* Copy() override { return new ATower2(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};