#pragma once
#include "CMyAI.h"
#include "CPlayer.h"
#include "CMonster.h"
#include "CLine.h"

class CMonsterAI : public CMyAI
{
public:
	explicit CMonsterAI();
	explicit CMonsterAI(CMonster * pOwner, CPlayer* pTarget);
	virtual ~CMonsterAI();
protected:
	// Ÿ�ٵ�
	CMonster*			m_pOwner;
	CPlayer*			m_pTarget;
	CLine*				m_pLeftLine;
	CLine*				m_pRightLine;
	// ã�� ����
	int					m_iFindRange;
	int					m_iAttackRange;
	int					m_iStopRange;
	int					m_iBackAttackRange;
	// �÷��̾���� �Ÿ��� ���� ����
	float				m_fDistance;
	// ã�Ҵ��� Ȯ���ϴ� Bool ����
	bool				m_bFindTarget;
	bool				m_bAttack;
	// Ÿ�̸�
	TIMER				m_tIdleTimer;
	TIMER				m_tMoveTimer;
	TIMER				m_tHurtTimer;
	TIMER				m_tCatchTimer;
	TIMER				m_tDeadTimer;

	bool				m_bDeadTimerOk;
	bool				m_bNotFind;
	// ������ �͵�
	EDIR				m_eSaveDirection;
	bool				m_bIdleTime;

	
public:
	// CAI��(��) ���� ��ӵ�
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Relase() override;

	virtual bool Collision_Situation();

	// Catch EqualYPos
	bool Is_Catch_EqualYPos(const float & fSelfYPos, const float & fTargetYPos, const int& iValue1, const int& iValue2);
protected:
	virtual void Control_Monster();

	bool Is_Check_In_Target_Forward();

	void Set_Go_Dir(const VECTOR2& v2Dir);
	void Set_Move_To_Dir(const VECTOR2& v2Dir);

	void Go_To_Rand();
public:
	const float& Get_Distance() { return m_fDistance; }
	const int& Get_Find_Range() const { return m_iFindRange; }
	const int& Get_Attack_Range() const { return m_iAttackRange; }
	const int& Get_BackAtk_Range() const { return m_iBackAttackRange; }
	const bool& Is_FindTarget() const { return m_bFindTarget; }
	const bool& Is_Attack() const { return m_bAttack; }
	const bool& Is_DeadTimerOk() const { return m_bDeadTimerOk; }
	const bool& Is_IdleTime() const { return m_bIdleTime; }
	const EDIR& Get_SaveDirection() const { return m_eSaveDirection; }

	void Set_Distance(const float& fDistance) { this->m_fDistance = fDistance; }
	void Set_Find_Range(const int& iFindRange) { this->m_iFindRange = iFindRange; }
	void Set_Attack_Range(const int& iAttackRange) { this->m_iAttackRange = iAttackRange; }
	void Set_FindAtk_Range(const int& iAttackRange) { this->m_iBackAttackRange = iAttackRange;  }
	void Set_FindTarget(const bool& IsFindTarget) { this->m_bFindTarget = IsFindTarget; }
	void Set_Attack(const bool& IsAttack) { this->m_bAttack = IsAttack; }
	void Set_SaveDirection(const EDIR& eDirection) { this->m_eSaveDirection = eDirection; }
};

