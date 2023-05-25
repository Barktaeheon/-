#pragma once
#include "ImGuiClass_Tool.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CGameObject;
END

BEGIN(Tool)
class CMapViewer_Tool final : public CImGuiClass_Tool
{
public:
	struct vertex3 
	{
		// vertexPos : 1���� Point ��ġ xyz
		_float3 vertexPos[3];
	};
private:
	CMapViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMapViewer_Tool(void) = default;
public:
	static CMapViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
public:
	virtual HRESULT NativeConstruct(void);
	virtual HRESULT NativeConstruct_InnerValue(void);
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Release_InnerValue(void);
	virtual HRESULT Render(void);
public:
	virtual void	Show_Window(void);
	void			Load_File(void);
	void			Show_Window_MapTool(void);
	void			Show_Window_Navigation(void);
	void			Show_Window_MonsterTool(void);
	void			Show_Window_ColliderTool(void);
public:
	virtual void	Free(void) override;
public:
	// CMapObj_Tool, CMapGround_Tool Ŭ�������� ����ϴ��Լ�, � ������ �Ѱ��ش� 
	CModel*			Get_SelectModel(void) { return m_pSelectModel; }
	CModel*			Get_GroundModel(void) { return m_pGroundModel; }
	CModel*			Get_MonsterModel(void) { return m_pSelectMonsterModel; }
private:
	int iToolMode = 0;
	int iVIBuffer = 0;
	int iCreateMode = 0;
	int iNaviMode = 0;
	int iMonsterMode = 0;
	// �� ������Ʈ �𵨵��� �̸�, �𵨷� ������ ����������
	map<_wstring, CModel*>	m_ObjModelMap;
	// ����Ʈ�ڽ����� ������ ��
	CModel*					m_pGroundModel = nullptr;
	CModel*					m_pSelectModel = nullptr;
	// Ground / �ʱ�ȭ�۾��� ó�� �����, ������Ʈ������ ��� ����, �����Ұ�
	CGameObject*			m_pGround = nullptr;
	//  Sky / �ʱ�ȭ�۾��� ó�� �����, ������Ʈ������ ��� ����, �����Ұ�
	CGameObject*			m_pSky = nullptr;
	// Modify��忡�� ��ŷ���� ������ ������Ʈ 
	CGameObject*			m_pModifyObj = nullptr;
	// ImGui�ʱ��۾��� �ʿ��� ����ü 
	DOCKDESC				m_tMenu;
	DOCKDESC				m_tMapToolView;
	DOCKDESC				m_tNavigationView;
	DOCKDESC				m_tColliderView;
	// �̸��� ����� ��������
	_tchar					m_GroundName[MAX_PATH] = L"";
	_tchar					m_SkyName[MAX_PATH] = L"";
	_string					m_strFindFolderFileName = "";
	// ����Ʈ�ڽ����� ���, �����̳� ���µ��� ī��Ʈ�� ���� 
	_uint					m_iIndex = 0;
	// �׺������� ���, �ﰢ�� �ϳ��� ���ͷ� ��������
	vector<vertex3>			m_vecSemo;
	vertex3					m_vertex3;
	// �׺������� ���, �ﰢ�� 1���� �Ǵ��� üũ�ϴ� ī��Ʈ 
	_uint					m_iCount = 0;
	_uint					m_iLoadCount = 0;
	// ���� �Һ���
	_bool					m_bInitSetting = false;
	_bool					m_bLoadCheck = false;
	_bool					m_bNaviLoadCheck = false;
	// �׶���, �� ������Ʈ�� ��ü ������ �Ǻ���Ʈ���� 
	_matrix					m_PivotMatrix;
	vector<_tchar*>			m_vecName;
	//=================Monster============================
	// ����Ʈ�ڽ����� ������ ���� �� 
	CModel*					m_pSelectMonsterModel = nullptr;
	CGameObject*			m_pModifyMonster = nullptr;
	// ���� ������Ʈ �𵨵� 
	map<_wstring, CModel*>	m_MonsterModelMap;
	DOCKDESC				m_tMonsterView;
	_uint					m_iMonsterIndex = 0;
	_matrix					m_MonsterPivotmatrix;
	_bool					m_bMonsterLoadCheck = false;
	vector<_tchar*>			m_vecMonsterName;

	_float					m_ColliderSize[3]{};
	_float					m_fColliderYPos = 0;
	_bool					m_bColliderCheck = false;
};
END