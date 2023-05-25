#pragma once
#include "Projectile.h"

BEGIN(Client)

// ����ü ���� Ŭ����
class CWindBall : public CProjectile
{
protected:
	explicit CWindBall(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CWindBall(const CWindBall& _rhs);
	virtual ~CWindBall() = default;

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
	virtual HRESULT Delete_Self(void) override;
	// ���� ��ü�� �����̴� �Լ�
	virtual void Move(const _double& _dTimeDelta) override;
	// Ÿ�̸Ӹ� �����Ѵ�.
	virtual void Running_Timer(const _double& _dTimeDelta) override;
	// �浹 �ƴ��� üũ�Ѵ�.
	virtual void Check_Collision() override;

public:
	static CWindBall* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END