#include "pch.h"
#include "CInputMgr.h"


CInputMgr::CInputMgr()
{
	ZeroMemory(&m_bKeyList, sizeof(bool) * 0xff);
}

CInputMgr::CInputMgr(CGameMgr *& pGameMgr) : CManager(pGameMgr)
{
	ZeroMemory(&m_bKeyList, sizeof(bool) * 0xff);
}

CInputMgr::~CInputMgr()
{

}

void CInputMgr::Initialize()
{
}

int CInputMgr::Update()
{
	// Mouse Move 
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);	// ��ũ�� ������ ���콺 ��ǥ�� �츮�� ������ ������ â ��ǥ�� �������ִ� �Լ�
	m_v2MousePos = VECTOR2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	return 0;
}

void CInputMgr::Release()
{
}

bool CInputMgr::Is_Press(const int & iKey)
{
	// ������ ���� ����, ȣ�� ������ ������ ��
	if (m_bKeyList[iKey] && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyList[iKey] = !m_bKeyList[iKey];
		return true;
	}
	// ���� ����, ȣ�� ������ ������ ��
	else if (!m_bKeyList[iKey] && (GetAsyncKeyState(iKey) & 0x8000))
		m_bKeyList[iKey] = !m_bKeyList[iKey];

	return false;
}

bool CInputMgr::Is_Pressing(const int & iKey)
{
	// ������ �ʾҴ� ����, ȣ�� ������ ������ ���� ��
	if (m_bKeyList[iKey] && !(GetAsyncKeyState(iKey) & 0x8001))
	{
		m_bKeyList[iKey] = !m_bKeyList[iKey];
		return true;
	}
	// ������ ����, ȣ�� ������ ������ ���� ��
	else if (!m_bKeyList[iKey] && (GetAsyncKeyState(iKey) & 0x8001))
		return true;
	
	return false;
}

bool CInputMgr::Is_MouseClick()
{
	bool press = GetAsyncKeyState(VK_LBUTTON) && 0x8001;
	if (true == press)
	{
		m_bKeyList[VK_LBUTTON] = true;
		return true;
	}
	else
		m_bKeyList[VK_LBUTTON] = false;
	return false;
}

