#include "pch.h"
#include "CPotal.h"
#include "CPlayer.h"
#include "CGameMgr.h"
#include "CScene.h"

CPotal::CPotal()
{
	Initialize();
}

CPotal::CPotal(const ESCAID& eScaID, const RECT & tRect) : CSceneActive(eScaID, tRect)
{
	Initialize();
}

CPotal::~CPotal()
{
	Release();
}

void CPotal::Initialize()
{
	CSceneActive::Initialize();
	// �����ֱ� ����
	m_iR = 100;
	m_iG = 100;
	m_iB = 100;
}

bool CPotal::Collision_Obj(CObj *& pObj, const VECTOR2& v2ScrollPos)
{
	if (nullptr == pObj)
		return false;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return false;

	RECT rc;
	// �÷��̾� ������
	CPlayer* pPlayer = static_cast<CPlayer*>(pObj);
	if (FALSE == IntersectRect(&rc, &pPlayer->Get_Rect(), &m_tRect))
		return false;
	// Stage Change
	CGameMgr::Get_Instance()->Get_SceneMgr()->Change_Stage(true, m_tRect);

	return false;
}

void CPotal::Render(HDC _hdc, const VECTOR2 & v2ScrollPos, const bool& IsRender)
{
	CSceneActive::Render(_hdc, v2ScrollPos, IsRender);
}

void CPotal::Release()
{
}

bool CPotal::Is_Check_Equal_YPos(RECT tRect)
{
	RECT rc;
	tRect.left = m_tRect.left;
	tRect.right = m_tRect.right;
	if (TRUE == IntersectRect(&rc, &tRect, &m_tRect))
		return true;

	return false;
}
