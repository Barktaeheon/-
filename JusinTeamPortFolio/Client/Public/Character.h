#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CBonesCollider;
class CController;
class CAnimator;
class CAnimationClip;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CBattle_Manager;
class CCutSceneModul;

class CCharacter : public CPawn
{
public:
	enum class CHAR_TYPE
	{
		AKAZA, SABITO, TANJIRO, ZENITSU, ONI, TEONI, CHAR_END
	};

	enum  INPUTDATA : _uint
	{
		INPUT_ANIMTYPE = 0, INPUT_TRANSITIONLIST,
		INPUT_CURANIMATION, INPUT_PREVANIMATION,

		INPUT_END
	}INPUTDATA;

	enum OUTDATA : _uint
	{
		OUT_STATE = 0, OUT_DIRECTION, OUT_TARGET_TRANSFORM,
		OUT_END
	};

	// KnockBack
	typedef struct tagKnockBack
	{
		// �ִϸ��̼��� �˹��� ����Ǵ� �� ����ð�
		_double dPlayTime = 0.0;
		// �̴� ��
		_float	 fPushedForce = 0.f;
		// �������� �ִ� ���� ( ������� �и��⸸ ��. )
		_float  fHeight = 0.f;
		// �и��� ���� ( ���� ��ü -> ���� ��ü�� ���ϴ� ���⺤�� )
		_float3 vAttackDir = _float3(0.f, 0.f, 0.f);
	}KNOCKBACK;

