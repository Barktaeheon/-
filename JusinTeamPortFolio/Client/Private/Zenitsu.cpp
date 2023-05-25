#include "Zenitsu.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Sword.h"
#include "Zenitsu_KeyBoard_Controller.h"
#include "Zenitsu_GamePad_Controller.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "Battle_Manager.h"

#include "Level_Story.h"
#include "CutSceneModul.h"
#include "CutScene_Manager.h"

CZenitsu::CZenitsu(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPlayer(_pDevice, _pContext)
{
}

CZenitsu::CZenitsu(const CZenitsu& _rhs)
	: CPlayer(_rhs)
{
}

HRESULT CZenitsu::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CZenitsu::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Animator(L"Zenitsu", m_pModelCom)))
		return E_FAIL;

	// ��Ʈ�ѷ� ����
	PLAYERDATA stPlayerData = *static_cast<PLAYERDATA*>(pArg);
	if (FAILED(SetUp_Controller(stPlayerData.eControllerType)))
		return E_FAIL;

	// �׺���̼� ����
	if (nullptr != m_pTransformCom && LEVEL_STORY == g_iCurrentLevel)
	{
		if (false == m_pTransformCom->Update_Navigation_Cell(XMVectorSet(-760.f, 175.7f, 351.f, 1.f)))
			return E_FAIL;
	}
	else if(nullptr != m_pTransformCom)
	{
		if (false == m_pTransformCom->Update_Navigation_Cell(XMVectorSet(0.f, 0.f, -50.f, 0.f)))
			return E_FAIL;
	}

	// ���� ����
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		PATHS Path = {L"Model", L"Item", L"Zenitsu", L"FBX_Converter", L"Data"};
		FILEGROUP* pFolder = pGameInstance->Find_Folder(&Path);
		{
			RETURN_IFNOT(nullptr == pFolder, E_FAIL);
		}
		{
			FILEDATA* pData = pFolder->FindData(L"Zenitsu_Sword");
			if (FAILED(SetUp_Weapon(pData, PROTO_COMP_MODEL_ZENITSU_SWORD, "RootNode", "RootNode")))
				return E_FAIL;
		}
		{
			FILEDATA* pData = pFolder->FindData(L"Zenitsu_Sheath");
			if (FAILED(SetUp_Sheath(pData, PROTO_COMP_MODEL_ZENITSU_SHEATH)))
				return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	// ������ ���� ����
	m_vGlowColor = _float4(0.7f, 0.9f, 0.6f, 0.8f);
	m_vEdgeGlowColor = _float4(0.7f, 0.9f, 0.6f, 0.f);
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			PATHS Path = { L"CutScene",  L"Data" };
			FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
			if (nullptr != pFileGroup)
			{
				Add_CutScene(pFileGroup->FindData(L"Zenitsu_Throw"));
				Add_CutScene(pFileGroup->FindData(L"Zenitsu_AtkSk"));
				Add_CutScene(pFileGroup->FindData(L"Zenitsu_Battle_End"));
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	m_eCharType = CHAR_TYPE::ZENITSU;

	return S_OK;
}

void CZenitsu::Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && !CBattle_Manager::Get_Instance()->IsStoryBattle())
		return;

    // ����� ���� ����Ʈ ȣ��
    if (m_tBattleData.bIsAwake)
    {
        m_fAwakeEffectTimer += (_float)_dTimeDelta;

        if (0.2f < m_fAwakeEffectTimer)
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            m_fAwakeEffectTimer = 0.f;


            CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW("StepParticle").c_str());
            if (nullptr != pEffect)
            {
                pEffect->Get_Transform()->Bind_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
                static_cast<CEffectObject*>(pEffect)->Set_Play(true);
            }

            RELEASE_INSTANCE(CGameInstance);
        }
    }

	__super::Tick(_dTimeDelta);
}

void CZenitsu::Late_Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && !CBattle_Manager::Get_Instance()->IsStoryBattle())
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CZenitsu::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CZenitsu::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
	__super::SetUp_Controller(_eType);
	if (CONTROLLER_GAMEPAD == _eType)
	{
		// ��Ʈ�ѷ��� �����Ѵ�.
		m_pController = CZenitsu_GamePad_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	else if (CONTROLLER_KEYBOARD == _eType)
	{	// ��Ʈ�ѷ��� �����Ѵ�.
		m_pController = CZenitsu_KeyBoard_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	m_eControllerType = _eType;
	return S_OK;
}

HRESULT CZenitsu::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZenitsu::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_PLAYER_ZENITSU, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CZenitsu::Fill_Datas()
{
	m_InsertVoidDatas.clear();
	m_InsertVoidDatas.reserve(1000);
	// Current State
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_AnimationType());
	// TransitionList
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_ActiveTransitionList());
	// Current AnimCLip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_CurrentAnimationClip());
	// Prev Animation Clip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_PrevAnimationClip());
}

