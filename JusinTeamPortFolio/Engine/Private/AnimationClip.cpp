#include "AnimationClip.h"
#include "Animation.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Sound_Manager.h"

CAnimationClip::CAnimationClip()
	:CBase()
{
}
CAnimationClip::CAnimationClip(const CAnimationClip& _rhs)
	: CBase(), m_EventList(_rhs.m_EventList), m_isLoop(_rhs.m_isLoop)
	, m_fSpeed(_rhs.m_fSpeed), m_pMainAnimation(_rhs.m_pMainAnimation)
	, m_pSubAnimation(_rhs.m_pSubAnimation)
{
	// �̸��� �����Ѵ�.
	m_strName = _rhs.m_strName;
}
const _string& CAnimationClip::Get_MainAnimationName()
{
	if (nullptr == m_pMainAnimation)
		return STR_NULL;
	return m_pMainAnimation->Get_Name();
}

const _string& CAnimationClip::Get_SubAnimationName()
{
	if (nullptr == m_pSubAnimation)
		return STR_NULL;
	return m_pSubAnimation->Get_Name();
}

_string CAnimationClip::Get_RootBoneName()
{
	if (nullptr == m_pMainAnimation)
		return STR_NULL;
	return m_pMainAnimation->Get_RootBoneName();
}

const _double& CAnimationClip::Get_Duration()
{
	return m_pMainAnimation->Get_Duration();
}

const _double& CAnimationClip::Get_TickPerSecond()
{
	return m_pMainAnimation->Get_TickPerSecond();
}

const _double& CAnimationClip::Get_TimeAcc()
{
	return m_pMainAnimation->Get_TimeAcc();
}

const _double& CAnimationClip::Get_EventPlayTime()
{
	if (0.0 == m_dEventPlayTime)
	{
		for (auto& event : m_EventList)
		{
			if (Engine::CAnimationClip::EVENTTYPE_BONETRANSFORM == event.tEventType)
			{
				// �� ������ ���Ѵ�.
				_double dRatio = event.dEndRatio - event.dStartRatio;
				m_dEventPlayTime = dRatio * (Get_Duration() / Get_TickPerSecond());

				return m_dEventPlayTime;
			}
		}
	}

	return m_dEventPlayTime;
}

_matrix CAnimationClip::Get_RootBoneMatrix()
{
	return m_pMainAnimation->Get_RootBoneMatrix();
}

_vector CAnimationClip::Get_RootTransform()
{
	return m_pMainAnimation->Get_RootTransform();
}

void CAnimationClip::Set_Ratio(_double dRatio)
{
	if (nullptr == m_pMainAnimation)
		return;
	// Ratio ���� TimeAcc�� ��ȯ�Ѵ�.
	_double TimeAcc = dRatio * m_pMainAnimation->Get_Duration();
	// MainAnimation�� TimeAcc���� �����Ѵ�.
	m_pMainAnimation->Set_TimeAcc(TimeAcc);
}

void CAnimationClip::Set_TimeAcc(_double dTimeAcc)
{
	if (nullptr == m_pMainAnimation)
		return;
	m_pMainAnimation->Set_TimeAcc(dTimeAcc);

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Set_TimeAcc(dTimeAcc);
}

void CAnimationClip::Set_MainAnimation(CAnimation* _pAnimation)
{
	m_pMainAnimation = _pAnimation;

	if (nullptr != m_pSubAnimation)
		m_pMainAnimation->Reset_Data();
}

void CAnimationClip::Set_SubAnimation(CAnimation* _pAnimation)
{
	m_pSubAnimation = _pAnimation;

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Reset_Data();
}

_bool CAnimationClip::Is_Finish()
{
	return m_pMainAnimation->Is_Finished();
}

_bool CAnimationClip::Is_RootBone()
{
	if (nullptr == m_pMainAnimation)
		return false;

	return m_pMainAnimation->Is_RootBone();
}

_float2 CAnimationClip::Get_TrailValues()
{
	return _float2(m_fTrailStartValue, m_fTrailEndValue);
}

_bool CAnimationClip::Is_SupplementSituation()
{
	return m_pMainAnimation->Is_SupplementSituation();
}

