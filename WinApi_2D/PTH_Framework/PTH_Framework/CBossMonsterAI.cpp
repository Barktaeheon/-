#include "pch.h"
#include "CBossMonsterAI.h"
#include "CBoss.h"
#include "CGameMgr.h"

CBossMonsterAI::CBossMonsterAI()
{
}

CBossMonsterAI::CBossMonsterAI(CMonster * pOwner, CPlayer * pTarget) : CMonsterAI(pOwner, pTarget)
{
	m_tHurtTimer.iPassTime = 1000;
}


CBossMonsterAI::~CBossMonsterAI()
{
}

int CBossMonsterAI::Update()
{
	Control_Monster();
	return 0;
}

void CBossMonsterAI::Late_Update()
{
}

void CBossMonsterAI::Relase()
{
}

void CBossMonsterAI::Control_Monster()
{
	CBoss* pBoss = static_cast<CBoss*>(m_pOwner);
	if (nullptr != m_pTarget && nullptr != pBoss)
	{
		VECTOR2 v2SelfPos = pBoss->Get_CurPos();
		VECTOR2 v2TargetPos = m_pTarget->Get_CurPos();
		// ���� ���Ϳ� �÷��̾�� �Ÿ��� ���ϱ� ���� ��
		VECTOR2 dir = v2TargetPos - v2SelfPos;
		m_fDistance = fabs(dir.Get_Length_Sqrt());
		VECTOR2 v2RealDir = pBoss->Get_Dir();

		if (true == m_pOwner->Is_Dead())
		{
			if (true == m_tDeadTimer.Is_PassTime())
			{
				m_bDeadTimerOk = true;
			}
			return;
		}
		m_bDeadTimerOk = false;
		m_tDeadTimer.dwTime = GetTickCount();
		bool IsCatch = Is_Catch_EqualYPos(v2SelfPos.y, v2TargetPos.y, 50, 100);
		EDIR eTargetDir = m_pTarget->Get_Direction();
		ECHARSTATE eCharState = pBoss->Get_CharState();
		EBOSSSTATE eBossState = static_cast<CBoss*>(pBoss)->Get_BossState();

		if (true == pBoss->Is_CollDoor() && m_eSaveDirection == pBoss->Get_Direction())
		{
			m_bFindTarget = false;
			m_eSaveDirection = pBoss->Get_Direction();
			if (DIR_FOR == pBoss->Get_Direction())
				pBoss->Set_Direction(DIR_BACK);
			else if(DIR_BACK == pBoss->Get_Direction())
				pBoss->Set_Direction(DIR_FOR);

			pBoss->Change_State(CHAR_MOVE);
			pBoss->Change_BossState(BOSS_END);
			return;
		}

		if (CHAR_HIT == eCharState)
		{
			if (true == m_tHurtTimer.Is_PassTime())
			{
				m_tHurtTimer.dwTime = GetTickCount();
				pBoss->Change_BossState(BOSS_END);
				m_pOwner->Change_State(CHAR_MOVE);
				m_bFindTarget = true;
			}
		}
		else
		{
			IsCatch = Is_Catch_EqualYPos(v2SelfPos.y, v2TargetPos.y, 50, 5);
			// ���� �ð� ���������� �ʱ�ȭ
			m_tHurtTimer.dwTime = GetTickCount();
			if (BOSS_END != eBossState)
				return;

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

			// Ÿ���� ã�� ������ ���
			if (false == m_bFindTarget)
			{
				if (m_fDistance <= m_iAttackRange)
					m_bFindTarget = true;
				// ���⿡ ���� ó��
				if (CHAR_IDLE == eCharState)
				{
					if (m_tMoveTimer.Is_PassTime())
					{
						Set_Move_To_Dir(dir);
						m_tMoveTimer.dwTime = GetTickCount();
						m_pOwner->Set_CharState(CHAR_MOVE);
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
					else
					{
						m_bFindTarget = true;
						Set_Move_To_Dir(dir);
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
						if (true == Is_Check_In_Target_Forward())
						{
							if (compareDir.x >= 0)
								m_pOwner->Set_Direction(DIR_FOR);
							else if (compareDir.x < 0)
								m_pOwner->Set_Direction(DIR_BACK);

							m_eSaveDirection = m_pOwner->Get_Direction();
						}
						m_pOwner->Change_State(CHAR_ATTACK);
					}
					else
					{
						if (true == Is_Catch_EqualYPos(v2SelfPos.y, v2TargetPos.y, 300, 50))
						{
							m_pOwner->Change_State(CHAR_IDLE);
							m_eSaveDirection = m_pOwner->Get_Direction();
							m_pOwner->Set_Direction(DIR_END);
						}
						else
						{
							Set_Move_To_Dir(dir);
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
				if (false == Is_Check_In_Target_Forward())
				{
					Set_Move_To_Dir(dir);
					m_bFindTarget = false;
				}

			}
		}
	}
}
