#include "Player.h"
#include "GameInstance.h"
#include "Sword.h"
#include "Sheath.h"
#include "Controller.h"
#include "Animator.h"
#include "Battle_Manager.h"
#include "Camera_Free.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "MainView.h"
#include "ImGuiClass_Manager.h"
#include "CutScene_Manager.h"

CPlayer::CPlayer(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _rhs) 
    : CCharacter(_rhs)
{
    m_bObjectMotionBlur = false;
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;
    
    m_fMoveSpeed = 80.f;
    m_fSaveMoveSpeed = m_fMoveSpeed;
    m_fRotateSpeed = XMConvertToRadians(90.f) * 3.f;

    if (CBattle_Manager::Get_Instance()->Get_BattleInfo().bSabitoAI)
    {
        m_tBattleData = BATTLEDATA(200.f, 5.f, 0.f);
        m_tBattleData.fChargeValue = m_tBattleData.fMaxChargeValue;
    }
    else
    {
        m_tBattleData = BATTLEDATA(100.f, 5.f, 0.f);
        m_tBattleData.fChargeValue = m_tBattleData.fMaxChargeValue;
    }
    return S_OK;
}

void CPlayer::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActiveCutScene)
    {
        Update_TargetTransform();

        Fill_Datas();
        if (nullptr != m_pController)
            m_pController->Tick(_dTimeDelta);

        VOIDDATAS Datas;
        if (nullptr != m_pController)
        {
            Datas = m_pController->Out_Datas();
            // ����ִ� Ʈ���Ÿ� ����.
            m_strCurrentTrigger = "";

           
            // ��Ʋ�� �����ִٸ� ��ǲ�� ���´�.
            if (true == m_tBattleData.bBattleStop)
            {
                if (CONTROLLER_TYPE::CONTROLLER_AI != m_eControllerType)
                    static_cast<vector<_string>*>(Datas[OUTDATA::OUT_STATE])->clear();
            }
        }

        if (true == m_tBattleData.bIsAwake)
            m_fMoveSpeed = 120.f;
        else
            m_fMoveSpeed = 80.f;

        // �޾ƿ� ��ǲ���� �ִϸ��̼� Ʈ���Ÿ� �ִ´�.
        Animation_Control_State(_dTimeDelta, Datas);

        if (false == m_bActive)
            return;

        if (nullptr != m_pAnimator)
        {
            // �ִϸ��̼� ������Ʈ
            m_pAnimator->Tick(_dTimeDelta);

            // �ִϸ��̼� Ÿ���� �޾ƿ´�.
            if (nullptr != m_pAnimator)
                m_strAnimType = m_pAnimator->Get_AnimationType();

            if (nullptr != m_pSword)
            {
                m_pSword->Change_Bind_Matrix(m_pAnimator->Get_WeaponBone());
            }
        }
        // ���¿� ���� �̵����� �ൿ�� �Ѵ�.
        Behavior_State(_dTimeDelta, Datas);
        Check_SuccessDash();
        Control_Trail_Active();

        __super::Tick(_dTimeDelta);
    }
    else
    {
        Control_Trail_Active();
    }

    // ���� ������¶�� ���� �۷ο츦 Ų��.
    if (true == m_tBattleData.bIsAwake)
    {
        m_vEdgeGlowColor.w = 1.f;
    }
    else
    {
        m_vEdgeGlowColor.w = 0.f;
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Late_Tick(const _double& _dTimeDelta)
{
    if (false == m_bRenderEnable)
    {
        if (nullptr != m_pSword)
            m_pSword->Set_RenderEnable(false);
        if (nullptr != m_pSheath)
            m_pSheath->Set_RenderEnable(false);
        return;
    }
    else
    {
        if (nullptr != m_pSword)
            m_pSword->Set_RenderEnable(true);
        if (nullptr != m_pSheath)
            m_pSheath->Set_RenderEnable(true);
    }

    if (false == m_bActiveCutScene)
    {
        if (nullptr != m_pController)
            m_pController->Late_Tick(_dTimeDelta);

        if (nullptr != m_pController)
            m_pController->Insert_Datas(m_InsertVoidDatas);

        __super::Late_Tick(_dTimeDelta);
    }
    else
    {
        __super::Late_Tick(_dTimeDelta);
    }
}

HRESULT CPlayer::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if(nullptr != m_pSword)
        m_pSword->Set_Active(m_pAnimator->Is_MeshActive());
    if (nullptr != m_pSheath)
        m_pSheath->Set_Active(m_pAnimator->Is_MeshActive());

    return S_OK;
}

