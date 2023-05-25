#include "pch.h"
#include "CSword.h"
#include "CGameMgr.h"
#include "CPlayer.h"
#include "CMonster.h"

CSword::CSword() : m_eSwordID(SWORD_END), m_bRotate(false)
{
	Initialize();
}

CSword::CSword(CObj* pOwner, const TCHAR * szImgPath, const wstring & strName, const ESWORDID & eSwordID)
	: CWeapon(pOwner, szImgPath, strName), m_eSwordID(eSwordID), m_pReset(NULL), m_pRotate(NULL), m_fSpinAngle(0.f)
	, m_bRotate(false), m_iMultipleValue(0)
{
	Initialize();
	m_eWeaponID = WEAP_SWORD;
}

CSword::CSword(CObj * pOwner, const TCHAR * szImgPath, const wstring & strName, const ESWORDID & eSwordID, const int & iPower, const int& iGold)
	: CWeapon(pOwner, szImgPath, strName), m_eSwordID(eSwordID), m_pReset(NULL), m_pRotate(NULL), m_fSpinAngle(0.f)
{
	Initialize();
	m_eWeaponID = WEAP_SWORD;
	m_tStatus.iAtkPower = iPower;
	m_iGold = iGold;
}


CSword::~CSword()
{
	Release();
}

void CSword::Initialize()
{
	m_pRotate = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_ROTATE");
	m_pReset = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(L"ITEM_RESET");
	m_v2DrawSize = VECTOR2(180.f, 180.f);
	m_v2Size = m_v2DrawSize;

	m_tSpinTimer.iPassTime = 200;
}

int CSword::Update()
{
	if (m_bDead)
		return OBJ_DEADEVENT;
	Update_OnwerInfo();
	Rotate();
	Fall();
	Update_Rect();
	return OBJ_NOEVENT;
}

void CSword::Late_Update()
{
}

void CSword::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	CBmpMgr* pBmpMgr = CGameMgr::Get_Instance()->Get_BmpMgr();
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
		return;

	HDC	hRotaionDC = m_pRotate->Get_MemDC();
	HDC	hResetDC = m_pReset->Get_MemDC();
	//Rectangle(_hdc, static_cast<int>(m_vecCollisionPoint[0].x + v2ScrollPos.x), static_cast<int>(m_vecCollisionPoint[0].y + v2ScrollPos.y),
	//	static_cast<int>(m_vecCollisionPoint[2].x + v2ScrollPos.x), static_cast<int>(m_vecCollisionPoint[2].y + v2ScrollPos.y));

	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));
	
	if (m_bRotate)
	{
		HDC	hRotaionDC = m_pRotate->Get_MemDC();
		HDC	hResetDC = m_pReset->Get_MemDC();

		PlgBlt(hRotaionDC,		// ȸ�� ��ų �̹����� ��� �׸� ���ΰ�6
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
			hRotaionDC, 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));

		BitBlt(hRotaionDC, 0, 0, static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y), hResetDC, 0, 0, SRCCOPY);
	}
	else
	{
		if (false == m_bItemPicked)
		{
			// ���� �ִϸ��̼��� �׷��� ��ġ
			GdiTransparentBlt(_hdc, static_cast<int>(m_tDrawRect.left + v2ScrollPos.x),
				static_cast<int>(m_tDrawRect.top + v2ScrollPos.y), static_cast<int>(m_v2DrawSize.x), static_cast<int>(m_v2DrawSize.y),
				// �׸� �ִϸ��̼� �׸��� ��ġ
				m_pImgBmp->Get_MemDC(), 0, 0, m_pImgBmp->Get_Width(), m_pImgBmp->Get_Height(), RGB(0, 0, 0));
		}
	}
}

void CSword::Collision_Actor(CObj *& pObj)
{
	if (false == m_bEnable)
		return;
	if (nullptr == pObj)
		return;
	if (nullptr == m_pOwner)
		return;
	RECT rc;
	if (OBJ_PLAYER == m_pOwner->Get_ObjID())
	{
		if (OBJ_MONSTER == pObj->Get_ObjID())
		{
			if (FALSE == IntersectRect(&rc, &pObj->Get_Rect(), &m_tRect))
				return;

			static_cast<CMonster*>(pObj)->Damaged(static_cast<float>(static_cast<int>(static_cast<CPlayer*>(m_pOwner)->Get_Status().iAtkPower)));
		}
	}
}

void CSword::Release()
{
}

CItem * CSword::Copy_Item()
{
	return nullptr;
}

void CSword::Update_Rect()
{
	VECTOR2 v2Size = m_v2DrawSize / 2.f;
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size

	// CenterPos, Left, Right
	m_v2CurPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y - v2RealSize.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CenterPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CenterPos.y - v2RealSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CenterPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CenterPos.y + v2RealSize.y);
	m_tRect = m_tDrawRect;

	m_v2AttackPos = VECTOR2(m_v2CenterPos.x, m_v2CenterPos.y - v2RealSize.y / 2.f);
}

void CSword::Rotate()
{
	VECTOR2 v2Size = m_v2DrawSize / 2.f;
	m_fDiagonal = sqrtf(v2Size.x * v2Size.x + v2Size.y * v2Size.y);
	//�� ���
	Rotation_Point(m_tPoint[0], VECTOR2(v2Size.x, v2Size.x), 135.f, m_fRotValue, m_fDiagonal, m_fDiagonal);
	//�� ���
	Rotation_Point(m_tPoint[1], VECTOR2(v2Size.x, v2Size.y), 45.f, m_fRotValue, m_fDiagonal, m_fDiagonal);
	//�� �ϴ�
	Rotation_Point(m_tPoint[2], VECTOR2(v2Size.x, v2Size.x), 225.f, m_fRotValue, m_fDiagonal, m_fDiagonal);

}

void CSword::Update_OnwerInfo()
{
	if (true == m_bUse && true == m_bRotate)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pOwner);
		if (nullptr != pPlayer)
		{
			if (m_tSpinTimer.Is_PassTime())
			{
				m_tSpinTimer.dwTime = GetTickCount();
				float fDistance = 33.f;
				if (m_iMultipleValue == -1)
					fDistance = 50.f;
				VECTOR2 v2Pos = VECTOR2(cosf(RADIAN_TO_DEGREE(m_fSpinAngle)), sinf(RADIAN_TO_DEGREE(m_fSpinAngle)));
				VECTOR2 v2WeaponPos = pPlayer->Get_EquipPos();
				m_v2CenterPos = v2WeaponPos + (v2Pos * fDistance);
				m_fRotValue -= 24.f * m_iMultipleValue;
				m_fSpinAngle += 24.f* m_iMultipleValue;
			}

		}
	}
}

void CSword::Fall()
{
	if (false == m_bUse && false == m_bRotate)
	{
		CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

		float fY = 0.f;
		bool bIsColl = pLineMgr->Collision_Line(m_v2CenterPos, &fY);
		if (false == bIsColl)
		{
			//�������� ����	
			m_v2CenterPos.y -= 50 * -9.8f * m_fDownTime;
			m_fDownTime += 0.001f;

			// ���� �浹 �����̰� �������� ���� ���¶��
			if (true == bIsColl)
			{
				// �׶��带 true�� ����
				// ���� Y���� ������Ų��. 
				if (m_v2CenterPos.y >= fY)
				{
					m_v2CenterPos.y = fY;
				}
			}

			// ���� �ð� ���� �� ���� Ÿ�̸� �ʱ�ȭ
			if (m_fDownTime > m_fRestrictTime)
				m_fDownTime = m_fRestrictTime;
		}
	}
}
