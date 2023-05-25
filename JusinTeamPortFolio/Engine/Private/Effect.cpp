#include "Effect.h"
#include "GameInstance.h"

CEffect::CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CEffect::CEffect(const CEffect& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEffect::NativeConstruct_Clone(void* _pArg)
{
	Ready_Transform(TEXT("Com_Transform"));
	m_pEffectTransform = CTransform::Create(m_pDevice, m_pContext);

	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	m_tEffectDesc.vScl = _float3(1.f, 1.f, 1.f);

	return S_OK;
}

void CEffect::Tick(const _double& _dTimeDelta)
{
	if (m_pParent == nullptr)
		return;

	if (!m_pParent->Is_Active())
		return;

	// ����Ʈ ���̰��� �ǽð����� ����
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	if (CEffectObject::PROJECT_TOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType() || CEffectObject::PROJECT_ANIMTOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
		pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);
	else
		pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);

 	Set_DepthValue(XMVectorGetX(XMVector3Length(pGameObject->Get_Transform()->Get_Pos() - m_pTransformCom->Get_Pos())));

	RELEASE_INSTANCE(CGameInstance);

	if (static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
	{
		// ����Ʈ LifeTime �� �����ٸ� �������
		if (m_bDisable)
			return;

		// ���� ���̶�� �÷������� �ƴҶ��� �������ϱ� ������ �����Ҷ� true�� ����ٰ� �����Ҷ� false�� �ٲ۴�.
		// �� ������ EnableTime�� 0.f �� �ƴҼ��� �ֱ⶧���̴�.
		if (CEffectObject::PROJECT_TOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
			m_bEnable = false;

		m_fEnableTime += (_float)_dTimeDelta;

		if (m_tEffectDesc.fEnableTime < m_fEnableTime)
			m_bEnable = true;

		// Enable �� false �ų�, ��������Ʈ�� Loop �� �ƴϰ� ����� �����ٸ� �ٷ� ����
		if (!m_bEnable || !m_bSlice)
			return;

		m_tEffectDesc.fCurFrame += (_float)_dTimeDelta;
		m_fLifeTime += _dTimeDelta;

		// ������ �ð�
		if (15 == m_tEffectDesc.iPassIndex)
		{
			m_fRatioTime += (_float)_dTimeDelta;
			_float fEffectRatio = m_fRatioTime / m_tEffectDesc.fMaxFrame;

			if (0.7f <= fEffectRatio)
			{
				if (1.f < fEffectRatio)
					fEffectRatio = 1.f;

				m_fDissolveTime += (_float)_dTimeDelta;
				m_fRatio = m_fDissolveTime / (m_tEffectDesc.fMaxFrame * 0.3f);
			}
		}
		
		if (!static_cast<CEffectObject*>(m_pParent)->Get_LoopState())
		{
			if (m_tEffectDesc.fLifeTime < m_fLifeTime)
				m_bDisable = true;
		}

		// �̵� ��ȯ
		ChangeTranslation(_dTimeDelta);

		// ������ ��ȯ
		ChangeScale();

		// ȸ�� ��ȯ
		RotationAngle();
		RotationLoop();

		// ������
		BillBoard();

		Alpha(_dTimeDelta);

		// ��������Ʈ �ؽ��� �ڸ���
		SliceTexture(_dTimeDelta);

		// UV Flow
		if (m_tEffectDesc.bUVFlow)
			UVFlow(_dTimeDelta);
	}
}

void CEffect::Late_Tick(const _double& _dTimeDelta)
{

}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::Set_Parent(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		MSG_BOX("Set_Parent Method Error!!!");

	m_pParent = pGameObject;
	//Safe_AddRef(m_pParent);

	SetUp_Position();
}

void CEffect::Set_PrototypePath(wstring strPath)
{
	m_strPrototypePath = strPath;
}

void CEffect::Set_NoisePath(wstring strPath)
{
	m_strNoisePath = strPath;
}

void CEffect::Set_MeshPrototypePath(wstring strPath)
{
	m_strMeshPrototypePath = strPath;
}

void CEffect::Set_ChangeLook()
{
	_float3 vLook = m_vParentLook;
	_float3 vUp = _float3(0.f, 1.f, 0.f);
	_float3 vRight = XMVector3Cross(XMLoadFloat3(&vUp), XMLoadFloat3(&vLook));
	vUp = XMVector3Cross(XMLoadFloat3(&vLook), XMLoadFloat3(&vRight));

	_float3 vScale = m_pEffectTransform->Get_Scale();
	m_pTransformCom->Set_LookF3(vLook);
	m_pTransformCom->Set_UpF3(vUp);
	m_pTransformCom->Set_RightF3(vRight);
	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Bind_WorldMatrix(m_pEffectTransform->Get_WorldMatrix());
}

void CEffect::EffectUpDate()
{

}

void CEffect::EffectReset()
{
	//// Effect ũ�� �ʱ�ȭ
	//m_pTransformCom->Set_RightF3(_float3(1.f, 0.f, 0.f));
	//m_pTransformCom->Set_UpF3(_float3(0.f, 1.f, 0.f));
	//m_pTransformCom->Set_LookF3(_float3(0.f, 0.f, 1.f));

	//// ������ ������ ������ �Ҵ�
	//m_pTransformCom->Set_Scale(m_tEffectDesc.vScl);
	//m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
	//m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));

	m_pEffectTransform->Set_RightF3(_float3(1.f, 0.f, 0.f));
	m_pEffectTransform->Set_UpF3(_float3(0.f, 1.f, 0.f));
	m_pEffectTransform->Set_LookF3(_float3(0.f, 0.f, 1.f));

	m_pEffectTransform->Set_Scale(m_tEffectDesc.vScl);
	m_pEffectTransform->Rotate_Turn(m_tEffectDesc.vRot);
	m_pEffectTransform->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));

	//m_vParentLook = _float3(0.f, 0.f, 0.f);

	m_tEffectDesc.fCurFrame = 0.f;
	m_fCurAcc = 0.f;
	m_tEffectDesc.fJumpTime = 0.f;

	m_fEnableTime = 0.f;
	m_fLifeTime = 0.f;
	m_fAlphaTime = 0.f;
	m_fRatioTime = 0.f;
	m_fRatio = 0.f;
	m_fDissolveTime = 0.f;

	m_vUV_Dir.x = 0.f;
	m_vUV_Dir.y = 0.f;
	m_fAlpha = 1.f;
	m_iSliceCurCount = 0;
	m_fSliceTime = 0.0;
	m_bSlice = true;
	m_fCurUseIndex = 0;

	if (CEffectObject::PROJECT_TOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
		m_bEnable = true;
	else
		m_bEnable = false;

	m_bDisable = false;

	m_vUV_Start.x = 0.f;
	m_vUV_Start.y = 0.f;

	// Tool �϶� ��������Ʈ �̹������ UV ��ǥ 0���� �ʱ�ȭ
	if (CEffectObject::PROJECT_TOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
	{
		m_vUV_End.x = 0.f;
		m_vUV_End.y = 0.f;
	}
	// Ŭ�� �϶� ��������Ʈ �̹������ 1��° �̹����� UV ��ǥ �ʱ�ȭ
	else
	{
		m_iSliceCount = m_tEffectDesc.iSliceX * m_tEffectDesc.iSliceY;

		if (2 <= m_iSliceCount)
		{
			m_fDeltaSliceX = (1.f / m_tEffectDesc.iSliceX);
			m_fDeltaSliceY = (1.f / m_tEffectDesc.iSliceY);

			m_vUV_End.x = m_fDeltaSliceX;
			m_vUV_End.y = m_fDeltaSliceY;
		}
		else
		{
			m_vUV_End.x = 0.f;
			m_vUV_End.y = 0.f;
		}
	}

	m_bSliceFirst = true;

	m_tEffectDesc.fAngle = 0.f;
	m_bAddScale = true;
	m_bAddRot = true;

	if (EFFECT_INSTANCE == m_eEffectType)
		static_cast<CInstanceEffect*>(this)->InstanceBufferReset();

	SetUp_Position();
}

void CEffect::ChangeTranslation(const _double& _dTimeDelta)
{
	m_fCurAcc += m_tEffectDesc.fAccSpeed * (_float)_dTimeDelta;

	m_tEffectDesc.fAcceleration = m_tEffectDesc.fForce * m_fCurAcc;

	_float3 vPos = m_pEffectTransform->Get_PosF3();

	if (m_tEffectDesc.bGoLook)
	{
		if (6 == static_cast<CEffectObject*>(m_pParent)->Get_Mode())
			vPos += (XMVector3Normalize(XMLoadFloat3(&m_vParentLook)) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);
		else
			vPos += (XMVector3Normalize(XMLoadFloat3(&m_pTransformCom->Get_LookF3())) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);
		//vPos += (XMVector3Normalize(XMLoadFloat3(&m_pTransformCom->Get_LookF3())) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);
	}
	else if (m_tEffectDesc.bGoUp)
		vPos += (XMVector3Normalize(XMLoadFloat3(&m_pTransformCom->Get_UpF3())) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);
	else if (m_tEffectDesc.bGoRight)
		vPos += (XMVector3Normalize(XMLoadFloat3(&m_pTransformCom->Get_RightF3())) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);
	else
		vPos += (XMVector3Normalize(XMLoadFloat3(&m_tEffectDesc.vDir)) * (m_tEffectDesc.fForce + m_tEffectDesc.fAcceleration) * (_float)_dTimeDelta);

	if (!m_tEffectDesc.bGravity)
		m_tEffectDesc.fGravity = 0.f;

	if (m_tEffectDesc.bJump)
	{
		m_tEffectDesc.fJumpTime += (_float)_dTimeDelta;
		vPos.y += m_tEffectDesc.fJumpPower + -0.5f * m_tEffectDesc.fGravity * m_tEffectDesc.fJumpTime * m_tEffectDesc.fJumpTime;
	}

	m_pEffectTransform->Set_Pos(XMLoadFloat3(&vPos));
}

void CEffect::RotationLoop()
{

}

void CEffect::RotationAngle()
{
	// ������ ����ŭ ȸ�� ��ȯ
	m_tEffectDesc.fDeltaAngleRatio = m_tEffectDesc.fCurFrame / m_tEffectDesc.fMaxFrame;
	if (1.f >= m_tEffectDesc.fDeltaAngleRatio)
	{
		if (0.95f < m_tEffectDesc.fDeltaAngleRatio)
			int i = 0;
		fDeltaValue = (m_tEffectDesc.fDestAngle * m_tEffectDesc.fDeltaAngleRatio);
		//m_tEffectDesc.fAngle = fDeltaValue;
		if (0 != fDeltaValue)
		{
			//_vector vAxis;
			_float3 vRot = _float3(0.f, 0.f, 0.f);
			if (EFFECTDESC::Axis_X == m_tEffectDesc.eRotAxisType)
			{
				/*vAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				m_pEffectTransform->Rotate_Fix(vAxis, fDeltaValue);*/
				vRot = _float3(m_tEffectDesc.vRot.x + fDeltaValue, m_tEffectDesc.vRot.y, m_tEffectDesc.vRot.z);
				m_pEffectTransform->Rotate_Turn(vRot);
			}
			else if (EFFECTDESC::Axis_Y == m_tEffectDesc.eRotAxisType)
			{
				/*vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				m_pEffectTransform->Rotate_Fix(vAxis, fDeltaValue);*/
				vRot = _float3(m_tEffectDesc.vRot.x, m_tEffectDesc.vRot.y + fDeltaValue, m_tEffectDesc.vRot.z);
				m_pEffectTransform->Rotate_Turn(vRot);
			}
			else if (EFFECTDESC::Axis_Z == m_tEffectDesc.eRotAxisType)
			{
				/*vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				m_pEffectTransform->Rotate_Fix(vAxis, fDeltaValue);*/
				vRot = _float3(m_tEffectDesc.vRot.x, m_tEffectDesc.vRot.y, m_tEffectDesc.vRot.z + fDeltaValue);
				m_pEffectTransform->Rotate_Turn(vRot);
			}
			else
			{
				vRot = _float3(m_tEffectDesc.vRot.x + fDeltaValue, m_tEffectDesc.vRot.y + fDeltaValue, m_tEffectDesc.vRot.z + fDeltaValue);
				m_pEffectTransform->Rotate_Turn(vRot);
			}
		}
	}
}

void CEffect::ChangeScale()
{
	// ������ ����ŭ ������ ��ȯ
	m_tEffectDesc.fDeltaScaleRatio = m_tEffectDesc.fCurFrame / m_tEffectDesc.fMaxFrame;
	if (1.f >= m_tEffectDesc.fDeltaScaleRatio)
	{
		fDeltaValue = (m_tEffectDesc.fDestScale * m_tEffectDesc.fDeltaScaleRatio);

		fDeltaScale.x = (m_tEffectDesc.vScaleDelta.x * m_tEffectDesc.fDeltaScaleRatio);
		fDeltaScale.y = (m_tEffectDesc.vScaleDelta.y * m_tEffectDesc.fDeltaScaleRatio);
		fDeltaScale.z = (m_tEffectDesc.vScaleDelta.z * m_tEffectDesc.fDeltaScaleRatio);

		if (0 != fDeltaValue)
		{
			_float3 vScale = m_pEffectTransform->Get_Scale();

			if (EFFECTDESC::Axis_X == m_tEffectDesc.eScaleAxisType)
				vScale.x = m_tEffectDesc.vScl.x + fDeltaValue;
			else if (EFFECTDESC::Axis_Y == m_tEffectDesc.eScaleAxisType)
				vScale.y = m_tEffectDesc.vScl.y + fDeltaValue;
			else if (EFFECTDESC::Axis_Z == m_tEffectDesc.eScaleAxisType)
				vScale.z = m_tEffectDesc.vScl.z + fDeltaValue;
			else
			{
				vScale.x = m_tEffectDesc.vScl.x + fDeltaValue;
				vScale.y = m_tEffectDesc.vScl.y + fDeltaValue;
				vScale.z = m_tEffectDesc.vScl.z + fDeltaValue;
			}

			if (0.f > vScale.x)
				vScale.x = 0.f;
			if (0.f > vScale.y)
				vScale.y = 0.f;
			if (0.f > vScale.z)
				vScale.z = 0.f;

			m_pEffectTransform->Set_Scale(vScale);
		}
		else
		{
			_float3 vScale = m_pEffectTransform->Get_Scale();

			if (0.f != m_tEffectDesc.vScaleDelta.x)
				vScale.x = m_tEffectDesc.vScl.x + fDeltaScale.x;
			if (0.f != m_tEffectDesc.vScaleDelta.y)
				vScale.y = m_tEffectDesc.vScl.y + fDeltaScale.y;
			if (0.f != m_tEffectDesc.vScaleDelta.z)
				vScale.z = m_tEffectDesc.vScl.z + fDeltaScale.z;

			if (0.f > vScale.x)
				vScale.x = 0.f;
			if (0.f > vScale.y)
				vScale.y = 0.f;
			if (0.f > vScale.z)
				vScale.z = 0.f;

			m_pEffectTransform->Set_Scale(vScale);
		}
	}
}

void CEffect::SliceTexture(const _double& _dTimeDelta)
{
	m_iSliceCount = m_tEffectDesc.iSliceX * m_tEffectDesc.iSliceY;

	// �����̽����� X,Y �� ���� 2���� ���ų� ũ�� �̹����� �����̽��ؼ� ���
	if (2 <= m_iSliceCount)
	{
		m_fDeltaSliceX = (1.f / m_tEffectDesc.iSliceX);
		m_fDeltaSliceY = (1.f / m_tEffectDesc.iSliceY);

		if (m_bSliceFirst)
		{
			if (60 < m_iSliceCount)
			{
				m_iSliceType = 1;
				_float fCount = 60.f * m_tEffectDesc.fMaxFrame;
				m_fPerSecondIndex = (_float)m_iSliceCount / fCount;
			}
			else
			{
				m_iSliceType = 0;
			}

			m_fFrameSpeed = (_double)m_tEffectDesc.fMaxFrame / (_double)m_iSliceCount;
			m_vUV_Start.x = 0.f;
			m_vUV_Start.y = 0.f;
			m_vUV_End.x = m_fDeltaSliceX;
			m_vUV_End.y = m_fDeltaSliceY;
			m_bSliceFirst = false;
		}

		// Effect �� Max Ÿ���� EffectObject �� Max Ÿ�Ӱ� �����ϰ� ���ָ� �� �����ӽð����� ��������Ʈ�� �ѵ������� �̾���
		m_fSliceTime += _dTimeDelta;

		if (m_iSliceCount < m_iSliceCurCount)
		{
			if (m_tEffectDesc.bSliceLoop)
			{
				m_fCurUseIndex = 0;
				m_fSliceTime = 0.0;
				m_iSliceCurCount = 0;
				m_vUV_Start.x = 0.f;
				m_vUV_Start.y = 0.f;
				m_vUV_End.x = m_fDeltaSliceX;
				m_vUV_End.y = m_fDeltaSliceY;
			}
			else
			{
				m_bSlice = false;
			}
		}

		if (m_fFrameSpeed <= m_fSliceTime)
		{
			m_fSliceTime = 0.0;

			if (1 == m_iSliceType)
			{
				m_fCurUseIndex += m_fPerSecondIndex;
				m_vUV_Start.y = _float((_int)m_fCurUseIndex / m_tEffectDesc.iSliceY) * m_fDeltaSliceY;
				m_vUV_End.y = m_vUV_Start.y + m_fDeltaSliceY;
				m_vUV_Start.x = _float((_int)m_fCurUseIndex % m_tEffectDesc.iSliceX) * m_fDeltaSliceX;
				m_vUV_End.x = m_vUV_Start.x + m_fDeltaSliceX;
			}
			else
			{
				m_iSliceCurCount++;

				if (m_iSliceCount > m_iSliceCurCount)
				{
					m_vUV_End.x += m_fDeltaSliceX;
					m_vUV_Start.x = m_vUV_End.x - m_fDeltaSliceX;
				}

				if (1.f < m_vUV_End.x)
				{
					if (m_iSliceCount > m_iSliceCurCount)
					{
						m_vUV_End.x = m_fDeltaSliceX;
						m_vUV_Start.x = 0.f;
						m_vUV_End.y += m_fDeltaSliceY;
						m_vUV_Start.y = m_vUV_End.y - m_fDeltaSliceY;
					}
				}

				if (1.f < m_vUV_End.y)
				{
					m_vUV_End.y = m_fDeltaSliceY;
					m_vUV_Start.y = 0.f;
				}
			}
		}

		m_vUV_Delta.x = m_vUV_End.x - m_vUV_Start.x;
		m_vUV_Delta.y = m_vUV_End.y - m_vUV_Start.y;
	}
}

void CEffect::UVFlow(const _double& _dTimeDelta)
{
	m_vUV_Dir.x += m_tEffectDesc.vUVSpeed.x * (_float)_dTimeDelta;
	m_vUV_Dir.y += m_tEffectDesc.vUVSpeed.y * (_float)_dTimeDelta;
}

void CEffect::BillBoard()
{
	if (m_tEffectDesc.bBillBoardAll || m_tEffectDesc.bBillBoardExceptY)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pGameObject = nullptr;

		if (CEffectObject::PROJECT_TOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType()
			|| CEffectObject::PROJECT_ANIMTOOL == static_cast<CEffectObject*>(m_pParent)->Get_ProjectType())
			pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);
		else
		{
			//// ��Ʋ
			//if (4 == pGameInstance->Get_CurLevel())
			//{
			//	pGameObject = pGameInstance->Find_GameObject(4, TEXT("Layer_Cam"), 0);
			//}
			//else if (5 == pGameInstance->Get_CurLevel())
			//{
			//	pGameObject = pGameInstance->Find_GameObject(5, TEXT("Layer_Cam"), 0);
			//}
			pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);
		}

		if (nullptr != pGameObject)
		{
			if (m_tEffectDesc.bBillBoardExceptY)
			{
				_float3 vPos = pGameObject->Get_Transform()->Get_PosF3();
				vPos.y = m_pTransformCom->Get_PosF3().y;
				m_pTransformCom->LookAt(XMLoadFloat3(&vPos));
			}
			else if (m_tEffectDesc.bBillBoardAll)
			{
				m_pTransformCom->LookAt(pGameObject->Get_Transform()->Get_Pos());
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CEffect::Alpha(const _double& _dTimeDelta)
{
	m_fAlphaTime += (_float)_dTimeDelta;

	if (m_tEffectDesc.fAlphaTime < m_fAlphaTime)
		m_fAlpha -= m_tEffectDesc.fAlphaSpeed * (_float)_dTimeDelta;

	if (0.f > m_fAlpha)
		m_fAlpha = 0.f;
}

void CEffect::SetUp_Position()
{
	if (nullptr != m_pParent)
	{
		if (!static_cast<CEffectObject*>(m_pParent)->Get_EffectObjectDesc().bPlay)
		{
			m_pEffectTransform->Set_RightF3(_float3(1.f, 0.f, 0.f));
			m_pEffectTransform->Set_UpF3(_float3(0.f, 1.f, 0.f));
			m_pEffectTransform->Set_LookF3(_float3(0.f, 0.f, 1.f));

			m_pEffectTransform->Set_Scale(m_tEffectDesc.vScl);
			m_pEffectTransform->Rotate_Turn(m_tEffectDesc.vRot);
			m_pEffectTransform->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));
		}
	}

	/*if (EFFECTTYPE::EFFECT_INSTANCE != m_eEffectType)
	{
		m_pTransformCom->Set_Scale(m_pEffectTransform->Get_Scale());
		m_pTransformCom->Bind_WorldMatrix(m_pEffectTransform->Get_WorldMatrix() * m_pParent->Get_Transform()->Get_WorldMatrix());
	}*/

	m_pTransformCom->Set_Scale(m_pEffectTransform->Get_Scale());
	m_pTransformCom->Bind_WorldMatrix(m_pEffectTransform->Get_WorldMatrix() * m_pParent->Get_Transform()->Get_WorldMatrix());
}

HRESULT CEffect::Init_Texture()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// ��ǻ�� �ʱ�ȭ
	m_pTextureCom[TEXTURE_DIFFUSE] = (CTexture*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Texture_EffectDiffuse"));

	// ������ �ʱ�ȭ
	m_pTextureCom[TEXTURE_NOISE] = (CTexture*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Texture_EffectMoise"));

	// ����ũ �ʱ�ȭ
	m_pTextureCom[TEXTURE_MASK] = (CTexture*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Texture_EffectMask"));

	m_iTextureIndex = 0;
	m_iNoiseIndex = 0;

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CEffect::Client_Init_Texture(_uint iType, wstring _strPath)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (TEXTURE_DIFFUSE == iType)
	{
		// ��ǻ�� �ʱ�ȭ
		CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, _strPath);
		if (nullptr != pTexture)
		{
			m_pTextureCom[TEXTURE_DIFFUSE] = (CTexture*)pTexture->Clone(nullptr);
			Set_PrototypePath(_strPath);
			Safe_Release(pTexture);
		}
	}
	else if (TEXTURE_NOISE == iType)
	{
		CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, _strPath);
		if (nullptr != pTexture)
		{
			m_pTextureCom[TEXTURE_NOISE] = (CTexture*)pTexture->Clone(nullptr);
			Set_NoisePath(_strPath);
			Safe_Release(pTexture);
		}
	}

	m_iTextureIndex = 0;
	m_iNoiseIndex = 0;

	Safe_Release(pGameInstance);
	return S_OK;
}

wstring CEffect::Get_TexturePath(_uint iType)
{
	if (TEXTURE_DIFFUSE == iType)
		return m_strPrototypePath;
	else if (TEXTURE_NOISE == iType)
		return m_strNoisePath;
	else
		return L"";
}

void CEffect::Set_TexturePath(_uint iType, wstring strPath)
{
	if (TEXTURE_DIFFUSE == iType)
		m_strPrototypePath = strPath;
	else if (TEXTURE_NOISE == iType)
		m_strNoisePath = strPath;
}

_uint CEffect::Get_TextureIndex(_uint iType)
{
	if (TEXTURE_DIFFUSE == iType)
		return m_iTextureIndex;
	else if (TEXTURE_NOISE == iType)
		return m_iNoiseIndex;
	else
		return 0;
}

void CEffect::Set_TextureIndex(_uint iType, _uint iIndex)
{
	if (TEXTURE_DIFFUSE == iType)
		m_iTextureIndex = iIndex;
	else if (TEXTURE_NOISE == iType)
		m_iNoiseIndex = iIndex;
}

CEffect* CEffect::Clone(void* _pArg)
{
	return nullptr;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffectTransform);
	Safe_Release(m_pRendererCom);
	for (int i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom_Rect);
	Safe_Release(m_pVIBufferCom_Model);
	Safe_Release(m_pVIBufferCom_Instance);
}
