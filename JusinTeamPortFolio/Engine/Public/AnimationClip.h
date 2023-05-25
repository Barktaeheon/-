#pragma once
#include "Engine_Defines.h"
#include "Base.h"

// �ִϸ��̼� Ŭ�� Ŭ������ ��쿡�� ���� �ִϸ��̼ǰ� ���� �ִϸ��̼��� ���� ������, ���� �ִϸ��̼��� ���൵�� ����
// �پ��� ������ �̺�Ʈ���� �߻���Ų��.
// Animator Ŭ�������� AnimationClip���� ������ָ� vector�� �־� �����Ѵ�.

BEGIN(Engine)
class CAnimation;
class CChannel;
class CHierarchyNode;

class ENGINE_DLL CAnimationClip final : public CBase
{
public:
	// �̺�Ʈ Ÿ��
	enum ANIMEVENTTYPE
	{
		// ����Ʈ, UI, ����,ī�޶�, Ʈ����  ��� �ʿ�� �߰�����
		EVENTTYPE_EFFECT,			// ����Ʈ
		EVENTTYPE_UI,				// UI
		EVENTTYPE_SOUND,			// SOUND
		EVENTTYPE_COLLIDER,			// COLLIDER
		EVENTTYPE_WEAPONBONE,		// WEAPONBONE
		EVENTTYPE_CAMERA,			// CAMERA
		EVENTTYPE_MESH,				// Ư�� �޽� �����
		EVENTTYPE_SPEEDSECTION,		// �ִϸ��̼� ����� Ư�� �������� �ӵ��� �����ϴ� �̺�Ʈ
		EVENTTYPE_BONETRANSFORM,	// �ִϸ��̼� ����� Ư�� �������� ���� ��ġ���� �����ϴ� �̺�Ʈ		
		EVENTTYPE_GRAVITY,			// �ִϸ��̼� ����� Ư�� �������� �߷°��� ������ �ȹ����� �����ϴ� �̺�Ʈ		
		EVENTTYPE_TIMECHECK,		// �ִϸ��̼ǿ��� Ư�� �ð��� ���ϱ� ���� �̺�Ʈ		
		EVENTTYPE_TRAIL,			// Ʈ������ �ִ°�� Ʈ������ ���� �״� �� �� �ִ� �̺�Ʈ 
		EVENTTYPE_END
	};

	// �ִϸ��̼��� �����ִ� �̺�Ʈ ���� ( Enum���� �̺�Ʈ�� �ʿ��� ������ ����Ѵ�. )
	typedef struct tagAnimEventDesc
	{
		// �̺�Ʈ ����
		ANIMEVENTTYPE	tEventType = EVENTTYPE_END;
		// �̺�Ʈ�� ���۵Ǵ� �ð� ( �ִϸ��̼��� ���൵ ) 0 - 1 ������ ������ ����
		_double		dStartRatio = 0.0;
		// �̺�Ʈ�� ������ �ð� ( �ִϸ��̼��� ���൵ ) 0 - 1 ������ ������ ���� -> �� �ʿ� ���°�� 0.0 ���� ����
		_double		dEndRatio = 0.0;
		// �̺�Ʈ�� ���� �ð� ( ���۵Ǵ� �ð����� ������ �ð��� ����ϰ� ���� �� )
		_double		dEndTime = 0.0;
		// �ӵ��� �� �������� ������ ����� �� ����� Value�� ( ����� �ε��� ��� ��밡�� )
		_float		fValue = 0.f;
		// �̺�Ʈ�� �̸�
		_string		strName = "";
		// �ӵ��� �� �������� ������ ����� �� ����� Value�� ( fValue�� ������ ��쿡 �����. )
		_float		fAddValue = 0.f;
		// ����ƴ����� ���� Ȯ�� ���ε��� �˸��� bool����
		_bool		bIsFinish = false;
		// �̺�Ʈ TimeAcc��
		_double		dEventTimeAcc = 0.0;
		// �޽� �ε������� ��Ƴ��� ���� �����̳� ( Ư�� �޽��� �Ⱥ��̰� �ϰų� �ϴ� ��쿡 ����Ѵ�. )
		//vector<_uint> MeshIndexList;
		// ����� ���� �̸��̳� �̺�Ʈ�� ����� �� �ִ� string���� �����Ѵ�.
		_string		strEventText = "";
		// ����� �� ��ġ ( ��ü ���� ��ġ -> ���� ����ϸ� �� ��ġ������ + �� ��ġ���� ������ ��. )
		_float4		vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
		// �̺�Ʈ�� ����ִ� Dir��
		_float4		vEventDir = _float4(0.f, 0.f, 0.f, 1.f);
		// �̺�Ʈ�� ����ִ� ���� value�� �Է� ������ float4
		_float4		vEventValues = _float4(0.f, 0.f, 0.f, 1.f);
	}ANIMEVENTDESC;

