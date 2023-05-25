#include "Battle_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Character.h"
#include "BonesCollider.h"
#include "AnimationClip.h"
#include "MainView.h"
#include "ImGuiClass_Manager.h"

#include "Oni.h"
#include "Level_Story.h"
#include "Level_Battle.h"
#include "CutScene_Manager.h"
#include "Player.h"
#include "CutSceneModul.h"

IMPLEMENT_SINGLETON(CBattle_Manager);

CBattle_Manager::CBattle_Manager()
{
}

HRESULT CBattle_Manager::NativeConstruct_StartCutScene()
{
    if (nullptr == m_pCharacterArr[CHAR_PLAYER] || nullptr == m_pCharacterArr[CHAR_AI])
        return E_FAIL;

    m_pCharacterArr[CHAR_PLAYER]->Start_Scene(m_pCharacterArr[CHAR_AI]);
    m_pCharacterArr[CHAR_AI]->Start_Scene(m_pCharacterArr[CHAR_PLAYER]);

    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        pInstance->Update_Battle(m_pCharacterArr[CHAR_PLAYER], m_pCharacterArr[CHAR_AI]);
    }
    RELEASE_INSTANCE(CCutScene_Manager);
    return S_OK;
}

void CBattle_Manager::Start_Battle(CCharacter* _pCharacter, const CHARACTER_TYPE& _eCharType)
{
    Safe_Release(m_pCharacterArr[_eCharType]);
    m_pCharacterArr[_eCharType] = _pCharacter;
    Safe_AddRef(m_pCharacterArr[_eCharType]);
}

CCharacter* CBattle_Manager::Get_BattleCharacter(const CHARACTER_TYPE& _eCharType){
    return m_pCharacterArr[_eCharType];
}

_bool CBattle_Manager::Is_MotionBlurOn()
{
    if (nullptr == m_pCharacterArr[CHAR_AI] || nullptr == m_pCharacterArr[CHAR_PLAYER])
        return false;

    return m_pCharacterArr[CHAR_AI]->Is_MotionBlur() | m_pCharacterArr[CHAR_PLAYER]->Is_MotionBlur();
}

void CBattle_Manager::Update(const _double& _dTimeDelta)
{
    CCutScene_Manager* pInstance = GET_INSTANCE(CCutScene_Manager);
    {
        if(nullptr == pInstance->Get_SelectCutSceneModel())
        {
            // ������Ʈ �ð� ����
            _double TimeDelta = _dTimeDelta * (_double)m_tBattleInfo.fUpdateRatio;

            InitBattle(TimeDelta);

            {
                // ���� ��
                if (LEVEL_BATTLE == g_iCurrentLevel || LEVEL_GAMEPLAY == g_iCurrentLevel)
                {
                    if (nullptr == m_pCharacterArr[CHAR_PLAYER] || nullptr == m_pCharacterArr[CHAR_AI])
                    {
                        RELEASE_INSTANCE(CCutScene_Manager);
                        return;
                    }

                    // �浹 ó��
                    Push();

                    Check_BattleDelayTimer(TimeDelta);

                    // ��Ʋ�� ����ɶ��� �ǰݵ��� �ʴ´�.
                    if (!m_pCharacterArr[CHAR_AI]->Get_BattleData().bBattleStop && !m_pCharacterArr[CHAR_PLAYER]->Get_BattleData().bBattleStop)
                    {

                        // �÷��̾ ������ �� ó��
                        m_pCharacterArr[CHAR_PLAYER]->Check_AttackCollision(m_pCharacterArr[CHAR_AI]);
                        // AI�� �����Ҷ� ó��
                        m_pCharacterArr[CHAR_AI]->Check_AttackCollision(m_pCharacterArr[CHAR_PLAYER]);
                    }

                    if (CGameInstance::Get_Instance()->Get_DIKeyDown(DIK_0))
                    {
                        m_pCharacterArr[CHAR_AI]->Get_BattleData().Decrease_HP(20.f);
                    }

                }
            }

            {
                // ���丮 ��
                if (LEVEL_STORY == g_iCurrentLevel)
                {
                    SetUp_OniLayer();
                    // ���Ͽ� �÷��̾� �ǰ� ó��
                    for (auto& pOni : m_pOniArr)
                    {
                        if ("DIE" == pOni->Get_Animator()->Get_AnimationType()
                            || "DOWN" == pOni->Get_Animator()->Get_AnimationType())
                            continue;

                        m_pCharacterArr[CHAR_PLAYER]->Push(pOni);
                        pOni->Push(m_pCharacterArr[CHAR_PLAYER]);

                        Check_StoryDelayTimer(TimeDelta);

                        // ���� ���� ó��
                        pOni->Check_AttackCollision(m_pCharacterArr[CHAR_PLAYER]);
                        // �÷��̾� ���� ó��
                        m_pCharacterArr[CHAR_PLAYER]->Check_AttackCollision(pOni);
                    }

                    
                }
            }

            if (m_tBattleInfo.bBattleEnd)
            {
                if (LEVEL_BATTLE == g_iCurrentLevel)
                {
                    CCharacter::BATTLEDATA& tPlayerData = m_pCharacterArr[CHAR_PLAYER]->Get_BattleData();
                    CCharacter::BATTLEDATA& tAIData = m_pCharacterArr[CHAR_AI]->Get_BattleData();

                    if (tPlayerData.iWinCount > tAIData.iWinCount)
                    {
                        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCharacterArr[CBattle_Manager::CHAR_PLAYER]);
                        if (nullptr != pPlayer)
                            pPlayer->Play_EndCutScene(m_tBattleInfo.bSabitoAI);
                    }
                    else
                    {
                        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pCharacterArr[CBattle_Manager::CHAR_AI]);
                        if (nullptr != pPlayer)
                            pPlayer->Play_EndCutScene();
                    }
                }
            }
        } 
        else
        {
            if (m_tBattleInfo.bBattleEnd)
            {
                if (true == pInstance->Get_SelectCutSceneModel()->Is_End() && false == m_bFadeOut)
                {
                    CRenderer* pRenderer = (CRenderer*)m_pCharacterArr[CHAR_PLAYER]->Get_Component(COM_RENDERER);
                    if (nullptr != pRenderer)
                    {
                        pRenderer->Start_FadeInOut(0.05f, false, 1);
                    }
                    m_bFadeOut = true;
                }
            }
        }
    }
    RELEASE_INSTANCE(CCutScene_Manager);
}