	// Battle Data
	typedef struct tagBattleData
	{
		tagBattleData() = default;
		tagBattleData(const _float& _HP, const _float& _Power, const _float& _DefensiveValue,
			const _float& _AttackTimer = 20.f, const _float& _fComboTimer = 1.f)
			: fMaxHp(_HP), fCurHp(_HP), fAttackPower(_Power), fDefensiveValue(_DefensiveValue),
			tAttackTimer(CUSTIMER(_AttackTimer)), tComboTimer(CUSTIMER(_fComboTimer))
		{}
		void Decrease_HP(const _float& _fDamage);

		void Init();
		// Die 
		_bool Is_Die() { if (fCurHp <= 0.f) return true; return false; }
		// Increase
		void Increase_Charge(const _float& fValue) { fChargeValue += fValue; if (fChargeValue >= fMaxChargeValue) fChargeValue = fMaxChargeValue; }
		// Decrease Charge Value
		void Decrease_Charge(const _float& fValue);
		// Is Charget Max Charge
		_bool Is_CheckMaxCharge() { if (fMaxChargeValue == fChargeValue) return true; else return false; }
		// Increase
		void Increase_Ultimate(const _float& fValue);
		// Increase_UltimateCount
		void Increase_UltimateCount();
		// Full of UltimateCount
		_bool Is_CheckMaxUltimateCount() { if (iUltimateCount == iUltimateMaxCount) return true; else return false; }
		// Decrease AwakeValue
		void Decrease_AwakeValue(const _float& fValue);
		// Awake Active
		void Active_AwakeMode();
		// Add Win Count
		void Increase_WinCount() { iWinCount++; }
		// Increase HP
		void Increase_HP(const _float& fValue) { fCurHp += fValue; if (fCurHp >= fMaxHp) fCurHp = fMaxHp; }

		// Value
		_float				fMaxHp = 100.f;
		_float				fCurHp = 0.f;
		_float				fAttackPower = 0.f;
		_float				fDefensiveValue = 0.f;
		_float				fMaxChargeValue = 100.f;
		_float				fChargeValue = 0.f;
		_float				fMaxUltimateValue = 100.f;
		_float				fUltimateValue = 0.f;
		_float				fAwakeValue = 100.f;
		// Ultimate Count
		_uint				iUltimateMaxCount = 3;
		_uint				iUltimateCount = 0;
		// Hit Count
		_uint				iHitCount = 0;
		// Awake
		_bool				bIsAwake = false;
		// Hit Charge Timer
		CUSTIMER		tChargeTimer = CUSTIMER(3.f);
		// Hit Timer
		CUSTIMER		tAttackTimer = CUSTIMER(20.f);
		// Combo Timer
		CUSTIMER		tComboTimer = CUSTIMER(1.0f);
		// Win Count
		_uint			iWinCount = 0;
		// Stop
		_bool			bBattleStop = false;
		// Over Hit
		_bool			bIsOverHit = false;
	}BATTLEDATA;

protected:
	explicit CCharacter(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CCharacter(const CCharacter& _rhs);
	virtual ~CCharacter() = default;
public:
	// Battle Data
	BATTLEDATA& Get_BattleData() { return m_tBattleData; }
	// BoneCollider
	BONECOLLIDERS Get_BoneCollider() { return m_BonesCollider; }
	// Character Type
	const CHAR_TYPE& Get_CharType() const { return m_eCharType; }

	const _bool& Is_ActiveCutScene() const { return m_bActiveCutScene; }
public:
	CAnimator* Get_Animator() { return m_pAnimator; }
	// Controller
	CController* Get_Controller() { return m_pController; }
	// Animation Attack Collider
	CCollider* Get_AttackCollider() { return m_pAnimAttackCollider; }
	// Get Hit Check
	const _bool& Is_Hit() const { return m_bIsHit; }
	// Get Air Check
	const _bool& Is_Air() const { return m_bIsAir; }
	// Motion Blur
	const _bool& Is_MotionBlur() const { return m_bMotionBlur; }
	// Set Hit Check
	void Set_Hit(const _bool& bIsHit) { m_bIsHit = bIsHit; }
	// Set BehaviorDir
	void Set_BehaviorDir(const _vector& _vDir) { m_vBehaviorDir = _vDir; }
	// Set KnockBackInfo
	void Set_KnockBackInfo(const KNOCKBACK& _tKnockBack) { m_tKnockBackInfo = _tKnockBack; }

	void Set_ActiveCutScene(const _bool& _bIsActiveCutScene) { this->m_bActiveCutScene = _bIsActiveCutScene; }

	void Set_Navigation(const _tchar* pNavigationTag);


public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT	RenderLight() override;
	virtual void Extern_ImGuiClass() override;
	// Start Scene 
	void Start_Scene(CCharacter* _Enemy);
public:
	// OtherChracter To distance
	_float OtherCharacterToDistance(CGameObject* _pOther);
	_float OtherCharacterDirToLook(CGameObject* _pOther);
	_float OhterCharacterDirToRight(CGameObject* _pOther);
	_float OtherCharacterDirToLookConverter(CGameObject* _pOther);
	_float3 OtherCharacterDirToLookVectorF3(CGameObject* _pOther);

	// OtherChracter To distance
	_float OtherCharacterToDistance(CTransform* _pOther);
	_float OtherCharacterDirToLook(CTransform* _pOther);
	_float OhterCharacterDirToRight(CTransform* _pOther);
	_float OtherCharacterDirToLookConverter(CTransform* _pOther);
	_float3 OtherCharacterDirToLookVectorF3(CTransform* _pOther);
protected:
	// SetUp Cut Scnee 
	HRESULT Add_CutScene(FILEDATA* _pFileData);
protected:
	_uint	Judge_Direction(const _float& _fDot);
	// ���� �÷��̾ �����̴��� üũ
	_bool	Is_JudgeMoveState();
public:
	// ���� �������� ���� ���� üũ
	_bool	Is_JudgeFallState();
	// �浹�� ��ŭ �о��ش�.
	virtual void	Push(CCharacter* _pOther);
	// �� ������ ���濡�� �¾��� �� ���� ó�� 
	virtual void	Check_AttackCollision(CCharacter* _pOther);
	// ����ü�� �¾��� ���
	virtual void	Hit_ByProjectile(_float _fPushForce, _float _fHeight, _float _fDamage, _float _fDelayTime, CCharacter* _pEnemyCharcter);
	// ���ݿ� ���� ����� ���¸� �˻��Ͽ� �ݶ��̴� üũ�� ���� ���� �����Ѵ�.
	_bool			Check_Enemy_State(CCharacter* _pEnemyCharcter);

protected:
	// AnimAttackCollider�� �¾��� ���
	virtual void	Success_AnimAttack(CCharacter* _pOther);
	// �ǰ� �������� ó��
	virtual void	Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
	// �ǰ� �������� ����Ʈ ó��
	virtual void	Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter);
	// ��ġ�� ���ݿ� �¾������� ó��
	virtual void	Hit_Pushed(_vector _vDir);
	// ���۾Ƹ� ���·�, �¾����� �������� ������ �и��ų� ���� �ʴ� ��츦 �Ǵ��Ѵ�.
	virtual _bool	Hit_SuperArmor();

	// SetUp Animator
	virtual HRESULT SetUp_Animator(const _tchar* _pName, CModel* _pModel);
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
	// Jump
	virtual void Jump(const _double& _dTimeDelta, const _string& _strTrigger);
	// Charge
	virtual void Charge(const _double& _dTimeDelta, const _string& _strTrigger);

