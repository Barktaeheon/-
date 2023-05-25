#pragma once
#include "Player.h"

BEGIN(Client)
class CAIController_Sabito;

class CSabito final : public CPlayer
{
public:
	enum STATE : _ulong
	{
		STATE_IDLE = 1,
		STATE_MOVE = 2,
		STATE_ATTACK = 3,
		STATE_DASH = 4,
		STATE_STEP = 5,
		STATE_GUARD = 6,
		STATE_JUMP = 7,
		STATE_CHARGE = 8,
		STATE_HIT = 9,

		STATE_RIGHT = 100,
		STATE_LEFT = 200,
		STATE_BACK = 300,
		STATE_FORWARD = 400,
		STATE_AIR = 500,
		STATE_NOTMOVE = 600,

		STATE_ATK_NORMAL = 1000,
		STATE_ATK_AIR = 2000,
		STATE_ATK_DOWN = 3000,
		STATE_ATK_DRAGON = 4000,
		STATE_ATK_MORNING = 5000,
		STATE_ATK_TYPHOON = 6000,
		STATE_ATK_STRONG = 7000,
		STATE_ATK_JUMPATTACK = 8000,
		STATE_ATK_CATCH = 9000,
		STATE_ATK_END = 9900,

		STATE_DASH_GROUND = 11000,
		STATE_DASH_AIR = 21000,
		STATE_DASH_END = 31000,

		STATE_GUARD_JUST = 12000,
		STATE_GUARD_CANCEL = 22000,
		STATE_GUARD_ATTACK = 23000,

		STATE_CHARGE_CONTINUE = 13000,
		STATE_CHARGE_END = 23000,

		STATE_ATK_CATCHSUCCESS = 30000,

		STATE_STEP_TWICE = 10000000,
		STATE_RETURN_IDLE = 20000000,

		STATE_JUDGE = 100,
		STATE_END = 0
	};

protected:
	explicit CSabito(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CSabito(const CSabito& _rhs);
	virtual ~CSabito() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
protected:
	// SetUp AI Controller
	virtual HRESULT SetUp_Controller(const CONTROLLER_TYPE& _eType);
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
protected:
	// Datas
	virtual void		Fill_Datas() override;
	// Chnage State
	virtual void			Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Animation Controller State 
	void					Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Trail Active Control
	virtual void			Control_Trail_Active();
protected:
	// Skill 1
	virtual void					Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 2 
	virtual void					Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 3
	virtual void					Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Move Normal
	virtual void					Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack
	virtual void					Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Upper
	virtual void					Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Down
	virtual void					Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Awake
	virtual void					Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Ultimae
	virtual void					Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Charge Attack				
	virtual void					Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Step Back
	virtual void					Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Ground
	virtual void					Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Jump
	virtual void					Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// �Ϲ����� Hit�� ��쿡 ���
	virtual void					Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// ���߿� ���� Hit ���� ��쿡 ���
	virtual void					Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Upper�¾Ƽ� AIR_DOWN ���°� �ưų� �ϴ� ��쿡 ���
	virtual void					Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Strike ( ���߿��� STRIKE ���ݿ� ���� ��쿡 ��� )
	virtual void					Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Bound �������� ����ó��
	virtual void					Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;

protected:
	virtual HRESULT Initialize_Sound() override;

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
	// Start Attack 
	_bool					m_bStartAttack = false;
	PLAYERDATA				m_tPlayerData;
public:
	static CSabito* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END