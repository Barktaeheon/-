#pragma once
#include"ATower.h"

class ATower12 : public ATower
{
public:
	ATower12() {}
	ATower12(class UGame* game, int updateOrder = 1);
	virtual ~ATower12() { Clear(); }

	ATower* Copy() override { return new ATower12(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};