void CPlayer::Success_Dash()
{
    m_bDashCollision = true;
    m_fMoveSpeed = 0.f;
}

void CPlayer::Check_SuccessDash()
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager)
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
        if (m_strEnemyState == "Dash")
        {
            CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
            // �뽬 ���� �ߴٴ� �Լ� ȣ��
            static_cast<CPlayer*>(pInstance->Get_BattleCharacter(eType))->Success_Dash();
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
}


// ������ ���� ��, ȣ�� �Ǵ� �Լ�
void CPlayer::Damaged(CPlayer* _pPlayer)
{
    if (nullptr == _pPlayer)
        return;

    BATTLEDATA tData = _pPlayer->Get_BattleData();
    // ���� ī��Ʈ�� ���� ��Ų��.
    ++m_tBattleData.iHitCount;
    // HP�� ���� ��Ų��. 
    m_tBattleData.Decrease_HP(tData.fAttackPower);
    // Damage Collision
    m_bDamagedToCollision = true;
}

void CPlayer::Hit_ByEnemy(CCharacter* _pEnemyCharcter, CBattle_Manager* _pBattleManager)
{
    __super::Hit_ByEnemy(_pEnemyCharcter, _pBattleManager);

    CImGuiClass_Manager* pInstance = GET_INSTANCE(CImGuiClass_Manager);
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());

        if (eType == CBattle_Manager::CHAR_PLAYER)
            static_cast<CMainView*>(pInstance->Get_ImGuiClass(IMGCLASS_MAIN))->Add_Explain("AI Damaged");
        else if (eType == CBattle_Manager::CHAR_AI)
            static_cast<CMainView*>(pInstance->Get_ImGuiClass(IMGCLASS_MAIN))->Add_Explain("Player Damaged");
    }
    RELEASE_INSTANCE(CImGuiClass_Manager);
}

void CPlayer::Hit_Effect(_string strColliderType, CCharacter* _pEnemyCharcter)
{
    __super::Hit_Effect(strColliderType, _pEnemyCharcter);
}

// SetUp Controller
HRESULT CPlayer::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
    CBattle_Manager* pInstnace = GET_INSTANCE(CBattle_Manager);
    {
        m_eControllerType = _eType;
        if (CONTROLLER_AI == _eType)
        {
            pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
        }
        else if (CONTROLLER_KEYBOARD == _eType)
        {
            if(nullptr == pInstnace->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER))
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_PLAYER);
            else
            {
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
            }
        }
        else if (CONTROLLER_GAMEPAD == _eType)
        {
            if (nullptr == pInstnace->Get_BattleCharacter(CBattle_Manager::CHAR_PLAYER))
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_PLAYER);
            else
            {
                pInstnace->Start_Battle(this, CBattle_Manager::CHAR_AI);
            }
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
    // Player�� �� ����

    // AI �ϋ� ����

    return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
    if (FAILED(__super::SetUp_Components()))
        return E_FAIL;

    // COM_NAVIGATION
    if (LEVEL_STORY == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }
    else if (LEVEL_BATTLE == g_iCurrentLevel)
    {
        if (FAILED(Set_BattleMap_Navigation()))
            return E_FAIL;
        //if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
    }
    else if (LEVEL_GAMEPLAY == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_STORY_TEONI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }
    else if (LEVEL_RATTAN == g_iCurrentLevel)
    {
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_RATTAN, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
    }

    CCollider::COLLIDERDESC ColliderDesc;
    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 2.5f, 4.f, 1.5f };
    ColliderDesc.vTranslation = _float3{ 0.f, 5.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_HIT, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 5.f, 10.f, 5.f };
    ColliderDesc.vTranslation = _float3{ 0.f, 10.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_PREV_OBB, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pPrevCollider, &ColliderDesc)))
        return E_FAIL;

    ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
    ColliderDesc.vScale = _float3{ 15.f, 15.f, 15.f };
    ColliderDesc.vTranslation = _float3{ 0.f, 2.f, 0.f };
    if (FAILED(__super::Add_Component(COM_COLLIDER_ATTACK, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pAttackColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::SetUp_Weapon(FILEDATA* _pFileData, const _wstring& _strModelProtoName, const _string& _strModelStartBone, const _string& _strModelEndBone)
{
    RETURN_IFNOT(nullptr == _pFileData, E_FAIL);

    CSword::SWORDDESC tDesc;
    {
        CSword::ITEMDESC tItemDesc;
        tItemDesc.pItemData = _pFileData;
        tItemDesc.pTargetModel = m_pModelCom;
        tItemDesc.pTargetTransform = m_pTransformCom;
        tItemDesc.pTargetAnimator = m_pAnimator;
        tItemDesc.strModelProtoName = _strModelProtoName;

        tDesc.tItemDesc = tItemDesc;
    }
    tDesc.strStartBoneName = _strModelStartBone;
    tDesc.strFinishBoneName = _strModelEndBone;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        m_pSword = static_cast<CSword*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_SWORD, PROTO_GOB_SWORD, &tDesc));
        Safe_AddRef(m_pSword);
    }
    RELEASE_INSTANCE(CGameInstance);

    RETURN_IFNOT(nullptr == m_pSword, E_FAIL);

    return S_OK;
}

