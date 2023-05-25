#pragma once
#include"UGame.h"

class AActor
{
public:
	AActor(){}
	AActor(class UGame* game, int updateOrder = 1);
	virtual ~AActor();

	// �ʱ�ȭ �Լ�
	virtual void Initialize(){}
	// ������Ʈ �ϴ� �Լ�
	virtual void Update(float deltaSecond) {  }
	// �׸��� �Լ�
	virtual void Draw(HDC memDC1, HDC memDC2, HBITMAP bitmap){}

protected:
public:
	// �ȿ� ���Դ��� Ȯ��
	bool IsInPos(Vector2 pos1, double width1, double height1, Vector2 pos2, int len = 3);
	// ȸ��
	void Rotate(HDC memDC1);
	// ȸ���� ������ �ʱ�ȭ
	void ResetRotate(HDC memDC1);
	// �����͸� �ʱ�ȭ �ϴ� �Լ�
	virtual void Clear() { DELETE_MEMORY(uGame); }

	//  GetSet�Լ�
	Vector2 GetPos() const { return v2Pos; }
	void SetPos(Vector2 pos) { this->v2Pos = pos; }
	Vector2 GetDir() const { return v2Dir; }
	void SetDir(Vector2 dir) { this->v2Dir = dir; }
	Vector2 GetScale() const { return v2Scale; }
	void SetScale(Vector2 scale) { this->v2Scale = scale; }
	double GetAngle() const { return dAngle; }
	void SetAngle(double angle) { this->dAngle = angle; }
	double GetWidth() const { return dWidth; }
	void SetWidth(double width) { this->dWidth = width; }
	double GetHeight() const { return dHeight; }
	void SetHeight(double height) { this->dHeight = height; }

	int GetUpdateOrder() const { return iUpdateOrder; }
protected:
	class UGame* uGame = nullptr;
	Vector2 v2Pos, v2Dir, v2Scale;
	double dAngle = 0.0, dWidth = 0.0, dHeight = 0.0;
	int iUpdateOrder = 0;
};

