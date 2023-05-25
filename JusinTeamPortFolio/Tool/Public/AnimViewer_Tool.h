#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Engine)
class CModel;
class CGameObject;
class CAnimationClip;
class CAnimator;
END

BEGIN(Tool)
class CAnimViewer_Tool final : public CImGuiClass_Tool
{
public:
    // �ִϸ��̼� Ŭ���� �̺�Ʈ Ÿ��
    enum CLIPEVENTTYPE
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
        EVENTTYPE_TRAIL,			// Ʈ����
        EVENTTYPE_END
    };

    // ���� Sorting �� ����ü�� ����ü��
    enum MyItemColumnID
    {
        // �ε��� ����
        MyItemColumnID_Index,
        // �̸�����
        MyItemColumnID_Name,
        // Animation�� Duration
        MyItemColumnID_Duration,
        // Animation�� TimePerSecond
        MyItemColumnID_TPS,
        // Channel�� ������ ī��Ʈ
        MyItemColumnID_FrameCount,
        // ���� Depth����
        MyItemColumnID_Depth,
        // ���� �θ� ����
        MyItemColumnID_Parent,
        // ���õƴ��� ���� ������ ����
        MyItemColumnID_Action,
        // �ִϸ������� ���鿡 ����ϰ� ��.
        MyItemColumnID_EventCount,
        MyItemColumnID_EnterRatio,
        MyItemColumnID_ExitRatio,
        MyItemColumnID_LerpSpeed,
        MyItemColumnID_InputRatio,
        MyItemColumnID_Trigger,
        // Ʈ�������� EnterState
        MyItemColumnID_EnterState,
        // �ִϸ������� EnterClip
        MyItemColumnID_EnterClip,
        // �ִϸ������� ExitClip
        MyItemColumnID_ExitClip,
        MyItemColumnID_StartRatio,
        MyItemColumnID_EndRatio,
        MyItemColumnID_EndTime,
        MyItemColumnID_AnimSpeed,
        MyItemColumnID_EventType,
        MyItemColumnID_AnimationType
    };

    struct MyItem
    {
        int         Index;
        char        Name[MAX_PATH];
        int         ParentIndex;
        char        ParentName[MAX_PATH];
        _float      LerpSpeed;
        _double     Duration;
        _double     Tps;
        _double     dStartRatio;
        _double     dEndRatio;
        _double     dEndTime;
        _float      fValue;
        _float      fAnimSpeed;
        _uint       iEventType;
        _bool       bIsActive;
        _float4     vEventPos;
        _double     EnterRatio;
        _double     ExitRatio;
        _double     TriggerRatio;
        _uint       FrameCount;
        _uint       Depth;
        _bool       Action;
        _bool       Loop;
        _bool       RootBone;
        _int        EventCount;
        _int        EnterState;
        char        Trigger[MAX_PATH];
        char        MainAnim[MAX_PATH];
        char        SubAnim[MAX_PATH];
        char        EnterAnim[MAX_PATH];
        char        ExitAnim[MAX_PATH];
        char        EventText[MAX_PATH];
        char        AnimationType[MAX_PATH];


        static const ImGuiTableSortSpecs* s_current_sort_specs;

        // Compare function to be used by qsort()
        static int CompareWithSortSpecs(const void* lhs, const void* rhs)
        {
            const MyItem* a = (const MyItem*)lhs;
            const MyItem* b = (const MyItem*)rhs;
            for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
            {
                // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
                // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
                const  ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
                int delta = 0;
                switch (sort_spec->ColumnUserID)
                {
                case MyItemColumnID_Index:          delta = (a->Index - b->Index);              break;
                case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));         break;
                case MyItemColumnID_EnterClip:      delta = (strcmp(a->EnterAnim, b->EnterAnim));         break;
                case MyItemColumnID_ExitClip:       delta = (strcmp(a->ExitAnim, b->ExitAnim));         break;
                case MyItemColumnID_Duration:       delta = (_int)(a->Duration - b->Duration);  break;
                case MyItemColumnID_TPS:            delta = (_int)(a->Tps - b->Tps);            break;
                case MyItemColumnID_FrameCount:     delta = (a->FrameCount - b->FrameCount);    break;
                case MyItemColumnID_Depth:          delta = (a->Depth - b->Depth);              break;
                case MyItemColumnID_Parent:         delta = (a->ParentIndex - b->ParentIndex);  break;
                case MyItemColumnID_Action:         delta = (_int)(a->Action - b->Action);      break;
                case MyItemColumnID_EnterState:     delta = (a->EnterState - b->EnterState);      break;
                case MyItemColumnID_EventCount:     delta = (a->EventCount - b->EventCount);      break;
                case MyItemColumnID_EnterRatio:     delta = (_int)(a->EnterRatio - b->EnterRatio);      break;
                case MyItemColumnID_ExitRatio:      delta = (_int)(a->ExitRatio - b->ExitRatio);      break;
                case MyItemColumnID_AnimSpeed:      delta = (_int)(a->fAnimSpeed - b->fAnimSpeed);      break;
                case MyItemColumnID_LerpSpeed:      delta = (_int)(a->LerpSpeed - b->LerpSpeed);      break;
                case MyItemColumnID_InputRatio:     delta = (_int)(a->TriggerRatio - b->TriggerRatio);      break;
                case MyItemColumnID_Trigger:        delta = (strcmp(a->Trigger, b->Trigger));       break;
                case MyItemColumnID_EventType:      delta = (_int)(a->iEventType - b->iEventType);       break;
                case MyItemColumnID_AnimationType:  delta = (strcmp(a->AnimationType, b->AnimationType));       break;
                default: IM_ASSERT(0); break;
                }
                if (delta > 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
                if (delta < 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
            }

            // qsort() is instable so always return a way to differenciate items.
            // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
            return (a->Index - b->Index);
        }
    };


