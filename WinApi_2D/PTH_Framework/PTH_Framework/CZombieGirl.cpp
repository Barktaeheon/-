#include "pch.h"
#include "CZombieGirl.h"
#include "CFollowText.h"
#include "CMonsterAI.h"
#include "CGameMgr.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CAnimation_L.h"
#include "CAnimClip.h"
#include "CAnimClip_N.h"
#include "CCollision.h"
#include "CWall.h"
#include "CDownWall.h"


CZombieGirl::CZombieGirl()
{
	Initialize();
}

CZombieGirl::CZombieGirl(CPlayer * pTarget) : CMonster(pTarget)
{
	Initialize();
}

CZombieGirl::~CZombieGirl()
{
	Release();
}

void CZombieGirl::Initialize()
{
	// Name Text
	m_strName = L"ZombieGirl";
	m_pNameText = new CFollowText(this, L"��������", tagRGB(255, 255, 255));
	m_pNameText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pNameText->Set_Size(VECTOR2(15.f, 0.f));
	m_pNameText->Set_Text(m_strName);
	m_pNameText->Set_Enable(true);
	m_eObjID = OBJ_MONSTER;
	// Monster AI
	CMonster* pMonster = this;
	m_pMonsterAI = new CMonsterAI(pMonster, m_pTarget);
	m_pMonsterAI->Set_Find_Range(300);
	m_pMonsterAI->Set_Attack_Range(150);
	m_v2Size = VECTOR2(60.f, 90);
	m_v2DrawSize = VECTOR2(200.f, 200.f);
	m_eDirection = DIR_BACK;
	// Jump Define
	m_fJumpPower = 10.f;
	m_fRestrictTime = 0.02f;
	// Status Define
	m_tStatus.iMoveSpeed = 2;
	m_tStatus.iAtkPower = 1;
	m_tStatus.iMaxHp = 10;
	m_tStatus.iMaxMp = 0;
	m_tStatus.iCurHp = m_tStatus.iMaxHp;
	m_tStatus.iCurMp = m_tStatus.iMaxMp;
	Anim_Setting();
}

int CZombieGirl::Update()
{
	if (true == m_bDead)
	{
		Dead();
		if (1 <= m_iDeadCount)
		{
			for (auto& iter : m_vecDamageText)
				iter->Set_Enable(false);
			return OBJ_DEADEVENT;
		}
	}

	if (nullptr != m_pMonsterAI)
	{
		if (CHAR_ATTACK != m_eCharState)
			m_pMonsterAI->Update();
		Attack();
		Move();
		Jump();
	}
	Update_Text();
	Update_Rect();

	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Update_Animation();
	}

	for (auto& iter : m_vecDamageText)
		iter->Update();

	return OBJ_NOEVENT;
}

void CZombieGirl::Late_Update()
{
	Collision_Wall();
	CObjMgr* pObjMgr = CGameMgr::Get_Instance()->Get_ObjMgr();
	list<CObj*> itemList = pObjMgr->Get_ObjList(OBJ_EQUIP);
	CObj* pAss = static_cast<CObj*>(this);
	CObj* pObj = static_cast<CObj*>(m_pTarget);
	for (auto& iter : itemList)
	{
		iter->Collision_Actor(pAss);
	}
	Collision_Actor(pObj);
}

void CZombieGirl::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	if (false == Check_Pos_IsInRect(v2ScrollPos, m_tRestrictDraw))
	{
		m_pNameText->Set_Enable(false);
		m_bActiveUI = false;
		return;
	}
	else if (false == m_bActiveUI)
	{
		m_pNameText->Set_Enable(true);
		m_bActiveUI = true;
	}
	//Rectangle(_hdc, static_cast<int>(m_tRect.left + v2ScrollPos.x), static_cast<int>(m_tRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tRect.right + v2ScrollPos.x), static_cast<int>(m_tRect.bottom + v2ScrollPos.y));

	//Rectangle(_hdc, static_cast<int>(m_tAttackRect.left + v2ScrollPos.x), static_cast<int>(m_tAttackRect.top + v2ScrollPos.y),
	//	static_cast<int>(m_tAttackRect.right + v2ScrollPos.x), static_cast<int>(m_tAttackRect.bottom + v2ScrollPos.y));


	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Render_Animation(_hdc, v2ScrollPos);
	}

	m_pNameText->Render(_hdc, v2ScrollPos);

	for (auto& iter : m_vecDamageText)
	{
		if (iter->Is_Enable())
			iter->Render(_hdc, v2ScrollPos);
	}
}

