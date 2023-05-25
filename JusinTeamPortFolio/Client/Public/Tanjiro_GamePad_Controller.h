
#pragma once
#include "Player_GamePad_Controller.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CAnimationClip;
class CGameObject;
END

BEGIN(Client)
class CCamera_Free;
class CPlayer;
class CTanjiro;

class CTanjiro_GamePad_Controller final : public CPlayer_GamePad_Controller
{
protected:
	explicit CTanjiro_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTanjiro_GamePad_Controller() = default;

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
	CPlayer* m_pTargetPlayer = nullptr;
	CCamera_Free* m_pCamera = nullptr;
	// Current Animation Clip
	CAnimationClip* m_pCurAnimationClip = nullptr;
	// Current Animation Clip
	CAnimationClip* m_pPrevAnimationClip = nullptr;
	// Cur Next Prev State Data 
	_uint							m_iCurStateData = 0;
	_uint							m_iNextStateData = 0;
	// Direction
	_uint							m_iDir = 0;
	// Cus Timer
	CUSTIMER						m_tChargeStateTimer = CUSTIMER(3.f);
	// Change direction
	_bool							m_bChangeDirection_StepLR = false;
public:
	static CTanjiro_GamePad_Controller* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject);
	virtual void Free() override;
};

END