HRESULT CPlayer::SetUp_Sheath(FILEDATA* _pFileData, const _wstring& _strModelProtoName)
{
    RETURN_IFNOT(nullptr == _pFileData, E_FAIL);

    CSheath::ITEMDESC tItemDesc;
    tItemDesc.pItemData = _pFileData;
    tItemDesc.pTargetModel = m_pModelCom;
    tItemDesc.pTargetTransform = m_pTransformCom;
    tItemDesc.pTargetAnimator = m_pAnimator;
    tItemDesc.strModelProtoName = _strModelProtoName;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        m_pSheath = static_cast<CSheath*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_SHEATH, PROTO_GOB_SHEATH, &tItemDesc));
        Safe_AddRef(m_pSheath);
    }
    RELEASE_INSTANCE(CGameInstance);

    RETURN_IFNOT(nullptr == m_pSheath, E_FAIL);

    return S_OK;
}

void CPlayer::Fill_Datas()
{
}

void CPlayer::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    // �뽬�� ���������� ���浵 �뽬���¿��ٸ� ƨ���.
    if (false == m_bIsSuccessDash && false == CMethod::Is_Vector_Zero(XMLoadFloat4(&m_vDashReflectDir)))
    {
        Dash_Reflect_State(_dTimeDelta);
        return;
    }

    // �����̱�
    if ("MOVE" == m_strAnimType || "Move" == m_strAnimType)
    {
        // MOVE�� ��� �����δ�.
        Move_State(_dTimeDelta, _Datas);
    }
    // ���� ���� �뽬
    else if ("DASH" == m_strAnimType || "Dash" == m_strAnimType)
    {
        Dash_Ground_State(_dTimeDelta, _Datas);
    }
    // ���� ���� �뽬
    else if ("AIR_DASH" == m_strAnimType)
    {
        Dash_Air_State(_dTimeDelta, _Datas);
    }
    // ����
    else if ("JUMP" == m_strAnimType || "Jump" == m_strAnimType)
    {
        Jump_State(_dTimeDelta, _Datas);
    }
    // ���� ������ ��쿡�� ������ �ϴµ��� �Ʒ��� �������� �ȴ�.
    else if ("AIR_STEP" == m_strAnimType )
    {
        Step_State(_dTimeDelta, _Datas);
    }
    // ���� ������ �ִϸ��̼� �̺�Ʈ���� Ư�� �������� �߷��� ���� �ʴ´�.
    else if ("AIR_ATTACK" == m_strAnimType || "Air_Attack" == m_strAnimType)
    {
        Attack_State(_dTimeDelta, _Datas);
    }
    else if ("ATTACK_AWAKE" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Attack_Awake_State(_dTimeDelta, _Datas);
    }
    else if ("ULTIMATE" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Ultimate_State(_dTimeDelta, _Datas);
    }
    else if ("CHARGE_ATTACK" == m_strAnimType || "Charge_Attack" == m_strAnimType)
    {
        Charge_Attack_State(_dTimeDelta, _Datas);
    }
    else if ("AIR_HIT" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Air_Hit_State(_dTimeDelta, _Datas);
    }
    else if ("AIR_DOWN" == m_strAnimType)
    {
        Air_Down_State(_dTimeDelta, _Datas);
    }
    else if ("HIT" == m_strAnimType && true == m_pAnimator->Is_EventPlay() || "Hit" == m_strAnimType)
    {
        Hit_State(_dTimeDelta, _Datas);
    }
    else if ("BOUND" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Bound_State(_dTimeDelta, _Datas);
    }
    else if ("STRIKE" == m_strAnimType)
    {
        Strike_State(_dTimeDelta, _Datas);
    }
    // ���� �����϶��� ���ݿ� ���� �ʰ�, ���ư���.
    else if ("SPIN" == m_strAnimType)
    {
        Spin_State(_dTimeDelta, _Datas);
    }
    // �ٿ� �����϶� �׾����� �Ǵ��� ���ش�.
    else if ("DOWN" == m_strAnimType)
    {
        Down_State(_dTimeDelta, _Datas);
    }
    // Blow �������� Air_Down�� ���� ���������� �����δ�.
    else if ("BLOW" == m_strAnimType && true == m_pAnimator->Is_EventPlay())
    {
        Blow_State(_dTimeDelta, _Datas);
    }
    // ���� ���� �뽬
    else if ("ROLL" == m_strAnimType)
    {
        Roll_State(_dTimeDelta, _Datas);
    }
    // DEAD �����϶�
    else if ("DIE" == m_strAnimType)
    {
        Dead_State(_dTimeDelta);
    }
    else if ("PASSIVE" == m_strAnimType)
    {
        Passive_State(_dTimeDelta, _Datas);
    }

    RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
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
        }
    }
}