	// �����Ӵ� �̵��� ��� �Լ� ( MoveSpeed�� �̿��Ͽ� ����. )
	_vector Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir, _float fMoveSpeed, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// �����Ӵ� �̵��� �Ի� �Լ� ( �ִ� �̵��� �Ÿ��� �̿��Ͽ� ����. )
	_vector Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// �������� ������ �Ʒ��� ������ ����� �Լ�
	_vector Strike(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// Bound
	virtual void Bound(const _double& _dTimeDelta);
	// Fall
	virtual void Fall(const _double& _dTimeDelta, const _string& _strTrigger);
	// Restrict Nav
	virtual void Restrict_Nav(const _double& _dTimeDelta);

protected:
	// Skill 1
	virtual void	Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Skill 2 
	virtual void	Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Skill 3
	virtual void	Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Move Normal
	virtual void	Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack
	virtual void	Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Upper
	virtual void	Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Down
	virtual void	Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Attack Awake
	virtual void	Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Ultimae
	virtual void	Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Charge Attack
	virtual void	Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Step Back
	virtual void	Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Dash Ground
	virtual void	Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Dash Air
	virtual void	Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Jump
	virtual void	Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// ����
	virtual void	Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// ������
	virtual void	Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// �Ϲ����� Hit�� ��쿡 ���
	virtual void	Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// ���߿� ���� Hit ���� ��쿡 ���
	virtual void	Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Down �Ǿ��ִ� �����϶��� ���� ó��
	virtual void	Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Upper�¾Ƽ� AIR_DOWN ���°� �ưų� �ϴ� ��쿡 ���
	virtual void	Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Strike ( ���߿��� STRIKE ���ݿ� ���� ��쿡 ��� )
	virtual void	Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Bound �������� ����ó��
	virtual void	Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Spin �������� ����ó�� ( Spin ���߿��� ������ )
	virtual void	Spin_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// BLOW �������� ����ó�� ( ��ų� �¾Ƽ� �ϴÿ� �� �ߴ� ��� �����. )
	virtual void	Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Push�� �¾������� ����ó��
	virtual void	Hit_PushedState(const _double& _dTimeDelta);
	// Dash���� �΋H�������� ����ó��
	virtual void	Dash_Reflect_State(const _double& _dTimeDelta);
	// DEAD �����ϋ��� ����ó��
	virtual void	Dead_State(const _double& _dTimeDelta);

public:
	// ������ ������ ������ �޾Ƽ� �ش� ������ ���߽� �̷������ϴ� �ൿ���� �����մϴ�. ( �ش� �Լ��� �ѹ��� ȣ����. )
	virtual void	Check_Attack_Type(_string _strAttackType);
	// ���� ������ ������ �޾Ƽ� �ش� ������ �¾������� �̷������ϴ� �۾��� �����մϴ�.
	virtual void	Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharcter);
protected:
	// Upper ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Attack_Upper();
	// Down ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Attack_Down();
	// ��ų 1 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_1();
	// ��ų 2 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_2();
	// ��ų 3 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_3();
	// ��� ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Throw();
	// ���� �߰� ���ݿ� �������� �� ȣ��ȴ�. ( �ƾ� ���� )
	virtual void	Success_Attack_Awake();
	// ���� �õ��⸦ ������ �� ȣ��ȴ�. ( �ƾ� ���� )
	virtual void	Success_Ultimate();

protected:
	// Coll Init
	virtual void Coll_Init() override;
	// Coll Tick
	virtual void Coll_Tick(_matrix _WorldMatrix) override;
#ifdef _DEBUG
	// Render���� �׷��ٶ�
	virtual void Coll_Render() override;
	// Renderer ������Ʈ�� �־��༭ �׷��ٶ�
	virtual void Coll_Renderer() override;
#endif
	// Bones Data 
	void Load_BonesData();
	// Input Trigger 
	void Input_Trigger(const _string& _strCurrentTrigger);
	// �뽬 ���� �Ǵ�
	void Success_Dash(CCharacter* _pOther, CBattle_Manager* _pBattleManager);

protected:
	// ���� �߰� �Լ����� ����ϴ� �Լ�
	virtual HRESULT Initialize_Sound();
	// ���带 �߰��մϴ�. ( 1���� = ����� Ű��, 2���� = ���� ���� �̸� )
	HRESULT Add_Sound(const _tchar* _szKey, const _tchar* _szFileName);
	// ��ϵ� ���� ����Ʈ�� �޾ƿɴϴ�.
	vector<const _tchar*>* Find_SoundList(const _tchar* _szKey);
	// ��ϵ� ������� �������� ����մϴ�.
	HRESULT Play_RandomSound(const _tchar* _szKey, _float fVolume = 0.5f);
	// ��ϵ� ������� �������� ����մϴ� ( �ߺ� üũ )
	HRESULT Play_RandomSound_ByCondition(const _tchar* _szKey, _float fVolume = 0.5f);
	// ��ϵ� ������� �׳� ����մϴ�.
	HRESULT Play_Sound(const _tchar* _szKey, const _tchar* _szFileName, _float fVolume = 0.5f);

protected:
	// Controller
	CController* m_pController = nullptr;
	// Animator 
	CAnimator* m_pAnimator = nullptr;
	// Collider
	CCollider* m_pAttackColliderCom = nullptr;
	// Collider											
	CCollider* m_pAnimAttackCollider = nullptr;
	// Navigation
	CNavigation* m_pNavigationCom = nullptr;
	// Current Animation Clip 
	CAnimationClip* m_pCurrentAnimationClip = nullptr;
	// Cut Scene Modul 
	map<_string, CCutSceneModul*>	m_CutSceneModulMap;
protected:
	// Previouse Position
	_float3													m_vPreviousPosition = _float3(0.f, 0.f, 0.f);
	// Bones Collider
	BONECOLLIDERS											m_BonesCollider;
	// Collision Hit Frame
	_bool													m_bDamagedToCollision = false;
	// �и��� ��
	_float													m_fPushedForce = 0.f;
	// Char Type
	CHAR_TYPE												m_eCharType = CHAR_TYPE::CHAR_END;
	// State 
	_uint													m_iState = 0;
	// Move Speed
	_float													m_fMoveSpeed = 0.f;
	// Save Move Speed
	_float													m_fSaveMoveSpeed = 0.f;
	// Rotate Speed
	_float													m_fRotateSpeed = 0.f;
	// Camera�� Right Up Look �������� ������� �÷��̾ �����̴� ��쿡 ����ϴ� ���⺯��
	_float3													m_vDirection = _float3(0.f, 0.f, 0.f);
	// ������ �뽬�� ��� �ִϸ��̼��� ���۵ɶ� ó������ ���� ���Ⱚ ( �� ���Ⱚ���� �ִϸ��̼��� ���������� ��� )
	_float3													m_vBehaviorDir = _float3(0.f, 0.f, 0.f);
	// Pushed Pos (�и��� ��ġ)
	_float3													m_vPushedPos = _float3(0.f, 0.f, 0.f);
	// �߷�
	_float													m_fGravity = 0.f;
	// TimeAcc
	_double													m_dJumpTimeAcc = 0.0;
	// Current Trigger
	_string													m_strCurrentTrigger = "";
	// Prev Trigger
	_string													m_strPrevTrigger = "";
	// Jump Height
	_float													m_fCurrentJumpHeight = 0.f;
	// Prev Jump Height
	_float													m_fPrevJumpHeight = 0.f;
	// Jump Speed
	const _float											m_fJumpSpeed = 120.f;
	// Damaged Count
	_uint													m_iDamgedCount = 0;
	// Collider Direction
	_float4													m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	// Hit Check
	_bool													m_bIsHit = false;
	// Check Push
	_bool													m_bIsHitPush = false;
	// Push TimeAcc
	_double													m_dPushTimeAcc = 0.0;
	// KnockBack ����
	KNOCKBACK												m_tKnockBackInfo;
	// ���߿� ���ִ����� �˷��ִ� �Һ���
	_bool													m_bIsAir = false;
	// �߶� �ð� Acc
	_double													m_dFallTimeAcc = 0.0;
	// ��ų ������
	_float													m_fSkillGauge = 0.f;
	// ��� ������
	_float													m_fStamina = 0.f;
	// ��ų ������ ī��Ʈ
	_uint													m_iSkillCount = 0;
	// ���� ���� ī��Ʈ
	CUSTIMER												m_tChargeAttackTimer = CUSTIMER(2.f);
	// ���� ������� �ִϸ��̼� Ŭ���� Ÿ��
	_string													m_strAnimType = "";
	// Battle Data
	BATTLEDATA												m_tBattleData;
	// �� ������ ����
	_float4													m_vGlowColor = _float4(1.f, 1.f, 1.f, 1.f);
	// �ܰ��� ������ ����
	_float4													m_vEdgeGlowColor = _float4(0.f, 0.f, 0.f, 0.f);
	// �뽬 ������ ���� ( �뽬 ������ ĳ���͵��� �����Ӱ� �ִϸ������� Tick�� �����. )
	_bool													m_bIsSuccessDash = false;
	// Object Motion Blur
	_bool													m_bObjectMotionBlur = false;
	_bool													m_bActiveCutScene = false;
	// �뽬���� �浹 �� ��������� ���⺤��
	_float4													m_vDashReflectDir = _float4(0.f, 0.f, 0.f, 0.f);
	_float													m_fDashReflectDistance = 30.f;
	// ���� ����Ʈ�� �����ϱ� ���� ����
	CGameObject* m_pAwakeEffect = nullptr;
	// ���� ����Ʈ ȣ�� Ÿ�̸�
	_float m_fAwakeEffectTimer = 0.f;

	// ���� �� ����Ʈ
	map<const _tchar*, vector<const _tchar*>>				m_SoundList;

	_bool													m_bMotionBlur = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END