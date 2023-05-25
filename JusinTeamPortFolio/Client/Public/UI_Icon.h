
#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Icon final : public CUI_Base
{
public:
	enum IconType { QUEST_CANDIDATE, QUEST_TALK, QUEST_CANDIDATE2, QUEST_TALK2, TARGETING_ICON, TARGETING_BASE, TARGETTING_RING, QUEST_END }; //�ʱ� ��ġ�� ����
	enum RattanType { QUEST_AOI, AOI_TALK, QUEST_MAIN, SISTER_TALK, ZENITSU_ICON, ZENITSU_TALK, KANAWO_ICON, KANAWO_TALK,
		CANDIDATE_ICON, CANDIDATE_TALK, GENYA_ICON, GENYA_TALK, RATTAN_END }; //�ʱ� ��ġ�� ����

	enum IconTexture { TALK, QUEST, QUSEST_CLEAR, MAIN, ARROW_TALK, TARGETING_ICONTEX, TARGETING_BASETEX, TARGETTING_RINGTEX, TEXTURE_END }; //�ؽ��ĸ� ����

protected:
	explicit CUI_Icon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_Icon(const CUI_Icon& _rhs);
	virtual ~CUI_Icon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;


public:
	void Change_Texture(_uint Type);//IconState�� ���� �ؽ��� ���� 
	void Setting_Positon(_uint Type);//IconType�� ���� �ʱ� ��ġ�� ���� 
	void Tick_Event(_uint Type, const _double& _dTimeDelta); //��� �ݺ������� �˻��ؾ��ϴ� ���ǵ�
	_float3 Oni_Targeting(const _double& _dTimeDelta); //�÷��̾�� ���� ����� ������ ��ġ�� ��ȯ

public:
	void Set_TalkRender(_bool render) { TalkQuest = render; };
	void Set_TextureType(_uint type) { CurrentTexture = type; };
	_uint Get_Type() { return m_iType; }

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_int  TextureIndex = 0;
	_uint m_iType = QUEST_END;
	_uint PrevTexture = TEXTURE_END;
	_uint CurrentTexture = TEXTURE_END;
	_float3 fCandidatePos = { 0.f,0.f,0.f };
	_bool  TalkQuest = true;
	_uint m_iCurOniLayer = 0;
	_double TickFrame = 0;
	CTransform* pMinOniTransform = nullptr;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Icon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
