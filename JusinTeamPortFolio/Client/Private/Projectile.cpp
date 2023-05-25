#include "Projectile.h"
#include "GameInstance.h"

CProjectile::CProjectile(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	:CPawn(_pDevice, _pContext)
{
}

CProjectile::CProjectile(const CProjectile& _rhs)
	: CPawn(_rhs)
{
}

HRESULT CProjectile::NativeConstruct_Prototype(void)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::NativeConstruct_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	// ����ü ����ü�� ä���.
	PROJECTILEDESC* pProjectileDesc = static_cast<PROJECTILEDESC*>(pArg);

	// ���ΰ�ü�� ä���.
	if (nullptr != pProjectileDesc->pOwner)
	{
		m_tProjectileDesc.pOwner = pProjectileDesc->pOwner;
		Safe_AddRef(m_tProjectileDesc.pOwner);
	}

	// Ÿ�ٰ�ü�� ä���.
	if (nullptr != pProjectileDesc->pTarget)
	{
		m_tProjectileDesc.pTarget = pProjectileDesc->pTarget;
		Safe_AddRef(m_tProjectileDesc.pTarget);
	}

	// �׺���̼� ������Ʈ�� ä���.
	if (nullptr != pProjectileDesc->pNavigation)
	{
		m_tProjectileDesc.pNavigation = pProjectileDesc->pNavigation;
		Safe_AddRef(m_tProjectileDesc.pNavigation);
	}

	// Ÿ���� �ݶ��̴��� ä���.
	if (nullptr != pProjectileDesc->pTargetCollider)
	{
		m_tProjectileDesc.pTargetCollider = pProjectileDesc->pTargetCollider;
		Safe_AddRef(m_tProjectileDesc.pTargetCollider);
	}

	m_tProjectileDesc.fDamage = pProjectileDesc->fDamage;
	m_tProjectileDesc.fLifeTime = pProjectileDesc->fLifeTime;
	m_tProjectileDesc.fSpeed = pProjectileDesc->fSpeed;
	m_tProjectileDesc.vCreatePos = pProjectileDesc->vCreatePos;
	m_tProjectileDesc.vMoveDirection = pProjectileDesc->vMoveDirection;

	// ��ġ�� �ٶ� ������ �����Ѵ�.
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tProjectileDesc.vCreatePos));	
		m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMLoadFloat3(&m_tProjectileDesc.vMoveDirection));
	}		

	return S_OK;
}

void CProjectile::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

	// �̵��Ѵ�
	Move(_dTimeDelta);

	if(nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CProjectile::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	Check_Collision();

	// �ð��� �ٵǸ� �����Ѵ�.
	Running_Timer(_dTimeDelta);
}

HRESULT CProjectile::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_ConstantTable(void)
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_Components(void)
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::Delete_Self(void)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// �� �ڽ��� ����
	pGameInstance->Add_DeleteObject(g_iCurrentLevel, LAYER_PROJECTILE, this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CProjectile::Move(const _double& _dTimeDelta)
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Move_Forward(_dTimeDelta, m_tProjectileDesc.fSpeed);
}

void CProjectile::Running_Timer(const _double& _dTimeDelta)
{	
	// ������ Ÿ���� �������� ���� ��ü ( ��� ����ְ��Ѵ� ������ �Ǹ� ���� )
	if (m_tProjectileDesc.fLifeTime == 0.f)
		return;

	// ������ Ÿ���� �ٵǸ� �����Ѵ�.
	if (m_tProjectileDesc.fLifeTime <= m_dTimeAcc)
		Delete_Self();
	else
		m_dTimeAcc += _dTimeDelta;
}

void CProjectile::Check_Collision()
{
}

CProjectile* CProjectile::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CProjectile* pInstance = new CProjectile(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CProjectile::Clone(void* pArg)
{
	CProjectile* pInstance = new CProjectile(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTeOni_Rock::Clone To Failed");
#endif
	}
	return pInstance;
}

void CProjectile::Free(void)
{
	Safe_Release(m_tProjectileDesc.pOwner);
	Safe_Release(m_tProjectileDesc.pTarget);
	Safe_Release(m_tProjectileDesc.pNavigation);
	Safe_Release(m_tProjectileDesc.pTargetCollider);
	Safe_Release(m_pColliderCom);
	//Safe_Release(m_pMainEffect);

	__super::Free();
}
