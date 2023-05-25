#include "pch.h"
#include "CMonsterAI.h"
#include "CGameMgr.h"
#include "CLineMgr.h"

CMonsterAI::CMonsterAI() : m_pTarget(nullptr), m_fDistance(0.f)
, m_iFindRange(0), m_iAttackRange(0), m_iStopRange(0),
m_bFindTarget(false), m_bAttack(false), 
m_tMoveTimer(TIMER(8000)),m_tHurtTimer(TIMER(1000)), m_tCatchTimer(TIMER(5000)), m_tIdleTimer(TIMER(2000)),
m_tDeadTimer(TIMER(1500)),
m_eSaveDirection(DIR_END), m_bDeadTimerOk(false), m_bNotFind(false), m_pLeftLine(nullptr), m_pRightLine(nullptr)
{
}

CMonsterAI::CMonsterAI(CMonster * pOwner, CPlayer* pTarget) : m_pOwner(pOwner), m_pTarget(pTarget)
, m_bFindTarget(false), m_bAttack(false), m_fDistance(0.f)
, m_iFindRange(0), m_iAttackRange(0), m_iStopRange(100),
	m_tMoveTimer(TIMER(5000)), m_tHurtTimer(TIMER(1500)), m_tCatchTimer(TIMER(5000)), m_tIdleTimer(TIMER(2000)),
	m_tDeadTimer(TIMER(5000)),
	m_eSaveDirection(DIR_END), m_bDeadTimerOk(false), m_bNotFind(false), m_pLeftLine(nullptr), m_pRightLine(nullptr)
{
	
}

CMonsterAI::~CMonsterAI()
{
	m_pLeftLine = nullptr;
}

int CMonsterAI::Update()
{
	Control_Monster();

	return 0;
}

void CMonsterAI::Late_Update()
{
}

void CMonsterAI::Relase()
{
}

bool CMonsterAI::Collision_Situation()
{
	if (CHAR_ATTACK != m_pTarget->Get_CharState())
		return false;

	RECT rc;
	if(FALSE == IntersectRect(&rc, &m_pOwner->Get_AttackRect(), &m_pTarget->Get_Rect()))
		return false;

	return false;
}

void CMonsterAI::Control_Monster()
{
	if (nullptr != m_pTarget && nullptr != m_pOwner)
	{
		VECTOR2 v2SelfPos = m_pOwner->Get_CurPos();
		VECTOR2 v2TargetPos = m_pTarget->Get_CurPos();
		// ���� ���Ϳ� �÷��̾�� �Ÿ��� ���ϱ� ���� ��
		VECTOR2 dir = v2TargetPos - v2SelfPos;
		dir.y = 0.f;
		VECTOR2 v2RealDir = m_pOwner->Get_Dir();

	
		bool IsCatch = false;
		EDIR eTargetDir = m_pTarget->Get_Direction();
		ECHARSTATE eCharState = m_pOwner->Get_CharState();
		if (true == m_pOwner->Is_Dead())
		{
			if (true == m_tDeadTimer.Is_PassTime())
			{
				m_bDeadTimerOk = true;
			}
			return;
		}
		if (false == m_pOwner->Is_Dead())
		{
			m_bDeadTimerOk = false;
			m_tDeadTimer.dwTime = GetTickCount();
			if (CHAR_HIT == eCharState)
			{
				if (true == m_tHurtTimer.Is_PassTime())
				{
					m_tHurtTimer.dwTime = GetTickCount();
					m_pOwner->Change_State(CHAR_MOVE);
					m_bFindTarget = true;
				}
			}
			else
			{
				CLineMgr* pLineMgr = CGameMgr::Get_Instance()->Get_LineMgr();
				float fY = 0.f;
				VECTOR2 v2LeftPos = m_pOwner->Get_LeftPos();
				VECTOR2 v2RightPos = m_pOwner->Get_RightPos();
				m_pLeftLine = pLineMgr->Get_Collision_Line(v2LeftPos - VECTOR2(10.f, 0.f), &fY);
				m_pRightLine = pLineMgr->Get_Collision_Line(v2RightPos + VECTOR2(10.f, 0.f), &fY);

				if (nullptr == m_pLeftLine)
				{
					m_pOwner->Set_Direction(DIR_FOR);
					return;
				}
				else if (nullptr == m_pRightLine)
				{
					m_pOwner->Set_Direction(DIR_BACK);
					return;
				}
				IsCatch = Is_Catch_EqualYPos(v2SelfPos.y, v2TargetPos.y, 50, 5);
				// ���� �ð� ���������� �ʱ�ȭ
				m_tHurtTimer.dwTime = GetTickCount();
				// Ÿ���� ã�� ������ ���
				if (false == m_bFindTarget)
				{
					// ���⿡ ���� ó��
					if (CHAR_IDLE == eCharState)
					{
						if (m_tMoveTimer.Is_PassTime())
						{
							Set_Move_To_Dir(dir);
							m_tMoveTimer.dwTime = GetTickCount();
						}
					}
					else if (CHAR_MOVE == eCharState)
					{
						if (m_tIdleTimer.Is_PassTime())
						{
							m_pOwner->Set_CharState(CHAR_IDLE);
							m_tIdleTimer.dwTime = GetTickCount();
						}
					}
					// �ڽ��� ������ �ϴ� ���⿡ �÷��̾ ������ �ʴ´ٸ� ����
					if (false == Is_Check_In_Target_Forward())
					{
						if (!(m_fDistance <= m_iAttackRange))
						{
	 						Set_Move_To_Dir(v2RealDir);
							return;
						}
					}
					// ���� Ÿ�ٰ� �ڽ��� ���� ��ġ�� ���� ������ ����
					if (false == IsCatch)
					{
 						Set_Move_To_Dir(v2RealDir);
						return;
					}
				}

				if (false == IsCatch)
				{
					if (true == m_tCatchTimer.Is_PassTime())
					{
						m_bFindTarget = false;
						m_tCatchTimer.dwTime = GetTickCount();
						return;
					}
				}
				else
				{
					m_tCatchTimer.dwTime = GetTickCount();
				}

				m_fDistance = fabs(dir.Get_Length_Sqrt());
				// ���� ���Ϳ� Ÿ���� �Ÿ��� ã�� ���� �ȿ� ���Դٸ�
				if (m_fDistance <= m_iFindRange)
				{
					m_bFindTarget = true;
					if (m_fDistance <= m_iAttackRange)
					{
						// ���� �ڸ��� ���� ��
						if (true == IsCatch)
						{
							v2SelfPos = m_pOwner->Get_CurPos();
							v2TargetPos = m_pTarget->Get_CurPos();
							VECTOR2 compareDir = v2TargetPos - v2SelfPos;
							// ���� ���� ���⿡ �ִ��� Ȯ��
							if (((compareDir.x < 0 && dir.x < 0) || (compareDir.x >= 0 && dir.x >= 0)))
							{
								Set_Go_Dir(compareDir);
								m_eSaveDirection = m_pOwner->Get_Direction();
							}
							m_pOwner->Change_State(CHAR_ATTACK);
						}
						else
						{
							if (true == Is_Catch_EqualYPos(v2SelfPos.y, v2TargetPos.y, 300, 300))
							{
								m_pOwner->Change_State(CHAR_IDLE);
								m_eSaveDirection = m_pOwner->Get_Direction();
								m_pOwner->Set_Direction(DIR_END);
							}
							else
							{
								Set_Move_To_Dir(dir);
								m_bFindTarget = false;
							}
						}
					}
					else
					{
						Set_Move_To_Dir(dir);
					}
				}
  				else
				{
					if (m_fDistance <= m_iAttackRange)
					 {
						m_eSaveDirection = m_pOwner->Get_Direction();
						m_pOwner->Change_State(CHAR_ATTACK);
					}
					else
					{
						m_bFindTarget = false;
					}
				}
			}
		}
		else
		{
			if (true == m_tDeadTimer.Is_PassTime())
			{
				m_bDeadTimerOk = true;
			}
		}
	}
}

