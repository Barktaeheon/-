#pragma once
#include "Include.h"

class CMainGame
{
public:
	explicit CMainGame();
	virtual ~CMainGame();
protected:
	// ���� DC
	HDC m_hDC;
protected:
	// �ʱ�ȭ
	void Initialize();
	// �ı�
	void Release();
public:
	void Update();
	void Late_Update();
	void Render();

};

