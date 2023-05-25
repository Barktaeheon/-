#include "pch.h"
#include "CBossSword.h"
#include "CBoss.h"
#include "CCastle.h"
#include "CGameMgr.h"
#include "CPlayer.h"
#include "CCollision.h"

CBossSword::CBossSword()
{
}

CBossSword::CBossSword(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const ESWORDID & eSwordID)
	: CSword(pOwner, szImgPath, strName, eSwordID), m_fSpinDistance(0.f)
	, m_v2SaveSpawnPos(VECTOR2()), m_tTimer(TIMER(200))
{
	Initialize();
	m_vecCollisionPoint.reserve(4);
	for (int i = 0; i < 4; ++i)
		m_vecCollisionPoint.emplace_back(VECTOR2());
}


CBossSword::~CBossSword()
{
	Release();
}

void CBossSword::Initialize()
{
	m_v2DrawSize = VECTOR2(300.f, 300.f);
	m_fSpinDistance = 300.f;
	m_fRotValue = 90.f;
	m_v2Size = VECTOR2(100.f, 100.f);
}

int CBossSword::Update()
{
	if (m_bDead)
		return OBJ_DEADEVENT;
	Update_OnwerInfo();
	Spin();
	Rotate();
	Fall();
	Update_Rect();
	return OBJ_NOEVENT;
}

void CBossSword::Late_Update()
{
}

void CBossSword::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
		return;

	CCastle* m_pBoss = static_cast<CCastle*>(m_pOwner);
	if (nullptr == m_pBoss)
		return;
	if (m_pBoss->Is_Magic1())
	{
		if (CGameMgr::Get_Instance()->Is_DevelopMode())
		{
			VECTOR2 v2Size = m_v2DrawSize / 2.f;
			// Left Up
			m_vecCollisionPoint[0].x = m_v2CenterPos.x - v2Size.x / 3.f;
			m_vecCollisionPoint[0].y = m_v2CenterPos.y - v2Size.y;
			// Right Up
			m_vecCollisionPoint[1].x = m_v2CenterPos.x + v2Size.x / 3.f;
			m_vecCollisionPoint[1].y = m_v2CenterPos.y - v2Size.y;
			// Right Down
			m_vecCollisionPoint[2].x = m_v2CenterPos.x + v2Size.x / 3.f;
			m_vecCollisionPoint[2].y = m_v2CenterPos.y + v2Size.y;
			// Left Down
			m_vecCollisionPoint[3].x = m_v2CenterPos.x - v2Size.x / 3.f;
			m_vecCollisionPoint[3].y = m_v2CenterPos.y + v2Size.y;

			Rotation_V2(m_vecCollisionPoint[0], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
			Rotation_V2(m_vecCollisionPoint[1], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
			Rotation_V2(m_vecCollisionPoint[2], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);
			Rotation_V2(m_vecCollisionPoint[3], m_v2CenterPos, m_v2Size.y, m_v2Size.x, -m_fRotValue);

			Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[0], m_vecCollisionPoint[1]);
			Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[1], m_vecCollisionPoint[2]);
			Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[2], m_vecCollisionPoint[3]);
			Draw_Line_V2(_hdc, v2ScrollPos, m_vecCollisionPoint[3], m_vecCollisionPoint[0]);
		}

		Rectangle(_hdc, m_tRect.left, m_tRect.top, m_tRect.right, m_tRect.bottom);

		PlgBlt(m_pRotate->Get_MemDC(),		// ȸ�� ��ų �̹����� ��� �׸� ���ΰ�6
			m_tPoint,			// �׸��� �׸� �簢�� ��ǥ ����Ʈ �迭�� �̸�(������ �� ���, �� ���, �� �ϴ�)
			m_pImgBmp->Get_MemDC(),				// ��� �ִ� �̹����� ȸ����Ų �̹��� DC�� �׸� ���ΰ�
			0,					// 4,5 ���� : �׸��� �׸� �̹����� X,Y ���� ��ǥ
			0,
			static_cast<int>(m_v2DrawSize.x),	// 6,7 ���� : �̹����� ����, ���� ����
			static_cast<int>(m_v2DrawSize.y),
			NULL,
			NULL,
			NULL);
		// ���� �ִϸ��̼��� �׷��� ��ġ
		GdiTransparentBlt(_hdc, static_cast<int>(m_tDrawRect.left + v2ScrollPos.x),
			static_cast<int>(m_tDrawRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
			// �׸� �ִϸ��̼� �׸��� ��ġ
			m_pRotate->Get_MemDC(), 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));

		BitBlt(m_pRotate->Get_MemDC(), 0, 0, static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y), m_pReset->Get_MemDC(), 0, 0, SRCCOPY);
	}
	else if (m_pBoss->Is_Magic2())
	{
		// ���� �ִϸ��̼��� �׷��� ��ġ
		GdiTransparentBlt(_hdc, static_cast<int>(m_tDrawRect.left + v2ScrollPos.x),
			static_cast<int>(m_tDrawRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
			// �׸� �ִϸ��̼� �׸��� ��ġ
			m_pImgBmp->Get_MemDC(), 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));
	}
}

