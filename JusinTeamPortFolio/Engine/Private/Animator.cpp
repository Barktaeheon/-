#include "Animator.h"
#include "AnimationClip.h"
#include "Animation.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "GameInstance.h"

CAnimator::CAnimator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CComponent(_pDevice, _pContext)
{
}

CAnimator::CAnimator(const CAnimator& _rhs)
	:CComponent(_rhs)
{
}

 _bool CAnimator::Is_RootBone()
{
	if (nullptr == m_pCurrentClip)
		 return false;

	return m_pCurrentClip->Is_RootBone();
}

 _bool CAnimator::Is_BoneTransform()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_BoneTransform();
 }

 _bool CAnimator::Is_Gravity()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_Gravity();
 }

 _bool CAnimator::Is_MeshActive()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_MeshActive();
 }

 _bool CAnimator::Is_TrailActive()
 {
	 if (nullptr == m_pCurrentClip)
		 return false;

	 return m_pCurrentClip->Is_TrailActive();
 }

 _float2 CAnimator::Get_TrailValues()
 {
	 if (nullptr == m_pCurrentClip)
		 return _float2(0.f, 0.f);

	 return m_pCurrentClip->Get_TrailValues();
 }

void CAnimator::Set_Transform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	m_pOwnerTransformCom = pTransform;
}

void CAnimator::Set_Model(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	// ���� �̹� ����־��ٸ� Safe_Release();
	Safe_Release(m_pModelCom);

	// �𵨰�ü�� pModel��
	m_pModelCom = pModel;

	// ���� ���۷����� ������Ų��.
	Safe_AddRef(pModel);

	// ���� ���� �ѹ� �ʱ�ȭ���ش�.
	Update_InitBoneState();
}

CAnimationClip* CAnimator::Get_NextAnimationClip()
{
	if (nullptr == m_pNextClip)
		return nullptr;

	return m_pNextClip;
}

CAnimationClip* CAnimator::Get_AnimationClip(_string _strName)
{
	// �̸��� ���� �ִϸ��̼� Ŭ���� ã�Ҵٸ� �ش� �ִϸ��̼� Ŭ���� �������ش�.
	if (0 != m_AnimationClips.size())
	{
		for (auto& pClip : m_AnimationClips)
		{
			if (_strName == pClip->Get_Name())
				return pClip;			
		}
	}
	return nullptr;
}

_string CAnimator::Get_CurrentTrigger()
{
	if (nullptr != m_pCurTransition)
		return m_pCurTransition->InputTrigger.strInput;
	else
		return _string("");
}

_float CAnimator::Get_MoveDistance()
{
	if (nullptr == m_pCurrentClip)
		return 0.f;

	// �ִϸ��̼� �̵����� �޾ƿ´�.
	return m_pCurrentClip->Get_MoveDistance();
}

_float	CAnimator::Get_MoveForce()
{
	_float fMoveForce;

	if (nullptr == m_pCurrentClip)
		fMoveForce = 0.f;
	else
		fMoveForce = m_pCurrentClip->Get_BoneMoveForce();

	return fMoveForce;
}

_vector CAnimator::Get_ColliderDir()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vDir = m_pCurrentClip->Get_ColliderDir();

	return m_pCurrentClip->Get_ColliderDir();
}

_vector CAnimator::Get_ColliderPos()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vPos = m_pCurrentClip->Get_ColliderPos();

	return m_pCurrentClip->Get_ColliderPos();
}

_float3 CAnimator::Get_RootBoneLook()
{
	return m_vRootBoneRotation;
}

_vector CAnimator::Get_ColliderScale()
{
	if (nullptr == m_pCurrentClip)
		return XMVectorZero();

	_vector vScale = m_pCurrentClip->Get_ColliderScale();

	return m_pCurrentClip->Get_ColliderScale();
}

_string CAnimator::Get_MainAnimationName()
{
	return m_pCurrentClip->Get_MainAnimationName();	
}

_string CAnimator::Get_SubAnimationName()
{
	return m_pCurrentClip->Get_SubAnimationName();
}

const _string& CAnimator::Get_AnimationType()
{
	// ���� ���� Ŭ���� �������̶�� ���� �ٲ� �ִϸ��̼��� �ִ��� Ȯ�� �� , ���� �ִϸ��̼��� Ÿ���� ��ȯ�Ѵ�.
	if (true == m_pCurrentClip->Is_SupplementSituation())
	{
		if (nullptr != m_pNextClip)		
			return m_pNextClip->Get_AnimationType();		
	}
	return m_pCurrentClip->Get_AnimationType();
}

void CAnimator::Set_Stop()
{
	if (nullptr == m_pCurrentClip)
		return;

	m_bIsPlay = false;
	// �����͸� �ʱ�ȭ�Ѵ�.
	m_pCurrentClip->Initialize_Data();
	Update_InitBoneState();
}

void CAnimator::Set_TimeAcc(_double dTimeAcc)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_TimeAcc(dTimeAcc);
}

void CAnimator::Set_CurrentAnimationClip(CAnimationClip* pClip)
{
	pClip->Initialize_Data();
	if(nullptr != m_pCurrentClip)
		m_pCurrentClip->Initialize_Data();
	m_pCurrentClip = pClip;
	if (nullptr != m_pNextClip)
		m_pNextClip = nullptr;
	Update_TransitionList();
	m_bIsPlay = true;
	m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);

	// ���� Ŭ���� OwnerTransform�� ���ٸ� ä���ش�.
	if (nullptr == m_pCurrentClip->Get_OwnerTransform())
		return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
	if (nullptr != m_pOwnerTransformCom)
		m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
}

void CAnimator::Set_CurrentAnimationClip(_string _strName)
{
	for (auto& pClip : m_AnimationClips)
	{
		// �̸��� ���ٸ� �ش� Ŭ���� ���� Ŭ������ �����Ѵ�.
		if (_strName == pClip->Get_Name())
		{
			pClip->Initialize_Data();
			if (nullptr != m_pCurrentClip)
				m_pCurrentClip->Initialize_Data();			
			m_pCurrentClip = pClip;
			if (nullptr != m_pNextClip)
				m_pNextClip = nullptr;
			Update_TransitionList();
			m_bIsPlay = true;
			m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);

			// ���� Ŭ���� OwnerTransform�� ���ٸ� ä���ش�.
			if (nullptr == m_pCurrentClip->Get_OwnerTransform())
				return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
			// Ʈ������ ������Ʈ�� �����ִٸ� ���� ��ü�� ���� ��ġ�� �޾ƿ´�.
			if (nullptr != m_pOwnerTransformCom)
				m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
		}
	}
}

void CAnimator::Set_NextDirectAnimationClip(CAnimationClip* pClip)
{
	m_pNextDirectClip = pClip;
}