void CPlayer::Skill_1(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Skill_2(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Skill_3(const _double& _dTimeDetla, VOIDDATAS _Datas)
{
}
void CPlayer::Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    _float3* vDirection = (_float3*)_Datas[OUTDATA::OUT_DIRECTION];
    if (!CMethod::Is_Vector_Zero(vDirection->Get_Vector()))
    {
        // �̵��Ѵ�.
        m_pTransformCom->Translate_DirSliding(vDirection->Get_Vector(), _dTimeDelta, m_fMoveSpeed);
    }
    m_bMotionBlur = false;
}

// ���߿� ���� �����ϴ� ��� �Ʒ��� �����ش�.
void CPlayer::Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
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
    m_bMotionBlur = false;
}

// ��� ����
void CPlayer::Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

// �ϴ� ����
void CPlayer::Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

void CPlayer::Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

void CPlayer::Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    m_bMotionBlur = false;
}

// ������
void CPlayer::Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (true == m_tChargeAttackTimer.Is_Over(_dTimeDelta))
    {
        Input_Trigger("CHARGE_ATTACK_END");
        m_tChargeAttackTimer.Reset_Timer();
    }

    m_bMotionBlur = false;
}

// ���߿� ���� ������ ��� �Ʒ��� �����ش�.
void CPlayer::Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
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
// ���� �߰� �뽬 ����
void CPlayer::Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (nullptr == m_pTargetTransform)
        return;

    // �����ϴ� ����� �Ĵٺ���.
    m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
    // �뽬�� �⺻ �̵��ӵ����� 1.5�� ������ �����δ�.
    m_pTransformCom->Translate_Dir(Get_DirToTarget(), _dTimeDelta, m_fMoveSpeed * 2.f);

    m_bMotionBlur = true;
}
// ���� �߰� �뽬 ����
void CPlayer::Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    if (nullptr == m_pTargetTransform)
        return;

    // �����ϴ� ����� �Ĵٺ���.
    m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
    _vector vDir = m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos();
    // �뽬�� �⺻ �̵��ӵ����� 1.5�� ������ �����δ�.
    m_pTransformCom->Translate_DirNotNavigation(XMVector3Normalize(vDir), _dTimeDelta, m_fMoveSpeed * 2.f);

    m_bMotionBlur = true;
}
void CPlayer::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // ������ FALL_END Ʈ���Ÿ� ��ǲ�Ѵ�.
    Jump(_dTimeDelta, "FALL_END");
    m_bMotionBlur = false;
}
void CPlayer::Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // �ڽ����κ��� ������ �ٶ󺸴� ���⺤���� �ݴ�������� �����Ѵ�.
    _vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, m_vBehaviorDir.Get_Vector(),60.f, 25.f, 0.3f, 1.f);

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
    m_bMotionBlur = false;
}
void CPlayer::Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // MoveDirection���� 0�� �ƴ϶�� Roll ���µ��� ������ �̵��Ѵ�.
    if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
    {
        m_pTransformCom->Translate_Dir(m_vDirection.Get_Vector(), _dTimeDelta, m_fMoveSpeed);
        m_pTransformCom->LookAt(Get_MoveDirection());
        m_bMotionBlur = false;
    }
}

