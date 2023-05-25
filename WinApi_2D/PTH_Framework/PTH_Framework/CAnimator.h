#pragma once
#include"CObj.h"
#include "CAnimation.h"

class CAnimator
{
public:
	explicit CAnimator();
	explicit CAnimator(CObj*& pObj);
	virtual ~CAnimator();
protected:
	// Animation List
	map<const TCHAR*, CAnimation*> m_mAnimList;
	// Current Play Anim
	CAnimation* m_pCurPlayAnim;
	// ���� �ִϸ������� ����
	CObj* m_pOwner;
	// ���� �ִϸ��̼� 
	const TCHAR* m_chCurAnimation;
public:
	// �ִϸ��̼� ����
	void Add_Animation(const TCHAR* strName, CAnimation*& pAnimation);
	// �ִϸ��̼� ������
	void Render_Animation(HDC hDC, const VECTOR2& v2ScrollPos);
	// �ִϸ��̼� ������Ʈ
	int Update_Animation();
	// Change Animation 
	void Change_Animation(const TCHAR* strName);
public:
	// �ִϸ��̼��� �ִ��� üũ
	bool Is_In_Animation(const TCHAR* pAnimName);
public:
	// get
	CObj* Get_Owner() { return m_pOwner; }
	// ���� �ִϸ��̼� set
	void Set_CurAnimation(const TCHAR* chCurAnimation);
	// ���� �ִϸ��̼��� ������
	CAnimation* Get_Animation() { return m_pCurPlayAnim; }
	// get _Cur Animation
	const TCHAR* Get_CurAnimName() const { return m_chCurAnimation; }
};

