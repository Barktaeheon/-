#pragma once
#include"AActor.h"

class APawn : public AActor
{
public:
	APawn() {}
	APawn(class UGame* game, int updateOrder = 1);
	virtual ~APawn();

	// ������ ������Ʈ �Լ�
	virtual  void Update(float deltaSecond) override;
	// �׸��� �Լ�
	void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap) override;

protected:

};