void CAnimationClip::Set_SupplementSituation(const _bool& _bSituation)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_SupplementSituation(_bSituation);
}

void CAnimationClip::Set_RootBone(const _bool& isRootBone)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_RootBone(isRootBone);
}

void CAnimationClip::Set_RootBoneName(const _string& _strRootBoneName)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_RootBoneName(_strRootBoneName);
}

void CAnimationClip::Set_Finish(const _bool& isFinish)
{
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_Finished(isFinish);
}

void CAnimationClip::Reset_AnimationData()
{
	// �ִϸ��̼ǵ��� �����͸� �ʱ�ȭ�Ѵ�.
	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Reset_Data_ClipAnimation();

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Reset_Data_ClipAnimation();
}

void CAnimationClip::Set_AnimationSpeed(const _float& fFastValue)
{
	m_fSpeed = fFastValue;

	if (nullptr != m_pMainAnimation)
		m_pMainAnimation->Set_FastValue(m_fSpeed);

	if (nullptr != m_pSubAnimation)
		m_pSubAnimation->Set_FastValue(m_fSpeed);
}

void CAnimationClip::Set_OwnerTransform(CTransform* _pOwnerTransform)
{
	// Transform�� �ִ´�.
	if (nullptr != _pOwnerTransform)
		m_pOwnerTransform = _pOwnerTransform;
}

_double CAnimationClip::Get_Ratio()
{
	return m_pMainAnimation->Get_Ratio();
}

_bool CAnimationClip::Compute_Distance()
{
	// Ÿ�ٰ��� �Ÿ��� ���� �ִϸ��̼��� �Ÿ������� ª�ٸ� TargetDistance�� �Ÿ������� ����Ѵ�.
	if (0.f == m_fTargetDistance)
		return false;

	return m_fTargetDistance < m_fMoveDistance;
}

HRESULT CAnimationClip::NativeConstruct()
{
	return S_OK;
}

HRESULT CAnimationClip::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CAnimationClip::NativeConstruct(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName)
{
	if (nullptr != pMainAnimation)
		m_pMainAnimation = pMainAnimation;

	if (nullptr != pSubAnimation)
		m_pSubAnimation = pSubAnimation;

	if (0 != EventList.size())
		m_EventList = EventList;

	m_strName = strName;

	return S_OK;
}

void CAnimationClip::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pMainAnimation)
		return;

	// �̺�Ʈ�� ������Ʈ�Ѵ�.
	Update_Event(_dTimeDelta);
}

void CAnimationClip::Late_Tick(const _double& _dTimeDelta)
{
}

/// <summary>
/// �ʱ�ȭ�Ǿ���ϴ� ������ �ʱ�ȭ�մϴ�.
/// </summary>
void CAnimationClip::Initialize_Data()
{
	if (nullptr != m_pMainAnimation)
	{
		m_pMainAnimation->Set_TimeAcc(0.0);
		m_pMainAnimation->Update_TransformMatrices(0.0);
		m_pMainAnimation->Set_FastValue(m_fSpeed);
		m_pMainAnimation->Set_RootTransform(XMVectorZero());
		m_pMainAnimation->Reset_Data_ClipAnimation();
	}

	if (nullptr != m_pSubAnimation)
	{
		m_pSubAnimation->Set_TimeAcc(0.0);
		m_pSubAnimation->Update_SubTransformMatrices(0.0);
		m_pSubAnimation->Set_FastValue(m_fSpeed);
		m_pSubAnimation->Set_RootTransform(XMVectorZero());
		m_pSubAnimation->Reset_Data_ClipAnimation();
	}

	// Ȥ�ö� �߷��� �����ִٸ� �ٽ� ���ش�.
	m_bIsGravity = true;
	// Ȥ�ö� �޽��� �����ִٸ� �ٽ� ���ش�.
	m_bIsMeshActive = true;
	// �̺�Ʈ �÷��� ������ false�� �����.
	m_bIsEventPlay = false;
	// �� Ʈ���������� false�� �����.
	m_bIsBoneTransform = false;

	// �������� �ʱ�ȭ�Ѵ�.
	// MoveDistance ���� �־��ٸ� 0���� �ٲ��ش�.
	m_fTargetDistance = 0.f;
	// RootTransform�� 0���� �ش�.
	m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
	m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
	m_vColliderScale = _float4(0.f, 0.f, 0.f, 0.f);
	m_bIsColliderActive = false;
	m_bIsTrailActive = false;
	m_fHeight = 0.f;
	m_fPushedForce = 0.f;
	m_fMoveDistance = 0.f;
	m_fTrailStartValue = 0.f;
	m_fTrailEndValue = 0.f;


	m_bIsRandomPlay = false;
	for (auto& event : m_EventList)
		event.bIsFinish = false;
}

