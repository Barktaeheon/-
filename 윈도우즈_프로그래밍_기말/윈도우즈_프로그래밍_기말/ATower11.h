#pragma once
#include"ATower.h"

class ATower11 : public ATower
{
public:
	ATower11() {}
	ATower11(class UGame* game, int updateOrder = 1);
	virtual ~ATower11() { Clear(); }

	ATower* Copy() override { return new ATower11(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};