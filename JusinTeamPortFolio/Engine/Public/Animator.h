#pragma once
#include "Component.h"

BEGIN(Engine)
class CAnimationClip;
class CModel;
class CHierarchyNode;

class ENGINE_DLL CAnimator final : public CComponent
{
public:
	typedef struct tagAnimatorDesc
	{
		CModel* pModel = nullptr;
		CTransform* pTransform = nullptr;
		_wstring strFilePath = L"";
	}ANIMATORDESC;

	// ������ Ʈ���� ��ǲ ����ü
	typedef struct tagInputTrigger
	{
		// �ִϸ��̼� ���� ( �ִϸ��̼��� ������ �����϶� �� Ʈ���Ű� ���Դ���? )
		_double dAnimRatio = 0.0;
		// ��ǲ Ʈ����
		_string	strInput = "";
		// �ִϸ��̼��� ���൵�� AnimRatio�� �̻��϶� ���Դ��� �����϶� ���Դ��� �Ǵ��ϱ����� �°� => true�� �̻� , false�� ����
		_bool	bIsOverRatio = true;
	}INPUTTRIGGER;

	// ������ Ŭ���� ����
	enum ENTERSTATE
	{
		ENTRY,		// ���� ( IDLE�� ��� �̰��� ��� )
		ANYSTATE,	// �ƹ� ���¿����� �� ���� ( HIT�� ��쿡 ��� )
		DEFAULT,	// ������ Ŭ���� �ٸ� Ŭ���� ��� ( ���� 2���� ���� ������ Ŭ���� ����. )
		ENTERSTATE_END
	};

	// �ִϸ��̼� Ŭ������ ����, ���� ���赵�� ���� �� �ִ� ����ü.
	typedef struct tagTransition
	{
		// Enter���°� ������� �����ϱ����� EnumState
		ENTERSTATE		tEnterState = ENTERSTATE_END;
		// ������ Ŭ���� ������ ��� Animator�� Ŭ���� ������ Ŭ���� ��쿡�� ���� Ŭ������ ���� Ŭ���� ���� �� ����.
		// ������ Ŭ���� �������� �ʴ� ��쿡�� � ���¿����� Trigger�� ���� ���� �� ����.
		CAnimationClip* pEnterClip = nullptr;
		/*���� Ŭ���� ������ ��� �ִϸ��̼��� ���� �ȴٸ� ��ü�� ���� �ִϸ��̼� Ŭ���� ���� Ŭ������ �ǵ����ϴ�.
		���� ����Ŭ���� ����Ŭ���� ��� �������� �ʴ� ��쿡�� ���ٸ� Ʈ���Ű� �߻����� �ʴ� �̻��� �ش� Ŭ����
		�ݺ��Ͽ� ����ϰ� �˴ϴ�.*/
		CAnimationClip* pExitClip = nullptr;
		// EnterClip�� ������ ������ �ִϸ��̼� ���൵ ( 0 ~ 1 )
		_double			dEnterRatio = 1.0;
		// ExitClip�� ������ ������ �ִϸ��̼� ���൵ ( 0 ~ 1 )
		_double			dExitRatio = 0.0;
		// Trigger�� ������ ��� �ִϸ��̼��� ��ü�� ������ ( �⺻�� 0���� �Ͽ� ���������� ������ �ٷ� ��ü�ǵ���, ������ �ߵ��ϴ°Ÿ� 1.0���� )
		_double			dChangeRatio = 0.0;
		// ChangeRatio�� ���� ��� True�� ChangedRatio���� Ŭ��, False�� �������� �����Ѵ�. �⺻�� True���Ѵ�.
		_bool			bIsOverChangeRatio = true;
		// ���� �ӵ� ( �⺻�� 1.0�� )
		_double			dLerpSpeed = 1.0;
		// Transition�� �Ǵ��� ��ǲ Ʈ����
		INPUTTRIGGER	InputTrigger;

		/// <summary>
		/// Ʈ���Ÿ� ���Ѵ�.
		/// </summary>
		/// <param name="_trigger">���� Ʈ�������� Ʈ����</param>
		/// <returns></returns>
		_bool Compare_Trigger(INPUTTRIGGER _trigger);
	}TRANSITION;