bool CMonsterAI::Is_Catch_EqualYPos(const float & fSelfYPos, const float & fTargetYPos, const int& iValue1, const int& iValue2)
{
	float fUnderSelfPosY = fSelfYPos - iValue1;
	float fUpSelfPosY = fSelfYPos + iValue2;

	if (fUnderSelfPosY <= fTargetYPos && fUpSelfPosY >= fTargetYPos)
		return true;
	return false;
}

bool CMonsterAI::Is_Check_In_Target_Forward()
{
	EDIR eSelfDir = m_pOwner->Get_Direction();
	EDIR eTargetDir = m_pTarget->Get_Direction();

	VECTOR2 v2Pos = m_pOwner->Get_CurPos();
	VECTOR2 v2TargetPos = m_pTarget->Get_CurPos();

	RECT rc;
	RECT FindRect;
	VECTOR2 dir = v2TargetPos - v2Pos;

	if (eSelfDir == DIR_FOR)
	{
		FindRect.left = static_cast<int>(v2Pos.x);
		FindRect.right = static_cast<int>(v2Pos.x + m_iFindRange);
		FindRect.top = static_cast<int>(v2Pos.y - 10);
		FindRect.bottom = static_cast<int>(v2Pos.y + 10);
	}
	else	if (eSelfDir == DIR_BACK)
	{
		FindRect.left = static_cast<int>(v2Pos.x - m_iFindRange) ;
		FindRect.right = static_cast<int>(v2Pos.x);
		FindRect.top = static_cast<int>(v2Pos.y - 10);
		FindRect.bottom = static_cast<int>(v2Pos.y + 10);
	}

	if (TRUE == IntersectRect(&rc, &FindRect, &m_pTarget->Get_Rect()))
		return true;

	return false;
}

void CMonsterAI::Set_Go_Dir(const VECTOR2 & v2Dir)
{
	// ���� �Ҵ�
	if (v2Dir.x >= 0)
		m_pOwner->Set_Direction(DIR_FOR);
	else
		m_pOwner->Set_Direction(DIR_BACK);
}

void CMonsterAI::Set_Move_To_Dir(const VECTOR2 & v2Dir)
{
	Set_Go_Dir(v2Dir);

	if (DIR_END != m_pOwner->Get_Direction())
		m_eSaveDirection = m_pOwner->Get_Direction();
	m_pOwner->Change_State(CHAR_MOVE);
}

void CMonsterAI::Go_To_Rand()
{
	if (false == m_bNotFind)
	{
		int ran = rand() % 2;
		if (1 == ran)
			Set_Move_To_Dir(VECTOR2(1.f, 0.f));
		else if (0 == ran)
			Set_Move_To_Dir(VECTOR2(-1.f, 0.f));

		m_bNotFind = true;
	}
}