// ���󿡼� Hit�� ��� ( ���̰� ���� )
void CPlayer::Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Hit_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}
void CPlayer::Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Air_Hit_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    // �ݶ��̴��� ���Ⱚ�� �޾ƿɴϴ�.
    _vector vColliderDir = m_pAnimator->Get_ColliderDir();
    // �ǰ� ȿ������ �Ѿ�� PushedForce; 
    _float fDistance = m_tKnockBackInfo.fPushedForce;
    // �ǰ� ȿ������ �Ѿ�� MaxHeight
    _float fMaxHeight = m_tKnockBackInfo.fHeight;
    // �´� �ִϸ��̼� ���� �ð�
    _double dEventPlayTime = m_tKnockBackInfo.dPlayTime;
    // �з��� ���⺤��
    _vector vAttackDir = m_tKnockBackInfo.vAttackDir.Get_Vector();

    // �ִϸ��̼��� �ִ���̿� �����ϴ� ����
    _float fMaxHeightRatio = 0.7f;

    // ����ؼ� ���� �����Ӵ� �̵��Ÿ�
    _vector vMovePos = Compute_MovePos_ByDist(_dTimeDelta, vAttackDir, fDistance, fMaxHeight, fMaxHeightRatio, (_float)dEventPlayTime);
    // �÷��̾��� ��ġ
    _vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
    _float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
    if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
    {
        m_dFallTimeAcc = 0.0;
        vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);

        // Ʈ���� ��ǲ�� �ƴ��� �����Ѵ�.
        _bool bIsInput = false;

        // �����ÿ� ����Ű�� �����־��ٸ� PASSIVE�� �ߵ���Ų��.

        if (CONTROLLER_AI != m_eControllerType)
        {
            vector<_string>* pData = static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
            if (nullptr != pData)
            {
                vector<_string> triggerList = *pData;
                for (auto& trigger : triggerList)
                {
                    if ("PASSIVE" == trigger)
                    {
                        if (true == m_pAnimator->Input_Trigger(trigger))
                        {
                            bIsInput = true;

                            CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

                            // ���濡�� �ڽ��� ���ϴ� ���͸� BehaviorDir�� ������ش�.
                            if (nullptr != m_pTargetTransform)
                            {
                                m_vBehaviorDir = XMVector3Normalize(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos());
                                m_vBehaviorDir.y = 0.f;
                            }
                            RELEASE_INSTANCE(CBattle_Manager);

                            break;
                        }
                    }
                }
            }
        }
        if (false == bIsInput)
            m_pAnimator->Input_Trigger("FALL_END");

        m_bIsAir = false;
    }
    else
        m_bIsAir = true;

    // Y�� ���� ������ Y��, XZ�� �׺� ���� ����
    m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);

    m_bMotionBlur = false;
}
void CPlayer::Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Strike_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Bound_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Blow_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
    __super::Blow_State(_dTimeDelta, _Datas);

    m_bMotionBlur = false;
}

void CPlayer::Check_Attack_Type(_string _strHitType)
{
    __super::Check_Attack_Type(_strHitType);
}

void CPlayer::Success_Attack_Upper()
{
    __super::Success_Attack_Upper();

    m_bMotionBlur = false;
}