void CBattle_Manager::SetUp_OniLayer()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        list<CGameObject*> zakolist;
        //����(ù��° �׷�)
        if (1 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI1);
        //����(�ι�° �׷�)
        else if (2 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI2);
        //����(����° �׷�)
        else if (3 == m_iCurrentOniLayer)
            zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI3);

        // ���� ���� ����
        for (auto& pPreZako : m_pOniArr)
            Safe_Release(pPreZako);
        m_pOniArr.clear();
        for (auto& pZako : zakolist)
        {
            m_pOniArr.push_back(static_cast<CCharacter*>(pZako));
            Safe_AddRef(pZako);
            if (m_bIsStoryBattle)
                pZako->Set_Active(true);
        }

        // ���� ���� ���̾�� ����
        if (zakolist.empty() && true == m_bIsStoryBattle && ("IDLE" == m_pCharacterArr[CHAR_PLAYER]->Get_Animator()->Get_AnimationType()))
        {
            //pGameInstance->Stop_All();
            m_bIsStoryBattle = false;
            CTransform* pStoryTransform = m_pCharacterArr[CHAR_AI]->Get_Transform();
            CTransform* pBattleTransform = m_pCharacterArr[CHAR_PLAYER]->Get_Transform();
            if (nullptr != pStoryTransform || nullptr != pBattleTransform)
            {
                pStoryTransform->Bind_WorldMatrix(pBattleTransform->Get_WorldMatrix());
                pStoryTransform->Update_Navigation_Cell(pStoryTransform->Get_Pos());
            }

            CRenderer* pRenderer = (CRenderer*)m_pCharacterArr[CHAR_PLAYER]->Get_Component(COM_RENDERER);
            if (nullptr != pRenderer)
            {
                pRenderer->Start_FadeInOut(0.03f, false, 0);
            }

            if (m_iCurrentOniLayer < 3)
                m_iCurrentOniLayer++;
        }
    }
    RELEASE_INSTANCE(CGameInstance);
}


