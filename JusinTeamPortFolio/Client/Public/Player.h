#pragma once
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CController;
class CAnimator;
class CAnimationClip;
END

BEGIN(Client)

class CPlayer : public CCharacter
{
public:
	typedef enum EControllerType {
		CONTROLLER_AI, CONTROLLER_KEYBOARD, CONTROLLER_GAMEPAD, CONTROLLER_END
	}CONTROLLER_TYPE;
	typedef struct tagPlayerData
	{
		EControllerType eControllerType = CONTROLLER_END;
	}PLAYERDATA;

protected:
	explicit CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CPlayer(const CPlayer& _rhs);
	virtual ~CPlayer() = default;

public:
	CTransform* Get_TargetTransform() const { return m_pTargetTransform; }
	class CSword* Get_Weapon() const { return m_pSword; }

	// ���� �����ܿ��� ���� Ʈ�������� �޾� Ÿ�� Ʈ���������� �����ϴ� �Լ� 
public:
	void Set_TargetTransform(CTransform* _TargetTransform) { m_pTargetTransform = _TargetTransform; };

public:
	virtual HRESULT		NativeConstruct_Prototype() override;
	// Ŭ���� �� Player Data�� ���� �־�� �Ѵ�!!
	virtual HRESULT		NativeConstruct_Clone(void* pArg) override;
	virtual void		Tick(const _double& _dTimeDelta) override;
	virtual void		Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	// Succes Dash
	void							Success_Dash();
	// Success Dash
	void							Check_SuccessDash();
	// Damaged
	void						Damaged(CPlayer* _pPlayer);
	// Coll Tick
	virtual void				Coll_Tick(_matrix _WorldMatrix) override;
public:
	// ������ ������ ������ �޾Ƽ� �ش� ������ ���߽� �̷������ϴ� �ൿ���� �����մϴ�. ( �ش� �Լ��� �ѹ��� ȣ����. )
	virtual void	Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharacter) override;
	// Play End Cut Scene 
	virtual void Play_EndCutScene(const _bool& _bIsAI = false);
protected:
	// Coll Init
	virtual void				Coll_Init() override;
	// Set Up Constant
	virtual HRESULT		SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT		SetUp_Components() override;
protected:
	// �ǰ� �������� ó��
	virtual void		Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager);
	// �ǰ� �������� ����Ʈ ó��
	virtual void		Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter);
	// SetUp AI Controller
	virtual HRESULT		SetUp_Controller(const CONTROLLER_TYPE& _eType);
	// SetUp Weapon
	virtual HRESULT		SetUp_Weapon(FILEDATA* _pFileData, const _wstring& _strModelProtoName, const _string& _strModelStartBone,
		const _string& _strModelEndBone);
	// SetUp Sheath
	virtual HRESULT		SetUp_Sheath(FILEDATA* _pFileData, const _wstring& _strModelProtoName);
	// Datas
	virtual void	Fill_Datas();
	// Chnage State
	virtual void				Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Animation Controller State 
	virtual void				Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas);

	// Trail Ȱ��ȭ ��Ʈ��
	virtual void Control_Trail_Active();
	// ImGui ����
	virtual void Extern_ImGuiClass() override;

protected:
	// Skill 1
	virtual void	Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 2 
	virtual void	Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 3
	virtual void	Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Move Normal
	virtual void	Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack
	virtual void	Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Upper
	virtual void	Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Down
	virtual void	Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Awake
	virtual void	Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Ultimae
	virtual void	Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Charge Attack
	virtual void	Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Step Back
	virtual void	Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Ground
	virtual void	Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Air
	virtual void	Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Jump
	virtual void	Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// ����
	virtual void	Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// ������
	virtual void	Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// �Ϲ����� Hit�� ��쿡 ���
	virtual void	Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// ���߿� ���� Hit ���� ��쿡 ���
	virtual void	Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Upper�¾Ƽ� AIR_DOWN ���°� �ưų� �ϴ� ��쿡 ���
	virtual void	Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Strike ( ���߿��� STRIKE ���ݿ� ���� ��쿡 ��� )
	virtual void	Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Bound �������� ����ó��
	virtual void	Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// BLOW �������� ����ó�� ( ��ų� �¾Ƽ� �ϴÿ� �� �ߴ� ��� �����. )
	virtual void	Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;

public:
	// ������ ������ ������ �޾Ƽ� �ش� ������ ���߽� �̷������ϴ� �ൿ���� �����մϴ�. ( �ش� �Լ��� �ѹ��� ȣ����. )
	virtual void	Check_Attack_Type(_string _strHitType) override;
protected:
	// Upper ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Attack_Upper() override;
	// Down ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Attack_Down() override;
	// ��ų 1 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_1() override;
	// ��ų 2 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_2() override;
	// ��ų 3 ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_SKILL_3() override;
	// ��� ���ݿ� �������� �� ȣ��ȴ�.
	virtual void	Success_Throw() override;
	// ���� �߰� ���ݿ� �������� �� ȣ��ȴ�. ( �ƾ� ���� )
	virtual void	Success_Attack_Awake() override;
	// ���� �õ��⸦ ������ �� ȣ��ȴ�. ( �ƾ� ���� )
	virtual void	Success_Ultimate() override;



protected:
	// �ʿ� ���� �׺���̼��� �����մϴ�.
	HRESULT Set_BattleMap_Navigation();

protected:
#ifdef _DEBUG
protected:
	virtual void				Coll_Render() override;
	virtual void				Coll_Renderer() override;
#endif
protected:
	// ���ߴ뽬 Ʈ���� �Է�
	_bool		Input_AirDash_Trigger(const _string& _strTrigger);
	// �Ϲ� �뽬 Ʈ���� �Է�
	_bool		Input_Dash_Trigger(const _string& _strTrigger);

protected:
	void								Update_TargetTransform();
protected:
	// Enemy Type
	_uint							Get_EnemyType();
	// Enemy Dot
	_float							Get_EnemyToPlayerDot();
protected:
	// ������ ������ �޾ƿɴϴ�.
	_vector							Get_MoveDirection();
	// �ڽ����κ��� ���� ��ü������ Y���� ������ ���� ���͸� �����մϴ�.
	_vector							Get_DirToTarget();
	// ����� ������ ���� Ʈ���Ÿ� �޾ƿɴϴ�.
	_string							Get_StepDir();
	// ������ ���⺰ Look�� �����մϴ�.
	void							Set_StepLook(const _string& _strTrigger);

protected:
	// ��°������� ����� �� �ִ��� üũ�Ѵ�.
	_bool							Check_ChargeValue();
protected:
	// Sword
	class CSword* m_pSword = nullptr;
	// Sheath
	class CSheath* m_pSheath = nullptr;
	// Prev Collision
	class CCollider* m_pPrevCollider = nullptr;;
	// TargetTransform
	CTransform* m_pTargetTransform = nullptr;
	// Controller Type 
	CONTROLLER_TYPE						m_eControllerType = CONTROLLER_END;
	// Enemy State
	_string								m_strEnemyState = "";
	// Animation Step Clip
	CAnimationClip* m_pAnimStepClip = nullptr;
	// Dash Collision
	_bool								m_bDashCollision = false;
	// Charge Acc
	CUSTIMER							m_tChargeTimer = CUSTIMER(3.f);
	VOIDDATAS					m_InsertVoidDatas;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};
END