void CZombieGirl::Collision_Actor(CObj *& pObj)
{
	if (nullptr == pObj)
		return;
	if (true == m_bCollision)
		return;

	if (OBJ_PLAYER != pObj->Get_ObjID())
		return;
	RECT rc;
	if (CHAR_ATTACK == m_eCharState)
	{
		// Update Attack Rect
		Update_Attack_Rect();
		// Intersect 
		if (FALSE == IntersectRect(&rc, &pObj->Get_Rect(), &m_tAttackRect))
			return;
		if (DIR_FOR == m_eDirection)
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() <= 3)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
		else
		{
			if (m_pAnimator->Get_Animation()->Get_AnimIndex() >= 9)
			{
				m_pTarget->Damaged(static_cast<float>(m_tStatus.iAtkPower));
				Blood_Effect_To_Player(pObj, 0);
			}
		}
	}
}

void CZombieGirl::Release()
{
	for (auto& iter : m_vecDamageText)
		DELETE_MEMORY(iter);

	DELETE_MEMORY(m_pNameText);
	DELETE_MEMORY(m_pAnimator);
	DELETE_MEMORY(m_pMonsterAI);
}

void CZombieGirl::Damaged(const float & fDamage)
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

void CZombieGirl::Change_State(const ECHARSTATE & eCharState)
{
	if (CHAR_HIT == eCharState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_L");

		m_eCharState = eCharState;
		m_v2DrawSize = VECTOR2(400.f, 400.f);
	}
	else if (CHAR_DIE == eCharState)
	{
		if (DIR_FOR == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_R");
		else if (DIR_BACK == m_eDirection)
			m_pAnimator->Set_CurAnimation(L"HIT_L");

		m_v2DrawSize = VECTOR2(400.f, 400.f);
		m_eCharState = eCharState;
	}
	else
	{
		if (CHAR_IDLE == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"IDLE_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"IDLE_L");
			m_v2DrawSize = VECTOR2(200.f, 200.f);
		}
		else if (CHAR_MOVE == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"RUN_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"RUN_L");
			m_v2DrawSize = VECTOR2(200.f, 200.f);
		}
		else if (CHAR_ATTACK == eCharState)
		{
			if (DIR_FOR == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"ATTACK_R");
			else if (DIR_BACK == m_eDirection)
				m_pAnimator->Set_CurAnimation(L"ATTACK_L");
			m_v2DrawSize = VECTOR2(400.f, 400.f);
		}
		m_eCharState = eCharState;
	}
}

CMonster * CZombieGirl::Copy_Monster()
{
	CMonster* pMonster = new CZombieGirl(m_pTarget);
	pMonster->Set_Enable(true);
	return pMonster;
}

void CZombieGirl::Obj_Active(const bool & IsActive)
{
	m_bEnable = IsActive;
	m_pNameText->Set_Enable(false);
}

void CZombieGirl::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos, Left, Right
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	m_v2LeftPos = VECTOR2(m_v2CurPos.x - v2RealSize.x / 6.f, m_v2CurPos.y);
	m_v2RightPos = VECTOR2(m_v2CurPos.x + v2RealSize.x / 6.f, m_v2CurPos.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2DrawSize.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}

void CZombieGirl::Update_Attack_Rect()
{
	if (DIR_FOR == m_eDirection)
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x + m_v2DrawSize.x / 2.5f);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f - 50);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f);
	}
	else
	{
		m_tAttackRect.left = static_cast<int>(m_v2CenterPos.x - m_v2DrawSize.x / 2.5f);
		m_tAttackRect.right = static_cast<int>(m_v2CenterPos.x);
		m_tAttackRect.top = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f - 50);
		m_tAttackRect.bottom = static_cast<int>(m_v2CenterPos.y + m_v2Size.y / 2.f);
	}
}