	// �������� ��Ȳ�� Ʈ�����ǵ��� �ϳ��� ��� �������� �׷� ( �ַ� �ƾ����� ��Ȳ���� ��� )
	typedef struct tagCutAnimationGroup
	{
		// �׷��� �̸�
		_string					strGroupName = "";
		// ��ϵ� Ʈ������ ���
		vector<CAnimationClip*>	AnimationClipList;
		// ��ϵ� Ʈ�����ǵ��� �� �÷���Ÿ��
		_float					fPlayTime = 0.f;
		// �׷��� ������ TimeAcc�� 
		_double					dGroupTimeAcc = 0.0;


		// ���ϴ� �ð����� �ִϸ��̼� Ŭ���� �����ɴϴ�.
		CAnimationClip*			Get_AnimationClip(_float fRatio);
		// ��ϵ� �ִϸ��̼� Ŭ������ �� Duration�� ����ؼ� �����ɴϴ�.
		_double					Get_Duration();
		// ��ϵ� �ִϸ��̼� Ŭ������ �� Duration�� 0 ~ 1 ������ ������ ������ ������ �޾ƿɴϴ�.
		_float					Get_CurrentRatio();
		// ��ϵ� Ʈ�����ǵ��� �� �÷��� Ÿ���� ����ؼ� �����ɴϴ�.
		_float					Get_PlayTime();

	}CUTANIMGROUP;

private:
	explicit CAnimator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CAnimator(const CAnimator& _rhs);
	virtual ~CAnimator() = default;

public:
	// ���� �ִϸ��̼� Ŭ���� ���� �޾ƿɴϴ�.
	CModel* Get_Model() { return m_pModelCom; }
	// ���� �ִϸ����� ������Ʈ�� �����ִ� �ִϸ��̼� Ŭ�� ����� �޾ƿɴϴ�.
	vector<CAnimationClip*> Get_AnimationClipList() { return m_AnimationClips; }
	// ���� ����ǰ��ִ� �ִϸ��̼� Ŭ���� �޾ƿɴϴ�.
	CAnimationClip* Get_CurrentAnimationClip() { return m_pCurrentClip; }
	// ���� ����� �ִϸ��̼� Ŭ���� �޾ƿɴϴ�.
	CAnimationClip* Get_NextAnimationClip();
	// ������ ����ǰ� �ִ� �ִϸ��̼� Ŭ��
	CAnimationClip* Get_PrevAnimationClip() { return m_pPrevClip; }
	// string ������ �ִϸ��̼� Ŭ���� ã�ƿɴϴ�.
	CAnimationClip* Get_AnimationClip(_string _strName);
	// Transition�� ���ǿ� ���缭 �ߵ������� ����Ǵ� CurTransition�� Input���� �޾ƿɴϴ�.
	_string Get_CurrentTrigger();
	// ���� ����ǰ��ִ� �ִϸ��̼� Ŭ���� MainAnimation �̸��� �޾ƿɴϴ�.
	_string Get_MainAnimationName();
	// ���� ����ǰ��ִ� �ִϸ��̼� Ŭ���� SubAnimation �̸��� �޾ƿɴϴ�.
	_string Get_SubAnimationName();
	// RootBoneMatrix�� �����ϴ� Bone�� �̸��� �޾ƿɴϴ�.
	_string Get_RootBoneName() { return m_strRootBoneName; }
	// ���� ������� Clip�� ���� AnimationType�� �޾ƿɴϴ�.
	const _string& Get_AnimationType();
	// Ʈ������ ����� �޾ƿɴϴ�.
	vector<TRANSITION> Get_TransitionList() { return m_TransitionList; }
	// ��Ƽ�� Ʈ������ ����Ʈ
	const vector<TRANSITION>& Get_ActiveTransitionList() { return m_ActiveTransitionList; }
	// WeaponBone ����Ʈ�� �޾ƿɴϴ�.
	vector<CHierarchyNode*> Get_WeaponBoneList();
	// WeaponBone�� �޾ƿɴϴ�.
	CHierarchyNode* Get_WeaponBone();
	// ���� �÷������� �ִϸ��̼� Ŭ���� ���൵�� �޾ƿ´�.
	_double Get_Ratio();
	// ���� �÷������� �ִϸ��̼� Ŭ���� Duration���� �޾ƿɴϴ�.
	_double Get_Duration();
	// ���� �÷������� �ִϸ��̼� Ŭ���� TimePerSecond���� �޾ƿɴϴ�.
	_double Get_TickPerSecond();
	// ���� �÷������� �ִϸ��̼� Ŭ���� TimeAcc���� �޾ƿɴϴ�.
	_double Get_TimeAcc();
	// ���� �÷������� �ִϸ��̼��� ����ӵ����� �޾ƿɴϴ�.
	_float	Get_AnimationSpeed();
	// ���� �÷������� �ִϸ��̼��� RootBone �̵����� �޾ƿɴϴ�.
	_matrix Get_RootBoneMatrix();
	// ���� �÷������� �ִϸ��̼��� BoneTransform �̺�Ʈ�� ���� �����Ӵ� �̵����� ��ȯ�մϴ�.
	_vector Get_BoneTransformPos() { return m_vBoneTransformPos.Get_Vector(); }	
	// ���� �÷������� �ִϸ��̼��� BoneTransfrom �̺�Ʈ���� ��ϵǾ��ִ� �ִ� �̵��Ÿ��� �޾ƿɴϴ�.
	_float Get_MoveDistance();
	// Get Move Force
	_float Get_MoveForce();
	// �ݶ��̴��� ���� ���Ⱚ�� ��ȯ�մϴ�.
	_vector Get_ColliderDir();
	// �ݶ��̴��� ���� Scale���� ��ȯ�մϴ�.
	_vector Get_ColliderScale();
	// �ݶ��̴��� ���� ��ġ���� ��ȯ�մϴ�.
	_vector Get_ColliderPos();
	// �ִϸ��̼� �� ��ȯ
	_float3 Get_RootBoneLook();