// ���¿� ���� �������� �����Ѵ�.
void CZenitsu::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Behavior_State(_dTimeDelta, _Datas);
}

void CZenitsu::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    RETURN_IFNOT(_Datas.size() <= 0, ;);

    // �̹� �����ӿ� ���� Ʈ���� ����Ʈ
    vector<_string> triggerList = *static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
    // �����̴� ���Ⱚ
    m_vDirection = *static_cast<_float3*>(_Datas[OUTDATA::OUT_DIRECTION]);

    // IDLE�� �ƴ϶�� ���� Ÿ�̸Ӹ� �����Ѵ�.
    if ("IDLE" != m_strAnimType)
        m_tChargeTimer.Reset_Timer();

    if (nullptr != m_pAnimator)
    {
        m_strAnimType = m_pAnimator->Get_AnimationType();

        for (auto& trigger : triggerList)
        {
            // �⺻ ������ ��
            if ("IDLE" == m_strAnimType)
            {
                // ���Ⱚ�� 0�� �ƴ϶�� MOVE ���·� �����Ѵ�.
                if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                {
                    if ("MOVE" == trigger)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            // �̵��������� �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
                            break;
                        }
                    }
                }
                else if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();

                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;

                        if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                            m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                        break;
                    }
                }
                // Ʈ���Ű� ������ ��
                else if ("GUARD" == trigger)
                {
                    // ���� Ʈ���Ű� �����ϸ� Look���� Target�� �ٶ󺻴�.
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ������ �ٶ󺻴�.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // �뽬 ����
                else if ("DASH" == trigger)
                {
                    if (true == Input_AirDash_Trigger("AIR_DASH"))
                        break;
                    else
                    {
                        if (true == Input_Dash_Trigger(trigger))
                            break;
                    }
                }
                else if ("STEP" == trigger)
                {
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        // ���ܽ� Look�� �����մϴ�.
                        m_strCurrentTrigger = trigger;
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                // SKILL_1 ����
                else if ("SKILL_1" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            // ������ �ٶ󺻴�.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ���� ����
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ���� ����
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ������ �ٶ󺻴�.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("CHARGE_ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();

                        // ������ �ٶ󺻴�.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        m_vBehaviorDir = XMVector3Normalize(Get_DirToTarget());
                        break;
                    }
                }
                // ��ų3 �������϶� ��ų3�� ������ �����ϴ�.
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            // ������ �ٶ󺻴�.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }

                // ������ �Ѿ��.
                // ���Ⱚ�� 0�� �ƴ϶�� MOVE ���·� �����Ѵ�.
                if (CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                {
                    if (false == m_tBattleData.Is_CheckMaxCharge() && true == m_tBattleData.tChargeTimer.bPass)
                    {
                        if (m_tChargeTimer.Is_Over(_dTimeDelta))
                        {
                            if (true == m_pAnimator->Input_Trigger("CHARGE"))
                                m_tChargeTimer.Reset_Timer();
                        }
                    }
                }
            }
            // ���� ������� �ִϸ��̼� Ÿ���� MOVE�� ���
            else if ("MOVE" == m_strAnimType)
            {
                // �����̰��ִٸ� ���������ϴ� ������ �Ĵٺ���.
                if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
                    m_pTransformCom->LookAt(Get_MoveDirection());

                // �޸��⸦ �����.
                if ("MOVE_END" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_vBehaviorDir = m_pTransformCom->Get_Look();//m_pTransformCom->LookAt(m_vDirection.Get_Vector());
                        break;
                    }
                }
                else if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("STEP" == trigger)
                {
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        // ���ܽ� Look�� �����մϴ�.
                        m_strCurrentTrigger = trigger;
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ����
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ��ų2 ����
                else if ("SKILL_2" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            // ������ �ٶ󺻴�.
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ��ų3 ����
                else if ("SKILL_3" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ������ �ٶ󺻴�.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ����
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ������ �ٶ󺻴�.
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // ���� ������� �ִϸ��̼��� Ÿ���� ������ ���
            else if ("ATTACK" == m_strAnimType)
            {
                // ��� ���ݽ�
                if (trigger == "ATTACK_UPPER")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // �ϴ� ���ݽ�
                else if (trigger == "ATTACK_DOWN")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // �Ϲ� ���ݽ�
                else if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ���� ��
                else if (trigger == "ATTACK_AWAKE")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��°������� �������� �ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ��
                else if (trigger == "JUMP")
                {
                    // ��°������� 20 �Ҹ��� �� �ִٸ� ������ �����Ѵ�.
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            m_strCurrentTrigger = trigger;

                            if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                                m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                            break;
                        }
                    }
                }
                // �뽬�� ���
                else if (trigger == "DASH")
                {
                    if (true == Input_AirDash_Trigger("AIR_DASH"))
                        break;

                    if (true == Check_ChargeValue())
                    {
                        if (true == Input_Dash_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            break;
                        }
                    }
                }
                else if (trigger == "STEP")
                {
                    // ��°������� 20 �Ҹ��� �� �ִٸ� ������ �����Ѵ�.
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);

                            m_strCurrentTrigger = trigger;
                            // ���ܽ� Look�� �����մϴ�.
                            Set_StepLook(strStepTrigger);
                            break;
                        }
                    }
                }
                // ���� ����
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ��ų2 ����
                else if ("SKILL_2" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ��ų3 ����
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ���� ����
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // ���� ���ݽ� �� �� �ִ� �ൿ��
            else if ("AIR_ATTACK" == m_strAnimType)
            {
                // �Ϲ� ���ݽ�
                if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if (trigger == "DASH")
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == Input_AirDash_Trigger("AIR_DASH"))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            break;
                        }
                    }
                }
                else if (trigger == "STEP")
                {
                    // ��°������� 20 �Ҹ��� �� �ִٸ� ������ �����Ѵ�.
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // ���ܽ� Look�� �����մϴ�.
                            Set_StepLook(strStepTrigger);
                            break;
                        }
                    }
                }
            }
            // ���� ������� �ִϸ��̼� Ÿ���� ������ ���
            else if ("STEP" == m_strAnimType)
            {
                if (trigger == "STEP")
                {
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ���ܽ� Look�� �����մϴ�.
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
            }
            // ���� ������� �ִϸ��̼� Ÿ���� ���� ������ ���
            else if ("CHARGE_ATTACK" == m_strAnimType)
            {
                if (trigger == "CHARGE_ATTACK_END")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // ���� ������� �ִϸ��̼��� Ÿ���� ������ ���
            else if ("JUMP" == m_strAnimType)
            {
                if (trigger == "ATTACK")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // ��� ȸ���ð��� �ʱ�ȭ�Ѵ�.
                        m_tBattleData.tChargeTimer.Reset_Timer();
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if (trigger == "STEP")
                {
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ���ܽ� Look�� �����մϴ�.
                        Set_StepLook(strStepTrigger);
                        m_dFallTimeAcc = 0.0;
                        break;
                    }
                }
                else if (trigger == "DASH")
                {
                    // ������ ���� �����뽬�� ������ ��� ���� �����뽬�� ����Ѵ�.
                    // ���� ���϶��� �Ϲ� �뽬�� ��� �Ұ����ϴ�.
                    if (true == Input_AirDash_Trigger("AIR_DASH"))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // ���� ������� �ִϸ��̼��� ������ ���
            else if ("GUARD" == m_strAnimType)
            {
                if (trigger == "GUARD_END")
                {
                    m_strCurrentTrigger = trigger;
                    m_pAnimator->Input_Trigger(trigger);
                    break;
                }
                // ���ĳ���
                else if (trigger == "PUSH")
                {
                    // ��ġ�⸦ �����ϸ� ������ �ٶ󺻴�.
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // �ݰ�
                else if (trigger == "PARING")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ��ų3 ���
                else if (trigger == "SKILL_3")
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ���
                else if (trigger == "THROW")
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // �뽬
                else if (trigger == "DASH")
                {
                    if (true == Input_Dash_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
            }
            // ���� ������� �ִϸ��̼� Ÿ���� DASH�� ���
            else if ("DASH" == m_strAnimType)
            {
                // ���� ����
                if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ����
                else if ("STEP" == trigger)
                {
                    // ������ ���⺰ Ʈ���Ÿ� �޾ƿ´�.
                    _string strStepTrigger = Get_StepDir();

                    if (true == m_pAnimator->Input_Trigger(strStepTrigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ���ܽ� Look�� �����մϴ�.
                        Set_StepLook(strStepTrigger);
                        break;
                    }
                }
                // ��� ����
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                // ���� ����
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // �뽬���� ������ ����ϸ� �뽬�� �ϴ� �������� ������ �Ѵ�.
                        m_vBehaviorDir = m_pTransformCom->Get_Look();
                        break;
                    }
                }
                // ���� ����
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                // ���� ����
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // �ٿ� �� ���
            else if ("DOWN" == m_strAnimType)
            {
                // �ٿ��� �� �����̸� ROLL�� ��ȯ�Ѵ�.
                if ("MOVE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger("ROLL"))
                    {
                        m_strCurrentTrigger = trigger;
                        break;
                    }
                }
                // ����
                else if ("PASSIVE" == trigger)
                {
                    // ���� �ÿ��� �������� �����ؾ��Ѵ� ( ������ �̵� �ؾ��Ѵ� )
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            else if ("CHARGE" == m_strAnimType)
            {
                // ���� ����
                if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("MOVE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        // �̵��������� �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector());
                        break;
                    }
                }
                else if ("JUMP" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;

                        if (!CMethod::Is_Vector_Zero(m_vBehaviorDir.Get_Vector()))
                            m_vBehaviorDir = _float4(0.f, 0.f, 0.f, 0.f);
                        break;
                    }
                }
                else if ("GUARD" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
                else if ("AWAKE" == trigger)
                {
                    if (m_tBattleData.iUltimateCount > 0)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Active_AwakeMode();
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("SKILL_1" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("SKILL_3" == trigger)
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                            m_strCurrentTrigger = trigger;
                            // ������ �ٶ󺻴�.
                            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                            break;
                        }
                    }
                }
                else if ("ULTIMATE" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
            // ��ų 3 ������϶� ���� ����
            else if ("SKILL_3" == m_strAnimType)
            {
                if ("ATTACK" == trigger)
                {
                    if (true == m_pAnimator->Input_Trigger(trigger))
                    {
                        m_strCurrentTrigger = trigger;
                        // ������ �ٶ󺻴�.
                        m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                        break;
                    }
                }
            }
        }
    }

	//__super::Animation_Control_State(_dTimeDelta, _Datas);
}

void CZenitsu::Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Skill_1(_dTimeDelta, _Datas);
}
void CZenitsu::Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Skill_2(_dTimeDelta, _Datas);
}
void CZenitsu::Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Skill_3(_dTimeDelta, _Datas);
}

void CZenitsu::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Move_State(_dTimeDelta, _Datas);
}

// ���߿� ���� �����ϴ� ��� �Ʒ��� �����ش�.
void CZenitsu::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (false == m_bActive)
		return;

	// �����Ӵ� �̵����� ����Ѵ�.
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 0.f);

	// ���� ��ġ�� �׺� Y������ ������ �׺� Y�� ����
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("FALL_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y�� ���� ������ Y��, XZ�� �׺� ���� ����
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
}

