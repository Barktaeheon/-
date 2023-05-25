#include "Player_GamePad_Controller.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Animator.h"
#include "Player.h"


CPlayer_GamePad_Controller::CPlayer_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGamePad_InputController(_pDevice, _pContext)
{
}

HRESULT CPlayer_GamePad_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_GamePad_Controller::Tick(const _double& _dTimeDelta)
{	// InputController���� �̹� �������� Ű �Է°��� �޾ƿ´�.
	__super::Tick(_dTimeDelta);

	m_TriggerList.clear();

	// ī�޶��� Right, Up, Look�� �޾ƿ´�

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator* pPlayerAnimator = nullptr;
	// �÷��̾� ��ü�� �޾ƿԴٸ� �ִϸ����͸� �޾ƿ´�.
	if (nullptr != m_pPlayer)
		pPlayerAnimator = m_pPlayer->Get_Animator();

	// �÷��̾� �ִϸ����Ͱ� nullptr�̶�� �����Ѵ�.
	if (nullptr == pPlayerAnimator)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// ���� ������� ī�޶� �޾ƿ´�.
	CCamera_Free* pCamera = (CCamera_Free*)pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CAM, 0);
	if (Is_Nullptr(pCamera))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	CTransform* pCamTransform = pCamera->Get_Transform();
	if (Is_Nullptr(pCamTransform))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// ī�޶��� Right Up Look�� �̿��Ͽ� Look�� ���� ����
	_vector vDir = XMVectorZero();

	_vector vCamRight = pCamTransform->Get_Right();
	_vector vCamUp = pCamTransform->Get_Up();
	_vector vCamLook = pCamTransform->Get_Look();

	// MOVEüũ -> MOVE�� ��쿡�� vDir�� ī�޶��� Right Up Look�� �ٲ�����Ѵ�.
	{
		if (true == m_GamePadPressing[GPK_LEFT_STICKUP])
		{
			// ī�޶��� Look�� Dir�� �����ش�
			vDir += vCamLook;

			// �޸��������� L������ STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK_UPPER");
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKLEFT])
		{
			// ī�޶��� Right�� Dir�� ���ش�
			vDir -= vCamRight;

			// �޸��������� L������ STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKDOWN])
		{
			// ī�޶��� Look�� Dir�� ���ش�
			vDir -= vCamLook;

			// �޸��������� L������ STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK_DOWN");
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKRIGHT])
		{
			// ī�޶��� Right�� Dir�� �����ش�
			vDir += vCamRight;

			// �޸��������� L������ STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		// STEP üũ -> LŰ�� �̹� �������ִ� ��Ȳ���� ����Ű�� Down
		if (m_GamePadPressing[GPK_B])
		{
			if (m_GamePadPressing[GPK_LEFT_STICKUP])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKDOWN])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKLEFT])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKRIGHT])
				Add_Trigger("STEP");

			if (true == m_GamePadDown[GPK_LEFT_STICKUP])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKDOWN])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKLEFT])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKRIGHT])
				Add_Trigger("STEP");
		}

		if (true == m_GamePadDown[GPK_LEFT_STICKUP])
		{
			// ���ݰ� �̵��� ���ÿ� �����ٸ� ������ ����
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//LŰ�� ���������� ���� ��쿡�� MOVE Ʈ���Ÿ� �����Ѵ�.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir += vCamLook;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKLEFT])
		{
			// ���ݰ� �̵��� ���ÿ� �����ٸ� ������ ����
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//LŰ�� ���������� ���� ��쿡�� MOVE Ʈ���Ÿ� �����Ѵ�.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir -= vCamRight;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKDOWN])
		{
			// ���ݰ� �̵��� ���ÿ� �����ٸ� ������ ����
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//LŰ�� ���������� ���� ��쿡�� MOVE Ʈ���Ÿ� �����Ѵ�.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir -= vCamLook;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKRIGHT])
		{
			// ���ݰ� �̵��� ���ÿ� �����ٸ� ������ ����
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//LŰ�� ���������� ���� ��쿡�� MOVE Ʈ���Ÿ� �����Ѵ�.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir += vCamRight;
				}
			}
		}
		// Dash üũ
		{
			if (m_GamePadDown[GPK_B])
			{
				// ����Ű�� ������ �ʾ����� L��ư�� ������ �뽬�Ѵ�.
				if (0.f == XMVectorGetX(vDir) && 0.f == XMVectorGetY(vDir) && 0.f == XMVectorGetZ(vDir))
					Add_Trigger("DASH");
			}
		}
		// ��ų üũ
		{
			if (m_GamePadDown[GPK_Y])
			{
				// ����Ű�� ������ �ʾ����� L��ư�� ������ �뽬�Ѵ�.
				if (0.f == XMVectorGetX(vDir) && 0.f == XMVectorGetY(vDir) && 0.f == XMVectorGetZ(vDir))
					Add_Trigger("SKILL_1");
			}
		}
		// ���� ��� üũ
		{
			if (m_GamePadDown[GPK_LEFT_TRIGGER])
				Add_Trigger("ULTIMATE");
		}
		// ���� üũ
		{
			if (m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
		}

		// ���� üũ
		{
			if (nullptr != m_pPlayer)
			{
				_string strAnimType = m_pPlayer->Get_Animator()->Get_AnimationType();
				if ("AIR_DOWN" == strAnimType)
				{
					if (m_GamePadPressing[GPK_A])
					{
						Add_Trigger("PASSIVE");
					}
				}
			}
		}

		// ������ �� ���� �ô��� üũ / ���� üũ
		{
			if (m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");
				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			if (m_GamePadPressing[GPK_RIGHT_TRIGGER])
				Add_Trigger("CHARGE_ATTACK");
			if (!m_GamePadPressing[GPK_RIGHT_TRIGGER])
				Add_Trigger("CHARGE_ATTACK_END");
		}

		// ���� üũ
		{
			if (m_GamePadDown[GPK_LEFT_SHOULDER])
				Add_Trigger("AWAKE");
		}

		// ��� üũ
		{
			if (m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");

			// O��ư�� Pressing�ǰ� ������
			if (m_GamePadPressing[GPK_RIGHT_SHOULDER])
			{
				// �뽬���̾��ٸ� GUARD Ʈ���Ÿ� �����Ѵ�.
				if ("DASH" == pPlayerAnimator->Get_AnimationType())
					Add_Trigger("GUARD");

				if (m_GamePadDown[GPK_LEFT_STICKUP])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKLEFT])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKDOWN])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKRIGHT])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_Y])
					Add_Trigger("SKILL_3");
				else if (m_GamePadDown[GPK_X])
					Add_Trigger("THROW");
			}
			else
				// Pressing�ǰ����� �ʴٸ� GUARD_END�� ������.
				Add_Trigger("GUARD_END");
		}

		if (m_GamePadPressing[GPK_LEFT_STICKUP] && m_GamePadPressing[GPK_LEFT_STICKRIGHT]
			&& m_GamePadPressing[GPK_LEFT_STICKDOWN] && m_GamePadPressing[GPK_LEFT_STICKRIGHT])
			vDir = XMVectorZero();

		m_vPlayerDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

		// ���� �÷��̾ �����̰��ִµ� �ƹ�Ű�� ������ �ʾҴٸ� MOVE_END�� Ʈ���ſ� �ִ´�.
		if ("MOVE" == pPlayerAnimator->Get_AnimationType())
		{
			if (CMethod::Is_Vector_Zero(m_vPlayerDir.Get_Vector()))
				Add_Trigger("MOVE_END");
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}
void CPlayer_GamePad_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CPlayer_GamePad_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CPlayer_GamePad_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.push_back(&m_TriggerList);
	m_OutVoidDatas.push_back(&m_vPlayerDir);
	return m_OutVoidDatas;
}

_bool CPlayer_GamePad_Controller::Make_Trigger(_string _strTrigger, _bool* pFirstInput, _bool* pSecondInput)
{
	if (false == *pFirstInput || false == *pSecondInput)
		return false;

	if (nullptr == pSecondInput)
	{
		if (true == *pFirstInput)
			Add_Trigger(_strTrigger);
	}
	else
	{
		if (true == *pFirstInput)
		{
			if (true == *pSecondInput)
				Add_Trigger(_strTrigger);
		}
	}

	return true;
}

void CPlayer_GamePad_Controller::Add_Trigger(_string strTrigger)
{
	_bool isSame = false;
	for (auto& trigger : m_TriggerList)
	{
		if (trigger == strTrigger)
		{
			isSame = true;
			return;
		}
	}

	// �����Ѱ� ���ٸ� Ʈ���� ����Ʈ�� Ʈ���Ÿ� �߰��Ѵ�.
	if (false == isSame)
		m_TriggerList.push_back(strTrigger);
}

void CPlayer_GamePad_Controller::Free()
{
	__super::Free();
}