private:
    explicit CAnimViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CAnimViewer_Tool(void) = default;
public:
    virtual HRESULT NativeConstruct(void);
    virtual HRESULT NativeConstruct_InnerValue(void);
    virtual HRESULT Tick(const _double& _dTimeDelta);
    virtual HRESULT Late_Tick(const _double& _dTimeDelta);
    virtual HRESULT Release_InnerValue(void);

private:
    virtual void Show_Window(void) override;

private:
    //	AnimViewerModel ��ü�� ���� �Լ� ( ó�� ���� �� ���� )
    HRESULT Create_AnimObject();
    //	AnimViewerModel ��ü�� ���� ��ü�Ѵ�.
    HRESULT Set_Model();
    //  AnimViewerModel ��ü�� �ִϸ��̼��� ��ü�Ѵ�.
    HRESULT Set_Animation();
    //  AnimationClip �ϳ��� Create �Ͽ� ���� ������Ʈ�� �־��ش�.
    HRESULT Add_AnimationClip();
    // ��ϵǾ��ִ� ��� AnimationClip�� ����Ѵ�.    
    HRESULT Add_AllAnimationClip();
    //  �𵨿��� Animator�� �����Ѵ�.
    HRESULT Set_Animator(_wstring _strPath);
    //  AnimationClip �ϳ��� Delete�Ѵ�.
    HRESULT Delete_AnimationClip(_string strName);
    //  ���� �ִϸ��̼� �׽�Ʈ ���� ���� �ִϸ����� ������ �����մϴ�.
    HRESULT Save_Animator();
    //  �ִϸ��̼� Ŭ���� �����մϴ�.
    HRESULT Save_AnimationClip();
    //  �ִϸ����� ������ �ҷ��ɴϴ�.
    HRESULT Load_Animator();
    //  �ִϸ��̼� Ŭ�� ������ �ҷ��ɴϴ�.
    HRESULT Load_AnimationClip();
    //  ������ �ʿ��� �������� ������Ʈ�մϴ�.
    HRESULT Initialize_ToolData();
    // ������ �ִϸ������� �����͸� �ҷ��ͼ� ���� Animator�� Clip����, Transition������ ��Ĩ�ϴ�.
    HRESULT Merge_Animator(_wstring _strPath);
    // �ִϸ����Ϳ��� ���� �̸��� ���� �ִϸ��̼� Ŭ���� �̺�Ʈ ������ �����ɴϴ�.
    HRESULT Merge_AnimatorEvent(_wstring _strPath);
    // ================== ���� �߰� ==================
    HRESULT Attach_SwordModel();
    HRESULT Attach_ShelteModel();