// ���� �ִϸ��̼� Ŭ���� ���� �ִϸ��̼ǰ� ���������Ѵ�.
void CAnimationClip::Update_NextAnimation(const _double& _dTimeDelta, CAnimationClip* _pNextClip)
{
	m_pMainAnimation->Updatet_NextAnim_TransformMatrix(_dTimeDelta, _pNextClip->m_pMainAnimation, m_bSupplementAnimation);
}

void CAnimationClip::Update_NextAnimation(const _double& _dTimeDelta, const _double& _dNextRatio, const _double& _dLerpSpeed, CAnimationClip* _pNextClip)
{
	// ���� �ִϸ��̼��� timeAcc���� ���Ѵ�.
	//_double timeAcc = _dNextRatio * _pNextClip->Get_Duration();

	_pNextClip->Set_Ratio(_dNextRatio);

	_double timeAcc = _pNextClip->Get_TimeAcc();

	// 2���ڿ� �������� �� Ű�������� ����� �� �ִ� TimeAcc���� ����.
	m_pMainAnimation->Update_NextAnimClip_TransformMatrix(_dTimeDelta, timeAcc, _dLerpSpeed, _pNextClip->m_pMainAnimation, m_bSupplementAnimation);
}

void CAnimationClip::Update_TransformMatrices(const _double& _dTimeDelta)
{
	if (nullptr == m_pMainAnimation)
		return;

	// ���� �ִϸ��̼��� ���� �������� ������Ʈ�Ѵ�.
	m_pMainAnimation->Update_TransformMatrices(_dTimeDelta, m_isLoop);

	// ���� ����ִϸ��̼� ( �� ǥ�� )���� �ִϸ��̼��� �����Ѵٸ� ������Ʈ�� �Ѵ�.
	if (nullptr != m_pSubAnimation)
	{
		// SubAnimation�� Ű�����Ӱ��� ���°͵鸸 ������Ʈ�� ���ش�.
		m_pSubAnimation->Update_SubTransformMatrices(_dTimeDelta);
	}
}

void CAnimationClip::Update_TransformMatrices()
{
	if (nullptr == m_pMainAnimation)
		return;

	m_pMainAnimation->Update_TransformMatrices();
}

CHierarchyNode* CAnimationClip::Find_HierarchyNode(const _string& _strBoneName)
{
	if (0 != m_HierarchyNodeList.size())
	{
		for (auto& pNode : m_HierarchyNodeList)
		{
			// ��� ����Ʈ���� �̸��� ���� ��带 ã���� �����Ѵ�.
			if (pNode->Get_Name() == _strBoneName)
				return pNode;
		}
	}

	// ��ã���� nullptr ����
	return nullptr;
}

