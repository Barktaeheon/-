
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

class CMiniGame_Note final : public CUI_Base
{
public:
	enum NoteType { KEY_Y, KEY_B, KEY_A, KEY_X, EFFECT_Y, EFFECT_B, EFFECT_A, EFFECT_X,
		EFFECT_GREAT, EFFECT_GOOD, EFFECT_RING, EFFECT_STAR, KEY_END}; //�ʱ� ��ġ�� ����
	//EFFECT_GREAT : ū�� 
	//EFFECT_GOOD : ������ 
	//EFFECT_STAR : ū��
	//EFFECT_Ring : ������

protected:
	explicit CMiniGame_Note(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMiniGame_Note(const CMiniGame_Note& _rhs);
	virtual ~CMiniGame_Note() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;


public:
	void Setting_Positon(_uint Type);//KeyType�� ���� �ʱ� ���� ���� 
	void Tick_Event(const _double& _dTimeDelta); //��� �ݺ��������ؾ��ϴ� ���ǵ�
	void Bezier(const _double& _dTimeDelta); //������ � 
	void Great_Eff(const _double& _dTimeDelta);//great�̰ų� good�϶� ������ ȿ�� 
	
private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_uint m_iType = KEY_END;
	_float2 Note[3]; //������,������,������ 

	_float m_t = 0.f;
	_double m_time = 0.0;
	_double deletetime = 0.0;

	_int type = KEY_END;
	_float3 m_RGBColor = { 0.f,0.f,0.f };
	
	_bool GreatEff = false;
	_int Efftype = KEY_END;
	_double EffTime = 0.0;
	_bool CreatEff = false;
	_double timeeff = 0.0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMiniGame_Note* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
