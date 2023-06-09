#pragma once
#include "CText.h"

class CFollowText : public CText
{
public:
	explicit CFollowText();
	explicit CFollowText(CObj* pObj, const TCHAR* szFontName, const RGBINFO& tInfo);
	virtual ~CFollowText();
protected:
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
};

