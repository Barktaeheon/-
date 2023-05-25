#pragma once
#include "Tool_Defines.h"
#include "ImGuiClass_Tool.h"



BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CComponent;
class CVIBuffer;
class CTexture;
class CLayer;
class CUI_Base;
END

BEGIN(Tool)
class CImGui_UI_Tool : public CImGuiClass_Tool
{
	struct TextInfo
	{
		char Text_Input[1000] = "";
		char Text_Tag[100] = "";
		_float TextX = 0.f;
		_float TextY = 0.f;
		_float TextR = 255.f;
		_float TextG = 255.f;
		_float TextB = 255.f;
	};
	
protected:
	explicit CImGui_UI_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CImGui_UI_Tool() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT NativeConstruct_InnerValue();
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();

	//picking�� ���õ� UI�� ��ġ�� �����Ѵ�
	void Picking();
	
	//UI������Ʈ ����� �����ְ� �����Ҽ� �ְ� �Ѵ�
	void ShowPrototypeObject();
	
	//���õ� UI�� ��ġ�� ����� �巡�׷� �����Ҽ��ִ�
	void Convert_Transform();
	
	//picking�ؼ� �浹�� UIObject�� ã�Ƽ� ���õ� UI�� �����Ѵ� 
	void Picking_Find_Object();
	
	//������ UI_Object�� Clone(Create)�Ѵ� +)create�Ҷ� �̸��� �ٿ��ִ� ���
	void Create_UI_Object();
	
	//���õ� UI_Object�� NameTag�� �ٲ۴� 
	void Change_UI_NameTag();

	//Texture���ϵ��� �����ش�
	void Show_Texture();

	//���õ� Texture�� �ٲ��ش�
	void Change_Texture();

	void Save();

	void Load();

	//���õ� UI_Object���� ȿ���� �ش� 
	void Effect_UI(const _double& _dTimeDelta);

	//���İ��� �����ϰų� ������ ������ų� £������ �Ѵ� 
	void Alpha_Control(const _double& _dTimeDelta);

	void Dialogue_UI();

	

private:

	
	const _tchar* prototypeObjectTag = nullptr;
	CGameObject* prototypeObject = nullptr;

	class CUI_Base* selectedObject = nullptr;
	
   map<const _tchar*, class CGameObject*> gameObject;
   FILEDATA* selectedTexture = nullptr;


private:
	_bool bPicking = false;
	_bool bFind = false;
	
	_bool bGrow = false;
	_bool bGrowDecrease = false;
	_bool bRight = false;
	_bool bLeft = false;
	_bool bDown = false;
	_bool bPlusAlpha = false;
	_bool bMinusAlpha = false;
	_bool bRepeatAlpha = false;
	_bool bNoneAlpha = false;
	
	_float speed = 0.f;
	_float maxsize = 0.f;
	_float minsize = 0.f;
	_float creattime = 0.f;
	_float deadtime = 0.f;
	_float movex = 0.f;
	_float movey = 0.f;

	_float alphaCreat = 0.f;
	_float alphaDead = 0.f;

	TextInfo text;
	
	_float2 v2MousePos = _float2(0.f, 0.f);
	_tchar strName[128]={0}; 
	
	char name_Input[100] = "";
	char prototype_Input[100] = "";
	char tag_Change_NameInput[100] = "";
	_char str[256] = {};
	_tchar m_szFONTS[MAX_PATH] = TEXT("");

	_string selectedtexname = "";
	
	vector<FILEDATA> SelectTexture;

	//vector<TextInfo> Textinfo;
	
protected:
	//ID3D11Device* m_pDevice = nullptr;
	//ID3D11DeviceContext* m_pContext = nullptr;
	WINDESC m_Windesc;
	ID3D11Resource* pResoure = nullptr;
	ID3D11ShaderResourceView* pView = nullptr;
	
public:
	static CImGui_UI_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
	virtual void Free() override;
};

END