void CAnimator::Set_NextDirectAnimationClip(const _string _strName)
{
	for (auto& pClip : m_AnimationClips)
	{
		// �̸��� ���ٸ� �ش� Ŭ���� ���� Ŭ������ �����Ѵ�.
		if (_strName == pClip->Get_Name())
		{
			m_pNextDirectClip = pClip;	
			return;
		}
	}
}

void CAnimator::Set_AnimationClipToEntry()
{
	for (auto& transition : m_TransitionList)
	{
		// Entry �ִϸ��̼��̶�� ���� Ŭ������ �������ش�.
		if (ENTRY == transition.tEnterState)		
			Set_CurrentAnimationClip(transition.pExitClip);		
	}
}

vector<CHierarchyNode*> CAnimator::Get_WeaponBoneList()
{
	return m_WeaponBoneList;
}

CHierarchyNode* CAnimator::Get_WeaponBone()
{
	if (0 == m_WeaponBoneList.size())
		return nullptr;

	return m_WeaponBoneList[m_pCurrentClip->Get_WeaponBoneIndex()];
}

_double CAnimator::Get_Ratio()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;

	return m_pCurrentClip->Get_Ratio();
}

_double CAnimator::Get_Duration()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_Duration();
}

_double CAnimator::Get_TickPerSecond()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_TickPerSecond();
}

_double CAnimator::Get_TimeAcc()
{
	if (nullptr == m_pCurrentClip)
		return 0.0;
	return m_pCurrentClip->Get_TimeAcc();
}

_float CAnimator::Get_AnimationSpeed()
{
	if (nullptr == m_pCurrentClip)
		return 0.0f;
	return m_pCurrentClip->Get_AnimationSpeed();
}

_matrix CAnimator::Get_RootBoneMatrix()
{
	return XMLoadFloat4x4(&m_vRootBoneMatrix);
}

const _bool& CAnimator::Is_AttackColliderActive()
{
	return m_pCurrentClip->Is_AttackColliderActive();
}

const _float& CAnimator::Get_PushedForce()
{
	return m_pCurrentClip->Get_PushedForce();
}

const _float& CAnimator::Get_Height()
{
	return m_pCurrentClip->Get_Height();
}

const _double& CAnimator::Get_EventPlayTime()
{
	return m_pCurrentClip->Get_EventPlayTime();
}

const _bool& CAnimator::Is_EventPlay()
{
	return m_pCurrentClip->Is_EventPlay();
}

void CAnimator::Play_OneFrame(_double TimeDelta)
{
	m_bIsPlay = false;

	// ������ ������Ʈ�Ѵ�.
	if (0 < Get_TimeAcc() + TimeDelta)
		Update_AnimationClip(TimeDelta);
	else if (0 >= Get_TimeAcc() + TimeDelta)
	{
		Set_TimeAcc(0.0);
		// 0.0���� �ִϸ��̼� ������Ʈ�� ������ ������ �������ش�.
		m_pCurrentClip->Update_TransformMatrices(0.0);

		// �� ��ü�� ������ ������Ʈ�Ѵ�.
		if (nullptr != m_pModelCom)
		{
			vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
			/* �θ�κ��� �ڽĻ����� �������� �����Ѵ�.(CombinedTransformationMatrix) */
			for (auto& pHierarchyNode : vecHierachyNodes)
				pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
}

void CAnimator::Set_Ratio(_double dRatio)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_Ratio(dRatio);
	// �ִϸ��̼� ���൵�� ������ �������� �ִϸ��̼� ������ �ʱ�ȭ�Ѵ�.
		// 0.0���� �ִϸ��̼� ������Ʈ�� ������ ������ �������ش�.
	m_pCurrentClip->Update_TransformMatrices(0.0);

	// �� ��ü�� ������ ������Ʈ�Ѵ�.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* �θ�κ��� �ڽĻ����� �������� �����Ѵ�.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
			pHierarchyNode->Update_CombinedTransformationMatrix();
	}
	//Update_InitBoneState();	
}

void CAnimator::Add_AnimationClip(CAnimationClip* _pClip)
{
	if (nullptr == _pClip)
		return;

	for (auto& pClip : m_AnimationClips)
	{
		if (pClip->Get_Name() == _pClip->Get_Name())
		{
			Safe_Release(_pClip);
			return;
		}
	}
	if(nullptr != m_pOwnerTransformCom)
		_pClip->Set_OwnerTransform(m_pOwnerTransformCom);

	// Clone�� ��ü���� �ٷ� �����ϱ����� Safe_Release���� �۾��� �н��Ѵ�.
	m_AnimationClips.push_back(_pClip);
}

void CAnimator::Delete_AnimationClip(_string strName)
{
	for (int i = 0; i < m_AnimationClips.size(); ++i)
	{
		if (m_AnimationClips[i]->Get_Name() == strName)
		{
			// Safe_Release�� Ŭ���� �����ش�.
			Safe_Release(m_AnimationClips[i]);
			// ����Ʈ���� �����Ѵ�
			m_AnimationClips.erase(m_AnimationClips.begin() + i);
		}
	}
}

void CAnimator::Make_Transition()
{
	TRANSITION transition;

	transition.tEnterState = DEFAULT;
	transition.pEnterClip = nullptr;
	transition.pExitClip = nullptr;
	
	// Ʈ������ ����Ʈ�� Ʈ�������� �߰��Ѵ�.
	m_TransitionList.push_back(transition);
}


