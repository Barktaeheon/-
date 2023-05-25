#pragma once
#include "GameObject_Tool.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CChannel;
class CMeshContainer;
class CAnimationClip;
class CAnimator;
class CBonesCollider;
END

BEGIN(Tool)
class CAnimViewerModel final : public CGameObject_Tool
{
private:
	explicit CAnimViewerModel(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CAnimViewerModel(const CAnimViewerModel& _rhs);
	virtual ~CAnimViewerModel() = default;
	// ���� �߰�
public:
	CModel* Get_Model() { return m_pModelCom; }
public:
	// �ִϸ����� ������Ʈ�� �޾ƿɴϴ�.
	CAnimator* Get_Animator() { return m_pAnimatorCom; }

	// ���� ����ǰ��ִ� �ִϸ��̼��� �̸��� �޾ƿɴϴ�.
	const _string& Get_CurrentAnimationName();
	// ���� ����ǰ��ִ� �ִϸ��̼��� �ε��� ��ȣ�� �޾ƿɴϴ�.
	const _uint& Get_CurretnAnimationIndex();
	// ���� ����ǰ��ִ� �ִϸ��̼��� Duration ���� �޾ƿɴϴ�.
	const _double& Get_AnimationDuration();
	// ���� ����ǰ��ִ� �ִϸ��̼��� �ִϸ��̼� ������� �޾ƿɴϴ� ( TimeAcc )
	const _double& Get_AnimationPlayTime();
	// ���� ����ǰ��ִ� �ִϸ��̼��� TickPerSecond�� �޾ƿɴϴ�
	const _double& Get_TickPerSecond();
	// ���� ����ǰ��ִ� �ִϸ��̼��� Ratio���� �޾ƿɴϴ�.
	_double Get_AnimationRatio();
	// ���� ���� ���̾��Ű ��� ����Ʈ�� �޾ƿɴϴ�.
	vector<CHierarchyNode*> Get_HierarchyNodeList();
	// ���� ����ǰ��ִ� �ִϸ��̼��� ChannelList�� �޾ƿɴϴ�.
	vector<CChannel*> Get_ChannelList();
	// ���� ���� �޽� �����̳� ����Ʈ�� �޾ƿɴϴ�.
	vector<CMeshContainer*> Get_MeshContainerList();


	// �ִϸ��̼��� Loop ���θ� ��ȯ�մϴ�.
	const _bool& Is_Loop() const { return m_bIsLoop; }
	// �ִϸ��̼��� �÷����������� ���� ���θ� ��ȯ�մϴ�.
	const _bool& Is_Play() const { return m_bIsPlay; }
	// �ִϸ��̼��� Loop���θ� �����մϴ�.
	void Set_Loop(const _bool& isLoop) { m_bIsLoop = isLoop; }
	// �ִϸ��̼��� Play���θ� �����մϴ�.
	void Set_Play(const _bool& isPlay) { m_bIsPlay = isPlay; }
	// �ִϸ��̼��� Stop �մϴ�. ( Play�� false�� ��ȯ , �ִϸ��̼��� ���� ������� �ִϸ��̼����� �ٽ� ��ü �� ���� �ʱ�ȭ )
	void Set_Stop();
	// �ִϸ��̼��� ���൵�� �����մϴ�.
	void Set_ProgressRatio(_double Ratio);	
	// �ִϸ��̼��� �� ������ ������ �����ŵ�ϴ�.
	void Play_OneFrame(_double TimeDelta);

	// Ȱ��ȭ �� �޽� �����̳��� �ε��� ����Ʈ�� �����մϴ�.
	void Set_MeshIndexList(vector<_uint> meshIndexList) { m_MeshList = meshIndexList; }
	/// <summary>
	/// �ִϸ��̼� Ŭ���� �߰��մϴ�.
	/// </summary>
	/// <param name="pClip">������ ���� �ִϸ��̼� Ŭ��</param>
	void Add_AnimationClip(CAnimationClip* pClip);

public:
	// ���� �����մϴ�.
	HRESULT Set_Model(CModel* pModel);
	// �ִϸ����͸� �����մϴ�.
	HRESULT Set_Animator(CAnimator* pAnimator);
	// ���� �ִϸ��̼� �ε����� �����մϴ�.
	HRESULT Set_Animation(_uint iAnimIndex);

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderValue();

public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override;
	virtual const _tchar* Get_Layer() override;

public:
	// ���� �߰�
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue();
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut);
	// Delete Self
	virtual HRESULT Delete_Self();
	// Save Data 
	virtual HRESULT Save_Data();
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr);
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr);
	// Reset Data 
	virtual void Reset_Data();
#pragma endregion USE_TOOL_METHOD

private:
	// �ִϸ����� ������Ʈ
	CAnimator* m_pAnimatorCom = nullptr;
	CCollider* m_pAttackColliderCom = nullptr;
private:
	// ���� ������� �ִϸ��̼� �ε���
	_uint m_iCurrentAnimationIndex = 0;
	// �ִϸ��̼� Loop ���θ� �Ǵ��ϴ� bool ����
	_bool m_bIsLoop = false;
	// �ִϸ��̼��� ��� ���θ� �Ǵ��ϴ� bool ����
	_bool m_bIsPlay = false;
	// Render���� �׸� �޽� �ε��� ��� ( int )
	vector<_uint> m_MeshList;
	// �׽�Ʈ �𵨿� �ִϸ��̼� Ŭ�� ( �ִϸ��̼� ������ �ִϸ��̼� Ŭ���� ���鶧 �̸� ȿ������ ���� ���ؼ� ������. )
	CAnimationClip* m_pAnimationClip = nullptr;
	// �׽�Ʈ �𵨿� �ִϸ��̼� Ŭ�� ����Ʈ( �ִϸ��̼� ������ ���� �ٷ� �Ѱ��� ) -> �ִϸ����� �����
	vector<CAnimationClip*> m_AnimClips;
	// ���� �߰�
	// Bones Collider
	vector<class CBonesCollider*> m_BonesCollider;
	// Select Collider
	CBonesCollider* m_pBonesSelectCollider = nullptr;
	// Str 
	_string m_strText = "";
	// Select Node
	CHierarchyNode* m_pSelectNode = nullptr;
	// Save To Find Bone
	list<CHierarchyNode*> m_SaveToFindBoneList;
	// Select Type
	_uint			m_iSelectType = 0;

	class CCollider* m_pSphereCollider = nullptr;
	class CCollider* m_pCubeCollider = nullptr;
	// Prev World Matrix
	_float4x4			m_PrevWorldMatrix;
	_bool				m_bWrapSampler = false;
public:
	static CAnimViewerModel* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END