#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CModel;
class CTransform;
class CNavigation;
class CEffect;
class CCollider;
END

BEGIN(Client)

// ����ü ���� Ŭ����
class CProjectile : public CPawn
{
public:
	// ����ü ���� ����ü
	typedef struct tagProjectileDesc
	{
		// ���� ��ü
		CGameObject*	pOwner = nullptr;
		// Ÿ�� ��ü
		CGameObject*	pTarget = nullptr;
		// �׺���̼� ��ü
		CNavigation*	pNavigation = nullptr;
		// Ÿ���� �ݶ��̴��� �浹�ߴ��� �Ǵ��ϱ� ���� ������ �ݶ��̴��� �޾ƿ´�.
		CCollider*		pTargetCollider = nullptr;
		// ��ü�� ������� �� �ð�
		_float			fLifeTime = 0.f;
		// ������
		_float			fDamage = 0.f;
		// ���ư� �ӵ�
		_float			fSpeed = 0.f;
		// ������ ���� �� ( ��ֶ����� �� �� )
		_float3			vMoveDirection = _float3(0.f, 0.f, 0.f);
		// ó�� ������ ��ġ
		_float3			vCreatePos = _float3(0.f, 0.f, 0.f);
	}PROJECTILEDESC;

protected:
	explicit CProjectile(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CProjectile(const CProjectile& _rhs);
	virtual ~CProjectile() = default;

public:
	// ����ü�� ���� �ݶ��̴� ��ü�� �޾ƿ´�.
	CCollider* Get_Collider() { return m_pColliderCom; }

public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual void	Tick(const _double& _dTimeDelta) override;
	virtual void	Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable(void) override;
	// SetUp Components
	virtual HRESULT SetUp_Components(void) override;

protected:	
	// ���� ��ü�� �����ϴ� �Լ�
	virtual HRESULT Delete_Self(void);
	// ���� ��ü�� �����̴� �Լ�
	virtual void Move(const _double& _dTimeDelta);
	// Ÿ�̸Ӹ� �����Ѵ�.
	virtual void Running_Timer(const _double& _dTimeDelta);
	// �浹 �ƴ��� üũ�Ѵ�.
	virtual void Check_Collision();

protected:
	// �ݶ��̴� ��ü
	CCollider*			m_pColliderCom = nullptr;
	// ����ü�� �پ �ٴ� ����Ʈ ��ü
	CGameObject*		m_pMainEffect = nullptr;
	// ����ü ����
	PROJECTILEDESC		m_tProjectileDesc;
	// ��ü�� ������Ÿ�� Ÿ�̸�
	_double				m_dTimeAcc = 0.0;	

public:
	static CProjectile*		Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void Free(void) override;
};
END