_bool CAnimator::Input_Trigger(_string _trigger)
{
	if (nullptr == m_pCurrentClip)
		return false;

	// �÷������� �ƴҶ��� Ʈ���� �Է��� ���� ����
	if (false == m_bIsPlay)
		return false;

	INPUTTRIGGER inputTrigger;
	inputTrigger.dAnimRatio = m_pCurrentClip->Get_Ratio();
	inputTrigger.strInput = _trigger;

	// AnyState�� ��� ( ���� �ǰ��� ��� ) Exit�� ���� ���� Trigger�� ���� Default�� Transition�� ������ ��� Default�� �ѱ������ ����ش�.
	TRANSITION* pTransition = nullptr;

	// �޾ƿ� Ʈ���Ÿ� Transition���� AnyState ���� �ű� �� �ִ� Ŭ������ �˻��մϴ�.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// �ƹ� ���¿����� �� �� �ִ� Ŭ���߿� Ʈ���Ű� ���� Ŭ���� ã�´�.
		if (ANYSTATE == Transition.tEnterState)
		{
			if (Transition.Compare_Trigger(inputTrigger))
			{
				// AnyState�� Transition�� ����ش�.
				pTransition = &Transition;
				
				for (auto& findTransition : m_ActiveTransitionList)
				{
					// Ʈ���Ű� �������� Default �����ΰ��� �ִٸ� ���� ������� �ִϸ��̼ǰ� �ش� Ʈ�������� EnterClip�� ���Ͽ� ��� ������ �����Ұ�� ��ü�Ѵ�.
					if (findTransition.InputTrigger.strInput == pTransition->InputTrigger.strInput)
					{
						if (DEFAULT == findTransition.tEnterState)
						{
							if (m_pCurrentClip == findTransition.pEnterClip)
							{
								// ���� �ִϸ��̼� Ŭ���� �������ش�.
								m_pNextClip = findTransition.pExitClip;
								m_pCurTransition = &findTransition;
								return true;
							}
							continue;
						}						
						continue;
					}
					continue;
				}
				// ���� �ִϸ��̼� Ŭ���� �������ش�.
				m_pNextClip = Transition.pExitClip;
				m_pCurTransition = &Transition;
				return true;
			}
		}
		// AnyState�� �ƴ� ��쿡 ��� ������ �����ߴٸ� Ŭ���� �����Ѵ�.
		else
		{
			//// �̹� �ٲ� Ŭ���� ���� ��쿡�� AnyState�� ������ �ٸ� Ʈ���Ÿ� ���� �ʴ´�.
			//if (nullptr != m_pNextClip)
			//	return false;

			// Ʈ������ ������ ������ ���
			if (Transition.Compare_Trigger(inputTrigger) && m_pNextClip != m_pCurrentClip)
			{
				// ChangeRatio������ ������ ������쿡 �۵��Ѵٸ� üũ �� ��ü�Ѵ�.
				if (true == Transition.bIsOverChangeRatio)
				{
					if (inputTrigger.dAnimRatio >= Transition.dChangeRatio)
					{
						// ���� �ִϸ��̼� Ŭ���� �������ش�.
						m_pNextClip = Transition.pExitClip;
						m_pCurTransition = &Transition;
						return true;
					}
					else
					{
						// �ٲ� �ִϸ��̼��� ������ Ʈ���Ÿ���Ʈ�� �߰��Ѵ�.
						// ���� Ʈ���Ű� �ִ��� �˻��ϴ� �Ұ�
						_bool isSame = false;
						for (auto& trigger : m_InputTriggerList)
						{
							if (trigger.strInput == inputTrigger.strInput)
							{
								trigger.dAnimRatio = inputTrigger.dAnimRatio;
								isSame = true;
							}
						}
						// ������ ���ٸ� �����͸� �־��ش�.
						if (isSame == false)
						{
							m_InputTriggerList.push_back(inputTrigger);
							return true;
						}
					}
				}
				// ChangeRatio������ ������ ������쿡 �۵��Ѵٸ� üũ �� ��ü�Ѵ�.
				else
				{
					if (inputTrigger.dAnimRatio <= Transition.dChangeRatio)
					{
						// ���� �ִϸ��̼� Ŭ���� �������ش�.
						m_pNextClip = Transition.pExitClip;
						m_pCurTransition = &Transition;
						return true;
					}
					else
					{
						// �ٲ� �ִϸ��̼��� ������ Ʈ���Ÿ���Ʈ�� �߰��Ѵ�.
						// ���� Ʈ���Ű� �ִ��� �˻��ϴ� �Ұ�
						_bool isSame = false;
						for (auto& trigger : m_InputTriggerList)
						{
							if (trigger.strInput == inputTrigger.strInput)
							{
								trigger.dAnimRatio = inputTrigger.dAnimRatio;
								isSame = true;
							}
						}
						// ������ ���ٸ� �����͸� �־��ش�.
						if (false == isSame)
						{
							m_InputTriggerList.push_back(inputTrigger);
							return true;
						}
					}
				}
			}			
		}
	}

	// ���� �ٲ��� �ʾҴٸ� false�� �����Ѵ�.
	return false;
}
void CAnimator::Set_RootBone(const _bool& _bIsRootBone)
{
	m_pCurrentClip->Set_RootBone(_bIsRootBone);
}
void CAnimator::Set_RootBoneName(_string _strRootBoneName)
{
	m_pCurrentClip->Set_RootBoneName(_strRootBoneName);
}

void CAnimator::Set_TargetDistance(const _float& _fTargetDistance)
{
	if (nullptr == m_pCurrentClip)
		return;

	m_pCurrentClip->Set_TargetDistance(_fTargetDistance);
}
void CAnimator::Add_WeaponBone(CHierarchyNode* pBone)
{
	m_WeaponBoneList.push_back(pBone);

	Safe_AddRef(pBone);
}
void CAnimator::Remove_WeaponBone(_uint iWeaponBoneIndex)
{
	if (iWeaponBoneIndex > m_WeaponBoneList.size())
		return;

	// �����͸� �����.
	m_WeaponBoneList.erase(m_WeaponBoneList.begin() + iWeaponBoneIndex);
}
#ifdef _DEBUG
// ����׿� ������Ÿ�� ���� �Լ� 
HRESULT CAnimator::NativeConstruct_Prototype()
{
	return S_OK;
}

#endif // _DEBUG

/// <summary>
/// ��θ� �޾ƿͼ� ������Ÿ���� ������ش�.
/// </summary>
/// <param name="strPath">�ִϸ����� ������ ���� ���</param>
/// <returns></returns>
HRESULT CAnimator::NativeConstruct_Prototype(_wstring strPath)
{
	return S_OK;
}