private:
    // ���� ������ ������Ʈ
    CGameObject* m_pAnimObject = nullptr;
private:
    // �ִϸ��̼� �� â�� ����
    WINDESC			m_tWinDesc;
    // ��θ� ã�Ƽ� ������ ���̳ʸ� ������ ������ ���� ����Ʈ
    vector<FILEDATA> m_BinaryList;
    // ��θ� ã�Ƽ� ������ �ִϸ����� ���嵥���� ����Ʈ
    vector<FILEDATA> m_SavedAnimatorList;
    // ��θ� ã�Ƽ� ���� �� ���
    vector<CModel*> m_ModelList;
    // ���� ������ �ִϸ��̼� �� �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
    _int			m_iModelIndex = -1;
    // ���� ������ ���� ���� �ִϸ��̼� �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
    _int			m_iAnimationIndex = -1;
    // ������ �ִϸ����� ���
    _int            m_iAnimatorIndex = -1;
    // ���� ����ǰ��ִ� �ִϸ��̼��� �����ִ� Channel�� ���� ������ ä�� -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
    _int			m_iChannelIndex = -1;
    // ���� ������ ���� ���� Bone �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
    _int			m_iBoneIndex = -1;
    // ���� ������ ���� ���� Render���� �׸����ִ� �޽� �����̳� �ε��� ����Ʈ
    vector<_uint>   m_MeshContainerList;
    // �ִϸ��̼� �� ���� ����ִ� �̺�Ʈ ����� ���� ������ �̺�Ʈ �ε���
    _int            m_iEventIndex = -1;
    // ���� ���õ� Ʈ������ �ε���
    _int            m_iTransitionIndex = -1;
    // Channel�� KeyFrame�� 2�� ��츦 ������ �� ����ϱ����� bool ����
    _bool			m_bIsFiltered = false;
    // ����Ʈ�� ������Ʈ�뵵.
    _bool           m_bIsUpdate = false;
    // �̺�Ʈ�� ������Ʈ �뵵
    _bool           m_bIsEventUpdate = false;
    // �ݶ��̴� Ÿ���� �Է��ؼ� ����� �� ��� �� ����
    _string         m_strAnimType = "";
    // Battle ���� ���� �ؽ�Ʈ ����
    vector<_string> m_SoundNameList;
    // ���� �ؽ�Ʈ �˻� ����Ʈ
    vector<_string> m_ResearchSoundList;
private:
    /*  ���� �߰�*/
    class CItem_Tool*      m_pSwordTool = nullptr;
    class CItem_Tool*      m_pShelteTool = nullptr;
    // ItemModelList
    vector<CModel*>     m_ItemModelList;
    // �ִϸ��̼� Ŭ������Ʈ ������Ʈ�� ����
    _bool           m_bIsClipUpdate = false;
    // ������ ���� AnimationClip
    CAnimationClip* m_pAnimationClip = nullptr;
    // ���� ��ϵ� Animator
    CAnimator*      m_pAnimatorCom = nullptr;
    // ���� ��ϵ� Animator�� �����ִ� AniamtionClip ����Ʈ
    vector<CAnimationClip*> m_ClipList;
public:
    static CAnimViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free(void) override;
};
END