void CZenitsu::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Upper_State(_dTimeDelta, _Datas);
}

void CZenitsu::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Down_State(_dTimeDelta, _Datas);
}

void CZenitsu::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// �ݶ��̴��� ���Ⱚ�� �޾ƿɴϴ�.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// �ǰ� ȿ������ �Ѿ�� PushedForce; 
	_float fDistance = 50.f;
	// �ǰ� ȿ������ �Ѿ�� MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// �´� �ִϸ��̼� ���� �ð�
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// �з��� ���⺤��
	_vector vDir = m_pTransformCom->Get_Look();

	// ����ؼ� ���� �����Ӵ� �̵��Ÿ�
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vDir, fDistance, 0.f, 0.f, (_float)0.5f);

	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;

	// �׺���̼��� Y�� �����Ѵ�.
	m_pTransformCom->Move_Pos(vPlayerPos);
	m_dFallTimeAcc = 0.f;
}

void CZenitsu::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// �ݶ��̴��� ���Ⱚ�� �޾ƿɴϴ�.
	_vector vColliderDir = m_pAnimator->Get_ColliderDir();
	// �ǰ� ȿ������ �Ѿ�� PushedForce; 
	_float fDistance = 40.f;
	// �ǰ� ȿ������ �Ѿ�� MaxHeight
	_float fMaxHeight = m_tKnockBackInfo.fHeight;
	// �´� �ִϸ��̼� ���� �ð�
	_double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
	// �з��� ���⺤��
	_vector vDir = m_pTransformCom->Get_Look();

	// ����ؼ� ���� �����Ӵ� �̵��Ÿ�
	_vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vDir, fDistance, 0.f, 0.f, (_float)0.3f);

	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;

	// �׺���̼��� Y�� �����Ѵ�.
	m_pTransformCom->Move_Pos(vPlayerPos);
	m_dFallTimeAcc = 0.f;
}

