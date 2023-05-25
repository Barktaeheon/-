#pragma once
#include "CUserInterface.h"
#include "CPlayer.h"
#include "CMyBmp.h"
#include "CInvenSlot.h"
#include "CItemSlot.h"
#include "CActionSlot.h"

class CInventory : public CUserInterface
{
public:
	explicit CInventory();
	explicit CInventory(CPlayer*& pPlayer, const TCHAR* szPathImg, const TCHAR* szPathBack, const RGBINFO& tInfo);
	virtual ~CInventory();
protected:
	// �κ��丮 ����
	vector<CInvenSlot*>		m_vecInvenSlot[3];
	vector<CActionSlot*>		m_vecActionSlot;
	vector<CItemSlot*>		m_vecItemSlot;
	// Select Slot
	CInvenSlot*				m_pSelectSlot;
	// Owner 
	CPlayer*					m_pOwner;
	// ============= IMG =============
	// Img
	CMyBmp*				m_pImgBmp;
	// Img
	CMyBmp*				m_pBackBmp;
	// Img
	CMyBmp*				m_pItemBackBmp;
	// Equip Img
	CMyBmp*				m_pEquipBmp;
	CMyBmp*				m_pItemBmp;
	CMyBmp*				m_pThingsBmp;
	// ============= IMG =============
		// ============= IMG =============
	CText*						m_pGoldText;
	CText*						m_pExplainText;
	// RGB Info 
	RGBINFO					m_tTranspercyRGB;
	// Slot Rect
	RECT						m_tSlotRect;
	// slot ��ġ�� ��� ���� ���� ������ 
	VECTOR2					m_v2StartSlotPos;
	VECTOR2					m_v2SpawnSlotPos;
	// ���� ��ġ�� ��� ���� ��ġ ����.. 
	VECTOR2					m_v2SelectSlotPos;
	// �κ��丮 �ε���
	int							m_iInvenIndex;
	// ���� ���̸� ������ ���� ����
	int							m_iSaveSlotLength;
	int							m_iSelectIndex;
	// Under Slot 
	int							m_iCheckUnderSlot;
	int							m_iShowStart;
	int							m_iShowEnd;

	bool							m_bUse;
public:
	// CObj��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;
	// ������ �߰�
	void Add_Item(CObj* pItem);
	// Judge Slot List
	void Judge_SlotList();
	// Slot�� ���õǾ����� �Ǵ��ϴ� �Լ�
	void Judge_Select_Slot();
	// �����鼭 �κ��丮 ������ �ʱ�ȭ
	void Reset_Info();
protected:
	// Update Rect
	virtual void Update_Rect() override;
	// ���Կ��� �������� ������ �������� �ƴ��� Ȯ��.. 
	void Check_SlotList();
	// Check in Rect
	bool Check_Pos_IsInRect(const VECTOR2& v2Pos, const RECT& tRect);
	// Player Can Use
	void Register_To_Player_Use();
public:
	// get
	const RECT& Get_SlotRect() const { return m_tSlotRect; }
	const bool& Is_Use() const { return m_bUse; }
	// set
	void Set_SlotRect(const RECT& tRect) { this->m_tSlotRect = tRect; }
	void Set_Use(const bool& IsUse) { this->m_bUse = IsUse; }
};

