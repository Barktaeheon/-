#pragma once
#include "Boss.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CTeOni final : public CBoss
{
public: 
	enum STATE : _ulong
	{
		STATE_IDLE = 1,
		STATE_WALK = 2, // STOP
		STATE_STEP = 3,
		STATE_ATTACK = 4, // ������ �Ÿ��Ǵ��ؼ�  S, L  ������ �־��ٰ�
		STATE_GUARD = 5,
		STATE_HIT = 6,

		STATE_FORWARD = 100,
		STATE_RIGHT = 200,
		STATE_LEFT = 300,
		STATE_BACK = 400,
		STATE_STOP = 500,
		STATE_FF = 600, 

		STATE_ATK_S_THROW = 1000,
		STATE_ATK_S_JUMP = 2000,
		STATE_ATK_S_SWING = 3000,
		STATE_ATK_S_ONEHAND = 4000,

		STATE_ATK_L_STONE = 5000,
		STATE_ATK_L_RUSH = 6000, // STOP
		STATE_ATK_L_JUMP = 7000,

		STATE_IDLE_BELLOW = 10000, // ���� �ǰ� ������� �¾����� �̰� �ϸ鼭 �÷��̾� �о�� 
		STATE_IDLE_SMILE = 20000, // �÷��̾� �ǰݻ����϶� �������� ���� 
		STATE_IDLE_DAMAGE = 30000,
		STATE_IDLE_DEAD = 40000,

		STATE_JUDGE = 100,
		STATE_END = 0
	};
protected:
	explicit CTeOni(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CTeOni(const CTeOni& _rhs);
	virtual ~CTeOni(void) = default;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual void Extern_ImGuiClass(void);
private:
	virtual HRESULT SetUp_ConstantTable(void) override;
	virtual HRESULT SetUp_Components(void) override;
private:
	virtual void Move_HitCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) override;
	virtual void Move_StepCheck(const _double& _dTimeDelta, CGameInstance* _pGameInstance) override;
	virtual void Give_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) override;
	virtual void Animation_Control_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance) override;
	virtual void Behavior_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) override;
	virtual void Init_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance) override;
private:
	void Idel_State(const _double& _dTimeDetla, CGameInstance* _pGameInstance, const _uint& _iOrder);
	void Walk_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder);
	void Step_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder);
	void Attack_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance, const _uint& _iOrder);
private:
	virtual void Jump(const _double& _dTimeDelta, const _string& _strTrigger) override;
protected:
	virtual HRESULT Initialize_Sound(void) override;
public:
	static CTeOni* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
private: 
	//  L_JIMP ���ݿ� �ʿ��� �ʱ�  ����, �Ÿ��� 
	_bool m_bJumpAtkCheck = false;
	_vector m_vJumpDir{};
	_float m_fJumpLength = 0.f;
	// ������ ���� ���� ������ üũ �Һ��� 
	_uint m_iRLCheck = 0;
	_bool m_bTimeCheck[2] = { false , false};
	_bool m_bCreateCheck[2] = { false , false };

	_bool m_bInit = false;
	_bool bCheck = false;
	CTexture* m_pDissolveTexture = nullptr;

	_float m_fDissolveAmount = 0.f;
	_bool m_bDissolve = false;
	_bool m_bRLCheck = false;
	_double m_dDieTime = 0.0;
	_bool m_bFadeOutCheck = false;
};
END

// awake ī�޶� ���� 