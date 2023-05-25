#include "Boss.h"
#include "GameInstance.h"
#include "Battle_Manager.h"

#include "AnimationClip.h"
#include "BonesCollider.h"

CBoss::CBoss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CBoss::CBoss(const CBoss& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CBoss::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eCharType = CHAR_TYPE::TEONI;
	return S_OK;
}

HRESULT CBoss::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_fMoveSpeed = 30.f;
	m_fRotateSpeed = XMConvertToRadians(90.f) * 3.f;

	CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
	pBattleManager->Start_Battle(this, CBattle_Manager::CHAR_AI);
	RELEASE_INSTANCE(CBattle_Manager);

	return S_OK;
}

void CBoss::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pTargetObject)
	{
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
			m_pTargetObject = pBattleManager->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER);
			RELEASE_INSTANCE(CBattle_Manager);
		}
		else if (LEVEL_STORY == g_iCurrentLevel)
		{
			CGameInstance* pGaemInstance = GET_INSTANCE(CGameInstance);
			m_pTargetObject = static_cast<CCharacter*>(pGaemInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));
			RELEASE_INSTANCE(CGameInstance);
		}
	}
	if (nullptr == m_pTargetObject)
		return;


	// �ִϸ����� 
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Tick(_dTimeDelta);
	}

	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	// ���� �ֱ� 
	Move_HitCheck(_dTimeDelta, pGameInstace); // IDLE -> ��ȿ 
	Move_StepCheck(_dTimeDelta, pGameInstace); // IDEL -> ���ݻ���  or ���� 
	Give_State(_dTimeDelta, pGameInstace); // IDEL -> �ս��� or ���� / ���ݻ��� -> ���� 
	// Ʈ���� 
	Animation_Control_State(_dTimeDelta, pGameInstace);
	// �ִϸ��̼ǿ� ���� �̵�
	Behavior_State(_dTimeDelta, pGameInstace);
	// ���� �ʱ�ȭ 
	Init_State(_dTimeDelta, pGameInstace);

	RELEASE_INSTANCE(CGameInstance);

	__super::Tick(_dTimeDelta);
}