void CAnimationClip::Update_Event(const _double& _dTimeDelta)
{
	// ���带 �������� ������ �ϱ����� ����Ʈ
	vector<CAnimationClip::ANIMEVENTDESC*> DefaultSoundList;
	vector<CAnimationClip::ANIMEVENTDESC*> RandomSoundList;

	for (auto& event : m_EventList)
	{
		if (EVENTTYPE_SOUND == event.tEventType)
		{
			if (0 == event.fAddValue)
				DefaultSoundList.push_back(&event);
			else
				RandomSoundList.push_back(&event);
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(0 != RandomSoundList.size())
		m_iRandomSoundIndex = pGameInstance->Get_RandomCount() % RandomSoundList.size();

	RELEASE_INSTANCE(CGameInstance);

	

	if (0 != m_EventList.size())
	{
		for (auto& event : m_EventList)
		{
			switch (event.tEventType)
			{
				//EFFECT
			case Engine::CAnimationClip::EVENTTYPE_EFFECT:
				Update_Event_Effect(event, _dTimeDelta);
				break;

				// �ݶ��̴� �̺�Ʈ
			case Engine::CAnimationClip::EVENTTYPE_COLLIDER:
				Update_Event_Collider(event, _dTimeDelta);
				break;

				// UI
			case Engine::CAnimationClip::EVENTTYPE_UI:
				break;

				// ����
			//case Engine::CAnimationClip::EVENTTYPE_SOUND:
			//	Update_Event_Sound(event, _dTimeDelta);
			//	break;

				// ī�޶�
			case Engine::CAnimationClip::EVENTTYPE_CAMERA:
				break;

				// �� ����� ( �޽� ���̵� )
			case Engine::CAnimationClip::EVENTTYPE_MESH:
				Update_Event_MeshHide(event, _dTimeDelta);
				break;

				// ���Ⱑ �پ��ִ� ���� �����ϴ� �̺�Ʈ
			case Engine::CAnimationClip::EVENTTYPE_WEAPONBONE:
				Update_Event_WeaponBone(event, _dTimeDelta);
				break;

				// �ӵ����� �̺�Ʈ�� ������Ʈ�� ������.
			case Engine::CAnimationClip::EVENTTYPE_SPEEDSECTION:
				Update_Event_SpeedSection(event, _dTimeDelta);
				break;

				// RootTransform �̺�Ʈ�� ������Ʈ�� ������.
			case Engine::CAnimationClip::EVENTTYPE_BONETRANSFORM:
				Update_Event_RootTransform(event, _dTimeDelta);
				break;

				// �߷� ������ �����ϴ� �̺�Ʈ
			case Engine::CAnimationClip::EVENTTYPE_GRAVITY:
				Update_Event_Gravity(event, _dTimeDelta);
				break;

				// Ư�� ���������� �̺�Ʈ Ȱ��ȭ ���ε��� üũ�ϴ� �̺�Ʈ
			case Engine::CAnimationClip::EVENTTYPE_TIMECHECK:
				Update_Event_TimeCheck(event, _dTimeDelta);
				break;

				// ��������� ����϶�� ���� �̺�Ʈ
			case Engine::CAnimationClip::EVENTTYPE_TRAIL:
				Update_Event_Trail(event, _dTimeDelta);
				break;
			}
		}				

		for(auto& event : DefaultSoundList)
			Update_Event_Sound(*event, _dTimeDelta);

		if (0 != RandomSoundList.size())
		{
			if (false == m_bIsRandomPlay)
			{
				Update_Event_Sound(*RandomSoundList[m_iRandomSoundIndex], _dTimeDelta);
				if (true == RandomSoundList[m_iRandomSoundIndex]->bIsFinish)
					m_bIsRandomPlay = true;
			}
		}
	}
}

void CAnimationClip::Update_Event_Collider(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// EndRatio�� �Ѿ�� bIsActive�� true�� �Ѵ�.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;
			// �ݶ��̴��� Ȱ��ȭ�� false�� �Ѵ�.
			m_bIsColliderActive = false;
			// �ݶ��̴��� ��Ȱ��ȭ �Ǹ� ��������� �����ִ� �ݶ��̴� Ÿ�� ������ �����ش�.
			m_strColliderType = "";
			// ��Ȱ��ȭ�Ǹ� Dir�� 0���� ������ش�.
			//m_vColliderDir = _float4(0.f, 0.f, 0.f, 0.f);
			//m_vColliderPos = _float4(0.f, 0.f, 0.f, 0.f);
			//m_vColliderScale = _float4(1.f, 1.f, 1.f, 0.f);

			m_fPushedForce = 0.f;
			m_fHeight = 0.f;
		}
	}

	// ����Ratio���� ũ�� EndRatio���� ���������Ѵ�.  
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// �ݶ��̴� �̺�Ʈ�� ������ �ݶ��̴����� Ȱ��ȭ ���θ� �ѱ�� PushedForce�� �����Ѵ�.
			m_bIsColliderActive = true;

			// �ݶ��̴��� ����
			m_strColliderType = _event.strEventText;
			// �ݶ��̴��� ���Ⱚ
			m_vColliderDir = _event.vEventDir;
			// �ݶ��̴� ��ġ
			m_vColliderPos = _event.vEventPos;
			// �ݶ��̴� ũ��
			m_vColliderScale = _event.vEventValues;
			// �и��� ��
			m_fPushedForce = _event.fValue;
			// Y�� ( ������ )
			m_fHeight = _event.fAddValue;
		}
	}
}

