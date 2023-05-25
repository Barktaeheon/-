#pragma once
#include "Input_Controller.h"

BEGIN(Engine)


class ENGINE_DLL CKeyBoard_InputController : public CInput_Controller
{
public:
	typedef struct tagInputDesc
	{
		// 인풋 종류 ( Down, UP, Pressing )
		INPUTTYPE	inputType = INPUTTYPE_END;
		// 들어온 키
		_ubyte		inputKey;

		// 정보를 집어넣는다.
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
	// 데이터를 내부에 집어 넣는 함수
	virtual void Insert_Datas(VOIDDATAS _pData) override;
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas() override;
protected:
	virtual void Input_Defines() override;
	// 인풋이 들어왔는지 체크한다.
	void Check_Input(INPUTDESC inputDesc);

protected:
	// 키 Pressing 체크
	_bool m_PressingKey[256] = { false };
	// 키 Down 체크
	_bool m_DownKey[256] = { false };
	// 키 Up 체크
	_bool m_UpKey[256] = { false };

	// 들어온 인풋의 집합
	vector<INPUTDESC> m_InputList;
public:
	virtual void Free() override;
};

END