	// �̺�Ʈ ���� ���ε��� üũ�� �� ������ ��� ����ü
	typedef struct tagEventCheckDesc
	{
		_string		strEventName = "";
		_bool		bIsActive = false;
	}EVENTCHECKDESC;

private:
	explicit CAnimationClip();
	explicit CAnimationClip(const CAnimationClip& _rhs);
	virtual ~CAnimationClip() = default;

public:
	// ���� Ŭ���� ��ϵ� Main�ִϸ��̼� Ŭ���� ��ȯ�մϴ�.
	CAnimation* Get_MainAnimation() { return m_pMainAnimation; }
	// ���� Ŭ���� ��ϵ� Sub�ִϸ��̼� Ŭ���� ��ȯ�մϴ�.
	CAnimation* Get_SubAnimation() { return m_pSubAnimation; }
	// ���� Ŭ���� ��ϵ� �̺�Ʈ ����Ʈ�� ��ȯ�մϴ�.
	vector<ANIMEVENTDESC> Get_EventList() { return m_EventList; }
	// ���� �ִϸ��̼� �̸��� �޾ƿɴϴ�.
	const _string& Get_MainAnimationName();
	// ���� �ִϸ��̼� �̸��� �޾ƿɴϴ�.
	 const _string& Get_SubAnimationName();
	// �ִϸ��̼� Ŭ���� �̸��� �޾ƿɴϴ�.
	const _string& Get_Name() { return m_strName; }
	// ��Ʈ���� �̸��� �޾ƿɴϴ�.
	_string Get_RootBoneName();
	// �ִϸ��̼��� Ÿ���� �޾ƿɴϴ�.
	const _string& Get_AnimationType() { return m_strAnimType; }
	// ���� �ִϸ��̼��� Duration���� �޾ƿɴϴ�.
	const _double& Get_Duration();
	// ���� �ִϸ��̼��� Tps���� �޾ƿɴϴ�.
	const _double& Get_TickPerSecond();
	// ���� �ִϸ��̼��� TimeAcc���� �޾ƿɴϴ�.
	const _double& Get_TimeAcc();
	// ���� �ִϸ��̼��� �̺�Ʈ ����ð��� ���� �ð����� �޾ƿɴϴ� ( �����ð� => �̺�Ʈ ������ �Ǵ� Ratio *  Duration / TickPerSecond )
	const _double& Get_EventPlayTime();
	// �ִϸ��̼� Ŭ���� ��� �ӵ��� �޾ƿɴϴ�.
	const _float& Get_AnimationSpeed() { return m_fSpeed; }
	// ���� �ִϸ��̼��� RootBone �̵����� �޾ƿɴϴ�.
	_matrix Get_RootBoneMatrix();
	// ���� �ִϸ��̼��� RootTransform �̵����� �޾ƿɴϴ�.
	_vector Get_RootTransform();	
	// WeaponBoneIndex�� �����Ѵ�.
	const _uint& Get_WeaponBoneIndex() { return m_iWeaponBoneIndex; }	
	// ���ΰ�ü�� Ʈ�������� �޾ƿ´�.
	CTransform* Get_OwnerTransform() { return m_pOwnerTransform; }
	// �ݶ��̴��� ���� ���Ⱚ�� ��ȯ�մϴ�.
	_vector Get_ColliderDir() { return XMLoadFloat4(&m_vColliderDir); }
	// ���൵�� �����Ѵ�.
	void Set_Ratio(_double dRatio);
	// �ִϸ��̼ǵ��� TimeAcc���� �����Ѵ�.
	void Set_TimeAcc(_double dTimeAcc);
	// ���� �ִϸ��̼��� �������ش�.
	void Set_MainAnimation(CAnimation* _pAnimation); 
	// ���� �ִϸ��̼��� �������ش�.
	void Set_SubAnimation(CAnimation* _pAnimation);
	// ���� �ִϸ��̼� Ŭ���� ������������ ���� ���θ� ��ȯ�մϴ�.
	const _bool& Is_Loop() { return m_isLoop; }
	// ���� �ִϸ��̼��� ���������� ���� ���θ� ��ȯ�մϴ�.
	_bool Is_Finish();
	// RootBone ������ ������ ���� ���θ� ��ȯ�մϴ�.
	_bool Is_RootBone();
	// BoneTransform ������ ������ ���� ���θ� ��ȯ�մϴ�.
	_bool Is_BoneTransform() { return m_bIsBoneTransform; }
	// �߷��� �����ų�� �Ƚ�ų���� ���� ���θ� ��ȯ�մϴ�.
	_bool Is_Gravity() { return m_bIsGravity; }	
	// �޽��� ���� ������ ���� ���θ� ��ȯ�մϴ�.
	_bool Is_MeshActive() { return m_bIsMeshActive; }
	// Ʈ������ Ȱ��ȭ ���θ� ��ȯ�մϴ�.
	_bool Is_TrailActive() { return m_bIsTrailActive; }
	// Ʈ���� Value�� �޾ƿɴϴ�.
	_float2 Get_TrailValues();
	// TimeCheck �̺�Ʈ�� ���� Ȱ��ȭ�� ���� �̺�Ʈ�� ��ȯ�մϴ�.
	const EVENTCHECKDESC& Get_EventCheckDesc() { return m_tEventCheck; }
	// BoneTransform�� �̵��� ������ �մϴ�.
	const _float& Get_MoveDistance() { return m_fMoveDistance; }
	//  Bone Move Force
	const _float& Get_BoneMoveForce() const { return m_fBoneAnimMoveForce; }
	// ���� �ݶ��̴� Ȱ��ȭ ���θ� �����մϴ�.
	const _bool& Is_AttackColliderActive() const { return m_bIsColliderActive; }
	// �ݶ��̴��� ���� ��ġ���� ��ȯ�մϴ�.
	_vector Get_ColliderPos() { return XMLoadFloat4(&m_vColliderPos); }
	// �ݶ��̴��� ���� Scale���� ��ȯ�մϴ�.
	_vector Get_ColliderScale() { return XMLoadFloat4(&m_vColliderScale); }
	// �ݶ��̴��� ���� �ݶ��̴� ������ ��ȯ�մϴ�.
	const _string& Get_ColliderType() { return m_strColliderType; }