void CBattle_Manager::Push()
{
    if (LEVEL_GAMEPLAY == g_iCurrentLevel)
        m_pCharacterArr[CHAR_PLAYER]->Push(m_pCharacterArr[CHAR_AI]);
    else
    {
        m_pCharacterArr[CHAR_PLAYER]->Push(m_pCharacterArr[CHAR_AI]);
        m_pCharacterArr[CHAR_AI]->Push(m_pCharacterArr[CHAR_PLAYER]);
    }
}

void CBattle_Manager::InitBattle(const _double& _dTimeDelta)
{
    if (LEVEL_BATTLE == g_iCurrentLevel)
    {
        // ���� �ƽ� ���Ŀ� Ÿ�̸Ӱ� ���ư���.
        if (false == CCutScene_Manager::Get_Instance()->Is_GameInit())
            return;

        CCharacter::BATTLEDATA& tPlayerData = m_pCharacterArr[CHAR_PLAYER]->Get_BattleData();
        CCharacter::BATTLEDATA& tAIData = m_pCharacterArr[CHAR_AI]->Get_BattleData();

        // ��Ʋ �ʱ�ȭ�� ������ �� �����Ӹ� true�� ������ش�.
        if (true == m_tBattleInfo.bBattleInit)
            m_tBattleInfo.bBattleInit = false;
        if (true == m_tBattleInfo.bBattleStart)
            m_tBattleInfo.bBattleStart = false;

        // �ʱ� �ѹ��� ���´�.
        if (false == m_bInit)
        {
            m_bInit = true;
            m_tBattleInfo.bBattleInit = true;
        }

        // ���ο� �� �� 1�� �ڿ� ���ο� Ǯ�� �� ȸ���Ѵ�.
        if (0.f != m_tBattleInfo.tStartTimer.fStandardTime)
        {
            if (m_tBattleInfo.tStartTimer.Is_Over(_dTimeDelta))
            {
                // ���� ����
                Change_UpdateRatio(1.f);

                m_tBattleInfo.tStartTimer.fStandardTime = 0.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();
                tPlayerData.Init();
                tAIData.Init();
            }
            return;
        }

        // ���尡 ������ �Ѵ� IDLE ��쿡 ���� �����
        if ((true == tAIData.bBattleStop) && (true == tPlayerData.bBattleStop))
        {
            // ��Ʋ����
            if (tPlayerData.iWinCount >= 2 || tAIData.iWinCount >= 2)
            {
                tAIData.bBattleStop = false;
                tPlayerData.bBattleStop = false;
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tBattleTimer.bPass = true;
                return;
            }

            // HP�� Charge�������� ���� ȸ��
            if (tPlayerData.fCurHp < tPlayerData.fMaxHp || tAIData.fCurHp < tAIData.fMaxHp 
                || tPlayerData.fChargeValue < tPlayerData.fMaxChargeValue || tAIData.fChargeValue < tAIData.fMaxChargeValue)
            {
                tPlayerData.Increase_HP(1.5f);
                tAIData.Increase_HP(1.5f);
                tPlayerData.Increase_Charge(1.5f);
                tAIData.Increase_Charge(1.5f);
            }
            else if (("IDLE" == m_pCharacterArr[CHAR_PLAYER]->Get_Animator()->Get_AnimationType()) &&
                (("IDLE" == m_pCharacterArr[CHAR_AI]->Get_Animator()->Get_AnimationType()) || m_tBattleInfo.bSabitoAI))
            {
                tAIData.bBattleStop = false;
                tPlayerData.bBattleStop = false;
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tBattleTimer.Reset_Timer();
            }
            return;
        }

        // �÷��̾ �װų� Ÿ�̸Ӱ� ������ ���ο� �ɰ� �÷��̾ ���� ó�� ���ش�.
        if (false == m_tBattleInfo.tBattleTimer.bPass)
        {
            if (m_tBattleInfo.tBattleTimer.Is_Over(_dTimeDelta) || tPlayerData.fCurHp <= 0.f || tAIData.fCurHp <= 0.f)
            {
                m_tBattleInfo.bBattleInit = true;
                m_tBattleInfo.tStartTimer.fStandardTime = 1.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();

                tAIData.bBattleStop = true;
                tPlayerData.bBattleStop = true;
                Change_UpdateRatio(0.2f);
                if (tPlayerData.fCurHp < tAIData.fCurHp)
                    tAIData.Increase_WinCount();
                else
                    tPlayerData.Increase_WinCount();
            }
        }
    }
    else if (LEVEL_GAMEPLAY == g_iCurrentLevel || LEVEL_STORY == g_iCurrentLevel)
    {
        if (_dTimeDelta > 0.1 && false == m_bInit)
        {
            m_bInit = true;
            m_tBattleInfo.tStartTimer.fStandardTime = 0.2f;
            return;
        }

        if (true == m_tBattleInfo.bBattleStart)
            m_tBattleInfo.bBattleStart = false;

        if (0.f != m_tBattleInfo.tStartTimer.fStandardTime)
        {
            if (m_tBattleInfo.tStartTimer.Is_Over(_dTimeDelta))
            {
                m_tBattleInfo.bBattleStart = true;
                m_tBattleInfo.tStartTimer.fStandardTime = 0.f;
                m_tBattleInfo.tStartTimer.Reset_Timer();
            }
        }

    }

}

