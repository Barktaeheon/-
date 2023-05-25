#include "pch.h"
#include "CMonster.h"
#include "CGameMgr.h"
#include "CBossScene1.h"
#include "CScene.h"
#include "CSceneActive.h"
#include "CWall.h"
#include "CMonsterAI.h"
#include "CFollowText.h"

CMonster::CMonster() : m_pMonsterAI(nullptr), m_pTarget(nullptr), m_pNameText(nullptr), m_iDamgeTextIndex(0), MAX_DAMGE_TEXT(0)
, m_iDeadCount(0), m_bDamaged(false), m_bCollision(false)
{
	m_eObjID = OBJ_MONSTER;
}

CMonster::CMonster(CPlayer * pTarget) : m_pMonsterAI(nullptr), m_pTarget(pTarget), m_pNameText(nullptr), m_iDamgeTextIndex(0)
, MAX_DAMGE_TEXT(6)
, m_iDeadCount(0), m_bDamaged(false), m_bCollision(false)
{
	m_eObjID = OBJ_MONSTER;
	m_tRestrictDraw = CGameMgr::Get_Instance()->Get_ClientRect();
	m_tRestrictDraw.left -= 200;
	m_tRestrictDraw.top -= 200;
	m_tRestrictDraw.right += 200;
	m_tRestrictDraw.bottom += 200;

	m_vecDamageText.reserve(MAX_DAMGE_TEXT);
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(255, 0, 0)));
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(0, 0, 255)));
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(0, 255, 0)));
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(255, 0, 0)));
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(0, 0, 255)));
	m_vecDamageText.emplace_back(new CDamageText(this, L"�������", RGBINFO(0, 255, 0)));
}

CMonster::~CMonster()
{
	Release();
}


void CMonster::Collision_Actor(CObj *& pObj)
{
}

void CMonster::Release()
{
	for (auto& iter : m_vecDamageText)
		DELETE_MEMORY(iter);

	m_vecDamageText.clear();
}

void CMonster::Damaged(const float & fDamage)
{
	if (false == m_bDead && CHAR_HIT != m_eCharState)
	{
		Change_State(CHAR_HIT);
		int iHp = static_cast<int>(m_tStatus.iCurHp - fDamage);
		m_tStatus.iCurHp = iHp;
		if (m_tStatus.iCurHp <= 0)
		{
			m_bDead = true;

			m_eDirection = m_pMonsterAI->Get_SaveDirection();
			Change_State(CHAR_DIE);
		}
		else
		{
			Active_DamageText(fDamage);
		}
	}
}

void CMonster::Obj_Active(const bool & IsActive)
{
}

void CMonster::Active_DamageText(const float& fDamage)
{
	VECTOR2 v2RandPos = VECTOR2(static_cast<float>(rand() % 5), static_cast<float>(rand() % 5));
	VECTOR2 v2Pos;
	int ranMinus = rand() % 2;
	if (0 == ranMinus)
	{
		v2Pos = m_v2CenterPos + v2RandPos;
	}
	else if (1 == ranMinus)
	{
		v2Pos = m_v2CenterPos - v2RandPos;
	}
	TCHAR szValue[50];
	
	wsprintf(szValue, L"%d", static_cast<int>(fDamage));
	m_vecDamageText[m_iDamgeTextIndex]->Set_Text(wstring(szValue));
	m_vecDamageText[m_iDamgeTextIndex]->Set_CurPos(v2Pos);
	m_vecDamageText[m_iDamgeTextIndex++]->Obj_Active(true);
	if (m_iDamgeTextIndex >= MAX_DAMGE_TEXT)
		m_iDamgeTextIndex = 0;
}

void CMonster::Blood_Effect_To_Player(CObj * pObj, const int& iBloodIndex)
{
	VECTOR2 v2Pos = m_v2CurPos;
	VECTOR2 v2MonPos = pObj->Get_CurPos();
	VECTOR2 v2Dir = v2MonPos - v2Pos;
	v2Dir.Normalize();
	v2Dir.y = 0.f;
	m_pTarget->Set_DamagedDir(v2Dir);
	CGameMgr::Get_Instance()->Get_EffectMgr()->Blood_Effect(pObj, v2Dir, iBloodIndex);
	m_bCollision = true;
}

void CMonster::Collision_Wall()
{
	CSceneMgr* pSceneMgr = CGameMgr::Get_Instance()->Get_SceneMgr();
	list<CSceneActive*> wallList = pSceneMgr->Get_CurScene()->Get_SceneActiveList(SCA_WALL);
	RECT rc;
	if (nullptr == m_pCollWall)
	{
		if (m_eCollDirection != m_eDirection)
		{
			for (auto& iter : wallList)
			{
				if (TRUE == IntersectRect(&rc, &iter->Get_Rect(), &m_tRect))
				{
					m_pCollWall = static_cast<CWall*>(iter);
				}
			}
		}
	}
	else
	{
		if (FALSE == IntersectRect(&rc, &m_pCollWall->Get_Rect(), &m_tRect))
		{
			m_pCollWall = nullptr;
		}
	}
}

