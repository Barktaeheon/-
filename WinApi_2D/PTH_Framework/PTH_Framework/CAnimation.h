#pragma once
#include"Include.h"

class CAnimClip;
class CMyBmp;
class CObj;

class CAnimation
{
public:
	explicit CAnimation();
	explicit CAnimation(const TCHAR* pAnimName);
	virtual ~CAnimation();
protected:
	// �ִϸ��̼��� ������ ���� vector(�ȿ� vector(AnimationClip))
	vector<CAnimClip*>	m_vecAnimClip;
	CMyBmp*					m_pAnimBP;
	// ����� �ִϸ��̼� �ε���
	int								m_iAnimIndex;
	// �ð�
	DWORD						m_dwTime;
	// ������
	int								m_iFps;
	// RGB
	int								m_iRed;
	int								m_iGreen;
	int								m_iBlue;
	// ������ �ִϸ��̼� ����
	bool								m_bLastAnimFix;	
	// �ִϸ��̼��� ������ �ε���
	int								m_iLastIndex;
	// �ִϸ��̼��� ù ��° �ε���
	int								m_iFirstIndex;
	// Direciton
	EDIR								m_eDir;
	// �ִϸ��̼� �н�
	bool								m_bPassAnimation;
	// �ִϸ��̼��� �ش� �����ӿ� ������Ű�� ���� ����
	bool								m_bAnimFix;
public:
	// Insert Animation
	virtual void Insert_Anim(CAnimClip*& pAnimClip);
	// ������Ʈ
	virtual int Update();
	// ����
	virtual void Render(HDC hDC, CObj*& pObj, const VECTOR2& v2ScrollPos);

	virtual void Reset_Animation_Index() { m_iAnimIndex = 0; }
public:
	void Set_RGB(const int& iRed, const int& iGreen, const int& iBlue) { this->m_iRed = iRed; this->m_iGreen = iGreen; m_iBlue = iBlue; }
	void Set_AnimationIndex(const int& iIndex) { this->m_iAnimIndex = iIndex; }
	void Set_LastAnimFix(const bool& bIsLastAnim) { this->m_bLastAnimFix = bIsLastAnim; }
	void Set_Fps(const int& iFps) { this->m_iFps = iFps; }
	void Set_AnimFix(const bool& bAnimFix) { this->m_bAnimFix = bAnimFix; }
	// Get Animation Index
	const  int& Get_AnimIndex() const { return m_iAnimIndex; }
	// ù��°, ������ �ε��� üũ
	const  int& Get_FirstIndex() const { return m_iFirstIndex; }
	const  int& Get_LastIndex() const { return m_iLastIndex; }
	// Dir
	const EDIR& Get_Direction() const { return m_eDir; }
	// ���������� üũ
	bool Is_last() const { if (m_iLastIndex == m_iAnimIndex) return true; else return false; }
	bool Is_First() const { if (m_iFirstIndex == m_iAnimIndex)return true; else return false; }
	const bool& Is_Pass() const { return m_bPassAnimation; }
};