void CAnimationClip::Update_Event_Sound(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			{
				if (nullptr != pGameInstance)
				{
					if(1 == _event.vEventValues.x)
						// ���带 �����Ų��.
						pGameInstance->Play_SoundW(CMethod::ConvertSToW(_event.strEventText).c_str(), _event.fValue);
					else
						pGameInstance->Play_Sound_ByCondition(CMethod::ConvertSToW(_event.strEventText).c_str(), _event.fValue);
				}
			}
			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CAnimationClip::Update_Event_Effect(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio()/* && m_pMainAnimation->Get_Ratio() <= _event.dEndRatio*/)
	{
		// ���� Ȱ��ȭ ���� �ʾҾ��ٸ� Ȱ��ȭ ���ָ�, �ִϸ��̼��� �ӵ��� �ٲ��ش�.
		if (false == _event.bIsFinish)
		{
			// Ȱ��ȭ
			_event.bIsFinish = true;
			// Bone �̸����� ���̾��Ű ��带 ã�ƿ´�.
			CHierarchyNode* pNode = Find_HierarchyNode(_event.strEventText);
			if (nullptr == pNode)
				return;

			// �̰� �Ʒ����� ���� pNode�� Effect �θ��ż� �־��ֽø� �˴ϴ�.
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			_float3 vLook = m_vRootBoneLook;
			_float3 vDir = m_vOwnerDirection;

			CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW(_event.strName).c_str());
			if (nullptr != pEffect)
			{
				static_cast<CEffectObject*>(pEffect)->SetUP_Matrix(pNode, m_pOwnerTransform, _event.fValue, _event.vEventValues, m_vOwnerDirection, m_vRootBoneLook);
				static_cast<CEffectObject*>(pEffect)->Set_Play(true);
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}
	/*else if (m_pMainAnimation->Get_Ratio() >= _event.dEndRatio)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pEffect = pGameInstance->Active_Object(CMethod::ConvertSToW(_event.strName).c_str());
		if (nullptr != pEffect)
		{
			static_cast<CEffectObject*>(pEffect)->EffectObjectReset();
			static_cast<CEffectObject*>(pEffect)->Set_Active(false);
		}

		RELEASE_INSTANCE(CGameInstance);
	}*/
}

void CAnimationClip::Update_Event_SpeedSection(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �ð��� ���ٸ� EndRatio�ε����� �ƴ϶�� �ð� ������ ������.

	// �̺�Ʈ�� ������ ���൵������ ���� �ִϸ��̼��� ���൵�� ���ٸ� �ִϸ��̼ǵ��� �ӵ��� ������� �����ش�.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// �ִϸ��̼� �ӵ��� �������ش�.
			_event.bIsFinish = true;
			m_pMainAnimation->Set_FastValue(m_fSpeed);
			// ���� �ִϸ��̼��� nullptr�� �ƴ϶�� ����ִϸ��̼��� �ӵ� ���� �������ش�.
			if (nullptr != m_pSubAnimation)
				m_pSubAnimation->Set_FastValue(m_fSpeed);
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// ���� Ȱ��ȭ ���� �ʾҾ��ٸ� Ȱ��ȭ ���ָ�, �ִϸ��̼��� �ӵ��� �ٲ��ش�.
		if (false == _event.bIsFinish)
		{
			// �ִϸ��̼� �ӵ��� �������ش�.
			m_pMainAnimation->Set_FastValue(_event.fValue);
			// ���� �ִϸ��̼��� nullptr�� �ƴ϶�� ����ִϸ��̼��� �ӵ� ���� �������ش�.
			if (nullptr != m_pSubAnimation)
				m_pSubAnimation->Set_FastValue(_event.fValue);
		}
	}
}