	// ���� �����ִ� �ִϸ��̼� Ŭ���� ���࿡ ���� ���� �ݶ��̴� Ȱ��ȭ ����
	const _bool& Is_AttackColliderActive();
	// ���� �����ִ� �ִϸ��̼� Ŭ���� �ݶ��̴� PushForce
	const _float&	Get_PushedForce();
	// ���� �����ִ� �ִϸ��̼� Ŭ���� �ݶ��̴� Height
	const _float&	Get_Height();
	// ���� �����ִ� �ִϸ��̼� Ŭ���� Ư�� �̺�Ʈ�� ����Ǵ� �� �ð� (�����ð�)
	const _double&	Get_EventPlayTime();
	// ���� �����ִ� �ִϸ��̼� Ŭ���� Ư�� �̺�Ʈ�� ����Ǵ����� üũ�ϴ� �Ұ�
	const _bool&	Is_EventPlay();
	// ���� �ִϸ������� Play ���θ� ��ȯ�մϴ�.
	const _bool& Is_Play() { return m_bIsPlay; }
	// ���� �����ִ� �ִϸ��̼� Ŭ���� RootBone �ִϸ��̼��� �����ϴ����� ���� ���θ� ��ȯ�մϴ�.
	_bool Is_RootBone();
	// ���� �����ִ� �ִϸ��̼� Ŭ���� BoneTransForm �ִϸ��̼��� �����ϴ����� ���� ���θ� ��ȯ�մϴ�.
	_bool Is_BoneTransform();
	// ���� �����ִ� �ִϸ��̼� Ŭ�� �̺�Ʈ�� �߷��� �����ϰ��ִ����� üũ�մϴ�.
	_bool Is_Gravity();
	// ���� �����ִ� �ִϸ��̼� Ŭ�� �̺�Ʈ�� �޽��� Ȱ��ȭ ���θ� üũ�մϴ�.
	_bool Is_MeshActive();
	// ���� �����ִ� �ִϸ��̼� Ŭ�� �̺�Ʈ�� Ʈ������ Ȱ��ȭ ���θ� üũ�մϴ�.
	_bool Is_TrailActive();
	// Ʈ������ Start �� End ������ ����� ���� �޾ƿɴϴ�.
	_float2 Get_TrailValues();
	
	const _float3& Get_RealBonePosition() const { return m_vRealBonePosition; }
	// �ִϸ����� ������Ʈ�� BoneDeltaPos�� ����� Ʈ������ ������Ʈ�� �������ݴϴ�.
	void Set_Transform(CTransform* pTransform);
	// ���� �ִϸ��̼� Ŭ���� ���� �����մϴ�.
	void Set_Model(CModel* pModel);
	// ���� �÷������� �ִϸ��̼� Ŭ���� ���൵�� �����մϴ�.
	void Set_Ratio(_double dRatio);
	// �ִϸ��̼� Ŭ���� Update ���� ���� �����ϴ� bool���� �����մϴ�.
	void Set_Play(const _bool& isPlay) { m_bIsPlay = isPlay; }
	// �ִϸ��̼� Ŭ���� ����ϴ�.
	void Set_Stop();	
	// �ִϸ��̼��� TimeAcc���� �������ݴϴ�.
	void Set_TimeAcc(_double dTimeAcc);
	// ��� �� �ִϸ��̼� Ŭ���� �����մϴ�.
	void Set_CurrentAnimationClip(CAnimationClip* pClip);
	// ��� �� �ִϸ��̼� Ŭ���� �����մϴ�. ( �̸����� ���� )
	void Set_CurrentAnimationClip(_string _strName);
	// ���� ��� �� �ִϸ��̼� Ŭ���� �����մϴ�. => �ƾ� ��� ���
	void Set_NextDirectAnimationClip(CAnimationClip* pClip);
	// ���� ��� �� �ִϸ��̼� Ŭ���� �����մϴ�. ( �̸����� ���� ) => �ƾ� ��� ���
	void Set_NextDirectAnimationClip(const _string _strName);

