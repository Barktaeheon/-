#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
END

BEGIN(Client)
class COni : public CCharacter
{
public:
	enum ONISTATE : _ulong
	{
		ONISTATE_IDLE = 1,
		ONISTATE_RECOGNIZE = 2,
		ONISTATE_BATTLE = 3,
		ONISTATE_DEAD = 4,

		ONISTATE_RECOGNIZE_APPEARANCE = 100,
		ONISTATE_RECOGNIZE_RUN = 200,
		ONISTATE_RECOGNIZE_STEP = 300,
		ONISTATE_RECOGNIZE_CHASE = 400,

		ONISTATE_BATTLE_NUT = 1000,
		ONISTATE_BATTLE_MOVELOOK_R = 1100,
		ONISTATE_BATTLE_MOVELOOK_L = 1200,
		ONISTATE_BATTLE_BURST = 1300,
		ONISTATE_BATTLE_LOOK = 1400,

		ONISTATE_BATTLE_RUN = 1500,
		ONISTATE_BATTLE_BACK = 1600,

		ONISTATE_BATTLE_STEP_R = 1700,
		ONISTATE_BATTLE_STEP_L = 1800,
		ONISTATE_BATTLE_STEP_B = 1900,

		ONISTATE_BATTLE_ATTACK1 = 2000,
		ONISTATE_BATTLE_ATTACK2 = 2100,
		ONISTATE_BATTLE_SKILL1 = 2200,
		ONISTATE_BATTLE_SKILL1_END = 2300,
		ONISTATE_BATTLE_SKILL2 = 2400,
		ONISTATE_BATTLE_SKILL2_END = 2500,
		ONISTATE_BATTLE_HIT = 2600,

		ONISTATE_BATTLE_STEP_F = 2700,
		ONISTATE_BATTLE_ATTACK3 = 2800,
		ONISTATE_BATTLE_TURN = 2900,

		ONISTATE_JUDGE = 100,
		ONISTATE_END = 0
	};
protected:
	explicit COni(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit COni(const COni& _rhs);
	virtual ~COni(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render(void) override;
protected:
	virtual HRESULT	SetUp_ConstantTable(void) override;
	virtual HRESULT	SetUp_Components(void) override;
protected:
	virtual void Coll_Tick(_matrix _WorldMatrix);
#ifdef _DEBUG
	virtual void Coll_Render(void);
#endif

protected:
	// �ǰ� �������� ó��
	virtual void Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
protected:
	// ���� �������ֱ� 
	virtual void Give_State(const _double& _dTimeDelta);

	virtual void Idle_Oni(const _double& _dTimeDelta) = 0;
	virtual void Recognize_Oni(const _double& _dTimeDelta) = 0;
	virtual void Battle_Oni(const _double& _dTimeDelta) = 0;
	// �����̱� 
	virtual void Animation_Control_State(const _double& _dTimeDelta);
	virtual void Idle_State(const _double& _dTimeDelta) = 0;
	virtual void Recognize_State(const _double& _dTimeDelta) = 0;
	virtual void Battle_State(const _double& _dTimeDelta) = 0;

	virtual void Behavior_State(const _double& _dTimeDelta);
protected:
	virtual void Restrict_Nav(const _double& _dTimeDelta) override;
protected:
	// ���͵鳢���� �浹ó�� 
	virtual void Collision_Oni(const _double& _dTimeDelta);
	// ������ ȣ���� ���� �Լ� 
	virtual HRESULT Delete_Self(void);
	// �÷��̾�� �浹ó��
	void Collision_Player(const _double& _dTimeDelta);
	//virtual _bool	Hit_SuperArmor(void);
public:
	// �� ������ ���濡�� �¾��� �� ���� ó�� 
	virtual void Check_AttackCollision(CCharacter* _pOther);
	// ���ݹް� hp������ �ܺο��� ȣ���� �Լ� 
	void Minus_OniHp(void) { m_tBattleData.fCurHp--;/*m_iHp--;*/ }
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free(void) override;
protected:
	// ��Ʋ�Ŵ���, �÷��̾ �������� ���� �ݶ��̴� 
	CCollider* m_pHitColliderCom = nullptr;
	// ���ϰ� ����ϴ� �÷��̾� 
	class CPlayer* m_pTargetObject = nullptr;
	// �÷��̾� �ν� ó�� ���������� �Ǵ��ϴ� �Һ��� 
	_bool	m_bFirstCheck = false;
	// ���� ���°�
	_uint	m_iOniState = 0;
	// �ð���꿡 ���̴� ����
	_double m_dBattleTime = 0.0;
	// ���� �帧 ������ ������ ī��Ʈ 
	_uint m_iPatternCount = 0;
	// ���� hp 
	//_int m_iHp = 0;
	// �ڽĿ��Ͽ��� �ٸ� ���̾� ���ϵ��� �׾����� �Ǵ��ϰ� ƽ, ���� ���������� �Һ��� 
	_bool m_bTickCheck = false;
	// ��ų�� ���������� üũ�ϴ� �Һ��� 
	_bool m_bSkillEndCheck = false;
	// �������� ��ġ��
	_float3 m_vOriginalPos = {};
	// �׺� ����� �� üũ�� �Һ��� 
	_bool m_bNaviCheck = true;
	// ������ �����ߴ��� üũ�ϴ� �Һ���
	_bool m_bIsAttackSuccess = false;
	_uint m_iOniLayerNum = 1;
	// �ܰ��� ���� 
	_float m_fEdge = 0.f;
	// ��ų üũ ����
	_bool m_bMSkillCheck = false;
	// ����� Delete Ÿ�̸�
	CUSTIMER m_tDeleteTimer = CUSTIMER(0.5f);
};
END