// ������
void CZenitsu::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Charge_Attack_State(_dTimeDelta, _Datas);
}

// ���߿� ���� ������ ��� �Ʒ��� �����ش�.
void CZenitsu::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Step_State(_dTimeDelta, _Datas);
}
// ���� �߰� �뽬 ����
void CZenitsu::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Dash_Ground_State(_dTimeDelta, _Datas);
}
// ���� �߰� �뽬 ����
void CZenitsu::Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Dash_Air_State(_dTimeDelta, _Datas);
}
void CZenitsu::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Jump_State(_dTimeDelta, _Datas);
}
void CZenitsu::Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	// �ڽ����κ��� ������ �ٶ󺸴� ���⺤���� �ݴ�������� �����Ѵ�.
	__super::Passive_State(_dTimeDelta, _Datas);
}
void CZenitsu::Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Roll_State(_dTimeDelta, _Datas);
}

// ���󿡼� Hit�� ��� ( ���̰� ���� )
void CZenitsu::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Hit_State(_dTimeDelta, _Datas);
}
void CZenitsu::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Hit_State(_dTimeDelta, _Datas);
}

void CZenitsu::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Down_State(_dTimeDelta, _Datas);
}
void CZenitsu::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Strike_State(_dTimeDelta, _Datas);
}
void CZenitsu::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Bound_State(_dTimeDelta, _Datas);
}

