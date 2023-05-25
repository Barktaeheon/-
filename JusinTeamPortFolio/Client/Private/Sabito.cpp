#include "Sabito.h"
#include "AIController_Sabito.h"
#include "GameInstance.h"
#include "AnimationClip.h"
#include "Sabito_KeyBoard_Controller.h"
#include "Sabito_GamePad_Controller.h"
#include "CutScene_Manager.h"
#include "Sword.h"
#include "Battle_Manager.h"

CSabito::CSabito(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayer(_pDevice, _pContext)
{
}

CSabito::CSabito(const CSabito& _rhs)
	: CPlayer(_rhs)
{
}

HRESULT CSabito::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSabito::NativeConstruct_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

    m_tPlayerData = *static_cast<PLAYERDATA*>(pArg);

	if (CONTROLLER_AI == m_tPlayerData.eControllerType)
	{        
		if (FAILED(SetUp_Animator(L"AI_Sabito", m_pModelCom)))
			return E_FAIL;
	}
	else
	{
        if (nullptr != m_pTransformCom)
        {
            if (false == m_pTransformCom->Update_Navigation_Cell(XMVectorSet(0.f, 0.f, -50.f, 0.f)))
                return E_FAIL;
        }

        if (FAILED(SetUp_Animator(L"Sabito", m_pModelCom)))
            return E_FAIL;
	}

	if (FAILED(SetUp_Controller(m_tPlayerData.eControllerType)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		PATHS Path = { L"Model", L"Item", L"Sabito", L"FBX_Converter", L"Data" };
		FILEGROUP* pFolder = pGameInstance->Find_Folder(&Path);
		{
			RETURN_IFNOT(nullptr == pFolder, E_FAIL);
		}
		FILEDATA* pData = pFolder->FindData(L"Sabito_Sword");
		if (FAILED(SetUp_Weapon(pData, PROTO_COMP_MODEL_SABITO_SWORD, "RootNode", "RootNode")))
			return E_FAIL;

		pData = pFolder->FindData(L"Sabito_Sheath");
		if (FAILED(SetUp_Sheath(pData, PROTO_COMP_MODEL_SABITO_SHEATH)))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	m_vGlowColor = _float4(0.9f, 0.7f, 0.5f, 0.8f);
    m_vEdgeGlowColor = _float4(0.f, 0.f, 0.9f, 0.f);
    {
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			PATHS Path = { L"CutScene",  L"Data" };
			FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
			if (nullptr != pFileGroup)
			{
				Add_CutScene(pFileGroup->FindData(L"Sabito_Throw"));
				Add_CutScene(pFileGroup->FindData(L"Sabito_AtkSk"));
                Add_CutScene(pFileGroup->FindData(L"Sabito_Battle_End"));
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
    m_eCharType = CHAR_TYPE::SABITO;

    

	return S_OK;
}

void CSabito::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);

    // ����� ���� ����Ʈ ȣ��
    if (m_tBattleData.bIsAwake)
    {
        m_fAwakeEffectTimer += (_float)_dTimeDelta;

        if (0.2f < m_fAwakeEffectTimer)
        {
            CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

            m_fAwakeEffectTimer = 0.f;


            CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW("StepParticle_Sab").c_str());
            if (nullptr != pEffect)
            {
                pEffect->Get_Transform()->Bind_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
                static_cast<CEffectObject*>(pEffect)->Set_Play(true);
            }

            RELEASE_INSTANCE(CGameInstance);
        }
    }

}

void CSabito::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CSabito::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CSabito::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
	if (CONTROLLER_AI == _eType)
	{
		m_pController = CAIController_Sabito::Create(m_pDevice, m_pContext, this);
	}
	else if(CONTROLLER_KEYBOARD == _eType)
	{
		m_pController = CSabito_KeyBoard_Controller::Create(m_pDevice, m_pContext, this);
	}
    else
    {
        m_pController = CSabito_GamePad_Controller::Create(m_pDevice, m_pContext, this);
    }
	__super::SetUp_Controller(_eType);
	return S_OK;
}

HRESULT CSabito::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSabito::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_PLAYER_SABITO, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CSabito::Fill_Datas()
{
    m_InsertVoidDatas.clear();
	// Current State
    m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_AnimationType());
	// TransitionList
    m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_ActiveTransitionList());
	// Current AnimCLip
    m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_CurrentAnimationClip());
	// Prev Animation Clip
    m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_PrevAnimationClip());
}

