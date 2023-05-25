#pragma once
#include "Input_Controller.h"

BEGIN(Engine)

class ENGINE_DLL CGamePad_InputController : public CInput_Controller
{
public:
	typedef struct tagInputDesc
	{
		// ��ǲ ���� ( Down, UP, Pressing )
		INPUTTYPE	inputType = INPUTTYPE_END;
		// ���� Ű
		_uint				inputKey;

		// ������ ����ִ´�.
		void Push_Info(INPUTTYPE inputType, const _ubyte& inputKey);
		_bool Compare_Info(tagInputDesc* input);
	}INPUTDESC;
protected:
	explicit CGamePad_InputController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGamePad_InputController() = default;

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
	virtual void Input_Defines() override;
	// ��ǲ�� ���Դ��� üũ�Ѵ�.
	void Check_Input(INPUTDESC inputDesc);
protected:

	_bool m_GamePadUp[GPK_END] = { false };
	_bool m_GamePadDown[GPK_END] = { false };
	_bool m_GamePadPressing[GPK_END] = { false };

	// ���� ��ǲ�� ����
	vector<INPUTDESC> m_InputList;
public:
	virtual void Free() override;
};

END