void CBossSword::Collision_Actor(CObj *& pObj)
{
	if (false == m_bEnable)
		return;
	if (nullptr == pObj)
		return;
	if (nullptr == m_pOwner)
		return;

	if (OBJ_PLAYER == pObj->Get_ObjID())
	{
		if (OBJ_MONSTER == m_pOwner->Get_ObjID())
		{
			if (static_cast<CPlayer*>(pObj)->Get_CharState() != CHAR_HIT)
				return;

			if (false == CCollision::Is_OBBColl(pObj, m_pOwner))
				return;
			static_cast<CPlayer*>(pObj)->Damaged(static_cast<float>(static_cast<CCastle*>(m_pOwner)->Get_Status().iAtkPower));
			static_cast<CPlayer*>(pObj)->Set_DamagedDir(m_pOwner->Get_Dir());

			VECTOR2 v2Pos = m_pOwner->Get_CurPos();
			VECTOR2 v2MonPos = pObj->Get_CurPos();
			VECTOR2 v2Dir = v2MonPos - v2Pos;
			v2Dir.Normalize();
			v2Dir.y = 0.f;
			static_cast<CMonster*>(pObj)->Set_DamagedDir(v2Dir * 2.f);
			if(!(static_cast<CPlayer*>(pObj)->Get_CharState() == CHAR_HIT || static_cast<CPlayer*>(pObj)->Get_CharState() == CHAR_DIE))
			CGameMgr::Get_Instance()->Get_EffectMgr()->Blood_Effect(pObj, v2Dir, 0);
			float fDamge = static_cast<float>(static_cast<CCharacter*>(m_pOwner)->Get_Status().iAtkPower +
				m_tStatus.iAtkPower);
			static_cast<CMonster*>(pObj)->Damaged(fDamge);
		}
	}
}

void CBossSword::Release()
{
	m_vecCollisionPoint.clear();
}

void CBossSword::Update_Rect()
{
	VECTOR2 v2Size = m_v2Size / 2.f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CenterPos.x - v2Size.x);
	m_tRect.top = static_cast<long>(m_v2CenterPos.y - v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CenterPos.x + v2Size.x);
	m_tRect.bottom = static_cast<long>(m_v2CenterPos.y + v2Size.y);

	// Draw Size
	v2Size = m_v2DrawSize / 2.f;

	m_tDrawRect.left = static_cast<long>(m_v2CenterPos.x - v2Size.x);
	m_tDrawRect.top = static_cast<long>(m_v2CenterPos.y - v2Size.y);
	m_tDrawRect.right = static_cast<long>(m_v2CenterPos.x + v2Size.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CenterPos.y + v2Size.y);
	// CenterPos, Left, Right
	m_v2CurPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y - v2Size.y);
}

void CBossSword::Rotate()
{
	VECTOR2 v2Size = m_v2DrawSize / 2.f;
	m_fDiagonal = sqrtf(v2Size.x * v2Size.x + v2Size.y * v2Size.y);
	//�� ���
	m_tPoint[0].x = long((m_v2DrawSize.x / 2.f) + cosf((m_fRotValue + 135.f) * (PI / 180.f))* m_fDiagonal);
	m_tPoint[0].y = long((m_v2DrawSize.x / 2.f) - sinf((m_fRotValue + 135.f) * (PI / 180.f))* m_fDiagonal);

	//�� ���
	m_tPoint[1].x = long((m_v2DrawSize.x / 2.f) + cosf((m_fRotValue + 45.f) * (PI / 180.f)) * m_fDiagonal);
	m_tPoint[1].y = long((m_v2DrawSize.y / 2.f) - sinf((m_fRotValue + 45.f) * (PI / 180.f))* m_fDiagonal);

	//�� �ϴ�
	m_tPoint[2].x = long((m_v2DrawSize.x / 2.f) + cosf((m_fRotValue + 225.f) * (PI / 180.f))* m_fDiagonal);
	m_tPoint[2].y = long((m_v2DrawSize.x / 2.f) - sinf((m_fRotValue + 225.f) * (PI / 180.f))* m_fDiagonal);
}

void CBossSword::Spin()
{
	CCastle* m_pBoss = static_cast<CCastle*>(m_pOwner);
	if (nullptr == m_pBoss)
		return;

	if (m_pBoss->Is_Magic1())
	{
		
		VECTOR2 v2BossPos = m_pBoss->Get_CenterPos();
		VECTOR2 v2Pos = VECTOR2(cosf(RADIAN_TO_DEGREE(m_fSpinAngle)), sinf(RADIAN_TO_DEGREE(m_fSpinAngle))) * m_fSpinDistance;
		m_v2CenterPos = v2BossPos + (v2Pos);

		m_fRotValue += 10.f;
		m_fSpinAngle -= 10.f;
	}
	else
	{

		m_fRotValue = 90.f;
		m_fSpinAngle = 0.f;
	}
}

void CBossSword::Fall()
{
	CCastle* m_pBoss = static_cast<CCastle*>(m_pOwner);
	if (nullptr == m_pBoss)
		return;
	if (m_pBoss->Is_Magic2())
	{
		if (true == m_tTimer.Is_PassTime())
		{
			CScrollMgr* pScrollMgr = CGameMgr::Get_Instance()->Get_CScrollMgr();

			//�������� ����	
			m_v2CenterPos.y += 9.8f * m_fJumpPower * m_fDownTime;
			m_fDownTime += 0.001f;
			// ���� �浹 �����̰� �������� ���� ���¶��
				// �׶��带 true�� ����
				// ���� Y���� ������Ų��. 
			if (m_v2CurPos.y >= 1600)
			{
				m_v2CenterPos = m_v2SaveSpawnPos;
				m_fDownTime = 0.f;
			}
		}
	}
	else
	{
		m_tTimer.dwTime = GetTickCount();
	}
}