void CSabito::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	RETURN_IFNOT(_Datas.size() <= 0, ;);

    if (CONTROLLER_AI == m_tPlayerData.eControllerType)
    {
        __super::Behavior_State(_dTimeDelta, _Datas);

        if ("SKILL_1" == m_strAnimType || "SKILL_1_AIR" == m_strAnimType)
        {
            Skill_1(_dTimeDelta, _Datas);
        }
    }
    else
    {
        __super::Behavior_State(_dTimeDelta, _Datas);

        if ("SKILL_1" == m_strAnimType || "SKILL_1_AIR" == m_strAnimType)
        {
            Skill_1(_dTimeDelta, _Datas);
        }
    }
}

void CSabito::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (CONTROLLER_AI == m_tPlayerData.eControllerType)
	{
		_uint iOrder = *static_cast<_uint*>(_Datas[OUTDATA::OUT_STATE]);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			if ("Hit" == m_pAnimator->Get_AnimationType())
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_IDLE)
			{
				if ((iOrder) == CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE)
				{
					Input_Trigger("RETURN_IDLE");
				}
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_MOVE)
			{
				Input_Trigger("NORMAL_MOVE");
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_ATTACK)
			{
				if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL)
				{
					Input_Trigger("NORMAL_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_DOWN)
				{
					Input_Trigger("DOWN_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR)
				{
					Input_Trigger("AIR_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING)
				{
					Input_Trigger("MORNING_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON)
				{
					Input_Trigger("TYPHOON_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON)
				{
					Input_Trigger("DRAGON_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG)
				{
					Input_Trigger("STRONG_ATTACK");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_JUMPATTACK)
				{
					Input_Trigger("JUMP_ATTACK_COMBO");
				}
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCH)
				{
					Input_Trigger("CATCH_ATTACK");
				}
                else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCHSUCCESS)
                {
                    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
                    {
                        pInstance->Set_CutSceneModul(m_CutSceneModulMap["SABITO_ATKSK"]);
                    }
                    RELEASE_INSTANCE(CCutScene_Manager);
                }
				else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_ATK_END)
				{
					Input_Trigger("RETURN_IDLE");
				}
                else if ((iOrder) == CSabito::STATE_ATTACK + CSabito::STATE_AIR)
                {
                    Input_Trigger("AIR_SUCCESS");
                }
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_STEP)
			{
				if (iOrder == (CSabito::STATE_STEP + CSabito::STATE_BACK)
					|| iOrder == (CSabito::STATE_STEP + CSabito::STATE_BACK + CSabito::STATE_STEP_TWICE))
				{
					Input_Trigger("STEP_BACK");
				}
				else 	if (iOrder == (CSabito::STATE_STEP + CSabito::STATE_FORWARD)
					|| iOrder == (CSabito::STATE_STEP + CSabito::STATE_FORWARD + CSabito::STATE_STEP_TWICE))
				{
					Input_Trigger("STEP_FRONT");
				}
				else if (iOrder == (CSabito::STATE_STEP + CSabito::STATE_LEFT)
					|| iOrder == (CSabito::STATE_STEP + CSabito::STATE_LEFT + CSabito::STATE_STEP_TWICE))
				{
					Input_Trigger("STEP_LEFT");
				}
				else if (iOrder == (CSabito::STATE_STEP + CSabito::STATE_RIGHT)
					|| iOrder == (CSabito::STATE_STEP + CSabito::STATE_RIGHT + CSabito::STATE_STEP_TWICE))
				{
					Input_Trigger("STEP_RIGHT");
				}
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_GUARD)
			{
				if ((iOrder == CSabito::STATE_GUARD + CSabito::STATE_GUARD_JUST))
				{
					Input_Trigger("GUARD");
				}
				else if ((iOrder == CSabito::STATE_GUARD + CSabito::STATE_GUARD_ATTACK))
				{
					Input_Trigger("GUARD_ADDVANCING");
				}
				else if ((iOrder == CSabito::STATE_GUARD + CSabito::STATE_GUARD_CANCEL))
				{
					Input_Trigger("GUARD_END");
				}
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_DASH)
			{
				if (iOrder == (CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND))
				{
					Input_Trigger("START_DASH");
				}
                else if (iOrder == (CSabito::STATE_DASH + CSabito::STATE_DASH_AIR))
                {
                    if (true == Check_ChargeValue())
                    {
                        if (true == Input_AirDash_Trigger("AIR_DASH"))
                        {
                            m_tBattleData.Decrease_Charge(20.f);
                        }
                    }
                }
				else if (iOrder == (CSabito::STATE_DASH + CSabito::STATE_DASH_END))
				{
					Input_Trigger("SUCCESS_DASH");
				}
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_JUMP)
			{
				Input_Trigger("JUMP");
			}
			else if ((iOrder % CSabito::STATE_JUDGE) == CSabito::STATE_CHARGE)
			{
				if ((iOrder) == CSabito::STATE_CHARGE + CSabito::STATE_CHARGE_CONTINUE)
				{
					if (m_tBattleData.Is_CheckMaxCharge())
					{
						Input_Trigger("CHARGE_END");
					}
					else
					{
						Input_Trigger("CHARGE_STATE");
					}
				}
				else if ((iOrder) == CSabito::STATE_CHARGE + CSabito::STATE_CHARGE_END)
				{
					Input_Trigger("CHARGE_END");
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	else
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
                    else if (trigger == "SKILL_1")
                    {
                        if (true == Check_ChargeValue())
                        {
                            if (true == m_pAnimator->Input_Trigger("SKILL_1_AIR"))
                            {
                                m_tBattleData.Decrease_Charge(20.f);
                                m_strCurrentTrigger = trigger;
                                m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                                break;
                            }
                        }
                    }
                    else if (trigger == "SKILL_2")
                    {
                        if (true == Check_ChargeValue())
                        {
                            if (true == m_pAnimator->Input_Trigger("SKILL_2_AIR"))
                            {
                                m_tBattleData.Decrease_Charge(20.f);
                                m_strCurrentTrigger = trigger;
                                m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
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
                else if ("AIR_STEP" == m_strAnimType)
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
                    else if (trigger == "SKILL_1")
                    {
                        if (true == Check_ChargeValue())
                        {
                            if (true == m_pAnimator->Input_Trigger("SKILL_1_AIR"))
                            {
                                m_tBattleData.Decrease_Charge(20.f);
                                m_strCurrentTrigger = trigger;
                                m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                                break;
                            }
                        }
                    }
                    else if (trigger == "SKILL_2")
                    {
                        if (true == Check_ChargeValue())
                        {
                            if (true == m_pAnimator->Input_Trigger("SKILL_2_AIR"))
                            {
                                m_tBattleData.Decrease_Charge(20.f);
                                m_strCurrentTrigger = trigger;
                                m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
                                break;
                            }
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
            }
        }
	}
}

void CSabito::Control_Trail_Active()
{
    if (nullptr == m_pSword)
        return;

    m_pSword->Set_TrailActive(m_pAnimator->Is_TrailActive());
    if (true == m_pAnimator->Is_TrailActive())
    {
        _float2 vTrailValues = m_pAnimator->Get_TrailValues();
        m_pSword->Update_TrailMatrix(vTrailValues.x, vTrailValues.y);
        m_pSword->Change_Trail_Texture(L"T_e_Skl_Wa_Slash001C");
    }
    else
    {
        m_pSword->Return_Trail_Texture();
    }
}

void CSabito::Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	if (m_strCurrentTrigger == "DRAGON_ATTACK")
	{
		_float3 v3Pos = m_pTransformCom->Get_PosF3();
		if (m_pAnimator->Get_Ratio() <= 0.4f)
		{
			v3Pos.y += (_float)(_dTimeDelta * 4.f * 9.8f);
			_float3 v3Look = m_pTransformCom->Get_Look();
			v3Look.y = 0.f;
			m_pTransformCom->Set_Pos(v3Pos.Get_Vector());
			m_pTransformCom->Translate_DirNotNavigation(v3Pos.Get_Vector(), _dTimeDelta, 0.1f);
		}
		else
		{
			v3Pos.y += (_float)(_dTimeDelta * 4.5f * -9.8f);
			_float3 v3Look = m_pTransformCom->Get_Look();
			v3Look.y = 0.f;
			m_pTransformCom->Set_Pos(v3Pos.Get_Vector() + v3Look.Get_Vector() * 0.1f);

			_float fCurCellYPos = m_pNavigationCom->Get_CurCell()->Compute_Height(XMLoadFloat3(&v3Pos));
			if (fCurCellYPos > v3Pos.y)
			{
				m_pTransformCom->Set_PosF3(_float3(m_pTransformCom->Get_PosF3().x, fCurCellYPos, m_pTransformCom->Get_PosF3().z));
				m_strCurrentTrigger = "";
			}
		}
	}

    // �׳� ��ų 1 ����
    if ("SKILL_1" == m_strAnimType)
    {
        _vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 10.f, 25.f, 0.9f, 0.4f);

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
    // ���߿��� ��ų 1 ����
    else if ("SKILL_1_AIR" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
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
}

void CSabito::Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CSabito::Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
}

void CSabito::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (CONTROLLER_AI == m_tPlayerData.eControllerType)
    {
        _float3* vDirection = (_float3*)_Datas[OUTDATA::OUT_DIRECTION];
        if (!CMethod::Is_Vector_Zero(vDirection->Get_Vector()))
        {
    	    m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + vDirection->Get_Vector());
    	    m_pTransformCom->Translate_Dir(vDirection->Get_Vector(), _dTimeDelta, m_fMoveSpeed);
    	    m_fMoveSpeed = m_fSaveMoveSpeed;
        }
    }
    else
        __super::Move_State(_dTimeDelta, _Datas);	

    m_bMotionBlur = false;
}

void CSabito::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (CONTROLLER_AI == m_tPlayerData.eControllerType)
    {
        if (false == m_bStartAttack)
        {
            if (_Datas.size() >= OUT_TARGET_TRANSFORM)
            {
                CTransform* pTransform = static_cast<CTransform*>(_Datas[OUT_TARGET_TRANSFORM]);
                _float3 v3Pos = pTransform->Get_Pos();
                v3Pos.y = 0.f;
                _float3 v3MyPos = m_pTransformCom->Get_Pos();
                v3MyPos.y = 0.f;
                m_pTransformCom->LookAt(v3Pos.Get_Vector() - v3MyPos.Get_Vector() + m_pTransformCom->Get_Pos());
            }
            m_bStartAttack = true;
        }
    }
    else
    {
        __super::Attack_State(_dTimeDelta, _Datas);
    }

    m_bMotionBlur = false;
}

void CSabito::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Upper_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CSabito::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Down_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CSabito::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Attack_Awake_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CSabito::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Ultimate_State(_dTimeDelta, _Datas);

}

void CSabito::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Charge_Attack_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CSabito::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
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

    m_bMotionBlur = true;
}

void CSabito::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Dash_Ground_State(_dTimeDelta, _Datas);
}

void CSabito::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Jump_State(_dTimeDelta, _Datas);
}

// ���󿡼� Hit�� ��� ( ���̰� ���� )
void CSabito::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Hit_State(_dTimeDelta, _Datas);
}
void CSabito::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Hit_State(_dTimeDelta, _Datas);
}

void CSabito::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Air_Down_State(_dTimeDelta, _Datas);
}
void CSabito::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Strike_State(_dTimeDelta, _Datas);
}