void CPlayer::Success_Attack_Down()
{
    __super::Success_Attack_Down();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_1()
{
    __super::Success_SKILL_1();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_2()
{
    __super::Success_SKILL_2();

    m_bMotionBlur = true;
}

void CPlayer::Success_SKILL_3()
{
    __super::Success_SKILL_3();

    m_bMotionBlur = true;
}

void CPlayer::Success_Throw()
{
}

void CPlayer::Success_Attack_Awake()
{
}

void CPlayer::Success_Ultimate()
{
}

void CPlayer::Control_Trail_Active()
{
    if (nullptr == m_pSword)
        return;

    m_pSword->Set_TrailActive(m_pAnimator->Is_TrailActive());
    if (true == m_pAnimator->Is_TrailActive())
    {
        _float2 vTrailValues = m_pAnimator->Get_TrailValues();
        m_pSword->Update_TrailMatrix(100, 60);
    }
}

void CPlayer::Extern_ImGuiClass()
{
    __super::Extern_ImGuiClass();

    // ���� ���߿� �ִ��� üũ�Ѵ�.
    ImGui::Checkbox("Air", &m_bIsAir);

    // �ִϸ��̼� Ÿ���� �����â�� ����.
    if (nullptr != m_pAnimator)
        ImGui::Text(m_strAnimType.c_str());

    _string strFallTime = "FallTime";
    strFallTime += CMethod::Convert_Number_S(m_dFallTimeAcc, 3);

    // �������� �ð� �����
    ImGui::Text(strFallTime.c_str());    
}

void CPlayer::Coll_Init()
{
    __super::Coll_Init();

    if (nullptr != m_pPrevCollider)
        m_pPrevCollider->Init_Collision();
}

void CPlayer::Coll_Tick(_matrix _WorldMatrix)
{
    __super::Coll_Tick(_WorldMatrix);

    if (nullptr != m_pPrevCollider)
    {   
        _WorldMatrix = CMethod::XMMatrixSetFloat3(m_vPreviousPosition, MATROW_POS, _WorldMatrix);
        m_pPrevCollider->Tick(_WorldMatrix);
    }
}

void CPlayer::Check_Hit_Type(_string _strHitType, CCharacter* _pEnemyCharacter)
{
    __super::Check_Hit_Type(_strHitType, _pEnemyCharacter);
}

void CPlayer::Play_EndCutScene(const _bool& _bIsAI)
{
    CCutScene_Manager* pCutSceneManager = GET_INSTANCE(CCutScene_Manager);
    {
        pCutSceneManager->Set_CutSceneModul(m_CutSceneModulMap["BATTLE_END"]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

HRESULT CPlayer::Set_BattleMap_Navigation()
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    if (nullptr == pInstance)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return E_FAIL;
    }

    CBattle_Manager::BATTLEMAP_TYPE battleMap = pInstance->Get_BattleInfo().eBattleMap;

    switch (battleMap)
    {
    case Client::CBattle_Manager::MAP_UROKODAKI:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_HUJIKASANE:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_ASAKUSA:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_UROKODAKI, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    case Client::CBattle_Manager::MAP_TAMAYO:
        if (FAILED(Add_Component(COM_NAVIGATION, g_iCurrentLevel, PROTO_COMP_NAVIGATION_TAMAYO, (CComponent**)&m_pNavigationCom)))
            return E_FAIL;
        break;
    }
   
    RELEASE_INSTANCE(CBattle_Manager);
    return S_OK;
}

#ifdef _DEBUG
void CPlayer::Coll_Render()
{
    __super::Coll_Render();
}
void CPlayer::Coll_Renderer()
{
    __super::Coll_Renderer();
}
#endif

_bool CPlayer::Input_AirDash_Trigger(const _string& _strTrigger)
{
    _bool bIsAirDash = false;

    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    // �� ��ü�� �޾ƿ´�.
    CCharacter* pEnemy = pInstance->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
    if (nullptr == pEnemy)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return false;
    }
    RELEASE_INSTANCE(CBattle_Manager);

    // �ִϸ��̼� Ÿ���� �˻��Ѵ�.
    if ("AIR_DOWN" == pEnemy->Get_Animator()->Get_AnimationType())
    {
        // ��°������� 20 �Ҹ��� �� �ִٸ� �뽬�� �����Ѵ�.
        if (true == m_pAnimator->Input_Trigger(_strTrigger))
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
            return true;
        }
    }
    return false;
}

_bool CPlayer::Input_Dash_Trigger(const _string& _strTrigger)
{
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager);

    // �� ��ü�� �޾ƿ´�.
    CCharacter* pEnemy = pInstance->Get_BattleCharacter((CBattle_Manager::CHARACTER_TYPE)Get_EnemyType());
    if (nullptr == pEnemy)
    {
        RELEASE_INSTANCE(CBattle_Manager);
        return false;
    }
    RELEASE_INSTANCE(CBattle_Manager);

    // �ִϸ��̼� Ÿ���� �˻��Ѵ�. �ٿ��� ��쿡�� �뽬���� ���Ѵ�.
    if ("DOWN" != pEnemy->Get_Animator()->Get_AnimationType())
    {
        // ��°������� 20 �Ҹ��� �� �ִٸ� �뽬�� �����Ѵ�.
        if (true == m_pAnimator->Input_Trigger(_strTrigger))
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + Get_DirToTarget());
            return true;
        }
    }
    return false;
}