void CAnimationClip::Update_Event_MeshHide(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �̺�Ʈ�� ������ ���൵������ ���� �ִϸ��̼��� ���൵�� ���ٸ� �߷��� �ٽ� true�� ���ش�.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// �̺�Ʈ�� �����ٰ� üũ�Ѵ�.
			_event.bIsFinish = true;
			// �߷°��� �ٽ� �����Ѵ�.
			m_bIsMeshActive = true;
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// ���� Ȱ��ȭ ���� �ʾҾ��ٸ� Ȱ��ȭ ���ָ�, �ִϸ��̼��� �ӵ��� �ٲ��ش�.
		if (false == _event.bIsFinish)
		{
			// �߷� ������ ����.
			m_bIsMeshActive = false;
		}
	}
}

void CAnimationClip::Update_Event_WeaponBone(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// ���� Ȱ��ȭ ���� �ʾҾ��ٸ� Ȱ��ȭ ���ָ�, �� �ε��� ��ȣ�� �ٲ۴�.
		if (false == _event.bIsFinish)
		{
			// Ȱ��ȭ
			_event.bIsFinish = true;
			// �ִϸ��̼� �ӵ��� �������ش�.
			m_iWeaponBoneIndex = (_uint)_event.fValue;
		}
	}
}

void CAnimationClip::Update_Event_RootTransform(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �̺�Ʈ�� ������ ���൵������ ���� �ִϸ��̼��� ���൵�� ���ٸ� �ִϸ��̼ǵ��� �ӵ��� ������� �����ش�.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			m_bIsEventPlay = false;
			m_bIsBoneTransform = false;


			// MoveDistance ���� �־��ٸ� 0���� �ٲ��ش�.
			m_fMoveDistance = 0.f;
			m_fTargetDistance = 0.f;
			_event.bIsFinish = true;
			// RootTransform�� 0���� �ش�.
			m_pMainAnimation->Set_RootTransform(XMVectorZero());
		}
	}

	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// ���� ������ �ʾҴٸ� �� �����Ӹ��� Root�� �̵����� �־��ش�
		if (false == _event.bIsFinish)
		{
			m_bIsEventPlay = true;
			m_bIsBoneTransform = true;

			m_fMoveDistance = _event.fValue;

			// �� ������ ���Ѵ�.
			_double dRatio = _event.dEndRatio - _event.dStartRatio;
			m_dEventPlayTime = dRatio * (Get_Duration() / Get_TickPerSecond());
			// ���� �÷���Ÿ���� ���Ѵ�.
			_double dRestRatio = _event.dEndRatio - Get_Ratio();
			m_dRestPlayTime = dRestRatio * (Get_Duration() / Get_TickPerSecond());

			if (nullptr == m_pOwnerTransform)
				return;

			// ���� ��ü�� Right, Up, Look�� �޾ƿ´�.
			_vector vRight = m_pOwnerTransform->Get_Right();
			_vector vUp = m_pOwnerTransform->Get_Up();
			_vector vLook = m_pOwnerTransform->Get_Look();
			// RootTransform ��갪�� ���´�.
			_vector vRootTransform = XMVectorZero();

			// �Ÿ����� �޾ƿ��� fMoveDistance�� �ش� �Ÿ������� �����Ѵ�.
			_float fMoveDistance = 0.f;

			if (true == Compute_Distance())
				fMoveDistance = m_fTargetDistance;
			else
				fMoveDistance = _event.fValue;

			if (_event.vEventPos.x != 0)
				vRootTransform += XMVector3Normalize(vRight) * _event.vEventPos.x * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();
			if (_event.vEventPos.y != 0)
				vRootTransform += XMVector3Normalize(vUp) * _event.vEventPos.y * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();
			if (_event.vEventPos.z != 0)
				vRootTransform += XMVector3Normalize(vLook) * _event.vEventPos.z * fMoveDistance / (_float)m_dEventPlayTime * _float(_dTimeDelta) * Get_AnimationSpeed();

			m_fBoneAnimMoveForce = _event.fValue;

			// �̺�Ʈ�� Y���� �������� �ʾҴٸ� RootTransform ��갪���� Y�� ����.
			if (0 == _event.vEventPos.y)
				vRootTransform = XMVectorSetY(vRootTransform, 0.f);

			_float ratio = 0.8f;
			_float MaxHeight = _event.fAddValue;

			_float fCurTime = (_float)(Get_TimeAcc() / Get_TickPerSecond());

			_float Gravity = (_float)(2 * MaxHeight / pow(ratio * m_dEventPlayTime, 2.f));
			_float Height = (_float)(Gravity * m_dEventPlayTime * ratio);
			_float fY = (_float)(Height - Gravity * fCurTime) * _float(_dTimeDelta);
			if (0.f != MaxHeight)
				vRootTransform = XMVectorSetY(vRootTransform, fY);

			// Root�� �̵����� �־��ش�.
			m_pMainAnimation->Set_RootTransform(vRootTransform);
		}
	}
}

