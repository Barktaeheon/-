#pragma once
#include"ABackground.h"

class ABackground1 : public ABackground
{
public:
	ABackground1() {}
	ABackground1(class UGame* game, int updateOrder = 1);
	virtual ~ABackground1();

	void Initialize() override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;

	void AddPos() override;
};

