#pragma once
#include<Windows.h>
#include"resource.h"
#include"Data.h"

class UGame
{
public:
	UGame() {}
	UGame(HINSTANCE inst) { this->hInst = inst; }
	virtual ~UGame() { Reset(); }
	// ���� Ŭ���� �ʱ�ȭ �Լ�
	void Initialize();
	// ������Ʈ �Լ�
	void Update();
	// ȭ���� ��������
	void RenderScreen(HDC hdc);
	// ���� ���콺 ��ư Ŭ��
	void LeftMouseButtonClick(LPARAM lParam);
	// ���� ���콺 ��ư ���� ��
	void UnLeftMouseButtonClick();
	// ������ ��ư Ŀ���
	void WM_ButtonCommand(HWND hWnd, WPARAM wParam, DLGPROC shopProc);
	// �巡��
	void Drag(LPARAM lParam);
	// �ؽ�ó�� �������� �Լ�
	UPicture* GetTexture(const TCHAR* str);
	// �ؽ�ó�� �ε��س��� �Լ�
	UPicture* LoadTexture(const TCHAR* load, const TCHAR* str, int width, int height);
	// ���� ������ �ؽ�Ʈ�� ������Ʈ �ϴ� �Լ�
	void UpdateTurnText();
	// ���� ����� ������Ʈ �ϴ� �Լ�
	void UpdateSurviveNum();
	// ���� ��带 ������Ʈ �ϴ� �Լ�
	void UpdateHaveGold();
	// �ٽ� �� ���ϱ�
	void Regame();
protected:
	void InitTexture();
	// �������� ��ư, ���� �͵��� �����ϴ� �Լ�
	void WindowInterfaceInit();
	// ȭ���� ������ �׸��� �Լ�
	void DrawScreen(HDC hdc);
	// �����ϴ� �Լ�
	void Reset();
public:
	void SetInstance(HINSTANCE inst) { this->hInst = inst; }
	HINSTANCE GetInst() const { return hInst; }
	void SetWindowHWND(HWND hwnd) { this->hWnd = hwnd; }
	HWND GetWindowHWND() const { return hWnd; }
	void SetGameHWND(HWND hwnd) { this->hGameHwnd = hwnd; }
	HWND GetGameHWND() const { return hGameHwnd; }

	class ABackground* GetBackground() const { return aBackground; }
	class UPlayerManager* GetPlayerManager() const { return uPlayerManager; }
	class UShopManager* GetShopManager() const { return uShopManager; }
	class UReinForceManager* GetReinForceManager() const { return uReinForceManager; }
	class UInfoManager* GetInfoManager() const { return uInfoManager; }
	class UEnemyManager* GetEnemyManager() const { return uEnemyManager; }
	class UStageManager* GetStageManager() const { return uStageManager; }

	RECT GetGameClientRect() const { return rGameClientRect; }

	Vector2 GetMousePosition() const { return v2MousePosition; }
	bool IsLeftMouseClick() const { return bIsLeftMouseClick; }

	void SetRightMouseClick(bool mouseClick) { this->bIsRightMouseClick = mouseClick; }
	bool IsRightMouseClick() const { return bIsRightMouseClick; }

	void SetWLDIalog(DLGPROC wldl) { this->dlWinLoseDialog = wldl; }
	DLGPROC GetWLDialog() const { return dlWinLoseDialog; }
protected:
	HWND hTurnText = NULL, hSurviveText = NULL, hHaveGold = NULL;
	HWND hButtonArr[5] = { NULL };
	DLGPROC dlWinLoseDialog = NULL;
	HINSTANCE hInst = NULL;
	HWND hWnd = NULL, hGameHwnd = NULL;
	HBITMAP hGameScreen = NULL;
	RECT rGameClientRect = RECT{ 0, 0, 0, 0 };
	CustomList<UPicture*>* clPictureList = nullptr;
	class ABackground* aBackground = nullptr;
	class UPlayerManager* uPlayerManager = nullptr;
	class UShopManager* uShopManager = nullptr;
	class UReinForceManager* uReinForceManager = nullptr;
	class UInfoManager* uInfoManager = nullptr;
	class UEnemyManager* uEnemyManager = nullptr;
	class UStageManager* uStageManager = nullptr;
	Vector2 v2MousePosition;
	bool bIsLeftMouseClick = false, bIsRightMouseClick = false, bIsGameEnd = false;
};

