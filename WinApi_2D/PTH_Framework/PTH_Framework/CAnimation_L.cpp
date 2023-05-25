#include "pch.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include "CGameMgr.h"
#include "CMyBmp.h"

CAnimation_L::CAnimation_L() 
{
	m_eDir = DIR_BACK;
}

CAnimation_L::CAnimation_L(const TCHAR * pAnimName) 
{	
	// �ִϸ��̼� MyBmp�� ������
	m_pAnimBP = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(pAnimName);

	m_dwTime = GetTickCount();
	// FPS 100���� ����
	m_iFps = 50;
	// Dir
	m_eDir = DIR_BACK;
}
CAnimation_L::~CAnimation_L()
{
	for (auto& iter : m_vecAnimClip)
	{
		DELETE_MEMORY(iter);
	}
	m_vecAnimClip.clear();
}
void CAnimation_L::Insert_Anim(CAnimClip *& pAnimClip)
{
	// �ִϸ��̼� Ŭ���� ��������� ����
	if (nullptr == pAnimClip)
		return;

	m_vecAnimClip.push_back(pAnimClip);
	++m_iFirstIndex;
	m_iAnimIndex = static_cast<int>(m_vecAnimClip.size() - 1);
}
int CAnimation_L::Update()
{
	// ���� �ð��� ��������
	if (m_iFps + m_dwTime <= GetTickCount())
	{
		// �ִϸ��̼� �ε����� ������Ų��.
		if (false == m_bAnimFix)
			--m_iAnimIndex;
		else
			m_bAnimFix = false;
		// �ٽ� �ð� �ʱ�ȭ
		m_dwTime = GetTickCount();
		m_bPassAnimation = true;
	}

	if (false == m_bLastAnimFix)
	{
		// �ִϸ��̼� �ε��� ����
		if (0 > m_iAnimIndex)
		{
			m_iAnimIndex = static_cast<int>(m_vecAnimClip.size() - 1);
		}		
	}
	else
	{
		// �ִϸ��̼� �ε��� ����
		if (0 > m_iAnimIndex)
		{
			m_bLastAnimFix = false;
			m_iAnimIndex = 0;
		}
	}
	return 0;
}

void CAnimation_L::Render(HDC hDC, CObj *& pObj, const VECTOR2& v2ScrollPos)
{
	if (m_iAnimIndex >= static_cast<int>(m_vecAnimClip.size()))
		m_iAnimIndex = static_cast<int>(m_vecAnimClip.size() - 1);
	// Play Animation Clip
	m_vecAnimClip[m_iAnimIndex]->Play(hDC, pObj, m_pAnimBP->Get_MemDC(), v2ScrollPos, m_iRed, m_iGreen, m_iBlue);
	m_bPassAnimation = false;
}