void CZombieGirl::Update_Text()
{
	if (nullptr != m_pNameText)
	{
		m_pNameText->Set_CurPos(VECTOR2(-35.f, -m_v2Size.y - 20.f) + m_v2CurPos);
		m_pNameText->Update();
	}
}

void CZombieGirl::Dead()
{
	if (CHAR_DIE == m_eCharState)
	{
		if (true == m_pMonsterAI->Is_DeadTimerOk())
		{
			++m_iDeadCount;
		}
		m_pAnimator->Get_Animation()->Set_LastAnimFix(true);
	}
}

void CZombieGirl::Attack()
{
	if (CHAR_ATTACK == m_eCharState)
	{
		if (true == m_pAnimator->Get_Animation()->Is_last())
		{
			if (m_pMonsterAI->Is_FindTarget())
				Change_State(CHAR_MOVE);
			else
				Change_State(CHAR_IDLE);

			m_bCollision = false;
		}
	}
}

void CZombieGirl::Move()
{
	CMonster::Move();
}

void CZombieGirl::Jump()
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
			if ((CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState) && false == m_bDamaged)
			{
				// Ÿ�̸� �ʱ�ȭ
				// ���� ���� ���°� �ƴ��� ����
				m_fDownTime = m_fRestrictTime;
				m_bIsGround = false;
				m_bJump = true;
				m_bDamaged = true;
			}
			else
			{
				if (!(CHAR_HIT == m_eCharState || CHAR_DIE == m_eCharState))
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

void CZombieGirl::Anim_Setting()
{
	CObj* pObj = this;
	m_pAnimator = new CAnimator(pObj);
	ANIM_IDLE();
	ANIM_RUN();
	ANIM_ATTACK();
	ANIM_HIT();
	ANIM_DAMAGED();
	ANIM_DEAD();
	// ������, �ִϸ��̼� �������ִ� �Լ� ȣ��
	m_pAnimator->Set_CurAnimation(L"DIE_R");
}

void CZombieGirl::ANIM_IDLE()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ZOMBIEGIRL_IDLE_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 23; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_R", pAnimation);
	pAnimation = new CAnimation(L"ZOMBIEGIRL_IDLE_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 23; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"IDLE_L", pAnimation);
}

void CZombieGirl::ANIM_RUN()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ZOMBIEGIRL_WALK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 11; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300) , 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_R", pAnimation);
	pAnimation = new CAnimation(L"ZOMBIEGIRL_WALK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(100);
	for (int i = 0; i < 11; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 300), 0 }, POINT{ 300, 300 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"RUN_L", pAnimation);
}

void CZombieGirl::ANIM_ATTACK()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ZOMBIEGIRL_ATTACK_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 14; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_R", pAnimation);
	pAnimation = new CAnimation(L"ZOMBIEGIRL_ATTACK_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 14; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"ATTACK_L", pAnimation);
}

void CZombieGirl::ANIM_HIT()
{
	CAnimation*		pAnimation = nullptr;
	CAnimClip*			pAnimClip = nullptr;
	pAnimation = new CAnimation_L(L"ZOMBIEGIRL_DAMAGE_TAKEN_R");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(175);
	for (int i = 0; i < 1; ++i) {

		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500) , 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_R", pAnimation);
	pAnimation = new CAnimation(L"ZOMBIEGIRL_DAMAGE_TAKEN_L");
	pAnimation->Set_RGB(0, 0, 0);
	pAnimation->Set_Fps(75);
	for (int i = 0; i < 1; ++i)
	{
		pAnimClip = new CAnimClip(POINT{ static_cast<int>(i * 500), 0 }, POINT{ 500, 500 });
		pAnimation->Insert_Anim(pAnimClip);
	}
	m_pAnimator->Add_Animation(L"HIT_L", pAnimation);
}

void CZombieGirl::ANIM_DAMAGED()
{
}

void CZombieGirl::ANIM_DEAD()
{
}