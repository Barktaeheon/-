#include "pch.h"
#include "CAnimation.h"
#include "CAnimClip.h"
#include "CGameMgr.h"
#include "CMyBmp.h"

CAnimation::CAnimation() : m_pAnimBP(nullptr), m_iAnimIndex(0), m_iRed(0), m_iGreen(0), m_iBlue(0)
, m_bLastAnimFix(false) , m_iLastIndex(0), m_iFirstIndex(0), m_eDir(DIR_FOR), m_bPassAnimation(false)
, m_bAnimFix(false)
{
}

CAnimation::CAnimation(const TCHAR * pAnimName) : m_pAnimBP(nullptr), m_iAnimIndex(0)
, m_iRed(0), m_iGreen(0), m_iBlue(0), m_bLastAnimFix(false), m_iLastIndex(0), m_iFirstIndex(0)
, m_eDir(DIR_FOR), m_bPassAnimation(false)
, m_bAnimFix(false)
{
	// �ִϸ��̼� MyBmp�� ������
	m_pAnimBP = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(pAnimName);

	m_dwTime = GetTickCount();
	// FPS 100���� ����
	m_iFps = 50;
}

CAnimation::~CAnimation()
{
	for (auto& iter : m_vecAnimClip)
	{
		DELETE_MEMORY(iter);
	}
	m_vecAnimClip.clear();
}

void CAnimation::Insert_Anim(CAnimClip*& pAnimClip)
{
	// �ִϸ��̼� Ŭ���� ��������� ����
	if (nullptr == pAnimClip)
		return;

	m_vecAnimClip.push_back(pAnimClip);
	m_iLastIndex = static_cast<int>(m_vecAnimClip.size() - 1);
}

int CAnimation::Update()
{
	// ���� �ð��� ��������
	if (m_iFps + m_dwTime <= GetTickCount())
	{
		// �ִϸ��̼� �ε����� ������Ų��.
		if (false == m_bAnimFix)
			++m_iAnimIndex;
		else
			m_bAnimFix = false;

		// �ٽ� �ð� �ʱ�ȭ
		m_dwTime = GetTickCount();
		m_bPassAnimation = true;
	}

	if (false == m_bLastAnimFix)
	{
		// �ִϸ��̼� �ε��� ����
		if (static_cast<int>(m_vecAnimClip.size()) <= m_iAnimIndex)
		{
			m_iAnimIndex = 0;
		}
	}
	else
	{
		// �ִϸ��̼� �ε��� ����
		if (static_cast<int>(m_vecAnimClip.size()) <= m_iAnimIndex)
		{
			m_bLastAnimFix = false;
			m_iAnimIndex = m_iLastIndex;
		}
	}

	return 0;
}

void CAnimation::Render(HDC hDC, CObj*& pObj, const VECTOR2& v2ScrollPos)
{
	HDC hdc = NULL;
	if (nullptr != m_pAnimBP)
		hdc = m_pAnimBP->Get_MemDC();

	if (static_cast<int>(m_vecAnimClip.size()) < m_iAnimIndex)
		m_iAnimIndex = 0;
	// Play Animation Clip
	m_vecAnimClip[m_iAnimIndex]->Play(hDC, pObj, hdc, v2ScrollPos, m_iRed, m_iGreen, m_iBlue);
	m_bPassAnimation = false;
}