	// �ִϸ��̼� Ŭ���� Entry�� �ǵ����ϴ�.
	void Set_AnimationClipToEntry();
	// �ִϸ��̼� Ŭ���� �߰��մϴ�
	void Add_AnimationClip(CAnimationClip* _pClip);
	// �ִϸ��̼� Ŭ���� �����մϴ�
	void Delete_AnimationClip(_string strName);
	// Ŭ�� ����Ʈ�� �����մϴ�.
	void Set_AnimationClipList(vector<CAnimationClip*> clipList) { m_AnimationClips = clipList; }
	// Ʈ�������� �߰��մϴ�.	
	void Make_Transition();
	// Ʈ������ ����� �����մϴ�.
	void Set_TransitionList(vector<TRANSITION> transitionList) { m_TransitionList = transitionList; Update_TransitionList(); }
	// Ʈ������ ����� �ֽ�ȭ�մϴ�.
	void Update_TransitionList() { m_bIsTransitionUpdate = true; }
	// ��ǲ Ʈ���Ÿ� �޾ƿɴϴ�. ( ���ο��� ���� ���� Ʈ���ſ� �ش�Ǵ� Transition�� �ִٸ� Ŭ���� �����ŵ�ϴ�. )
	_bool Input_Trigger(_string _trigger);	
	// RootBone�ִϸ��̼��� ��� ���θ� �����մϴ�.
	void Set_RootBone(const _bool& _bIsRootBone);
	// RootBoneMatrix�� ���� RootBone�� �̸��� �����մϴ�.
	void Set_RootBoneName(_string _strRootBoneName);
	// AdjustTransformPos�� �����Ѵ�.
	void Set_AdjustTransformPos(_vector _vPos) { XMStoreFloat4(&m_vAdjustPos,_vPos); }
	// �ִϸ����͸� ���� ��ü�� �����̴� ���Ⱚ�� �����մϴ�.
	void Set_OwnerDirection(_vector _vDir) { XMStoreFloat3(&m_vOwnerDirection, _vDir); }
	// �ִϸ��̼��� �̵����� �̿��� ������� �Ÿ����� �����մϴ�.
	void Set_TargetDistance(const _float& _fTargetDistance);
	// ���⿡ �� �� ������ ����Ʈ�� ��´�.
	void Add_WeaponBone(CHierarchyNode* pBone);	
	// WeaponBoneList���� �������� �����͸� �����.
	void Remove_WeaponBone(_uint iWeaponBoneIndex);
	// BeginTransform�� ������ �ʱ�ȭ �մϴ�.
	void Initialize_BeginTransform() { XMStoreFloat4x4(&m_vBeginTransformMatrix, XMMatrixIdentity()); }
	// ���� ������� �ִϸ��̼��� �̵����� ���ڷ� �޾ƿ� Distance�� ���Ͽ� �̵��Ÿ����� ���� ª������ 
public:
#ifdef _DEBUG
	virtual HRESULT NativeConstruct_Prototype();
#endif // _DEBUG
	virtual HRESULT NativeConstruct_Prototype(_wstring strPath);
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);

private:
	//	InputTrigger�� Transition�� �����ִ� InputTrigger�� ���Ͽ� ������ �����ϸ� ���� �ִϸ��̼� Ŭ���� �����Ų��.
	void Change_AnimationClip();
	// ���� �ִϸ��̼� Ŭ���� �����Ѵ�.
	void Set_NextAnimClip(CAnimationClip* _pNextClip) { m_pNextClip = _pNextClip; }
	// �ִϸ��̼� Ŭ���� ������Ʈ�Ѵ�.
	void Update_AnimationClip(const _double& _dTimeDelta);
	// �ڽſ��� �ʿ��� Trigger�� ���� Ʈ�����ǵ��� �ݺ����� ���� ����ó���� �ϴ� �Լ�
	void Check_Transition_Condition();