HRESULT CZenitsu::Initialize_Sound()
{
	__super::Initialize_Sound();

	// HIT_VOICE
	{
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit01"));
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit02"));
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit03"));
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit04"));
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit05"));
		Add_Sound(TEXT("HIT_VOICE"), TEXT("Zenitsu_Hit06"));
	}
	// HIT_S_VOICE
	{
		Add_Sound(TEXT("HIT_S_VOICE"), TEXT("Zenitsu_HitStrong01"));
		Add_Sound(TEXT("HIT_S_VOICE"), TEXT("Zenitsu_HitStrong02"));
		Add_Sound(TEXT("HIT_S_VOICE"), TEXT("Zenitsu_HitStrong03"));
		Add_Sound(TEXT("HIT_S_VOICE"), TEXT("Zenitsu_HitUpper01"));
	}

	return S_OK;
}

void CZenitsu::Check_Attack_Type(_string _strHitType)
{
	__super::Check_Attack_Type(_strHitType);

	if ("BLOW" == _strHitType)
	{
		if ("SKILL_1" == m_strAnimType)
			Success_SKILL_1();
		else if ("SKILL_2" == m_strAnimType)
			Success_SKILL_2();
	}
}

void CZenitsu::Success_Attack_Upper()
{
	// ���ݿ� ������ �ϴ÷� �ö󰣴�.
	if ("ATTACK" == m_strAnimType)
	{
		_vector vJump = XMVectorZero();
		vJump = XMVectorSetY(vJump, m_pTargetTransform->Get_PosF3().y * 2.f);

		m_pTransformCom->Move_Pos_NotNavigateY(m_pTransformCom->Get_Pos() + vJump);
	}
}

void CZenitsu::Success_Attack_Down()
{
}

void CZenitsu::Success_SKILL_1()
{
}

void CZenitsu::Success_SKILL_2()
{
}

void CZenitsu::Success_SKILL_3()
{
}

void CZenitsu::Success_Throw()
{
	CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
	{
		pInstance->Set_CutSceneModul(m_CutSceneModulMap["ZENITSU_THROW"]);
	}
	RELEASE_INSTANCE(CCutScene_Manager);
}

void CZenitsu::Success_Attack_Awake()
{
	CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
	{
		pInstance->Set_CutSceneModul(m_CutSceneModulMap["ZENITSU_ATKSK"]);
	}
	RELEASE_INSTANCE(CCutScene_Manager);
}

void CZenitsu::Success_Ultimate()
{
}

CZenitsu* CZenitsu::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CZenitsu* pInstance = new CZenitsu(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZenitsu::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CZenitsu::Clone(void* pArg)
{
	CZenitsu* pInstance = new CZenitsu(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CZenitsu::Clone To Failed");
#endif
	}
	return pInstance;
}

void CZenitsu::Free()
{
	__super::Free();
}
