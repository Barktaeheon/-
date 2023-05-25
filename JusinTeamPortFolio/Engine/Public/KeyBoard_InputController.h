#pragma once
#include "Input_Controller.h"

BEGIN(Engine)


class ENGINE_DLL CKeyBoard_InputController : public CInput_Controller
{
public:
	typedef struct tagInputDesc
	{
		// ��ǲ ���� ( Down, UP, Pressing )
		INPUTTYPE	inputType = INPUTTYPE_END;
		// ���� Ű
		_ubyte		inputKey;

		// ������ ����ִ´�.
		void Push_Info(INPUTTYPE inputType, const _ubyte& inputKey);
		_bool Compare_Info(tagInputDesc* input);
	}INPUTDESC;
protected:
	explicit CKeyBoard_InputController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CKeyBoard_InputController() = default;

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
	// Ű Pressing üũ
	_bool m_PressingKey[256] = { false };
	// Ű Down üũ
	_bool m_DownKey[256] = { false };
	// Ű Up üũ
	_bool m_UpKey[256] = { false };

	// ���� ��ǲ�� ����
	vector<INPUTDESC> m_InputList;
public:
	virtual void Free() override;
};

END