	// �и��� ���� �޾ƿɴϴ�.
	const _float& Get_PushedForce() const { return m_fPushedForce; }
	// ���� ���� �޾ƿɴϴ� ( Y�� )
	const _float& Get_Height() const { return m_fHeight; }
	// �̺�Ʈ�� ����Ǵ����� �޾ƿɴϴ�.
	const _bool& Is_EventPlay() const { return m_bIsEventPlay; }

	// �������� ������ ���θ� �޾ƿɴϴ�.
	_bool Is_SupplementSituation();
	// �������� ������ �˷��ִ� bool���� �����մϴ�.
	void Set_SupplementSituation(const _bool& _bSituation);
	// �ִϸ��̼� Ŭ���� �̸��� �����մϴ�.
	void Set_Name(const _string& _strName) { m_strName = _strName; }
	// ���� ���θ� �����մϴ�.
	void Set_Loop(const _bool& isLoop) { m_isLoop = isLoop; }
	// ���� �ִϸ����Ͱ� ���� Model�� HierarchyNodeList�� �����մϴ�.
	void Set_HierarchyNodeList(vector<CHierarchyNode*> _HierarchyNodeList) { m_HierarchyNodeList = _HierarchyNodeList; }
	// RootBone ���θ� �����մϴ�.
	void Set_RootBone(const _bool& isRootBone);
	// RootBone�� �̸��� Animation���� �����ϴ�.
	void Set_RootBoneName(const _string& _strRootBoneName);
	// RootTransform�� �ʿ��� �ִϸ����� ������Ʈ�� ����ִ� ��ü�� Look ���͸� �����մϴ�.
	void Set_RootTransformDir(const _fvector _vLook) { XMStoreFloat3(&m_vOwnerLook, _vLook); }
	// �ִϸ��̼��� Ÿ���� �����մϴ�.
	void Set_AnimationType(const _string& _strAnimType) { m_strAnimType = _strAnimType; }
	// Finish ���θ� �����մϴ�
	void Set_Finish(const _bool& isFinish);
	// �̺�Ʈ ����Ʈ�� �����Ѵ�
	void Set_Event(const vector<ANIMEVENTDESC>& _EventList) { m_EventList = _EventList; }
	// �ִϸ��̼� �ӵ����� �����Ѵ�.
	void Set_AnimationSpeed(const _float& fFastValue);
	// OwnerTransform�� �����Ѵ�.
	void Set_OwnerTransform(CTransform* _pOwnerTransform);
	// OwnerDirection�� �����Ѵ�.
	void Set_OwnerDirection(_vector _vDir) { XMStoreFloat3(&m_vOwnerDirection,_vDir); }
	// RootBone�� Look�� ����ش�.
	void Set_RootBoneLook(_vector _vRootBoneLook) { XMStoreFloat3(&m_vRootBoneLook, _vRootBoneLook); }
	// Ratio ���� ����
	_double Get_Ratio();
	// �ִϸ��̼ǵ��� �����͸� �ʱ�ȭ�Ѵ�.
	void Reset_AnimationData();
	// Ÿ�ٰ��� �Ÿ��� �޾ƿɴϴ�.
	void Set_TargetDistance(const _float& _fTargetDistance) { m_fTargetDistance = _fTargetDistance; }
	// BoneTransform �ִϸ��̼��� ��� �̵��Ÿ��� ����մϴ�. ( ������ �ΰ���� ��� ��� )
	_bool Compute_Distance();