HRESULT CAnimator::NativeConstruct_Clone(void* _pArg)
{
	if (nullptr != _pArg)
	{
		ANIMATORDESC animDesc = *static_cast<ANIMATORDESC*>(_pArg);

		_ulong dwByte = 0;
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		PATHS paths = { L"Data", L"Animator", animDesc.strFilePath};
		FILEGROUP* AnimatorGroup = pGameInstance->Find_Folder(&paths);
		if (nullptr == AnimatorGroup)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		// ���� �����Ѵ�.
		Set_Model(animDesc.pModel);
		// �ִϸ����� ������Ʈ�� ����ִ� ��ü�� Ʈ�������� �־��ش�.
		if (nullptr != animDesc.pTransform)
			Set_Transform(animDesc.pTransform);

		_wstring strPath = AnimatorGroup->FindData(animDesc.strFilePath)->strFilePath;

		// ��θ� �޾ƿ´�.

		RELEASE_INSTANCE(CGameInstance);

		HANDLE hFile = CMethod::Begin_Load(strPath.c_str());

		_uint iClipSize = 0;
		ReadFile(hFile, &iClipSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint i = 0; i < (_uint)iClipSize; ++i)
		{
			CAnimationClip* pClip = CAnimationClip::Create();
			if (nullptr == pClip)
				return E_FAIL;

			// �ִϸ��̼� Ŭ���� �̸��� �޾ƿ´�.
			pClip->Set_Name(CMethod::Load_File_String(hFile, dwByte));
		
			// ���� �ִϸ��̼��� �޾Ƽ� �ֽ��ϴ�.
			_string strMainAnim = CMethod::Load_File_String(hFile, dwByte);
			_int iStart = (_int)strMainAnim.find("(", 0) + 1;
			_int iEnd = (_int)strMainAnim.find(")", 0) - iStart;
			strMainAnim = strMainAnim.substr(iStart, iEnd);
			pClip->Set_MainAnimation(animDesc.pModel->Get_Animation(strMainAnim));
			// ���� �ִϸ��̼��� �޾Ƽ� �ֽ��ϴ�.
			_string strSubAnim = CMethod::Load_File_String(hFile, dwByte);
			pClip->Set_SubAnimation(animDesc.pModel->Get_Animation(strSubAnim));

			// ���� ���θ� �����Ѵ�.
			_bool isLoop = false;
			ReadFile(hFile, &isLoop, sizeof(_bool), &dwByte, nullptr);
			pClip->Set_Loop(isLoop);

			// ��Ʈ�� ���θ� �����Ѵ�.
			_bool isRootBone = false;
			ReadFile(hFile, &isRootBone, sizeof(_bool), &dwByte, nullptr);
			pClip->Set_RootBone(isRootBone);

			// ��Ʈ�� �̸��� �����Ѵ�.
			pClip->Set_RootBoneName(CMethod::Load_File_String(hFile, dwByte));

			// ���ǵ尪�� �ҷ��´�.
			_float fAnimSpeed = 0.f;
			ReadFile(hFile, &fAnimSpeed, sizeof(_float), &dwByte, nullptr);
			pClip->Set_AnimationSpeed(fAnimSpeed);

			// �ִϸ��̼� Ÿ�԰��� �����´�.
			pClip->Set_AnimationType(CMethod::Load_File_String(hFile, dwByte));

			// �̺�Ʈ ������ �����Ѵ�.
			_uint iEventSize = 0;
			ReadFile(hFile, &iEventSize, sizeof(_uint), &dwByte, nullptr);

			vector<CAnimationClip::ANIMEVENTDESC> eventList;

			// �̺�Ʈ�� �����Ѵ�.
			for (_uint k = 0; k < iEventSize; ++k)
			{
				CAnimationClip::ANIMEVENTDESC event;

				// �̺�Ʈ Ÿ���� �ҷ��´�.
				ReadFile(hFile, &event.tEventType, sizeof(_uint), &dwByte, nullptr);
				// �̺�Ʈ �̸��� �ҷ��´�
				event.strName = CMethod::Load_File_String(hFile, dwByte);
				// �̺�Ʈ�� ���۵Ǵ� �ð�����
				ReadFile(hFile, &event.dStartRatio, sizeof(_double), &dwByte, nullptr);
				// �̺�Ʈ�� ������ �ð�����
				ReadFile(hFile, &event.dEndRatio, sizeof(_double), &dwByte, nullptr);
				// �̺�Ʈ�� ���� �ð� ( ���� �Ŀ� �ð��ʷ� ������ ������ )
				ReadFile(hFile, &event.dEndTime, sizeof(_double), &dwByte, nullptr);
				// Value�� ����
				ReadFile(hFile, &event.fValue, sizeof(_float), &dwByte, nullptr);
				// AddValue�� ����
				ReadFile(hFile, &event.fAddValue, sizeof(_float), &dwByte, nullptr);
				// ���� ��ġ
				ReadFile(hFile, &event.vEventPos, sizeof(_float4), &dwByte, nullptr);
				// �̺�Ʈ�� Dir
				ReadFile(hFile, &event.vEventDir, sizeof(_float4), &dwByte, nullptr);
				// �̺�Ʈ������ Values
				ReadFile(hFile, &event.vEventValues, sizeof(_float4), &dwByte, nullptr);
				// ����� ���� �̸�
				event.strEventText = CMethod::Load_File_String(hFile, dwByte);

				eventList.push_back(event);
			}
			// ������ Ŭ������ �̺�Ʈ����Ʈ�� �־��ش�.
			pClip->Set_Event(eventList);
			// ������ Ŭ������ ���� �� ��ü�� ��� ����Ʈ�� �־��ش�.
			pClip->Set_HierarchyNodeList(m_pModelCom->Get_HierachyNodeList());
			this->Add_AnimationClip(pClip);
		}

		// ����� ����� �����Ѵ�.
		_uint iWeaponBoneSize = 0;
		ReadFile(hFile, &iWeaponBoneSize, sizeof(_uint), &dwByte, nullptr);

		for (_uint j = 0; j < iWeaponBoneSize; ++j)
		{
			// ������� ����� ���� ����ش�.
			CHierarchyNode* pBone = animDesc.pModel->Get_HierarchyNodes(CMethod::Load_File_String(hFile, dwByte).c_str());
			m_WeaponBoneList.push_back(pBone);
			//Safe_AddRef(pBone);
		}

		while (true)
		{
			TRANSITION transition;

			// Ʈ�������� EnterState
			ReadFile(hFile, &transition.tEnterState, sizeof(_uint), &dwByte, nullptr);

			if (0 == dwByte)
				break;

			// EnterClip�� Ʈ�����ǿ� �־��ش�.
			transition.pEnterClip = this->Get_AnimationClip(CMethod::Load_File_String(hFile, dwByte));
			// ExitClip�� Ʈ�����ǿ� �־��ش�.
			transition.pExitClip = this->Get_AnimationClip(CMethod::Load_File_String(hFile, dwByte));
			
			// Ʈ�������� EnterRatio
			ReadFile(hFile, &transition.dEnterRatio, sizeof(_double), &dwByte, nullptr);
			// Ʈ�������� ExitRatio
			ReadFile(hFile, &transition.dExitRatio, sizeof(_double), &dwByte, nullptr);
			// Ʈ�������� ChangeRatio
			ReadFile(hFile, &transition.dChangeRatio, sizeof(_double), &dwByte, nullptr);
			// Ʈ�������� OverChange �Ұ�
			ReadFile(hFile, &transition.bIsOverChangeRatio, sizeof(_bool), &dwByte, nullptr);
			// Ʈ�������� LerpSpeed
			ReadFile(hFile, &transition.dLerpSpeed, sizeof(_double), &dwByte, nullptr);
			// Ʈ�������� Trigger		
			transition.InputTrigger.strInput = CMethod::Load_File_String(hFile, dwByte);
			// Ʈ�������� Trigger Input Ratio
			ReadFile(hFile, &transition.InputTrigger.dAnimRatio, sizeof(_double), &dwByte, nullptr);
			// Ʈ�������� InputRatio �̻�, ���� �Ǵ�
			ReadFile(hFile, &transition.InputTrigger.bIsOverRatio, sizeof(_bool), &dwByte, nullptr);

			this->m_TransitionList.push_back(transition);
		}		
		CloseHandle(hFile);
	}

	for (auto& transition : m_TransitionList)
	{
		// �ش� Ʈ�������� ENTRY ���
		if (ENTRY == transition.tEnterState)
		{
			if (nullptr != transition.pExitClip)
			{
				m_pCurrentClip = transition.pExitClip;
				m_bIsPlay = true;
				m_bIsTransitionUpdate = true;
				break;
			}
		}
	}

	return S_OK;
}