public:
	// �ִϸ��̼��� ������ ������Ʈ���ش�. ( TimeDelta���� 0���� ó�� )
	void Update_InitBoneState();
	// �� ������ ������ �����Ų��.
	void Play_OneFrame(_double TimeDelta);

private:
	// �ִϸ����� Ŭ������ ����ִ� ��ü�� Ʈ������ ������Ʈ
	CTransform*				m_pOwnerTransformCom = nullptr;
	// �ִϸ��̼� Ŭ���� �����ų �ִϸ��̼� �� ������Ʈ
	CModel*					m_pModelCom = nullptr;
	// ���� �����ִ� �ִϸ��̼� Ŭ��
	CAnimationClip*			m_pCurrentClip = nullptr;
	// ���� ���� �ִϸ��̼� Ŭ��
	CAnimationClip*			m_pNextClip = nullptr;
	// ���� ���� �ִϸ��̼� Ŭ�� ( ������ ���� �ʰ� �ִϸ��̼��� ������ ��� Set�� �̿��Ͽ� ���� Ŭ������ ����. )
	CAnimationClip*			m_pNextDirectClip = nullptr;
	// ���� �ִϸ��̼� Ŭ��
	CAnimationClip*			m_pPrevClip = nullptr;
	// �ִϸ��̼� Ŭ����
	vector<CAnimationClip*>	m_AnimationClips;
	// Ʈ���� ����Ʈ ( string ���ڿ��� ���� )
	vector<_string>			m_TriggerList;
	// ���� �ִϸ��̼� Ŭ���� ����Ǵ� ���� ���� Ʈ���� ��ɵ�
	vector<INPUTTRIGGER>	m_InputTriggerList;
	// Transition ���
	vector<TRANSITION>		m_TransitionList;
	// �ִϸ��̼� Ŭ���� �����ִ� Ʈ���ſ� ���� �̸��� ���� Ʈ�����ǵ�
	vector<TRANSITION>		m_ActiveTransitionList;
	// ���⸦ ���� ���� ��Ƴ��� ����Ʈ
	vector<CHierarchyNode*> m_WeaponBoneList;
	// �ִϸ��̼� Ŭ���� ����ƴ��� üũ�Ѵ�.
	_bool					m_bIsChange = false;
	// �ִϸ��̼� Ŭ���� �����ų�� �����ϴ� bool��
	_bool					m_bIsPlay = false;
	// �ִϸ��̼� ������ �� ����� Transition
	TRANSITION*				m_pCurTransition = nullptr;
	// Root�� �ִϸ��̼� ����� �� ����� �� �̸�
	_string					m_strRootBoneName = "Root";
	// Root�� �ִϸ��̼��� ����ϱ� ���� �ִϸ��̼��� �ٲ� ���� ��ġ�� ����ش�.
	_float4x4				m_vBeginTransformMatrix;
	// Root�� �ִϸ��̼��� ������ ������ �����ϱ����� �����ϴ� ����
	_float3					m_vRootBoneRotation = _float3(0.f, 0.f, 0.f);
	// Root�� Transform �ִϸ��̼� ( ���� ������ �̵������� �̵��ϴ°� )�� �����Ӵ� �̵����� ��� ����
	_float4					m_vBoneTransformPos;
	// Root�� �ִϸ��̼����� ������ m_vRootBoneTransform + BoneDeltaPos ���
	_float4x4				m_vRootBoneMatrix;
	// Prev Root Bone matrix
	_float4x4				m_vPrevRootBoneMatrix;
	// Real Bone Position
	_float3					m_vRealBonePosition = _float3(0.f, 0.f, 0.f);
	// Root�� Transform �ִϸ��̼� ���� �ִϸ��̼��� ��ġ�� �����ϱ����� ����
	_float4					m_vAdjustPos;
	// �ִϸ����͸� ���� ��ü�� ���� �����ϰ��ִ� ���Ⱚ
	_float3					m_vOwnerDirection = _float3(0.f, 0.f, 0.f);
	// Ʈ������ ����� ������Ʈ ������ ���� ����
	_bool					m_bIsTransitionUpdate = false;
public:
	static	CAnimator*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring strPath);
	static	CAnimator*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	// Animator������Ʈ�� pArg�� �� ��ü�� �־�����
	virtual CComponent* Clone( void* pArg)override;
	virtual void Free() override;
};

END