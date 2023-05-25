#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
class CGameInstance;
END

BEGIN(Client)
class CBoss : public CCharacter
{
protected:
	explicit CBoss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CBoss(const CBoss& _rhs);
	virtual ~CBoss(void) = default;
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

public:
	// �� ������ ���濡�� �¾��� �� ���� ó�� 
	virtual void Check_AttackCollision(CCharacter* _pOther);
	virtual void Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter) override;

protected:
	// �����ֱ� / ��ȿ / ���� / ����
	virtual void Move_HitCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	virtual void Move_StepCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	virtual void Give_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	// �ִϸ��̼� ó�� 
	virtual void Animation_Control_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) = 0;
	// ������ ó�� 
	virtual void Behavior_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) = 0;
	// ���� �ʱ�ȭ 
	virtual void Init_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) = 0;
protected:
	virtual void Restrict_Nav(const _double& _dTimeDelta) override;
	// �¾����� ó��
	virtual void Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager) override;
	// �ǰ� �������� ����Ʈ ó��
	virtual void Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter) override;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free(void) override;
protected:
	// �÷��̾� 
	CCharacter* m_pTargetObject = nullptr; 
	// ���� �����Ҷ� �׺� ��Ҵ��� �ʿ��� �Һ��� 
	_bool m_bNaviCheck = true; 
	// ��Ʈī��Ʈ 
	_uint m_iHitCount = 0;
	//// hp
	//_int m_iHP = 0;
	// ���� ���� 
	_uint m_iPreState = 0;
	_uint m_iPreAtkState = 0;
	_uint m_iPreIdleState = 0;
	// ������ �Һ��� 
	_bool m_bPhase2 = false;
	// �� ī��Ʈ 
	_uint m_iTurnCount = 0;
	_uint m_iWalkCount = 0;
	// �ܰ��� ���� 
	_float m_fEdge = 0.f;
	// ������� ���� 
	_bool m_bGuard = false;
};
END