void CBattle_Manager::Change_UpdateRatio(_float fUpdateRatio)
{
    // ������Ʈ �ð� ����
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float* pUpdateTime = nullptr, * pLateUpdateTime = nullptr;
        pGameInstance->Out_LevelUpdateTime(pUpdateTime, pLateUpdateTime);
        if (nullptr != pUpdateTime)
            *pUpdateTime = fUpdateRatio;
        if (nullptr != pLateUpdateTime)
            *pLateUpdateTime = fUpdateRatio;
        m_tBattleInfo.fUpdateRatio = fUpdateRatio;
    }
    RELEASE_INSTANCE(CGameInstance);
}

void CBattle_Manager::Check_BattleDelayTimer(const _double& _dTimeDelta)
{
    // ������ Ÿ�̸Ӱ� �����Ǿ��ִٸ� ĳ���͸� �����.
    if (0.0 != m_tDelayTimer.fStandardTime)
    {
        _bool bIsDelay = false;
        // ����ġ�� ���� �ʾ����� ���� ������.
        bIsDelay = m_tDelayTimer.Is_Over(_dTimeDelta);

        // ĳ���͵� SetActive�� �����Ѵ�.
        m_pCharacterArr[CHAR_PLAYER]->Set_Active(bIsDelay);
        m_pCharacterArr[CHAR_AI]->Set_Active(bIsDelay);

        if (true == bIsDelay)
        {
            m_tDelayTimer.Reset_Timer();
            m_tDelayTimer.fStandardTime = 0.0;
        }
    }
    else
    {
        if (false == m_pCharacterArr[CHAR_PLAYER]->Is_Active())
        {
            // ĳ���͵� SetActive�� �����Ѵ�.
            m_pCharacterArr[CHAR_PLAYER]->Set_Active(true);
        }
        if (false == m_pCharacterArr[CHAR_AI]->Is_Active())
        {
            // ĳ���͵� SetActive�� �����Ѵ�.
            m_pCharacterArr[CHAR_AI]->Set_Active(true);
        }
    }
}

void CBattle_Manager::Check_StoryDelayTimer(const _double& _dTimeDelta)
{
    if (0.0 != m_tDelayTimer.fStandardTime)
    {
        _bool bIsDelay = false;
        // ����ġ�� ���� �ʾ����� ���� ������.
        bIsDelay = m_tDelayTimer.Is_Over(_dTimeDelta);

        // ĳ���͵� SetActive�� �����Ѵ�.
        for (auto& pOni : m_pOniArr)
        {
            if (pOni->Is_Hit())
                pOni->Set_Active(bIsDelay);
        }
        m_pCharacterArr[CHAR_PLAYER]->Set_Active(bIsDelay);

        if (true == bIsDelay)
        {
            m_tDelayTimer.Reset_Timer();
            m_tDelayTimer.fStandardTime = 0.0;
        }
    }
    else
    {
        // ĳ���͵� SetActive�� �����Ѵ�.
        for (auto& pOni : m_pOniArr)
        {
            if (false == pOni->Is_Active())
                pOni->Set_Active(true);
        }
        if (false == m_pCharacterArr[CHAR_PLAYER]->Is_Active())
            m_pCharacterArr[CHAR_PLAYER]->Set_Active(true);
    }
}

void CBattle_Manager::Free()
{
    for (_uint i = 0; i < CHAR_END; ++i)
        Safe_Release(m_pCharacterArr[i]);

    for (auto& pOni : m_pOniArr)
        Safe_Release(pOni);
    m_pOniArr.clear();
}
