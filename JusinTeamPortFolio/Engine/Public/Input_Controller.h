#pragma once
#include "Controller.h"

BEGIN(Engine)


class ENGINE_DLL CInput_Controller : public CController
{
public:
	// ���� ��ǲ�� ����
	enum INPUTTYPE { DOWN, UP, PRESSING, INPUTTYPE_END };
protected:
	explicit CInput_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CInput_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
		// �����͸� ���ο� ���� �ִ� �Լ�
		virtual void Insert_Datas(VOIDDATAS _pData) override;
		// �����͸� �ٱ����� ���� �Լ�
		virtual  VOIDDATAS Out_Datas() override;
protected:
	virtual void Input_Defines();
protected:
	// Input Con Type 
	EInputCon_Type m_eInputConType = INPUTCON_TYPE::TYPE_END;
public:
	virtual void Free() override;
};

END
