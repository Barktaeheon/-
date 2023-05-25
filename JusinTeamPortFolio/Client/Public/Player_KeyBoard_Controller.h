#pragma once
#include "KeyBoard_InputController.h"
#include "Client_Defines.h"

BEGIN(Client)
class CPlayer;

class CPlayer_KeyBoard_Controller : public CKeyBoard_InputController
{
public:
	typedef struct tagCondition
	{
		// Ű �Է��� �޾Ƽ� Ű�� ������ִ� input ���
		vector<INPUTDESC> input;				
	}CONDITION;

protected:
	explicit CPlayer_KeyBoard_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayer_KeyBoard_Controller() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;

public:
	// �����͸� ���ο� ���� �ִ� �Լ�
	virtual	 void Insert_Datas(VOIDDATAS _pData) override;
	// �����͸� �ٱ����� ���� �Լ�
	virtual  VOIDDATAS Out_Datas() override;
	
protected:
	// ���� Ű�Է��� 2�������� ( �켱�� )
	_bool Make_Trigger(_string _strTrigger, _bool* pFirstInput, _bool* pSecondInput = nullptr);
	// Ʈ���Ÿ� �߰��Ѵ�.
	void Add_Trigger(_string strTrigger);

protected:
	// ī�޶� �������� ������� ���⺤�� ( �÷��̾����� ������. �ش� ���⺤�Ͱ� 0���� �ƴ����� �پ��� �Ǵ� ���� )
	_float3				m_vPlayerDir;
	// Ʈ���� ����Ʈ ( InputController���� �Էµ� Ű���� ���� �������. )
	vector<_string>		m_TriggerList;
	// ���� ����Ʈ
	vector<CONDITION>	m_ConditionList;
	// �÷��̾� ��ü
	CPlayer*			m_pPlayer = nullptr;
public:
	virtual void Free() override;
};

END