	void Set_SupplementAnim(const _bool& _bIsSupplementAnim) { this->m_bSupplementAnimation = _bIsSupplementAnim; }
public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT NativeConstruct_Clone(void* pArg);
	virtual HRESULT NativeConstruct(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName);
	void  Tick(const _double& _dTimeDelta);
	void  Late_Tick(const _double& _dTimeDelta);
	
public:
	// �ִϸ����Ϳ��� Ŭ���� �ٲ� Ŭ���� ������ �ʱ�ȭ���ݴϴ�.
	void Initialize_Data();
	// ���� �ִϸ��̼����� ���������Ѵ�.
	void Update_NextAnimation(const _double& _dTimeDelta, CAnimationClip* _pNextClip);
	// ���� �ִϸ��̼����� ���������Ѵ�. ( 0.0�� �ƴ� ���ڷε����� �������� ���� )
	void Update_NextAnimation(const _double& _dTimeDelta,const _double& _dNextRatio,const _double& _dLerpSpeed, CAnimationClip* _pNextClip);
	// Transform Matrices�� ������Ʈ �Ѵ�.
	void Update_TransformMatrices(const _double& _dTimeDelta);
	// Transform Matrices�� ������Ʈ �Ѵ�.
	void Update_TransformMatrices();

private:
	// string������ ���̾��Ű ��带 ã�ƿɴϴ�.
	CHierarchyNode* Find_HierarchyNode(const _string& _strBoneName);

private:
	// �̺�Ʈ���� ������Ʈ�� �Ѵ�. ( �̺�Ʈ ������� �۾��� �̷������. )
	void Update_Event(const _double& _dTimeDelta);
	void Update_Event_Collider(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Sound(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Effect(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_MeshHide(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_SpeedSection(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_WeaponBone(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_RootTransform(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_Gravity(ANIMEVENTDESC& _event, const _double& _dTimeDelta);
	void Update_Event_TimeCheck(ANIMEVENTDESC& _event, const _double& _dTimeDelta);	
	void Update_Event_Trail(ANIMEVENTDESC& _event, const _double& _dTimeDelta);

private:
	// �ִϸ��̼� Ŭ���� �̸� -> Default�� �ִϸ��̼��� �̸������� ���Ѵٸ� �ִϸ����Ϳ��� �������� �� ����.
	_string m_strName = "";
	// �ִϸ��̼� Ÿ�� -> ���� ������� �ִϸ��̼� Ŭ���� ����, ��ų, ����, �̵����� ��ɵ��߿� ��Ϳ� �ش�Ǵ����� �Ǵ��Ѵ�.
	_string m_strAnimType = " ";
private:
	// ���� �ִϸ��̼�
	CAnimation* m_pMainAnimation = nullptr;
	// ���� �ִϸ��̼� -> ���� �ִϸ��̼��� ���� �ִϸ��̼��� Update_Matrices�� ��� �̷���� �� �ѹ� �� ���� ������Ʈ��.
	CAnimation* m_pSubAnimation = nullptr;
	// �ִϸ����Ϳ��� ���޹��� HierarchyNodeList
	vector<CHierarchyNode*> m_HierarchyNodeList;
	// ���� �ִϸ��̼��� ������ �޴� ä��
	vector<CChannel*>	m_MainChannelList;
	// ���� �ִϸ��̼��� ������ �޴� ä��
	vector<CChannel*>	m_SubChannelList;
	// �ִϸ��̼� �⺻ ��� �ӵ�
	_float					m_fSpeed = 1.f;
	// �̺�Ʈ ���
	vector<ANIMEVENTDESC>	m_EventList;
	// Loop ���θ� ��� �Ұ�
	_bool					m_isLoop = false;
	// BoneTransform�� ����ϴ����� ���� ���θ� ��� �Ұ�
	_bool					m_bIsBoneTransform = false;
	// WeaponBoneIndex
	_uint					m_iWeaponBoneIndex = 0;
	// ColliderActive
	_bool					m_bIsColliderActive = false;
	// Collider���� ����ϴ� Dir��
	_float4					m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	// Collider Scale
	_float4					m_vColliderScale = _float4(0.f, 0.f, 0.f, 0.f);
	// Collider Position
	_float4					m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
	// PushedForce ( �ݶ��̴� �浹�� �и��� �� )
	_float					m_fPushedForce = 0.f;
	// Height ( �ݶ��̴� �浹�� ���� �� )
	_float					m_fHeight = 0.f;
	// RootTransform�� ����Ҷ� ����� �ִϸ����� ������Ʈ�� ����ִ� ��ü�� Transform
	CTransform*				m_pOwnerTransform = nullptr;
	// RootTransform�� ����Ҷ� ����� �ִϸ����� ������Ʈ�� ����ִ� ��ü�� Look ����
	_float3					m_vOwnerLook = _float3(0.f, 0.f, 0.f);
	// RootTransform�� ����Ҷ� ����� �ִϸ����� ������Ʈ�� ����ִ� ��ü�� �̵� ����
	_float3					m_vOwnerDirection = _float3(0.f, 0.f, 0.f);
	// Root���� Look
	_float3					m_vRootBoneLook = _float3(0.f, 0.f, 0.f);
	// Root�� �ִϸ��̼��� ����ϱ� ���� �ִϸ��̼��� �ٲ� ���� ��ġ�� ����ش�.
	_float4					m_vBeginTransform = _float4(0.f, 0.f, 0.f, 0.f);
	// Bone Anim Move Force
	_float					m_fBoneAnimMoveForce = 0.f;
	// Ȱ��ȭ�Ǵ� �޽��� �ѹ��� ��´�. ( size�� 0�̶�� ��� �޽��� �� �����Ѵ�. )
	vector<_uint>			m_MeshIndexList;
	// �ִϸ��̼��� RootTransform �̺�Ʈ�� ����Ǵ� ����
	_double					m_dEventPlayTime = 0.0;
	// ���� �ִϸ��̼� RootTransform ����ð�
	_double					m_dRestPlayTime = 0.0;	
	// �ִϸ��̼��� RootTransform �̺�Ʈ�� ����Ǵ��� üũ�ϴ� �Ұ�
	_bool					m_bIsEventPlay = false;
	// �ִϸ��̼ǿ� �̺�Ʈ�� ��ϵǾ��ִ� �̵��Ÿ���
	_float					m_fMoveDistance = 0.f;
	// Ÿ���� �ִٸ� Ÿ�ٰ��� �Ÿ��� �޾ƿɴϴ�.
	_float					m_fTargetDistance = 0.f;
	// Ʈ������ ���۰�
	_float					m_fTrailStartValue = 0.f;
	// Ʈ������ ����
	_float					m_fTrailEndValue = 0.f;
	// �߷°��� ���� ���θ� �޾ƿɴϴ�.
	_bool					m_bIsGravity = true;
	// �޽� Ȱ��ȭ�� ���� ���θ� �޾ƿɴϴ�.
	_bool					m_bIsMeshActive = true;
	// Ʈ���� Ȱ��ȭ ���θ� �޾ƿɴϴ�.
	_bool					m_bIsTrailActive = false;
	// �̺�Ʈ���� ������ �ݶ��̴��� Ÿ��.
	_string					m_strColliderType = "";
	// TimeCheck �̺�Ʈ���� Ȱ��ȭ�� �̺�Ʈ�� ��� ����ü ���� 
	EVENTCHECKDESC			m_tEventCheck;
	// Supplement Animation
	_bool					m_bSupplementAnimation = true;
	// ���� ���� �ε���
	_int					m_iRandomSoundIndex = 0;
	// ���� ���� ���� ���� ( �ѹ��� �ǵ��� )
	_bool					m_bIsRandomPlay = false;
	_string				STR_NULL = " ";
public:
	// ���ڰ����� �̺�Ʈ ����� �޴´�.
	static CAnimationClip* Create(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName);
	static CAnimationClip* Create();
	CAnimationClip* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END