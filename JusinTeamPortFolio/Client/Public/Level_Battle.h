
#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "UI_CharSelect.h"

BEGIN(Client)

class CLevel_Battle final : public CLevel
{
private:
	CLevel_Battle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Battle() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Light();
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Lines(const _tchar* pLayerTag);
	// �� �ҷ������ �Լ� / ���� �ҷ��ͼ� �� ������Ʈ�� ���� 
	HRESULT Load_Map();
	HRESULT Ready_UrokodakiMap();
	HRESULT Ready_TamayoMap();
	HRESULT Ready_HujikasaneMap();
	HRESULT Ready_Asakusa();

public:
	static CLevel_Battle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
private:
	SelectInfo m_tSelectInfo;
	vector<_matrix> m_vecInstanceMatrix[30];
};

END