void CBoss::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pTargetObject)
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CBoss::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	//  ��Ʈ�ݶ��̴� 
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 25.f, 25.f, 25.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 15.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_HIT, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	// ���ݹ��� �ݶ��̴� 
	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale = _float3{ 5.f, 5.f, 5.f };
	ColliderDesc.vTranslation = _float3{ 0.f, 0.f, 0.f };
	if (FAILED(Add_Component(COM_COLLIDER_LIMIT, LEVEL_STATIC,
		PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pAttackColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CBoss::Coll_Tick(_matrix _WorldMatrix)
{
	__super::Coll_Tick(_WorldMatrix);
}

#ifdef _DEBUG
void CBoss::Coll_Render(void)
{
	__super::Coll_Render();
}
#endif

void CBoss::Check_AttackCollision(CCharacter* _pOther)
{
	// �ٿ�����ϋ� �ݶ��̴� üũ�� ���� �ʴ´�.
	if ("DOWN" == _pOther->Get_Animator()->Get_AnimationType())
	{
		if ("DASH" == m_strAnimType || "AIR_DASH" == m_strAnimType)
			m_pAnimator->Input_Trigger("SUCCESS_DASH");

		return;
	}

	CCollider* pOtherHitCollider = static_cast<CCollider*>(_pOther->Get_Component(COM_COLLIDER_HIT));
	if (Is_Nullptr(pOtherHitCollider))
		return;
	{

		CCollider* pRockCollider = (CCollider*)CGameInstance::Get_Instance()->Get_Component(g_iCurrentLevel, LAYER_PROJECTILE, COM_COLLIDER_SPHERE);
		if (nullptr != pRockCollider)
		{
			if (pRockCollider->Is_Collision(pOtherHitCollider))
			{
				_pOther->Hit_ByProjectile(10.f, 0.f, 5.f, 0.1f, this);
				return;
			}
		}


		// ���� ������ �ְ�, ������ ���� �� �ִ��� Ȯ��        
		if (true == m_pAnimator->Is_AttackColliderActive() && false == _pOther->Is_Hit())
		{
			// ���� ���� �� true
			if (m_pAnimAttackCollider->Is_Collision(pOtherHitCollider))
			{
				Success_AnimAttack(_pOther);
			}

		}
		// ���� �ݶ��̴��� ������ ��밡 �ٽ� ���� �� �ִ� ���·� ����
		else if (false == m_pAnimator->Is_AttackColliderActive())
		{
			_pOther->Set_Hit(false);
		}
	}
}

void CBoss::Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter)
{
	__super::Hit_ByProjectile(_fPushForce, _fHeight, _fDamage, _fDelayTime, _pEnemyCharcter);
}

void CBoss::Restrict_Nav(const _double& _dTimeDelta)
{
	// Root�� �ִϸ��̼��� Root���� �޾Ƽ� �̵��մϴ�. ( �׺���̼ǿ� ������ �̵����� �ʽ��ϴ�. )
	if (m_pAnimator->Is_Play())
	{
		if (true == m_pAnimator->Is_RootBone())
		{
			_matrix vRootBoneMatrix = m_pAnimator->Get_RootBoneMatrix();

			_vector vRootBonePos = CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS);

			// �̵��� �� �ִ°��̶�� ( �׺���̼� �޽� ���̶�� ) ���� ��ġ�� RootBonePos�� �����մϴ�.
			if (true == m_pNavigationCom->Is_Move(vRootBonePos))
				m_pTransformCom->Move_PosNotNavigation(vRootBonePos);
		}
		// �ִϸ��̼ǿ��� �־��� �̵����� �ִٸ� �̵��Ѵ�.
		if (true == m_pAnimator->Is_BoneTransform())
		{
			_vector BoneTransformPos = m_pAnimator->Get_BoneTransformPos();

			_bool bIsIn = false;

			// �̵��� �� �ִ°��̶�� ( �׺���̼� �޽� ���̶�� ) ���� ��ġ�� RootBonePos�� �����մϴ�.
			bIsIn = m_pNavigationCom->Is_Move(m_pTransformCom->Get_Pos() + BoneTransformPos);


			if (true == bIsIn)
			{
				// Y���� ���ԵǾ��ְų� AIR_ATTACK , AIR_STEP ���� ����� �ִϸ��̼��� �׺���̼� Y���� �¿��� �ʴ´�.
				if (0 != XMVectorGetY(BoneTransformPos) || "AIR_ATTACK" == m_strAnimType || "AIR_STEP" == m_strAnimType ||
					"AIR_DOWN" == m_strAnimType || "AIR_HIT" == m_strAnimType || "BOUND" == m_strAnimType || "JUMP" == m_strAnimType)
					m_pTransformCom->Move_PosNotNavigation(m_pTransformCom->Get_Pos() + BoneTransformPos);
				// �� ���� ��쿡�� �׺���̼� Y�� �¿��.
				else
					m_pTransformCom->Move_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
			}

			m_bNaviCheck = bIsIn;
		}
	}
}


void CBoss::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
	/*__super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

	m_tBattleData.Decrease_HP(5.f);
	if (true == m_bIsHit)
		m_iHitCount += 1;*/

	// �¾Ҵٴ°��� üũ���ش�.
	m_iHitCount += 1;

	m_bIsHit = true;

	// ���� ����� �ִϸ����͸� ���´�.
	CAnimator* pEnemyAnimator = _pEnemyCharcter->Get_Animator();
	CTransform* pEnemyTransform = _pEnemyCharcter->Get_Transform();

	// Collider�� Dir���� �����ִ� ������ �����´�. ( x = ������ , y = ���ߴ� �ð�, z = ���� �Ⱦ�( ���Ŀ� �߰� ���� ) )
	_vector vHitInformation = pEnemyAnimator->Get_ColliderDir();
	if(false == m_bGuard)
		m_tBattleData.Decrease_HP(XMVectorGetX(vHitInformation));
	else 
		m_tBattleData.Decrease_HP(XMVectorGetX(vHitInformation) / 3);

	// ��ǲ�� �����ߴ��� Ȯ���ϴ� �� ����
	_bool bIsInput = false;

	// �ݶ��̴��� Ÿ��
	_string strColliderType = "";
	_string strSuccessTrigger = "";

	if (nullptr != pEnemyAnimator->Get_CurrentAnimationClip())
		strColliderType = pEnemyAnimator->Get_CurrentAnimationClip()->Get_ColliderType();

	// ������ ��ü�� ��Ʈ ī��Ʈ�� �ø���.
	_pEnemyCharcter->Get_BattleData().iHitCount += 1;
	// �޺� Ÿ�̸Ӹ� 0���� �ʱ�ȭ�Ѵ�.
	_pEnemyCharcter->Get_BattleData().tComboTimer.Reset_Timer();

	// ��Ʈ ����Ʈ�� �����Ѵ�.
	Hit_Effect(strColliderType, _pEnemyCharcter);

	Check_Hit_Type(strColliderType, _pEnemyCharcter);

	// ���� Ÿ���� ���� üũ�Ѵ�.
	_pEnemyCharcter->Check_Attack_Type(strColliderType);

	// ��Ʋ�Ŵ����� ���� ĳ������ �ð��� �����.
	_pBattleManager->Set_DelayTime(0.1f);

}