void CSabito::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	__super::Bound_State(_dTimeDelta, _Datas);
}

HRESULT CSabito::Initialize_Sound()
{
    __super::Initialize_Sound();

    // ��Ʈ
    {
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg01");
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg02");      
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg03");
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg04");
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg05");
        Add_Sound(L"HIT_VOICE", L"Sabito_Dmg06");
        Add_Sound(L"HIT_S_VOICE", L"Sabito_Dmg_S01");
        Add_Sound(L"HIT_S_VOICE", L"Sabito_Dmg_S02");
        Add_Sound(L"HIT_S_VOICE", L"Sabito_Dmg_S03");
        Add_Sound(L"HIT_S_VOICE", L"Sabito_Dmg_S04");
        Add_Sound(L"HIT_S_VOICE", L"Sabito_Dmg_S05");
    }

    return S_OK;
}

void CSabito::Check_Attack_Type(_string _strHitType)
{
    __super::Check_Attack_Type(_strHitType);
}

void CSabito::Success_Attack_Upper()
{
}

void CSabito::Success_Attack_Down()
{
}

void CSabito::Success_SKILL_1()
{
}

void CSabito::Success_SKILL_2()
{
}

void CSabito::Success_SKILL_3()
{
}

void CSabito::Success_Throw()
{
    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        pInstance->Set_CutSceneModul(m_CutSceneModulMap["SABITO_THROW"]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

void CSabito::Success_Attack_Awake()
{
    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        pInstance->Set_CutSceneModul(m_CutSceneModulMap["SABITO_ATKSK"]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

void CSabito::Success_Ultimate()
{
}

CSabito* CSabito::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CSabito* pInstance = new CSabito(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CSabito::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CSabito::Clone(void* pArg)
{
	CSabito* pInstance = new CSabito(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CSabito::Clone To Failed");
#endif
	}
	return pInstance;
}

void CSabito::Free()
{
	__super::Free();
}