void CAnimator::Tick(const _double& _dTimeDelta)
{
	// ���� Ŭ���� nullptr�̶�� return �Ѵ�.
	if (nullptr == m_pCurrentClip)
		return;

	// ���� �ִϸ��̼��� ����ִٸ� return �Ѵ�.
	if (nullptr == m_pCurrentClip->Get_MainAnimation())
		return;
	
	// �÷��� ���̶�� �ִϸ��̼��� ������Ʈ�� �����Ѵ�.
	if (true == m_bIsPlay)
	{
		// �ִϸ��̼� Ŭ���� ������Ʈ�� �����Ѵ�. ( ������ ���� ������Ʈ )
		Update_AnimationClip(_dTimeDelta);		

		// RootBone�� ����Ѵٸ� ���� ��ġ�� ����ش�.
		if (Is_RootBone())
		{
			if (false == m_pCurrentClip->Is_SupplementSituation())
			{
				_matrix OffSetMatrix = XMMatrixIdentity();
				_matrix RootBoneMatrix = XMMatrixIdentity();

				vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
				/* �θ�κ��� �ڽĻ����� �������� �����Ѵ�.(CombinedTransformationMatrix) */
				for (auto& pHierarchyNode : vecHierachyNodes)
				{
					if ("Root" == pHierarchyNode->Get_Name())
					{
						OffSetMatrix = pHierarchyNode->Get_OffsetMatrix();
						RootBoneMatrix = pHierarchyNode->Get_RootTransformationMatrix();
						break;
					}
				}			
				OUTMATRIX matRootBone = CMethod::Out_Matrix(RootBoneMatrix);
				m_vRootBoneRotation = matRootBone.vRot;
				m_pCurrentClip->Set_RootBoneLook(XMLoadFloat3(&m_vRootBoneRotation));

				 // Owner ��ü�� ��������� �޾ƿ´�.
				 _matrix WorldMat = m_pOwnerTransformCom->Get_WorldMatrix();
				 _vector vBeginPos = CMethod::XMMatrix4x4GetVector(m_vBeginTransformMatrix, MATROW_POS);
				 WorldMat = CMethod::XMMatrixSetVector(vBeginPos, MATROW_POS, WorldMat);
				 // ���� ��Ʈ������ �����.
				 MATRIXES matrixes = { OffSetMatrix, RootBoneMatrix, m_pModelCom->Get_PivotMatrix() , WorldMat };

				 _matrix BoneMatrix = CMethod::XMMatrixMultiple_P(matrixes);

				 m_vRootBoneMatrix = CMethod::XMMatrixStoreFloat4x4_P(BoneMatrix);   
			}
		}
		// BoneTransform�� ����ϴ� ��쿡 �����Ӵ� �̵����� ������ش�.
		if (Is_BoneTransform())
		{
			if (nullptr != m_pOwnerTransformCom)
			{
				if (nullptr == m_pCurrentClip->Get_OwnerTransform())
					return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);

				// ���� ������� Ŭ���� RootTransform�� ����ϱ����� Dir���� ������.
				m_pCurrentClip->Set_RootTransformDir(m_pOwnerTransformCom->Get_Look());
				// 0�� �ƴҶ��� �����δ�.
				if (!CMethod::Is_Vector_Zero(m_pCurrentClip->Get_RootTransform()))
				{
					// Owner ��ü�� ��������� �޾ƿ´�.
					_float4x4 WorldMat = m_pOwnerTransformCom->Get_WorldFloat4x4();
					// ���ؿ� World��Ŀ� ��ġ���� 0�� ���Ͽ� ��ġ���� 0���θ����.
					WorldMat = CMethod::XMMatrix4x4SetFloat3(_float3(0.f, 0.f, 0.f), MATROW_POS, WorldMat);
					// ��ġ���� 0���ε� World����� �̿��Ͽ� �����Ӵ� �̵����� ���Ѵ�.
					MATRIXES matrixes = { CMethod::XMMatrixLoadFloat4x4_P(WorldMat),  m_pModelCom->Get_PivotMatrix(), m_pCurrentClip->Get_RootBoneMatrix() };
					m_vBoneTransformPos = m_pCurrentClip->Get_RootTransform() + CMethod::XMMatrix4x4GetVector(CMethod::XMMatrixMultiple4x4_P(matrixes), MATROW_POS);
				}
				else
					m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);
			}
		}

		// ����� 0�̶�� �ѹ� ä���ش�. 
		if (0 == m_ActiveTransitionList.size() || true == m_bIsTransitionUpdate)
		{
			// �̸� ActiveTransition���� ����ش�.
			m_ActiveTransitionList.clear();
			for (auto& Transition : m_TransitionList)
			{
				// ���� �����ִ� Ŭ���� Enter�� ������� ( �ڱ� �ڽ��̶�� )
				if (Transition.pEnterClip == m_pCurrentClip)
				{
					m_ActiveTransitionList.push_back(Transition);
				}
				// �ƹ� ���¿����� �� �� �ִ� Ʈ�����ǵ��� Trigger�� ���� �� üũ����
				else if (ANYSTATE == Transition.tEnterState)
				{
					m_ActiveTransitionList.push_back(Transition);
				}
				else if (ENTRY == Transition.tEnterState)
					m_ActiveTransitionList.push_back(Transition);

				// ��� ���°� Entry�� ���ư� �� �ִٴ°��� ��������.
			}
			m_bIsTransitionUpdate = false;
		}

		// Owner ��ü�� ���� �̵��ϴ� �̵����⺤�͸� �������ش�.
		m_pCurrentClip->Set_OwnerDirection(XMLoadFloat3(&m_vOwnerDirection));
		// �ִϸ��̼� Ŭ���� ������ Update�Ѵ�.
		m_pCurrentClip->Tick(_dTimeDelta);
		
		// Ʈ�������� �ߵ� ���� ���縦 �Ѵ�.
		Check_Transition_Condition();

		// �ִϸ��̼� Ŭ���� ������ ��� �ִϸ��̼� Ŭ���� �ٲ۴�.
		if (m_pCurrentClip->Is_Finish())		
			Change_AnimationClip();	
	}
}

void CAnimator::Late_Tick(const _double& _dTimeDelta)
{
}