void CMonster::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-20.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CMonster::Dead()
{
	if (CHAR_DIE == m_eCharState)
	{
		if (true == m_pMonsterAI->Is_DeadTimerOk())
		{
			m_pNameText->Set_Dead(true);
			m_bDead = true;
			++m_iDeadCount;
		}
	}
}

void CMonster::Move()
{
	if (nullptr != m_pCollWall && DIR_END == m_eCollDirection)
	{
		if (WALL_LR == m_pCollWall->Get_WallID())
		{
			VECTOR2 v2CollPos = m_pCollWall->Get_CenterPos();
			VECTOR2 v2Dir = m_v2CurPos - v2CollPos;
			if (v2Dir.x > 0)
				m_eCollDirection = DIR_FOR;
			else
				m_eCollDirection = DIR_BACK;
		}
	}
	else if (nullptr == m_pCollWall)
		m_eCollDirection = DIR_END;

	if (DIR_FOR == m_eDirection)
		m_v2Dir = VECTOR2(1.f, 0.f);
	else if (DIR_BACK == m_eDirection)
		m_v2Dir = VECTOR2(-1.f, 0.f);

	float fSpeed = static_cast<float>(m_tStatus.iMoveSpeed);
	if (CHAR_HIT == m_eCharState)
	{
		fSpeed = 3.f;
	}
	if (DIR_END != m_eDirection)
	{
		if (CHAR_IDLE == m_eCharState || CHAR_MOVE == m_eCharState)
			Change_State(CHAR_MOVE);
	}

	if (CHAR_MOVE == m_eCharState)
	{
		if (nullptr != m_pCollWall)
			m_v2Dir = VECTOR2() - m_v2Dir;
		m_v2CurPos = m_v2CurPos + (m_v2Dir * fSpeed);
	}
	else if ((CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState) && false == m_bIsGround)
	{
		if (nullptr != m_pCollWall)
			m_v2DamagedDir = VECTOR2() - m_v2DamagedDir;
		m_v2CurPos = m_v2CurPos + (m_v2DamagedDir * fSpeed);
	}
}

void CMonster::Jump()
{
	CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();

	float fY = 0.f;
	bool bIsColl = pLineMgr->Collision_Line(m_v2LeftPos, &fY) || pLineMgr->Collision_Line(m_v2RightPos, &fY);
	// ����
	if (false == m_bJump)
	{
		// �ٴڿ� ���� ���� ���� Ȯ���ϰ� �����Ѵ�. (�ߺ� ���� ����) 
		if (true == m_bIsGround)
		{
			// ���� ���� ���¶��
			if ((CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState) && false ==  m_bDamaged)
			{
				// Ÿ�̸� �ʱ�ȭ
				// ���� ���� ���°� �ƴ��� ���
				m_fDownTime = m_fRestrictTime;
				m_bIsGround = false;
				m_bJump = true;
				m_bDamaged = true;
			}
			else
			{
				if(!(CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState))
					m_bDamaged = false;
			}
		}
		//�������� ����	
		m_v2CurPos.y -= 50 * -9.8f * m_fDownTime;
		m_fDownTime += 0.001f;
	}
	// ���� ���� �� ��
	if (true == m_bJump)
	{
		// ������ ��, Wall ���� Ȯ��
		if (nullptr != m_pCollWall)
		{
			if (WALL_DW == m_pCollWall->Get_WallID())
			{
				// ���� ���� �ƴ�, Ÿ�̸� �ʱ�ȭ
				m_bJump = false;
				m_fDownTime = 0.f;
			}
		}
		// ���� �ھƿ����� �ִ� ��
		float fChangeValue = m_fJumpPower;
		m_v2CurPos.y += fChangeValue * -9.8f * m_fDownTime;
		m_fDownTime -= 0.0008f;
		// ���� ���� ������ �������� ���� �Ÿ� �̻� �پ� �ö��ٸ�
		if (m_fDownTime < 0.f)//y������ �÷��̾�y���� Ŭ��
		{
			// ���� ���� �ƴ�, Ÿ�̸� �ʱ�ȭ
			m_bJump = false;
			m_fDownTime = 0.f;
		}
	}
	// ���� �浹 �����̰� �������� ���� ���¶��
	if (true == bIsColl && false == m_bJump)
	{
		// �׶��带 true�� ����
		// ���� Y���� ������Ų��. 
		if (m_v2CurPos.y >= fY)
		{
			m_v2CurPos.y = fY;
			m_bIsGround = true;
		}
	}

	// ���� �ð� ���� �� ���� Ÿ�̸� �ʱ�ȭ
	if (m_fDownTime > m_fRestrictTime)
		m_fDownTime = m_fRestrictTime;
}