void CPlayer::Update_TargetTransform()
{
    CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);
    {
        if (LEVEL_BATTLE == g_iCurrentLevel || LEVEL_GAMEPLAY == g_iCurrentLevel)
        {
            if (nullptr == m_pTargetTransform)
            {
                CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
                if (nullptr != pBattleInstance->Get_BattleCharacter(eType))
                {
                    m_pTargetTransform = pBattleInstance->Get_BattleCharacter(eType)->Get_Transform();
                }
            }
        }
        else if (LEVEL_STORY == g_iCurrentLevel)
        {
            vector<CCharacter*>& pOniArr = pBattleInstance->Get_OniArr();

            _float fMinLength = FLT_MAX;
            _float fMinLengthOniDegree = 360.f;
            // Ÿ�� ���� ���ؿ���
            CCharacter* pTargetOni = nullptr;
            for (auto& pOni : pOniArr)
            {
                if (pOni->Get_BattleData().bBattleStop)
                    continue;

                {
                    _vector vPlayerLook = m_pTransformCom->Get_Look();
                    _vector vPlayerToOni = pOni->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos();

                    _float fCurDegree = fabs(CMethod::Compute_Degree_XZ_Plane(vPlayerLook, vPlayerToOni));
                    _float fCurLength = XMVectorGetX(XMVector3Length(vPlayerToOni));

                    // MinLength���� ���� ��� Ÿ�� ����
                    if (fMinLength > fCurLength)
                    {
                        pTargetOni = pOni;
                        fMinLength = fCurLength;
                        fMinLengthOniDegree = fCurDegree;
                    }
                    // MinLength���� ũ���� 5���Ϸ� ���̳� ��� ������ �ѹ� �� üũ
                    else if (fMinLength + 5.f > fCurLength)
                    {
                        if (fMinLengthOniDegree > fCurDegree)
                        {
                            pTargetOni = pOni;
                            fMinLength = fCurLength;
                            fMinLengthOniDegree = fCurDegree;
                        }
                    }
                }
            }
            
            // Ÿ�� ���� ����
            if (nullptr != pTargetOni)
            {
                if (CGameInstance::Get_Instance()->Get_DIKeyDown(DIK_RSHIFT)
                    || nullptr == m_pTargetTransform)
                {
                    m_pTargetTransform = pTargetOni->Get_Transform();
                }
            }
        }
    }
    RELEASE_INSTANCE(CBattle_Manager);
}

_uint CPlayer::Get_EnemyType()
{
    CBattle_Manager::CHARACTER_TYPE eType = CBattle_Manager::CHAR_END;
    if (CONTROLLER_AI == m_eControllerType)
    {
        eType = CBattle_Manager::CHAR_PLAYER;
    }
    else if (CONTROLLER_GAMEPAD == m_eControllerType || CBattle_Manager::CHAR_AI == m_eControllerType)
    {
        CBattle_Manager* pBattleManager = GET_INSTANCE(CBattle_Manager);
        {
            if(this == pBattleManager->Get_BattleCharacter(CBattle_Manager::CHAR_AI))
                eType = CBattle_Manager::CHAR_PLAYER;
            else
                eType = CBattle_Manager::CHAR_AI;
        }
        RELEASE_INSTANCE(CBattle_Manager);
    }

    return eType;
}

_float CPlayer::Get_EnemyToPlayerDot()
{
    _float fDot = 0.f;
    CBattle_Manager* pInstance = GET_INSTANCE(CBattle_Manager)
    {
        CBattle_Manager::CHARACTER_TYPE eType = (CBattle_Manager::CHARACTER_TYPE)(Get_EnemyType());
        fDot = XMConvertToDegrees(OtherCharacterDirToLook(pInstance->Get_BattleCharacter(eType)));
    }
    RELEASE_INSTANCE(CBattle_Manager);
    return fDot;
}

_vector CPlayer::Get_MoveDirection()
{
    return m_pTransformCom->Get_Pos() + m_vDirection.Get_Vector();
}

_vector CPlayer::Get_DirToTarget()
{
    // ������ Transform�� �������� �ʴٸ� �ڱ� �ڽ��� Look�� ��ȯ�Ѵ�.
    if (nullptr == m_pTargetTransform)
        return m_pTransformCom->Get_Look();

    // Y���� ���ܽ�Ų��.
    _vector vDir = m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos();
    vDir = XMVectorSetY(vDir, 0.f);

    m_vBehaviorDir = XMVector3Normalize(vDir);

    return XMVector3Normalize(vDir);
}