void CAnimator::Change_AnimationClip()
{	
	// ���� �ִϸ��̼��� ���ٸ� return�Ѵ�.
	if (nullptr == m_pCurrentClip)
		return;

	if (true == m_pCurrentClip->Is_RootBone())
	{
		// RootBone �ִϸ��̼��̾��ٸ� �θ�ü�� Look�� m_vRootBoneMatrix�� Look���� �����Ѵ�. ( �׽�Ʈ �ڵ��Դϴ�. )				
		if (nullptr != m_pOwnerTransformCom)
		{
			m_pOwnerTransformCom->Rotate_TurnToNotFix(m_vRootBoneRotation);
			m_vRootBoneRotation = _float3(0.f, 0.f, 0.f);
		}
	}

	// ���� �Ѿ ���̷�Ʈ Ŭ���� �ִٸ� Set���� ���� �ִϸ��̼����� �Ѿ��. ( ������ ���� �ʴ´� )
	if (nullptr != m_pNextDirectClip)
	{
		Set_CurrentAnimationClip(m_pNextDirectClip);
		m_pNextDirectClip = nullptr;
	}

	CAnimationClip* pFindClip = nullptr;

	// �濡 ���� ���������� �ڱ��ڽ��� Enter�� �ϴ� Clip�� ���ٸ� Entry�� �ǵ�����.
	for (auto& Transition : m_ActiveTransitionList)
	{
		if (nullptr == Transition.pEnterClip)
			continue;

		// ���� ����ǰ� �ִ� Ŭ���� ���� EnterClip�� ���� Ʈ�������� ���ٸ� nullptr ��ȯ, �ִٸ� �����.
		if (m_pCurrentClip == Transition.pEnterClip)
		{
			pFindClip = Transition.pEnterClip;
			break;
		}
	}

	//FindClip�� nullptr�̶�� Loop ���θ� üũ�� �� Loop�� �ƴ϶�� �ش� Play�� false�� �ϰ� Loop ���¶�� �ش� Ŭ���� ����Ͽ� ������.
	if (nullptr == pFindClip)
	{
		// Loop���¶�� ����Ͽ� ������.
		if (m_pCurrentClip->Is_Loop())		
			return;		
		// Loop�� �ƴ϶�� Play�� False�� �ٲ۴�.
		else
		{				
			m_bIsPlay = false;
			// ó�� �κ����� �ǵ������ϴ� Bone���¸� ������Ʈ�Ѵ�.
			Update_InitBoneState();
		}
	}

	TRANSITION EntryTransition;

	// ���� �ڱ� �ڽ����κ��� �������� Transition�� ������ ��� �ش� Transition�� �̿��ؼ� �ִϸ��̼��� �����Ų��.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// ���°� Entry�� Ʈ�������� �н��Ѵ�.
		if (ENTRY == Transition.tEnterState)
		{
			EntryTransition = Transition;
			continue;
		}

		// EnterClip�� ���� CurrentClip�� ������ CurrentClip�� Ʈ���Ÿ� �������� �ʴٸ� NextClip���� �ٷ� �ٲ۴�.
		if ("" == Transition.InputTrigger.strInput)
		{
			// ���� �ִϸ��̼� Ŭ���� �����Ѵ�.
			m_pNextClip = Transition.pExitClip;
			m_pCurTransition = &Transition;
			// Finish �ʱ�ȭ
			m_pCurrentClip->Set_Finish(false);
			m_pCurrentClip->Get_MainAnimation()->Set_SupplementSituation(false);
			if(nullptr != m_pNextClip)
				m_pNextClip->Set_Finish(false);
			return;
		}
		// Ʈ���Ű� �����ϴ� ���
		else
		{
			// ���� ������� Ŭ���� EnterClip�� ���� ����� Ʈ�����ǵ鸸 ���Ѵ�.
			for (auto& inputTrigger : m_InputTriggerList)
			{
				// ��ǲ Ʈ������ Input���� �������� �ִٸ� ratio���� ���Ͽ� true�� ������ Transition�� �����´�.
				if (true == Transition.Compare_Trigger(inputTrigger))
				{
					// ���� Ʈ�������� ã���� Clip�� �����Ѵ�.
					// ���� �ִϸ��̼� Ŭ���� �����Ѵ�.
					m_pNextClip = Transition.pExitClip;
					m_pCurTransition = &Transition;
					// Finish �ʱ�ȭ
					m_pCurrentClip->Set_Finish(false);
					m_pCurrentClip->Get_MainAnimation()->Set_SupplementSituation(false);
					m_pNextClip->Set_Finish(false);
					return;
				}
			}
		}
	}
	// Loop���¶�� ����Ͽ� ������.
	if (m_pCurrentClip->Is_Loop())
	{
		m_pCurrentClip->Initialize_Data();
		return;
	}

	// ��� ������ ������� ���ߴٸ� Entry�� �ǵ�����. ( Ʈ���Ű� ������ Ʈ������ �Է� ���а� ���� ������ �˴ϴ�. )
	m_pNextClip = EntryTransition.pExitClip;
	return;
}

