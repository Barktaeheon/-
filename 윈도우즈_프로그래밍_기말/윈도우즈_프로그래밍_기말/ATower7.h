#pragma once
#include"ATower.h"

class ATower7 : public ATower
{
public:
	ATower7() {}
	ATower7(class UGame* game, int updateOrder = 1);
	virtual ~ATower7() { Clear(); }

	ATower* Copy() override { return new ATower7(uGame); }
	// �ʱ�ȭ �Լ�
	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;
protected:

};