void CBoss::Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	BONECOLLIDERS EnemyBoneCollider = _pEnemyCharcter->Get_BoneCollider();
	if (EnemyBoneCollider.size() < 2)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// ���� 1�� ���� �ݶ��̴��� ���� Hit�ݶ��̴��� �Ⱥε����� 2�� ���� �ݶ��̴� ���
	CBonesCollider* pAttackCollider = EnemyBoneCollider[1];
	CCollider* pMyHitCollider = (CCollider*)Get_Component(COM_COLLIDER_HIT);
	if (nullptr != pMyHitCollider)
	{
		if (false == pMyHitCollider->Is_Collision(pAttackCollider->Get_Collider()) && EnemyBoneCollider.size() >= 3)
		{
			pAttackCollider = EnemyBoneCollider[2];
		}
	}

	_float3 vAttackDir = pAttackCollider->Get_Direction();

	_float fSworldAngleXY = CMethod::Compute_Radian_XY_Plane(XMVectorSet(0.f, 1.f, 0.f, 0.f), vAttackDir);

	_matrix PlayerMatrix = m_pTransformCom->Get_WorldMatrix();
	PlayerMatrix.r[3] += XMVectorSet(0.f, 7.f, 0.f, 0.f);

	_matrix ScaleMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	_matrix RotationMatrix = XMMatrixRotationZ(fSworldAngleXY);

	_matrix WorldMatrix = ScaleMatrix * RotationMatrix * PlayerMatrix;

	if (("HIT_FRONT" == strColliderType) || ("HIT_FRONT_AIR" == strColliderType) || ("HIT_BACK" == strColliderType) ||
		("HIT_BACK_AIR" == strColliderType) || ("HIT_RIGHT" == strColliderType) || ("HIT_RIGHT_AIR" == strColliderType)
		|| ("HIT_LEFT" == strColliderType) || ("HIT_LEFT_AIR" == strColliderType))
	{
		_uint randomDmg = rand() % 3;
		if (0 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_1", WorldMatrix);
		else if (1 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_2", WorldMatrix);
		else if (2 == randomDmg)
			pGameInstance->SetEffectStateCall(L"SlashDmg1_3", WorldMatrix);

		if (true == m_bGuard && 2 < m_iHitCount)
		{
			pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
			m_iHitCount = 0;
		}
	}
	else if (("HIT_FRONT_S" == strColliderType) || ("HIT_BACK_S" == strColliderType) || ("HIT_S" == strColliderType)
		|| ("HIT_UPPER" == strColliderType) || ("STRIKE" == strColliderType) || ("HIT_DOWN" == strColliderType)
		|| ("BLOW" == strColliderType))
	{
		pGameInstance->SetEffectStateCall(L"SlashDmg2", WorldMatrix);
		pGameInstance->Clone_GameObject(g_iCurrentLevel, LAYER_ITEM, PROTO_GOB_ITEM_TEONITE);
	}

	RELEASE_INSTANCE(CGameInstance);
}

CGameObject* CBoss::Clone(void* pArg)
{
	return nullptr;
}

void CBoss::Free(void)
{
	__super::Free();
}