void CAnimator::Update_AnimationClip(const _double& _dTimeDelta)
{
	// ��ϵ� �ִϸ��̼� Ŭ���� ���ٸ� return�Ѵ�.
	if (0 == m_AnimationClips.size())
		return;

	// ��ϵ� Ŭ���� ���ٸ� Ŭ�� ����Ʈ�� 0�� Ŭ������ �������ش�.
	if (nullptr == m_pCurrentClip)			
		m_pCurrentClip = m_AnimationClips[0];
	

	// ���� ������ Ŭ���� �ִٸ� ���� �ִϸ��̼� Ŭ������ �������ش�.
	if (nullptr != m_pNextClip)
	{
		// ���� �ִϸ��̼� Ŭ���� ���Ͽ� ������Ʈ ���ش�. ( CurTransition�� ���ٸ� �׳� ������ �ִٸ� NextClip Ratio���� ������.
		if(nullptr != m_pCurTransition)
			m_pCurrentClip->Update_NextAnimation(_dTimeDelta, m_pCurTransition->dExitRatio,m_pCurTransition->dLerpSpeed, m_pNextClip);
		else
			m_pCurrentClip->Update_NextAnimation(_dTimeDelta, m_pNextClip);
		if (false == m_pCurrentClip->Is_SupplementSituation())
		{
			// ���������� �����ٸ� ���� Ŭ���� �����͵��� �ʱ�ȭ���ش�.
			m_pCurrentClip->Initialize_Data();		
			// ���� Ŭ���� PrevClip���� �ٲ۴�.
			m_pPrevClip = m_pCurrentClip;
			// ���� ���� ������̾��� Ŭ���� RootBone �ִϸ��̼��̶�� AdjustPos���� �����.
			if (true == m_pCurrentClip->Is_RootBone())
			{
				m_vAdjustPos = _float4(0.f, 0.f, 0.f, 0.f);
			}
			// �ִϸ��̼� �̺�Ʈ�� ������� �̵����� �����Ѵٸ� ���ش�.
			m_vBoneTransformPos = _float4(0.f, 0.f, 0.f, 0.f);
			// ���������� �����ٸ� ���� Ŭ���� ���� Ŭ������ �����Ѵ�.
			m_pCurrentClip = m_pNextClip;
			// ���� Ŭ���� nullptr�� ä���ش�.
			m_pNextClip = nullptr;
			// ���� Ʈ�������� nullptr�� ä���ش�.
			m_pCurTransition = nullptr;
			// ���� Ŭ������ �ٲ����� ���±��� ���� InputTrigger���� ����ش�.
			m_InputTriggerList.clear();
			// ActiveTransition���� Clear�Ѵ�.
			m_ActiveTransitionList.clear();
			// �̸� ActiveTransition���� ����ش�.
			for (auto& Transition : m_TransitionList)
			{
				// ���� �����ִ� Ŭ���� Enter�� ������� ( �ڱ� �ڽ��̶�� )
				if (Transition.pEnterClip == m_pCurrentClip)				
					m_ActiveTransitionList.push_back(Transition);				
				// �ƹ� ���¿����� �� �� �ִ� Ʈ�����ǵ��� Trigger�� ���� �� üũ����
				else if (ANYSTATE == Transition.tEnterState)				
					m_ActiveTransitionList.push_back(Transition);				
				else if (ENTRY == Transition.tEnterState)
					m_ActiveTransitionList.push_back(Transition);
			}
			// Ʈ������ ������Ʈ�� �����ִٸ� ���� ��ü�� ���� ��ġ�� �޾ƿ´�.
			if (nullptr != m_pOwnerTransformCom)
			{
				m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();
				// ���� Ŭ���� OwnerTransform�� ���ٸ� ä���ش�.
				if (nullptr == m_pCurrentClip->Get_OwnerTransform())
					return m_pCurrentClip->Set_OwnerTransform(m_pOwnerTransformCom);
			}

			// �ִϸ��̼� Change�� true�� ���ش�. ( �ٸ��ʿ��� ��밡���� �� �־ ���� bool�� )
			m_bIsChange = true;
			// �ִϸ��̼� Transition����� �ʱ�ȭ �ϱ� ���� bool������ true�� �����.
			m_bIsTransitionUpdate = true;
			/* ���� �ִ��� ����ϱ����� ä��(��)���� �ð��� �´� ���·� ������ TransformationMatrix �����Ѵ�.  */
			m_pCurrentClip->Update_TransformMatrices();
		}
	}
	else
	{
		/* ���� �ִ��� ����ϱ����� ä��(��)���� �ð��� �´� ���·� ������ TransformationMatrix �����Ѵ�.  */
		m_pCurrentClip->Update_TransformMatrices(_dTimeDelta);

		// �ִϸ��̼� Change�� �ٽ� false�� �ٲ��ش�. ( �ٸ��ʿ��� ��밡���� �� �־ ���� bool�� )
		m_bIsChange = false;
	}

	// �� ��ü�� ������ ������Ʈ�Ѵ�.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* �θ�κ��� �ڽĻ����� �������� �����Ѵ�.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
		{	
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
}

void CAnimator::Check_Transition_Condition()
{
	// �ڽ��� �����ִ� Ʈ���� string���� ���� InputƮ���Ű� �����ִ� Ʈ������ ��ϵ��� ���� �˻縦 �Ѵ�.
	for (auto& Transition : m_ActiveTransitionList)
	{
		// ���°� Anystate�� �ƴ� ActiveTransition ����� �˻��Ѵ�.
		if (ANYSTATE != Transition.tEnterState)
		{
			// Entry�� ���� ���� �ʴ´�.
			if (ENTRY != Transition.tEnterState)
			{
				// ȣ�� Ʈ���Ű� ���ٸ�
				if ("" == Transition.InputTrigger.strInput)
				{
					// EnterClip�� ���൵�� Ʈ�������� �����ִ� EnterRatio���� ũ�ٸ� ���� Ŭ���� Exit�� �־��ش�.
					if (Transition.dEnterRatio <= Transition.pEnterClip->Get_Ratio())
					{
						// ���� �ִϸ��̼� Ŭ���� Transition�� ExitClip���� �����Ѵ�.
						Set_NextAnimClip(Transition.pExitClip);
						// ���� Ʈ�����Ƕ��� �������ش�.
						m_pCurTransition = &Transition;
						return;
					}
				}
			}
			
			for (auto& input : m_InputTriggerList)
			{
				// ȣ�� Ʈ���Ű� ���� input time�� ���ǿ� �����Ѵٸ�
				if (true == input.bIsOverRatio)
				{
					if (input.strInput == Transition.InputTrigger.strInput && input.dAnimRatio <= Transition.InputTrigger.dAnimRatio) //&& input.dAnimRatio >= Transition.InputTrigger.dAnimRatio)
					{
						// ChangeRatio �� �Ѿ�� ������ �����Ǵ� ���
						if (true == Transition.bIsOverChangeRatio)
						{
							// EnterClip�� ���൵�� Ʈ�������� �����ִ� EnterRatio���� ũ�ٸ� ���� Ŭ���� Exit�� �־��ش�.
							if (Transition.dChangeRatio <= Transition.pEnterClip->Get_Ratio())
							{
								// ���� �ִϸ��̼� Ŭ���� Transition�� ExitClip���� �����Ѵ�.
								Set_NextAnimClip(Transition.pExitClip);
								// ���� Ʈ�����Ƕ��� �������ش�.
								m_pCurTransition = &Transition;
								return;
							}
						}
						else
						{
							// EnterClip�� ���൵�� Ʈ�������� �����ִ� EnterRatio���� ũ�ٸ� ���� Ŭ���� Exit�� �־��ش�.
							if (Transition.dChangeRatio >= Transition.pEnterClip->Get_Ratio())
							{
								// ���� �ִϸ��̼� Ŭ���� Transition�� ExitClip���� �����Ѵ�.
								Set_NextAnimClip(Transition.pExitClip);
								// ���� Ʈ�����Ƕ��� �������ش�.
								m_pCurTransition = &Transition;
								return;
							}
						}
					}
				}
				else
				{
					if (input.strInput == Transition.InputTrigger.strInput && input.dAnimRatio >= Transition.InputTrigger.dAnimRatio) //&& input.dAnimRatio >= Transition.InputTrigger.dAnimRatio)
					{
						// ChangeRatio �� �Ѿ�� ������ �����Ǵ� ���
						if (true == Transition.bIsOverChangeRatio)
						{
							// EnterClip�� ���൵�� Ʈ�������� �����ִ� EnterRatio���� ũ�ٸ� ���� Ŭ���� Exit�� �־��ش�.
							if (Transition.dChangeRatio <= Transition.pEnterClip->Get_Ratio())
							{
								// ���� �ִϸ��̼� Ŭ���� Transition�� ExitClip���� �����Ѵ�.
								Set_NextAnimClip(Transition.pExitClip);
								// ���� Ʈ��ksmdgkrp���Ƕ��� �������ش�.
								m_pCurTransition = &Transition;
								return;
							}
						}
						else
						{
							// EnterClip�� ���൵�� Ʈ�������� �����ִ� EnterRatio���� ũ�ٸ� ���� Ŭ���� Exit�� �־��ش�.
							if (Transition.dChangeRatio >= Transition.pEnterClip->Get_Ratio())
							{
								// ���� �ִϸ��̼� Ŭ���� Transition�� ExitClip���� �����Ѵ�.
								Set_NextAnimClip(Transition.pExitClip);
								// ���� Ʈ�����Ƕ��� �������ش�.
								m_pCurTransition = &Transition;
								return;
							}
						}
					}
				}
			}
		}
	}
}

void CAnimator::Update_InitBoneState()
{
	if (nullptr == m_pCurrentClip)
		return;

	// Ʈ������ ������Ʈ�� �����ִٸ� ���� ��ü�� ���� ��ġ�� �޾ƿ´�.
	if (nullptr != m_pOwnerTransformCom)
		m_vBeginTransformMatrix = m_pOwnerTransformCom->Get_WorldFloat4x4();

	// 0.0���� �ִϸ��̼� ������Ʈ�� ������ ������ �������ش�.
	m_pCurrentClip->Update_TransformMatrices(0.0);

	// �� ��ü�� ������ ������Ʈ�Ѵ�.
	if (nullptr != m_pModelCom)
	{
		vector<CHierarchyNode*>vecHierachyNodes = m_pModelCom->Get_HierachyNodeList();
		/* �θ�κ��� �ڽĻ����� �������� �����Ѵ�.(CombinedTransformationMatrix) */
		for (auto& pHierarchyNode : vecHierachyNodes)
			pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

CAnimator* CAnimator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _wstring strPath)
{
	CAnimator* pInstance = new CAnimator(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(strPath)))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimator* CAnimator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimator* pInstance = new CAnimator(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(L"")))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CAnimator::Clone(void* pArg)
{
	CAnimator* pInstance = new CAnimator(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
	//	MSG_BOX("Failed to Cloned : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimator::Free()
{
	// ���� Safe_Release�Ѵ�.
	Safe_Release(m_pModelCom);

	// �ִϸ��̼� Ŭ���� Release�Ѵ�.
	for (auto& pAnimClip : m_AnimationClips)
		Safe_Release(pAnimClip);

	m_AnimationClips.clear();

	__super::Free();
}

_bool CAnimator::tagTransition::Compare_Trigger(INPUTTRIGGER _trigger)
{
	// Trigger�� �񱳰��� ��� ���ٸ� true�� ��ȯ�Ѵ�.

	// InputRatio���� ���� �ִϸ��̼��� Ratio�� �̻��� ��쿡 Trigger�� �޾ƿ´�.
	if (true == this->InputTrigger.bIsOverRatio)
	{
		if (this->InputTrigger.strInput == _trigger.strInput && this->InputTrigger.dAnimRatio <= _trigger.dAnimRatio)
			return true;
	}
	// InputRatio���� ���� �ִϸ��̼��� Ratio�� ������ ��쿡 Trigger�� �޾ƿ´�.
	else
	{
		if (this->InputTrigger.strInput == _trigger.strInput && this->InputTrigger.dAnimRatio >= _trigger.dAnimRatio)
			return true;
	}
	return false;	
}

/// <summary>
/// �׷쿡 ��ϵǾ��ִ� �ִϸ��̼� Ŭ���� ��ȯ�մϴ�.
/// </summary>
/// <param name="fRatio">0 ~ 1 ������ ������ �� ���� ��</param>
/// <returns></returns>
CAnimationClip* CAnimator::tagCutAnimationGroup::Get_AnimationClip(_float fRatio)
{
	// ���� Ratio���������� TimeAcc�� ���մϴ�.
	_float fTimeAcc = fRatio * (_float)this->Get_Duration();

	// �ִϸ��̼� Ŭ������Ʈ�� �޾ƿ´�.
	if (0 == this->AnimationClipList.size())
		return nullptr;

	// Clip���� Duration���� ���� ����
	_double dDuration = 0.0;
	
	for (_uint i = 0; i < (_uint)this->AnimationClipList.size(); ++i)
	{
		dDuration += this->AnimationClipList[i]->Get_Duration();

		// ������ duration���� ���ڷε��� TimeAcc���� ���� 0�� ������� �ش� Ŭ���� �����Ѵ�.
		if (0 > dDuration - fTimeAcc)
			return this->AnimationClipList[i];
	}

	return nullptr;
}

/// <summary>
/// ���� �׷��� �����ִ� Ʈ�����ǵ��� �ִϸ��̼� Ŭ������ �����ִ� Duration�� �� ��
/// </summary>
/// <returns></returns>
_double CAnimator::tagCutAnimationGroup::Get_Duration()
{
	if (0 == AnimationClipList.size())
		return 0.f;

	_double dAllDuration = 0.0;

	// �ִϸ��̼� ����Ʈ�� �ִϸ��̼� Ŭ������ �� �÷��� Ÿ���� ���մϴ�.
	for (auto& pClip : AnimationClipList)
	{
		_double dDuration = pClip->Get_Duration();

		dAllDuration += dDuration;
	}

	return dAllDuration;
}

/// <summary>
/// ���� TimeAcc�� �̿��Ͽ� ������ �����ɴϴ�.
/// </summary>
/// <returns></returns>
_float CAnimator::tagCutAnimationGroup::Get_CurrentRatio()
{
	return _float(this->dGroupTimeAcc / this->Get_Duration());
}

/// <summary>
/// �� �÷��� Ÿ���� ���մϴ�. ( TimeAcc �� )
/// </summary>
/// <returns></returns>
_float CAnimator::tagCutAnimationGroup::Get_PlayTime()
{
	if (0 == AnimationClipList.size())
		return 0.f;

	_double dPlayTime = 0.0;

	// �ִϸ��̼� ����Ʈ�� �ִϸ��̼� Ŭ������ �� �÷��� Ÿ���� ���մϴ�.
	for (auto& pClip : AnimationClipList)
	{
		_double dTickPerSecond =  pClip->Get_TickPerSecond();
		_double dDuration = pClip->Get_Duration();

		dPlayTime += dDuration / dTickPerSecond;
	}

	return (_float)dPlayTime;
}
