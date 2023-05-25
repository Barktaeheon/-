#pragma once
#include "CBar.h"

class CMpBar : public CBar
{
public:
	explicit CMpBar();
	explicit CMpBar(CCharacter* pOwner, const TCHAR* szMainPath,
		const TCHAR* szBgPath, const RGBINFO& tInfo, const int& iBarValue);
	virtual ~CMpBar();

protected:
public:
	// CObj��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
protected:
	// Calculate Bar
	virtual void Calculate_Bar() override;
};

