#pragma once
#include "CSlot.h"

class CEquip;

class CActionSlot final : public CSlot
{
public:
	explicit CActionSlot();
	explicit CActionSlot(CObj* pOwner, const TCHAR* szSlotPath, const TCHAR* szBackImg, const RGBINFO& tInfo
	, const int& iIndex);
	virtual ~CActionSlot();
protected:
	// Owner�� �����ϰ� �ִ� Equip�� Ȯ��
	CMyBmp*	    m_pWearableEquipBmp;
	// �� �̹���
	CMyBmp*		m_pBackImg;
public:
	// CObj��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

protected:
	// Check Player Item
	void Check_Player_EquipItem();

public:
	void Set_WeableEquipBmp(CMyBmp* pBmp) { this->m_pWearableEquipBmp = pBmp; }
};