void CAnimationClip::Update_Event_Gravity(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �̺�Ʈ�� ������ ���൵������ ���� �ִϸ��̼��� ���൵�� ���ٸ� �߷��� �ٽ� true�� ���ش�.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// �̺�Ʈ�� �����ٰ� üũ�Ѵ�.
			_event.bIsFinish = true;
			// �߷°��� �ٽ� �����Ѵ�.
			m_bIsGravity = true;
		}
	}
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		// ���� Ȱ��ȭ ���� �ʾҾ��ٸ� Ȱ��ȭ ���ָ�, �ִϸ��̼��� �ӵ��� �ٲ��ش�.
		if (false == _event.bIsFinish)
		{
			// �߷� ������ ����.
			m_bIsGravity = false;
		}
	}
}

void CAnimationClip::Update_Event_TimeCheck(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �̺�Ʈ���� ������ EndRatio�� ���� �������� Ratio������ �۾����� �̺�Ʈ�� ������.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// �̺�Ʈ�� �����ٰ� üũ�Ѵ�.

			if (true == m_tEventCheck.bIsActive && _event.strName == m_tEventCheck.strEventName)
			{
				_event.bIsFinish = true;
				m_tEventCheck.bIsActive = false;
			}
		}
	}

	// �̺�Ʈ���� ������ StartRatio�� ���� �������� Ratio������ �۾����� �̺�Ʈ�� �����Ѵ�.
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			// ������ �����Ǹ� üũ�� �̺�Ʈ ������ ��´�.
			if (false == m_tEventCheck.bIsActive)
			{
				m_tEventCheck.bIsActive = true;
				m_tEventCheck.strEventName = _event.strName;
			}
		}
	}
}

void CAnimationClip::Update_Event_Trail(ANIMEVENTDESC& _event, const _double& _dTimeDelta)
{
	// �̺�Ʈ���� ������ EndRatio�� ���� �������� Ratio������ �۾����� �̺�Ʈ�� ������.
	if (_event.dEndRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			_event.bIsFinish = true;

			m_bIsTrailActive = false;
			m_fTrailStartValue = 0.f;
			m_fTrailEndValue = 0.f;
		}
	}

	// �̺�Ʈ���� ������ StartRatio�� ���� �������� Ratio������ �۾����� �̺�Ʈ�� �����Ѵ�.
	if (_event.dStartRatio <= m_pMainAnimation->Get_Ratio())
	{
		if (false == _event.bIsFinish)
		{
			m_bIsTrailActive = true;
			m_fTrailStartValue = _event.fValue;
			m_fTrailEndValue = _event.fAddValue;
		}
	}
}

CAnimationClip* CAnimationClip::Create(CAnimation* pMainAnimation, CAnimation* pSubAnimation, const vector<ANIMEVENTDESC>& EventList, _string strName)
{
	CAnimationClip* pInstance = new CAnimationClip();

	if (FAILED(pInstance->NativeConstruct(pMainAnimation, pSubAnimation, EventList, strName)))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationClip* CAnimationClip::Create()
{
	CAnimationClip* pInstance = new CAnimationClip();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimationClip* CAnimationClip::Clone(void* pArg)
{
	CAnimationClip* pInstance = new CAnimationClip(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimationClip::Free()
{
	__super::Free();
}