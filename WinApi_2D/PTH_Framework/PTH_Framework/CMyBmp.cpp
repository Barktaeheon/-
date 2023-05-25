#include "pch.h"
#include "CMyBmp.h"

CMyBmp::CMyBmp()
{
}

CMyBmp::~CMyBmp()
{
	Release();
}

void CMyBmp::Load_Bmp(const TCHAR * pFilePath)
{
	HDC hDC = GetDC(g_hWnd);

	m_hMemDC = CreateCompatibleDC(hDC);
	m_hBmp  = static_cast<HBITMAP>(LoadImage(nullptr, pFilePath, 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	m_hOldBmp = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBmp));

	BITMAP tInfo{};
	GetObject(m_hBmp, sizeof(BITMAP), &tInfo);
	this->m_iWidth = static_cast<int>(tInfo.bmWidth);
	this->m_iHeight = static_cast<int>(tInfo.bmHeight);

	ReleaseDC(g_hWnd, hDC);
}

void CMyBmp::Release(void)
{
	static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hOldBmp));
	//�ҷ��� ������Ʈ ����,���� ���ǰ� �ִ� ��ü�� �Ժη� �����ϴ� ���� ����
	DeleteObject(m_hBmp);
	DeleteObject(m_hOldBmp);
	//���۷��� ī���͸� ����
	DeleteDC(m_hMemDC);
}
