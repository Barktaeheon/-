#pragma once
#include "Client_Defines.h"
#include "Player_KeyBoard_Controller.h"

BEGIN(Client)

class CTanjiro_KeyBoard_Controller final : public CPlayer_KeyBoard_Controller
{
private:
	explicit CTanjiro_KeyBoard_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTanjiro_KeyBoard_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
	// �����͸� ���ο� ���� �ִ� �Լ�
	virtual void Insert_Datas(VOIDDATAS _pData);
	// �����͸� �ٱ����� ���� �Լ�
	virtual  VOIDDATAS Out_Datas();

public:
	static CTanjiro_KeyBoard_Controller* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, class CGameObject* _pGameObject);
	virtual void Free() override;
};

END