_string CPlayer::Get_StepDir()
{
    // Ÿ���� ���� ��쿡�� �����ϰ� �뽬�Ѵ�.
    if (nullptr == m_pTargetTransform)
    {
        // �̵� ���Ͱ� 0�� �ƴ� ��쿡 ����Ѵ�.
        if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
        {
            // Step Ʈ������ ������ �˻��Ͽ� �ùٸ� Ʈ���Ÿ� �־��ָ�, Look���� ���ǿ� ���� �ٲ��ش�.

            // m_vDirection �� Target���� Look �� ���Ͽ� Ʈ���Ÿ� �����Ѵ�.
            _vector vLook = XMVector3Normalize(m_pTransformCom->Get_Look());

            // ��Ȯ�� ���� ����� ���� Y�� ���ش�.
            vLook = XMVectorSetY(vLook, 0);
            m_vDirection.y = 0;

            // Target������ Look�� ������� Direct�� ������ ������� �Ǵ��ϱ� ���� �����Ѵ�.
            _vector vDot = XMVector3Dot(vLook, XMVector3Normalize(m_vDirection.Get_Vector()));

            // Degree������ ���Ѵ�.
            _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

            // Ÿ���� �ٶ󺸴� �������� ��������� ���������� ���������� �Ǵ��ϱ����� �����Ѵ�.
            _vector vCross = XMVector3Cross(vLook, m_vDirection.Get_Vector());

            // fDegree�� 50�� ���� �۰ų� ���ٸ� STEP_FRONT�� �����Ѵ�.
            if (80 >= fDegree)
                return "STEP_FRONT";
            // 50�� ����ũ�� 140���� �۰ų� ���ٸ� �¿� �Ǻ��� ���� STEP_LEFT Ȥ�� STEP_RIGHT�� �����Ѵ�.
            else if (80 < fDegree && 100 >= fDegree)
            {
                // Ÿ���� ���� Look �������� ��������� �����϶�
                if (0 < XMVectorGetY(vCross))
                    return "STEP_RIGHT";
                // Ÿ���� ���� Look �������� ��������� �����϶�
                else
                    return "STEP_LEFT";
            }
            // 140���� fDegree�� ũ�ٸ� STEP_BACK�� ����Ѵ�.
            else if (100 < fDegree)
                return "STEP_BACK";
        }
    }
    else
    {
        // Step Ʈ������ ������ �˻��Ͽ� �ùٸ� Ʈ���Ÿ� �־��ָ�, Look���� ���ǿ� ���� �ٲ��ش�.

        // m_vDirection �� Target���� Look �� ���Ͽ� Ʈ���Ÿ� �����Ѵ�.
        _vector vLookToTarget = XMVector3Normalize(m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos());

        // ��Ȯ�� ���� ����� ���� Y�� ���ش�.
        vLookToTarget = XMVectorSetY(vLookToTarget, 0);
        m_vDirection.y = 0;

        // Target������ Look�� ������� Direct�� ������ ������� �Ǵ��ϱ� ���� �����Ѵ�.
        _vector vDot = XMVector3Dot(vLookToTarget, XMVector3Normalize(m_vDirection.Get_Vector()));

        // Degree������ ���Ѵ�.
        _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

        // Ÿ���� �ٶ󺸴� �������� ��������� ���������� ���������� �Ǵ��ϱ����� �����Ѵ�.
        _vector vCross = XMVector3Cross(vLookToTarget, m_vDirection.Get_Vector());

        // fDegree�� 50�� ���� �۰ų� ���ٸ� STEP_FRONT�� �����Ѵ�.
        if (50 >= fDegree)
            return "STEP_FRONT";
        // 50�� ����ũ�� 140���� �۰ų� ���ٸ� �¿� �Ǻ��� ���� STEP_LEFT Ȥ�� STEP_RIGHT�� �����Ѵ�.
        else if (50 < fDegree && 140 >= fDegree)
        {
            // Ÿ���� ���� Look �������� ��������� �����϶�
            if (0 < XMVectorGetY(vCross))
                return "STEP_RIGHT";
            // Ÿ���� ���� Look �������� ��������� �����϶�
            else 
                return "STEP_LEFT";            
        }
        // 140���� fDegree�� ũ�ٸ� STEP_BACK�� ����Ѵ�.
        else if (140 < fDegree)        
            return "STEP_BACK";        
    }

    return _string();
}

void CPlayer::Set_StepLook(const _string& _strTrigger)
{
    if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
    {
        if ("STEP_RIGHT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_pTransformCom->Rotate_TurnToNotFix(_float3(0.f, -90.f, 0.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());


        }
        else if ("STEP_LEFT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_pTransformCom->Rotate_TurnToNotFix(_float3(0.f, 90.f, 0.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
        else if ("STEP_FRONT" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector()));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
        else if ("STEP_BACK" == _strTrigger)
        {
            m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVector3Normalize(m_vDirection.Get_Vector() * -1.f));
            m_vBehaviorDir = m_pTransformCom->Get_Look();
            m_pAnimator->Set_OwnerDirection(m_vDirection.Get_Vector());
        }
    }
}

_bool CPlayer::Check_ChargeValue()
{
    // ��°������� 20��ŭ ��Ƶ� 0���� ũ�ٸ� true �� return�Ѵ�.
    if (0 < m_tBattleData.fChargeValue - 20.f)
        return true;
    else
        return false;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    return nullptr;
}

void CPlayer::Free()
{
    Safe_Release(m_pSheath);
    Safe_Release(m_pSword);
    Safe_Release(m_pPrevCollider);

    __super::Free();
}
