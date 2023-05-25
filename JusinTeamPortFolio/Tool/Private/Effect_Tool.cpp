#include "Effect_Tool.h"
#include "GameInstance.h"
#include "EffectObject.h"
#include "Effect.h"
#include "ImageEffect.h"
#include "StaticMeshEffect.h"
#include "AnimMeshEffect.h"
#include "InstanceEffect.h"

CEffect_Tool::CEffect_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CEffect_Tool::NativeConstruct()
{
	m_pName = "Effect_Tool";
	m_tWinDesc = WINDESC("Effect_Tool", ImGuiWindowFlags_NoCollapse, ImVec2(1200.f, 50.f), ImVec2(700.f, 1000.f), false);
	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&m_tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&m_tInstanceEffectDesc, sizeof(INSTANCEDESC));

	return S_OK;
}

HRESULT CEffect_Tool::NativeConstruct_InnerValue()
{
	return S_OK;
}

HRESULT CEffect_Tool::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	ImGui::SetNextWindowPos(m_tWinDesc.v2Pos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);
	ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable, m_tWinDesc.imGuiWindowFlags);


	/*_float a = (float(rand()) / float((RAND_MAX))) + 360.f;
	ImGui::Text("%f", a);*/

	// ����ü ������ �ٲٱ�뵵
	/*if (ImGui::Button("AllLoad"))
	{
		AllLoad();
	}
	if (ImGui::Button("AllSave"))
	{
		AllSave();
	}*/

	ImGui::PushItemWidth(150.f);
	ImGui::InputText("EffectName", m_szAddEfcPoolName, sizeof(char) * MAX_PATH);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputInt(u8"������ ī��Ʈ(0���� Ŀ����)", &m_iPoolCount);
	ImGui::PopItemWidth();

	if (ImGui::Button(u8"AddObjectPool"))
	{
		if (0 < m_iPoolCount)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_tchar* pEfcName = new _tchar[MAX_PATH];
			lstrcpy(pEfcName, CMethod::ConvertSToW(m_szAddEfcPoolName).c_str());
			m_StrList.push_back(pEfcName);

			if (FAILED(pGameInstance->Clear_List(pEfcName)))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_ObjectPool(pEfcName, 0, m_iPoolCount, pEfcName, 0)))
				return E_FAIL;

			RELEASE_INSTANCE(CGameInstance);
		}
	}

	ImGui::NewLine();

	if (ImGui::BeginTabBar("MyTabBrr"))
	{
		if (ImGui::BeginTabItem("EffectObjects"))
		{
			ImGui::Text("EffectObject List");
			if (ImGui::BeginListBox("", ImVec2(400.f, 100.f)))
			{
				int iCount = 0;
				for (int n = 0; n < m_vecEffect.size(); n++)
				{
					string strSelectName = "";
					const bool is_selected = (m_iCurEffectObjectIndex == n);
					if (!lstrcmp(L"", static_cast<CEffectObject*>(m_vecEffect[n])->Get_Name().c_str()))
						strSelectName = CMethod::ConvertWToS(L"EffectObject_" + to_wstring(iCount++));
					else
						strSelectName = CMethod::ConvertWToS(static_cast<CEffectObject*>(m_vecEffect[n])->Get_Name());

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
						m_iCurEffectObjectIndex = n;

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			// ����Ʈ ������Ʈ ����
			CreateEffect();
			// ���� : 0�̸� EffectObject, 1�̸� Effect
			DeleteEffect(0);
			// ����Ʈ �ε�
			Load();
			// EffectObject ����Ʈ ��� ���
			ShowEffectObject();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Effects"))
		{
			// Effect ���� �ɼ�
			if (0 < m_vecEffect.size())
			{
				// ����Ʈ ���οɼ�
				if (0 < static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount())
				{
					m_iCurEffectIndex = static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_ToolCurEffect();
					vector<CEffect*> vecEffects = static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_vecEffect();

					ImGui::Text("EffectList");
					if (ImGui::BeginListBox("", ImVec2(400.f, 100.f)))
					{
						for (int n = 0; n < vecEffects.size(); n++)
						{
							string strSelectName = "";
							const bool is_selected = (m_iCurEffectIndex == n);

							strSelectName = to_string(n);
							strSelectName += CMethod::ConvertWToS(static_cast<CEffect*>(vecEffects[n])->Get_EffectName());

							if (ImGui::Selectable(strSelectName.c_str(), is_selected))
							{
								m_iCurEffectIndex = n;
								static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_ToolCurEffect(m_iCurEffectIndex);
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndListBox();
					}

					// �÷��� ��ư
					if (ImGui::Button("Play"))
					{
						static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Play(true);
					}
					else
					{
						m_bPause = false;
						static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Pause(m_bPause);
					}
					ImGui::SameLine();
					if (ImGui::Checkbox("Loop", &m_tEffectObjectDesc.bLoop))
						static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Loop(true);
					else
						static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Loop(false);
					ImGui::SameLine();
					if (ImGui::Button("Pause"))
					{
						if (static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_Play())
						{
							m_bPause = !m_bPause;
							static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Pause(m_bPause);
						}
					}

					// 0�̸� EffectObject, 1�̸� Effect
					DeleteEffect(1);

					ImGui::SameLine();

					// ����Ʈ ����
					if (ImGui::Button("Copy"))
					{
						EffectCopy();
					}

					// Effect �� ������ �����ͼ� Tool �� EffectDesc �� �Ҵ�����
					Get_EffectDesc(vecEffects[m_iCurEffectIndex]);

					// ���� Effects�� size�� ���� Index�� ������ ���λ��� X
					if (m_iCurEffectIndex != vecEffects.size())
					{
						// Effect �� ���������� ������
						ShowEffect(vecEffects[m_iCurEffectIndex], static_cast<CEffect*>(vecEffects[m_iCurEffectIndex])->Get_EffectType(), m_iCurEffectIndex);
					}

					// EffectDesc �� ������ ������ ������ ����
					Set_EffectDesc(vecEffects[m_iCurEffectIndex]);
				}
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CEffect_Tool::Render()
{
	if (__super::Render())
		return E_FAIL;


#ifdef _DEBUG
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		BoundingBox XBox(_float3(2.f, 0.f, 0.f), _float3(2.f, 0.1f, 0.1f));
		if (pGameInstance->Draw(XBox, CDrawing::VIEW_3D, Colors::Red))
		{
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}
		BoundingBox YBox(_float3(0.f, 2.f, 0.f), _float3(0.1f, 2.f, 0.1f));
		if (pGameInstance->Draw(YBox, CDrawing::VIEW_3D, Colors::Green))
		{
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}
		BoundingBox ZBox(_float3(0.f, 0.f, 2.f), _float3(0.1f, 0.1f, 2.f));
		if (pGameInstance->Draw(ZBox, CDrawing::VIEW_3D, Colors::Blue))
		{
			RELEASE_INSTANCE(CGameInstance);
			return S_OK;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
#endif // _DEBUG

	return S_OK;
}

HRESULT CEffect_Tool::Release_InnerValue()
{
	for (auto& iter : m_vecEffect)
		Safe_Release(iter);

	m_vecEffect.clear();
	m_vecTextureTag.clear();
	m_StrList.clear();
	for (_uint i = 0; i < TYPE_END; ++i)
		m_vecTextureIndex[i].clear();
	for (_uint i = 0; i < MESH_END; ++i)
		m_vecMeshIndex[i].clear();

	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&m_tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&m_tInstanceEffectDesc, sizeof(INSTANCEDESC));
	return S_OK;
}

void CEffect_Tool::Show_Window()
{
}

void CEffect_Tool::Get_EffectObjectDesc(_uint iCount)
{
	ZeroMemory(&m_tEffectObjectDesc, sizeof(EFFECTDESC));
	m_tEffectObjectDesc = static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectObjectDesc();

	m_fEffectObjectPosArr[0] = m_tEffectObjectDesc.vPos.x;
	m_fEffectObjectPosArr[1] = m_tEffectObjectDesc.vPos.y;
	m_fEffectObjectPosArr[2] = m_tEffectObjectDesc.vPos.z;

	m_fEffectObjectRotArr[0] = m_tEffectObjectDesc.vRot.x;
	m_fEffectObjectRotArr[1] = m_tEffectObjectDesc.vRot.y;
	m_fEffectObjectRotArr[2] = m_tEffectObjectDesc.vRot.z;

	m_fEffectObjectSclArr[0] = m_tEffectObjectDesc.vScl.x;
	m_fEffectObjectSclArr[1] = m_tEffectObjectDesc.vScl.y;
	m_fEffectObjectSclArr[2] = m_tEffectObjectDesc.vScl.z;

	m_fMaxFrame = m_tEffectObjectDesc.fMaxFrame;
	m_fCurFrame = m_tEffectObjectDesc.fCurFrame;
}

void CEffect_Tool::Set_EffectObjectDesc(_uint iCount)
{
	m_tEffectObjectDesc.vPos.x = m_fEffectObjectPosArr[0];
	m_tEffectObjectDesc.vPos.y = m_fEffectObjectPosArr[1];
	m_tEffectObjectDesc.vPos.z = m_fEffectObjectPosArr[2];

	m_tEffectObjectDesc.vRot.x = m_fEffectObjectRotArr[0];
	m_tEffectObjectDesc.vRot.y = m_fEffectObjectRotArr[1];
	m_tEffectObjectDesc.vRot.z = m_fEffectObjectRotArr[2];

	m_tEffectObjectDesc.vScl.x = m_fEffectObjectSclArr[0];
	m_tEffectObjectDesc.vScl.y = m_fEffectObjectSclArr[1];
	m_tEffectObjectDesc.vScl.z = m_fEffectObjectSclArr[2];

	static_cast<CEffectObject*>(m_vecEffect[iCount])->Set_EffectObjectDesc(m_tEffectObjectDesc);
}


void CEffect_Tool::Get_EffectDesc(CEffect* pEffect)
{
	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	m_tEffectDesc = static_cast<CEffect*>(pEffect)->Get_EffectDesc();

	m_fEffectPosArr[0] = m_tEffectDesc.vPos.x;
	m_fEffectPosArr[1] = m_tEffectDesc.vPos.y;
	m_fEffectPosArr[2] = m_tEffectDesc.vPos.z;

	m_fEffectRotArr[0] = m_tEffectDesc.vRot.x;
	m_fEffectRotArr[1] = m_tEffectDesc.vRot.y;
	m_fEffectRotArr[2] = m_tEffectDesc.vRot.z;

	m_fEffectSclArr[0] = m_tEffectDesc.vScl.x;
	m_fEffectSclArr[1] = m_tEffectDesc.vScl.y;
	m_fEffectSclArr[2] = m_tEffectDesc.vScl.z;

	/*m_fEffectDeltaScl[0] = m_tEffectDesc.vScaleDelta.x;
	m_fEffectDeltaScl[1] = m_tEffectDesc.vScaleDelta.y;
	m_fEffectDeltaScl[2] = m_tEffectDesc.vScaleDelta.z;*/


	m_fScrollSpeeds[0] = m_tEffectDesc.vScrollSpeeds.x;
	m_fScrollSpeeds[1] = m_tEffectDesc.vScrollSpeeds.y;
	m_fScrollSpeeds[2] = m_tEffectDesc.vScrollSpeeds.z;

	m_fScales[0] = m_tEffectDesc.vNoiseScale.x;
	m_fScales[1] = m_tEffectDesc.vNoiseScale.y;
	m_fScales[2] = m_tEffectDesc.vNoiseScale.z;

	m_fDistortion1[0] = m_tEffectDesc.vDistortion1.x;
	m_fDistortion1[1] = m_tEffectDesc.vDistortion1.y;

	m_fDistortion2[0] = m_tEffectDesc.vDistortion2.x;
	m_fDistortion2[1] = m_tEffectDesc.vDistortion2.y;

	m_fDistortion3[0] = m_tEffectDesc.vDistortion3.x;
	m_fDistortion3[1] = m_tEffectDesc.vDistortion3.y;

	m_fEffectDir[0] = m_tEffectDesc.vDir.x;
	m_fEffectDir[1] = m_tEffectDesc.vDir.y;
	m_fEffectDir[2] = m_tEffectDesc.vDir.z;

	m_fEffectColor[0] = m_tEffectDesc.vColor.x;
	m_fEffectColor[1] = m_tEffectDesc.vColor.y;
	m_fEffectColor[2] = m_tEffectDesc.vColor.z;
	m_fEffectColor[3] = m_tEffectDesc.vColor.w;

	m_fEffectUVSpeed[0] = m_tEffectDesc.vUVSpeed.x;
	m_fEffectUVSpeed[1] = m_tEffectDesc.vUVSpeed.y;
}

void CEffect_Tool::Set_EffectDesc(CEffect* pEffect)
{
	m_tEffectDesc.vPos.x = m_fEffectPosArr[0];
	m_tEffectDesc.vPos.y = m_fEffectPosArr[1];
	m_tEffectDesc.vPos.z = m_fEffectPosArr[2];

	m_tEffectDesc.vRot.x = m_fEffectRotArr[0];
	m_tEffectDesc.vRot.y = m_fEffectRotArr[1];
	m_tEffectDesc.vRot.z = m_fEffectRotArr[2];

	m_tEffectDesc.vScl.x = m_fEffectSclArr[0];
	m_tEffectDesc.vScl.y = m_fEffectSclArr[1];
	m_tEffectDesc.vScl.z = m_fEffectSclArr[2];

	/*m_tEffectDesc.vScaleDelta.x = m_fEffectDeltaScl[0];
	m_tEffectDesc.vScaleDelta.y = m_fEffectDeltaScl[1];
	m_tEffectDesc.vScaleDelta.z = m_fEffectDeltaScl[2];*/

	m_tEffectDesc.vScrollSpeeds.x = m_fScrollSpeeds[0];
	m_tEffectDesc.vScrollSpeeds.y = m_fScrollSpeeds[1];
	m_tEffectDesc.vScrollSpeeds.z = m_fScrollSpeeds[2];

	m_tEffectDesc.vNoiseScale.x = m_fScales[0];
	m_tEffectDesc.vNoiseScale.y = m_fScales[1];
	m_tEffectDesc.vNoiseScale.z = m_fScales[2];

	m_tEffectDesc.vDistortion1.x = m_fDistortion1[0];
	m_tEffectDesc.vDistortion1.y = m_fDistortion1[1];

	m_tEffectDesc.vDistortion2.x = m_fDistortion2[0];
	m_tEffectDesc.vDistortion2.y = m_fDistortion2[1];

	m_tEffectDesc.vDistortion3.x = m_fDistortion3[0];
	m_tEffectDesc.vDistortion3.y = m_fDistortion3[1];

	m_tEffectDesc.vDir = _float3(m_fEffectDir[0], m_fEffectDir[1], m_fEffectDir[2]);

	m_tEffectDesc.vColor = _float4(m_fEffectColor[0], m_fEffectColor[1], m_fEffectColor[2], m_fEffectColor[3]);

	m_tEffectDesc.vUVSpeed = _float2(m_fEffectUVSpeed[0], m_fEffectUVSpeed[1]);

	static_cast<CEffect*>(pEffect)->Set_EffectDesc(m_tEffectDesc);
}

void CEffect_Tool::EffectButtonDraw(_uint iCount)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
	InitEffectDesc(&tEffectDesc);

	_wstring strEffectName = L"";


	if (ImGui::Button("Add_ImageEffect"))
	{
		CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_IMAGE_TOOL, &tEffectDesc);
		if (nullptr != pEffect)
		{
			// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����			
			//strEffectName = to_wstring(static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectCount());
			strEffectName = L"_ImageEffect";
			static_cast<CEffect*>(pEffect)->Set_EffectName(strEffectName);
			// 0 ��° �̹����� �ʱ�ȭ
			pEffect->Init_Texture();
			static_cast<CEffectObject*>(m_vecEffect[iCount])->Add_Effect(pEffect);
		}
		else
			MSG_BOX("�̹��� ����Ʈ ���� ����");
	}
	ImGui::SameLine();
	if (ImGui::Button("Add_MeshEffect"))
	{
		PATHS vecPath = { L"Model",L"EffectModel",L"FBX_Converter" };
		FILEGROUP* pEffectFolder = pGameInstance->Find_Folder(&vecPath);
		if (0 < pEffectFolder->vecFileDataList.size())
		{
			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_STATICMESH_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				//strEffectName = to_wstring(static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectCount());
				strEffectName = L"_MeshEffect";
				// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����
				static_cast<CEffect*>(pEffect)->Set_EffectName(strEffectName);
				// 0 ��° �̹����� �ʱ�ȭ
				pEffect->Init_Texture();

				// ó�� �����ϸ� ������ 0 ��° �ε����� ���ҽ��� �� �ʱ�ȭ
				_matrix PivotMatrix = XMMatrixIdentity();
				PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
				_wstring strProtoTag = L"Prototype_Component_Model_";
				strProtoTag += pEffectFolder->vecFileDataList[0].strFileName;
				pEffect->Set_MeshProtoTag(strProtoTag);
				CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
				//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pEffectFolder, &pEffectFolder->vecFileDataList[0], PivotMatrix);
				if (nullptr != pModel)
				{
					static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, 0);
					static_cast<CStaticMeshEffect*>(pEffect)->Set_MeshPrototypePath(pEffectFolder->vecFileDataList[0].strFilePath);
					static_cast<CEffectObject*>(m_vecEffect[iCount])->Add_Effect(pEffect);
					//Safe_Release(pModel);

					m_vecMeshIndex[MESH_STATIC].push_back(0);
				}
			}
			else
				MSG_BOX("����ƽ �޽� ���� ����");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Add_AnimMeshEffect"))
	{
		PATHS vecPath = { L"AnimationModel",L"EffectModel",L"FBX_Converter" };
		FILEGROUP* pEffectFolder = pGameInstance->Find_Folder(&vecPath);
		if (0 < pEffectFolder->vecFileDataList.size())
		{
			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_ANIMMESH_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				//strEffectName = to_wstring(static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectCount());
				strEffectName = L"_AnimEffect";
				// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����
				static_cast<CEffect*>(pEffect)->Set_EffectName(strEffectName);
				// ó�� �����ϸ� ������ 0 ��° �ε����� ���ҽ��� �� �ʱ�ȭ
				_matrix PivotMatrix = XMMatrixIdentity();
				PivotMatrix *= XMMatrixScaling(0.001f, 0.001f, 0.001f);
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pEffectFolder, &pEffectFolder->vecFileDataList[0], PivotMatrix);
				if (nullptr != pModel)
				{
					static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel);
					static_cast<CAnimMeshEffect*>(pEffect)->Set_MeshPrototypePath(pEffectFolder->vecFileDataList[0].strFilePath);
					static_cast<CEffectObject*>(m_vecEffect[iCount])->Add_Effect(pEffect);
					//Safe_Release(pModel);

					m_vecMeshIndex[MESH_ANIM].push_back(0);
				}
			}
			else
				MSG_BOX("�ִϸ��̼� �޽� ���� ����");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Add_InstanceEffect"))
	{
		CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_INSTANCE_TOOL, &tEffectDesc);
		if (nullptr != pEffect)
		{
			//strEffectName = to_wstring(static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectCount());
			strEffectName = L"_InstanceEffect";
			// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����
			static_cast<CEffect*>(pEffect)->Set_EffectName(strEffectName);
			// 0 ��° �̹����� �ʱ�ȭ
			pEffect->Init_Texture();
			static_cast<CEffectObject*>(m_vecEffect[iCount])->Add_Effect(pEffect);
		}
		else
			MSG_BOX("�ν��Ͻ� ����Ʈ ���� ����");
	}

	Safe_Release(pGameInstance);
}

// �߰��� ����Ʈ ���� ����â ���
void CEffect_Tool::ShowEffect(CEffect* pEffect, _uint iEffectType, _uint iCount)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	wstring strTemp;
	wstring strTextureListBox;
	wstring strMeshListBox;

	// ����Ʈ ����
	wstring strDeleteW = L"";
	wstring strNameW = L"";

	// ����Ʈ������Ʈ�� ���� �������� �����´�.
	Get_EffectObjectFrame(m_iCurEffectObjectIndex);
	// ����Ʈ �����
	RatioBar();

	if (0 == iEffectType)
	{
		ImGui::PushItemWidth(200.f);
		if (ImGui::TreeNode((_string("Transform##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("Position", m_fEffectPosArr, 0.01f);
			ImGui::DragFloat3("Rotation", m_fEffectRotArr, 0.1f);
			ImGui::DragFloat3("Scale", m_fEffectSclArr, 0.01f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"�̵�##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("Dir", m_fEffectDir, 0.01f);
			ImGui::InputFloat("Force", &m_tEffectDesc.fForce);
			ImGui::Text("Acceleration");
			ImGui::SameLine();
			ImGui::Text("%.3f", m_tEffectDesc.fAcceleration);
			ImGui::SameLine();
			ImGui::InputFloat("AccSpeed", &m_tEffectDesc.fAccSpeed);
			ImGui::TreePop();
		}

		ImGui::PopItemWidth();

		ImGui::NewLine();

		// �ؽ��� ����Ʈ
		PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };

		if (ImGui::TreeNode((_string("Diffuse##_") + to_string(iCount)).c_str()))
		{
			Find_Texture(vecTexturePath, pEffect, CEffect::TEXTURE_DIFFUSE);
			ImGui::TreePop();
		}

		//ImGui::NewLine();
		if (ImGui::TreeNode((_string("Color##_") + to_string(iCount)).c_str()))
		{
			ImGui::PushItemWidth(300.f);
			static bool alpha_preview = true;
			static bool options_menu = true;
			static bool alpha_bar = true;
			ImGuiColorEditFlags flags = (alpha_bar ? ImGuiColorEditFlags_AlphaBar : 0) | (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0);

			ImGui::ColorPicker4("MyColor", (float*)&m_fEffectColor, flags);

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		//ImGui::NewLine();

		ImGui::Checkbox("Enable Noise", &m_tEffectDesc.bNoise);

		if (m_tEffectDesc.bNoise)
		{
			// ������ �ؽ��� ����Ʈ
			PATHS vecNoisePath = { L"NoiseTexture" };

			if (ImGui::TreeNode((_string("Noise##_") + to_string(iCount)).c_str()))
			{
				Find_Texture(vecNoisePath, pEffect, CEffect::TEXTURE_NOISE);
				ImGui::TreePop();
			}
		}
		ImGui::NewLine();


		ImGui::PushItemWidth(200.f);
		

		ImGui::InputInt("PassIndex", &m_tEffectDesc.iPassIndex);
		if (0 > m_tEffectDesc.iPassIndex)
			m_tEffectDesc.iPassIndex = 0;

		const char* RenderGroups[] = { "ALPHABLEND" , "RENDER_DISTORTION" , "NONLIGHT" };
		iRenderGroup = m_tEffectDesc.iRenderID;
		ImGui::Combo("RENDERGRUOP", &iRenderGroup, RenderGroups, IM_ARRAYSIZE(RenderGroups));
		m_tEffectDesc.iRenderID = iRenderGroup;

		ImGui::NewLine();

		ImGui::Checkbox(u8"��ü������", &m_tEffectDesc.bBillBoardAll);
		if (m_tEffectDesc.bBillBoardAll)
			m_tEffectDesc.bBillBoardExceptY = false;
		ImGui::SameLine();
		ImGui::Checkbox(u8"Y�� ����������", &m_tEffectDesc.bBillBoardExceptY);
		if (m_tEffectDesc.bBillBoardExceptY)
			m_tEffectDesc.bBillBoardAll = false;
		ImGui::SameLine();
		ImGui::Checkbox("CutBlack", &m_tEffectDesc.bCutBlack);
		ImGui::SameLine();
		ImGui::Checkbox("Glow", &m_tEffectDesc.bGlow);
		ImGui::SameLine();
		ImGui::Checkbox(u8"��������", &m_tEffectDesc.bOrthogonal);

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("EnableTime", &m_tEffectDesc.fEnableTime);
		ImGui::SameLine();
		ImGui::InputFloat("LifeTime", &m_tEffectDesc.fLifeTime);
		ImGui::InputFloat("AlphaTime", &m_tEffectDesc.fAlphaTime);
		ImGui::SameLine();
		ImGui::InputFloat("AlphaSpeed", &m_tEffectDesc.fAlphaSpeed);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(80.f);

		ImGui::NewLine();

		// UV Flow
		if (ImGui::TreeNode((_string("UV##_") + to_string(iCount)).c_str()))
		{
			ImGui::Checkbox("UVFlow", &m_tEffectDesc.bUVFlow);
			ImGui::PushItemWidth(250.f);
			ImGui::DragFloat2("UVSpeed", m_fEffectUVSpeed, 0.01f);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"��������Ʈ##_") + to_string(iCount)).c_str()))
		{
			ImGui::Checkbox("SliceLoop", &m_tEffectDesc.bSliceLoop);
			ImGui::InputInt("SliceX", &m_tEffectDesc.iSliceX);
			ImGui::InputInt("SliceY", &m_tEffectDesc.iSliceY);
			ImGui::TreePop();
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("MaxFrame", &m_tEffectDesc.fMaxFrame);
		ImGui::SameLine();
		ImGui::DragFloat("DelayTime", &m_tEffectDesc.fDelayTime);
		ImGui::PopItemWidth();


		static _int iImageScaleAxis = m_tEffectDesc.eScaleAxisType;
		ImGui::RadioButton(u8"X�� ũ��", &iImageScaleAxis, 0);
		ImGui::RadioButton(u8"Y�� ũ��", &iImageScaleAxis, 1);
		ImGui::RadioButton(u8"Z�� ũ��", &iImageScaleAxis, 2);
		ImGui::RadioButton(u8"��ü ũ��", &iImageScaleAxis, 3);
		if (0 == iImageScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_X;
		else if (1 == iImageScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Y;
		else if (2 == iImageScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Z;
		else
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_ALL;

		ImGui::InputFloat(u8"��ǥ ũ��", &m_tEffectDesc.fDestScale);

		static _int iImageRotAxis = m_tEffectDesc.eRotAxisType;
		ImGui::RadioButton(u8"X�� ȸ��", &iImageRotAxis, 0);
		ImGui::RadioButton(u8"Y�� ȸ��", &iImageRotAxis, 1);
		ImGui::RadioButton(u8"Z�� ȸ��", &iImageRotAxis, 2);
		if (0 == iImageRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_X;
		else if (1 == iImageRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Y;
		else
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Z;

		ImGui::InputFloat(u8"��ǥ ����", &m_tEffectDesc.fDestAngle);

		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
	}
	else if (1 == iEffectType)
	{
		ImGui::PushItemWidth(200.f);

		if (ImGui::TreeNode((_string("Transform##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("EffectPos", m_fEffectPosArr, 0.01f);
			ImGui::DragFloat3("EffectRot", m_fEffectRotArr, 0.1f);
			ImGui::DragFloat3("EffectScl", m_fEffectSclArr, 0.01f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"�̵�##_") + to_string(iCount)).c_str()))
		{
			if (ImGui::Checkbox(u8"Dir Look", &m_tEffectDesc.bGoLook))
			{
				m_tEffectDesc.bGoUp = false;
				m_tEffectDesc.bGoRight = false;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(u8"Dir Up", &m_tEffectDesc.bGoUp))
			{
				m_tEffectDesc.bGoLook = false;
				m_tEffectDesc.bGoRight = false;
			}
			ImGui::SameLine();
			if (ImGui::Checkbox(u8"Dir Right", &m_tEffectDesc.bGoRight))
			{
				m_tEffectDesc.bGoUp = false;
				m_tEffectDesc.bGoLook = false;
			}
			ImGui::DragFloat3("Dir", m_fEffectDir, 0.01f);
			ImGui::Checkbox("Jump", &m_tEffectDesc.bJump);
			ImGui::InputFloat("JumpPower", &m_tEffectDesc.fJumpPower);
			ImGui::Checkbox("Gravity", &m_tEffectDesc.bGravity);
			ImGui::InputFloat("GravityValue", &m_tEffectDesc.fGravity);
			ImGui::InputFloat("Force", &m_tEffectDesc.fForce);
			ImGui::Text("Acceleration");
			ImGui::Text("%.3f", m_tEffectDesc.fAcceleration);
			ImGui::SameLine();
			ImGui::InputFloat("AccSpeed", &m_tEffectDesc.fAccSpeed);
			ImGui::TreePop();
		}

		ImGui::PopItemWidth();

		ImGui::NewLine();



		// ����ƽ�޽�
		PATHS vecEffectPath = { L"Model",L"EffectModel",L"FBX_Converter" };
		// ����ƽ�޽� �ؽ���
		PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };

		if (ImGui::TreeNode((_string("Diffuse##_") + to_string(iCount)).c_str()))
		{
			Find_Texture(vecTexturePath, pEffect, CEffect::TEXTURE_DIFFUSE);
			ImGui::TreePop();
		}

		//ImGui::NewLine();
		if (ImGui::TreeNode((_string("Color##_") + to_string(iCount)).c_str()))
		{
			ImGui::PushItemWidth(300.f);
			static bool alpha_preview = true;
			static bool options_menu = true;
			static bool alpha_bar = true;
			ImGuiColorEditFlags flags = (alpha_bar ? ImGuiColorEditFlags_AlphaBar : 0) | (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0);

			ImGui::ColorPicker4("MyColor", (float*)&m_fEffectColor, flags);

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		//ImGui::NewLine();

		ImGui::Checkbox("Enable Noise", &m_tEffectDesc.bNoise);

		if (m_tEffectDesc.bNoise)
		{
			// ������ �ؽ��� ����Ʈ
			PATHS vecNoisePath = { L"NoiseTexture" };

			if (ImGui::TreeNode((_string("Noise##_") + to_string(iCount)).c_str()))
			{
				Find_Texture(vecNoisePath, pEffect, CEffect::TEXTURE_NOISE);
				ImGui::TreePop();
			}

			ImGui::PushItemWidth(200.f);
			ImGui::InputFloat3(u8"Scroll X�ӵ�", &m_tEffectDesc.vScrollSpeedsX.x);
			ImGui::InputFloat3(u8"ScrollSpeeds(�⺻: 1.3f, 10.f, 2.3f)", m_fScrollSpeeds);
			ImGui::InputFloat3(u8"Scalse(�⺻: 1.f, 2.f, 3.f)", m_fScales);
			ImGui::InputFloat2(u8"Distortion1(�⺻: 0.1f, 0.2f)", m_fDistortion1);
			ImGui::InputFloat2(u8"Distortion2(�⺻: 0.1f, 0.3f)", m_fDistortion2);
			ImGui::InputFloat2(u8"Distortion3(�⺻: 0.1f, 0.1f)", m_fDistortion3);
			ImGui::InputFloat(u8"DistortionScale(�⺻: 0.8f)", &m_tEffectDesc.fDistortionScale);
			ImGui::InputFloat(u8"DistortionBias(�⺻: 0.5f)", &m_tEffectDesc.fDistortionBias);
			ImGui::PopItemWidth();
		}

		ImGui::Checkbox(u8"����ũ�ؽ��� ����", &m_tEffectDesc.bEnableMask);
		if (m_tEffectDesc.bEnableMask)
		{
			ImGui::PushItemWidth(100.f);
			ImGui::InputInt("MaskIndex", &m_tEffectDesc.iMaskIndex);
			ImGui::PopItemWidth();
		}


		ImGui::NewLine();

		strMeshListBox = L"StaticMesh List_";
		strMeshListBox += to_wstring(m_iStaticMeshEffectCount);
		string strMeshListBoxName = CMethod::ConvertWToS(strMeshListBox);

		if (ImGui::TreeNode((_string("Mesh##_") + to_string(iCount)).c_str()))
		{
			Find_Mesh(vecEffectPath, pEffect, CEffect::EFFECT_STATICMESH);
			ImGui::TreePop();
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(200.f);



		ImGui::InputInt("PassIndex", &m_tEffectDesc.iPassIndex);
		if (0 > m_tEffectDesc.iPassIndex)
			m_tEffectDesc.iPassIndex = 0;

		const char* RenderGroups[] = { "ALPHABLEND" , "RENDER_DISTORTION" , "NONLIGHT" };
		iRenderGroup = m_tEffectDesc.iRenderID;
		ImGui::Combo("RENDERGRUOP", &iRenderGroup, RenderGroups, IM_ARRAYSIZE(RenderGroups));
		m_tEffectDesc.iRenderID = iRenderGroup;

		ImGui::NewLine();

		ImGui::Checkbox(u8"��ü������", &m_tEffectDesc.bBillBoardAll);
		if (m_tEffectDesc.bBillBoardAll)
			m_tEffectDesc.bBillBoardExceptY = false;
		ImGui::SameLine();
		ImGui::Checkbox(u8"Y�� ����������", &m_tEffectDesc.bBillBoardExceptY);
		if (m_tEffectDesc.bBillBoardExceptY)
			m_tEffectDesc.bBillBoardAll = false;
		ImGui::SameLine();
		ImGui::Checkbox("CutBlack", &m_tEffectDesc.bCutBlack);
		ImGui::SameLine();
		ImGui::Checkbox("Glow", &m_tEffectDesc.bGlow);
		if (m_tEffectDesc.bGlow)
			ImGui::InputFloat("GlowValue", &m_tEffectDesc.fGlowValue);

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("EnableTime", &m_tEffectDesc.fEnableTime);
		ImGui::SameLine();
		ImGui::InputFloat("LifeTime", &m_tEffectDesc.fLifeTime);
		ImGui::InputFloat("AlphaTime", &m_tEffectDesc.fAlphaTime);
		ImGui::SameLine();
		ImGui::InputFloat("AlphaSpeed", &m_tEffectDesc.fAlphaSpeed);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(100.f);

		ImGui::NewLine();

		// UV Flow
		if (ImGui::TreeNode((_string("UV##_") + to_string(iCount)).c_str()))
		{
			_int iSamplerMode = m_tEffectDesc.iSamplerMode;
			ImGui::RadioButton(u8"BASIC", &m_tEffectDesc.iSamplerMode, 0);
			ImGui::SameLine();
			ImGui::Text(u8"���� ����Ʈ ȣȯ��");
			ImGui::RadioButton(u8"WARP", &m_tEffectDesc.iSamplerMode, 1);
			ImGui::SameLine();
			ImGui::Text(u8"1�� �Ѿ�� �ݺ�");
			ImGui::RadioButton(u8"CLAMP", &m_tEffectDesc.iSamplerMode, 2);
			ImGui::SameLine();
			ImGui::Text(u8"1�� �Ѿ�� 1�� ����");
			ImGui::RadioButton(u8"UWVC", &m_tEffectDesc.iSamplerMode, 3);
			ImGui::SameLine();
			ImGui::Text(u8"U : WARP, V : CLAMP");
			ImGui::RadioButton(u8"UCVW", &m_tEffectDesc.iSamplerMode, 4);
			ImGui::SameLine();
			ImGui::Text(u8"U : CLAMP, V : WARP");

			ImGui::NewLine();

			ImGui::Text(u8"UV�� �ݺ��� ��� (�⺻ : 1)");
			ImGui::PushItemWidth(250.f);
			ImGui::InputFloat2("UV_Mul", &m_tEffectDesc.vUV_Mul.x);
			ImGui::PopItemWidth();

			ImGui::NewLine();

			ImGui::Checkbox("UVFlow", &m_tEffectDesc.bUVFlow);
			ImGui::PushItemWidth(250.f);
			ImGui::DragFloat2("UVSpeed", m_fEffectUVSpeed, 0.01f);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"��������Ʈ##_") + to_string(iCount)).c_str()))
		{
			ImGui::Checkbox("SliceLoop", &m_tEffectDesc.bSliceLoop);
			ImGui::InputInt("SliceX", &m_tEffectDesc.iSliceX);
			ImGui::InputInt("SliceY", &m_tEffectDesc.iSliceY);
			ImGui::InputFloat("SliceSpeed", &m_tEffectDesc.fSliceSpeed);
			ImGui::TreePop();
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("MaxFrame", &m_tEffectDesc.fMaxFrame);
		ImGui::SameLine();
		ImGui::DragFloat("DelayTime", &m_tEffectDesc.fDelayTime);
		ImGui::PopItemWidth();

		ImGui::Text("fCurDeltaAnlge : %.3f", m_tEffectDesc.fAngle);

		ImGui::NewLine();

		ImGui::Text(u8"������ ���� ������ ������ ��,��ü �� �ѹ��� 1������ ũ�⺯ȯ �Ҽ�������\nNew ũ�⺯ȯ�� ���ϴ� �࿡ ���� �Է��ϸ� ���� �ٸ� ũ�⺯ȯ�� ����");
		ImGui::PushItemWidth(200.f);
		ImGui::InputFloat3(u8"New ũ�⺯ȯ", &m_tEffectDesc.vScaleDelta.x);
		ImGui::PopItemWidth();

		_int iStaticMeshScaleAxis = m_tEffectDesc.eScaleAxisType;
		ImGui::RadioButton(u8"X�� ũ��", &iStaticMeshScaleAxis, 0);
		ImGui::RadioButton(u8"Y�� ũ��", &iStaticMeshScaleAxis, 1);
		ImGui::RadioButton(u8"Z�� ũ��", &iStaticMeshScaleAxis, 2);
		ImGui::RadioButton(u8"��ü ũ��", &iStaticMeshScaleAxis, 3);

		if (0 == iStaticMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_X;
		else if (1 == iStaticMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Y;
		else if (2 == iStaticMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Z;
		else
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_ALL;

		ImGui::InputFloat(u8"��ǥ ũ��", &m_tEffectDesc.fDestScale);

		_int iStaticMeshRotAxis = m_tEffectDesc.eRotAxisType;
		ImGui::RadioButton(u8"X�� ȸ��", &iStaticMeshRotAxis, 0);
		ImGui::RadioButton(u8"Y�� ȸ��", &iStaticMeshRotAxis, 1);
		ImGui::RadioButton(u8"Z�� ȸ��", &iStaticMeshRotAxis, 2);
		if (0 == iStaticMeshRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_X;
		else if (1 == iStaticMeshRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Y;
		else if (2 == iStaticMeshRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Z;

		ImGui::InputFloat(u8"��ǥ ����", &m_tEffectDesc.fDestAngle);

		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
	}
	else if (2 == iEffectType)
	{
		// �ִϸ��̼� �޽�
		PATHS vecEffectPath = { L"AnimationModel",L"EffectModel",L"FBX_Converter" };
		strMeshListBox = L"AnimMesh List_";
		strMeshListBox += to_wstring(m_iAnimMeshEffectCount);
		string strMeshListBoxName = CMethod::ConvertWToS(strMeshListBox);

		//ImGui::Text("AnimMesh List");
		if (ImGui::TreeNode((_string("AnimMesh##_") + to_string(iCount)).c_str()))
		{
			Find_Mesh(vecEffectPath, pEffect, CEffect::EFFECT_ANIMMESH);
			ImGui::TreePop();
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(200.f);

		if (ImGui::TreeNode((_string("Transform##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("EffectPos", m_fEffectPosArr, 0.01f);
			ImGui::DragFloat3("EffectRot", m_fEffectRotArr, 0.1f);
			ImGui::DragFloat3("EffectScl", m_fEffectSclArr, 0.01f, 0.1f, 100.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"�̵�##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("Direction", m_fEffectDir, 0.01f);
			ImGui::InputFloat("Force", &m_tEffectDesc.fForce);
			ImGui::Text("Acceleration");
			ImGui::Text("%.3f", m_tEffectDesc.fAcceleration);
			ImGui::SameLine();
			ImGui::InputFloat("AccSpeed", &m_tEffectDesc.fAccSpeed);
			ImGui::TreePop();
		}

		const char* RenderGroups[] = { "ALPHABLEND" , "RENDER_DISTORTION" , "NONLIGHT" };
		iRenderGroup = m_tEffectDesc.iRenderID;
		ImGui::Combo("RENDERGRUOP", &iRenderGroup, RenderGroups, IM_ARRAYSIZE(RenderGroups));
		m_tEffectDesc.iRenderID = iRenderGroup;

		ImGui::Checkbox("Glow", &m_tEffectDesc.bGlow);

		ImGui::PushItemWidth(100.f);

		ImGui::InputInt("PassIndex", &m_tEffectDesc.iPassIndex);
		if (0 > m_tEffectDesc.iPassIndex)
			m_tEffectDesc.iPassIndex = 0;

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("EnableTime", &m_tEffectDesc.fEnableTime);
		ImGui::SameLine();
		ImGui::InputFloat("LifeTime", &m_tEffectDesc.fLifeTime);
		ImGui::InputFloat("AlphaTime", &m_tEffectDesc.fAlphaTime);
		ImGui::SameLine();
		ImGui::InputFloat("AlphaSpeed", &m_tEffectDesc.fAlphaSpeed);


		ImGui::DragFloat("MaxFrame", &m_tEffectDesc.fMaxFrame);
		ImGui::SameLine();
		ImGui::DragFloat("DelayTime", &m_tEffectDesc.fDelayTime);
		ImGui::PopItemWidth();

		static _int iAnimMeshScaleAxis = m_tEffectDesc.eScaleAxisType;
		ImGui::RadioButton(u8"X�� ũ��", &iAnimMeshScaleAxis, 0);
		ImGui::RadioButton(u8"Y�� ũ��", &iAnimMeshScaleAxis, 1);
		ImGui::RadioButton(u8"Z�� ũ��", &iAnimMeshScaleAxis, 2);
		ImGui::RadioButton(u8"��ü ũ��", &iAnimMeshScaleAxis, 3);
		if (0 == iAnimMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_X;
		else if (1 == iAnimMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Y;
		else if (2 == iAnimMeshScaleAxis)
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_Z;
		else
			m_tEffectDesc.eScaleAxisType = EFFECTDESC::Axis_ALL;

		ImGui::InputFloat(u8"��ǥ ũ��", &m_tEffectDesc.fDestScale);

		static _int iAnimMeshRotAxis = m_tEffectDesc.eRotAxisType;
		ImGui::RadioButton(u8"X�� ȸ��", &iAnimMeshRotAxis, 0);
		ImGui::RadioButton(u8"Y�� ȸ��", &iAnimMeshRotAxis, 1);
		ImGui::RadioButton(u8"Z�� ȸ��", &iAnimMeshRotAxis, 2);
		if (0 == iAnimMeshRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_X;
		else if (1 == iAnimMeshRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Y;
		else
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Z;

		ImGui::InputFloat(u8"��ǥ ����", &m_tEffectDesc.fDestAngle);

		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
	}
	else
	{
		// �ν��Ͻ� ����Ʈ �ؽ���
		PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };

		if (ImGui::TreeNode((_string("Diffuse##_") + to_string(iCount)).c_str()))
		{
			Find_Texture(vecTexturePath, pEffect, CEffect::TEXTURE_DIFFUSE);
			ImGui::TreePop();
		}

		//ImGui::NewLine();
		if (ImGui::TreeNode((_string("Color##_") + to_string(iCount)).c_str()))
		{
			ImGui::PushItemWidth(300.f);
			static bool alpha_preview = true;
			static bool options_menu = true;
			static bool alpha_bar = true;
			ImGuiColorEditFlags flags = (alpha_bar ? ImGuiColorEditFlags_AlphaBar : 0) | (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0);

			ImGui::ColorPicker4("MyColor", (float*)&m_fEffectColor, flags);

			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		//ImGui::NewLine();

		ImGui::Checkbox("Enable Noise", &m_tEffectDesc.bNoise);

		if (m_tEffectDesc.bNoise)
		{
			// ������ �ؽ��� ����Ʈ
			PATHS vecNoisePath = { L"NoiseTexture" };

			if (ImGui::TreeNode((_string("Noise##_") + to_string(iCount)).c_str()))
			{
				Find_Texture(vecNoisePath, pEffect, CEffect::TEXTURE_NOISE);
				ImGui::TreePop();
			}
		}
		ImGui::NewLine();

		ImGui::PushItemWidth(200.f);

		if (ImGui::TreeNode((_string("Transform##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("EffectPos", m_fEffectPosArr, 0.01f);
			ImGui::DragFloat3("EffectRot", m_fEffectRotArr, 0.1f);
			ImGui::DragFloat3("EffectScl", m_fEffectSclArr, 0.01f, 0.f, 100.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"�̵�##_") + to_string(iCount)).c_str()))
		{
			ImGui::DragFloat3("Dir", m_fEffectDir, 0.01f);
			ImGui::InputFloat("Force", &m_tEffectDesc.fForce);
			ImGui::Text("Acceleration");
			ImGui::Text("%.3f", m_tEffectDesc.fAcceleration);
			ImGui::SameLine();
			ImGui::InputFloat("AccSpeed", &m_tEffectDesc.fAccSpeed);
			ImGui::TreePop();
		}

		ImGui::InputInt("PassIndex", &m_tEffectDesc.iPassIndex);
		if (0 > m_tEffectDesc.iPassIndex)
			m_tEffectDesc.iPassIndex = 0;

		const char* RenderGroups[] = { "ALPHABLEND" , "RENDER_DISTORTION" , "NONLIGHT" };
		iRenderGroup = m_tEffectDesc.iRenderID;
		ImGui::Combo("RENDERGRUOP", &iRenderGroup, RenderGroups, IM_ARRAYSIZE(RenderGroups));
		m_tEffectDesc.iRenderID = iRenderGroup;

		ImGui::NewLine();

		/*ImGui::Checkbox(u8"��ü������", &m_tEffectDesc.bBillBoardAll);
		if (m_tEffectDesc.bBillBoardAll)
			m_tEffectDesc.bBillBoardExceptY = false;
		ImGui::SameLine();
		ImGui::Checkbox(u8"Y�� ����������", &m_tEffectDesc.bBillBoardExceptY);
		if (m_tEffectDesc.bBillBoardExceptY)
			m_tEffectDesc.bBillBoardAll = false;
		ImGui::SameLine();*/
		ImGui::Checkbox("CutBlack", &m_tEffectDesc.bCutBlack);
		ImGui::SameLine();
		ImGui::Checkbox("Glow", &m_tEffectDesc.bGlow);
		if (m_tEffectDesc.bGlow)
			ImGui::InputFloat("GlowValue", &m_tEffectDesc.fGlowValue);

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::InputFloat("EnableTime", &m_tEffectDesc.fEnableTime);
		ImGui::SameLine();
		ImGui::InputFloat("LifeTime", &m_tEffectDesc.fLifeTime);
		ImGui::InputFloat("AlphaTime", &m_tEffectDesc.fAlphaTime);
		ImGui::SameLine();
		ImGui::InputFloat("AlphaSpeed", &m_tEffectDesc.fAlphaSpeed);
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(100.f);

		ImGui::NewLine();

		// UV Flow
		if (ImGui::TreeNode((_string("UV##_") + to_string(iCount)).c_str()))
		{
			ImGui::Checkbox("UVFlow", &m_tEffectDesc.bUVFlow);
			ImGui::PushItemWidth(250.f);
			ImGui::DragFloat2("UVSpeed", m_fEffectUVSpeed, 0.01f);
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode((_string(u8"��������Ʈ##_") + to_string(iCount)).c_str()))
		{
			ImGui::Checkbox("SliceLoop", &m_tEffectDesc.bSliceLoop);
			ImGui::InputInt("SliceX", &m_tEffectDesc.iSliceX, 0, 0);
			ImGui::InputInt("SliceY", &m_tEffectDesc.iSliceY, 0, 0);
			ImGui::TreePop();
		}

		ImGui::NewLine();

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("MaxFrame", &m_tEffectDesc.fMaxFrame); ImGui::NewLine();
		ImGui::PopItemWidth();

		ImGui::Text(u8"New ũ�⺯ȯ�� ���ϴ� �࿡ ���� �Է��ϸ� ���� �ٸ� ũ�⺯ȯ�� ����");
		ImGui::PushItemWidth(200.f);
		ImGui::InputFloat3(u8"New ũ�⺯ȯ", &m_tEffectDesc.vScaleDelta.x); ImGui::NewLine();
		ImGui::PopItemWidth();

		static _int iInstanceRotAxis = m_tEffectDesc.eRotAxisType;
		ImGui::RadioButton(u8"X�� ȸ��", &iInstanceRotAxis, 0); ImGui::SameLine();
		ImGui::RadioButton(u8"Y�� ȸ��", &iInstanceRotAxis, 1); ImGui::SameLine();
		ImGui::RadioButton(u8"Z�� ȸ��", &iInstanceRotAxis, 2);
		if (0 == iInstanceRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_X;
		else if (1 == iInstanceRotAxis)
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Y;
		else
			m_tEffectDesc.eRotAxisType = EFFECTDESC::Axis_Z;
		ImGui::InputFloat(u8"��ǥ ����", &m_tEffectDesc.fDestAngle);	ImGui::NewLine();


		ImGui::InputInt(u8"�ν��Ͻ� ����", &m_tEffectDesc.iInstanceCount, 0, 0);
		if (0 > m_tEffectDesc.iInstanceCount)
			m_tEffectDesc.iInstanceCount = 0;
		ImGui::SameLine();
		if (ImGui::Button(u8"�ν��Ͻ� �ʱ�ȭ"))
		{
			if (FAILED(static_cast<CInstanceEffect*>(pEffect)->ChangeInstanceCount(m_tEffectDesc.iInstanceCount)))
				MSG_BOX("�ν��Ͻ� ���Ҵ� ����");
		}
		ImGui::PopItemWidth();

		// �ν��Ͻ� ���� ���� ����
		if (ImGui::TreeNode((_string(u8"�ν��Ͻ� �ɼ�##_") + to_string(iCount)).c_str()))
		{
			m_tInstanceEffectDesc = static_cast<CInstanceEffect*>(pEffect)->Get_InstanceEffectDesc();

			m_tInstanceEffectDesc.fMaxTime = m_tEffectDesc.fLifeTime;
			m_tInstanceEffectDesc.fPassTime = m_tEffectDesc.fEnableTime;

			ImGui::PushItemWidth(300.f);
			//ImGui::InputFloat("MaxTime", &m_tEffectDesc.fLifeTime);
			//ImGui::InputFloat("EnableTime", &m_tEffectDesc.fEnableTime);   

			ImGui::Checkbox("Billboard", &m_tInstanceEffectDesc.bBillboard);
			ImGui::DragFloat("Loop", &m_tInstanceEffectDesc.fLoop, 0.001f, 0.f, 10.f);
			ImGui::DragFloat("LastTime", &m_tInstanceEffectDesc.fLastTime, 0.001f, -10.f, 10.f);

			ImGui::Text("Scale Control");
			ImGui::DragFloat("Scale", &m_tInstanceEffectDesc.vScale.x, 0.001f, 0.f, 2.f);
			ImGui::DragFloat("Scale_Speed", &m_tInstanceEffectDesc.vScale_Speed.x, 0.01f, 0.f, 30.f);

			ImGui::Text("Speed Control");
			ImGui::DragFloat("Speed", &m_tInstanceEffectDesc.fSpeed, 0.001f, -10.f, 10.f);
			ImGui::DragFloat("SpeedRand", &m_tInstanceEffectDesc.fSpeedRand, 0.001f, -10.f, 10.f);

			ImGui::Text("Movement Control");
			ImGui::DragFloat3("Movement", &m_tInstanceEffectDesc.vMovement.x, 0.001f, -1.f, 1.f);
			ImGui::DragFloat3("MovementRand", &m_tInstanceEffectDesc.vMovementRand.x, 0.001f, -1.f, 1.f);

			ImGui::Text("Position Control");
			ImGui::DragFloat3("Position", &m_tInstanceEffectDesc.vPosition.x);
			ImGui::DragFloat3("PostionRand", &m_tInstanceEffectDesc.vPositionRand.x);

			ImGui::Text("Color");
			ImGui::ColorEdit4("Color1", &m_tInstanceEffectDesc.vColor1.x, ImGuiColorEditFlags_AlphaBar);
			ImGui::ColorEdit4("Color2", &m_tInstanceEffectDesc.vColor2.x, ImGuiColorEditFlags_AlphaBar);

			_float4 RelaxSpeed = _float4(m_tInstanceEffectDesc.KeyFrame_1_RelaxSpeed, m_tInstanceEffectDesc.KeyFrame_2_RelaxSpeed
				, m_tInstanceEffectDesc.KeyFrame_3_RelaxSpeed, m_tInstanceEffectDesc.KeyFrame_4_RelaxSpeed);
			if (ImGui::DragFloat4("KeyFrame RelaxSpeed", &RelaxSpeed.x, 0.001f, 0.8f, 1.1f))
			{
				m_tInstanceEffectDesc.KeyFrame_1_RelaxSpeed = RelaxSpeed.x;
				m_tInstanceEffectDesc.KeyFrame_2_RelaxSpeed = RelaxSpeed.y;
				m_tInstanceEffectDesc.KeyFrame_3_RelaxSpeed = RelaxSpeed.z;
				m_tInstanceEffectDesc.KeyFrame_4_RelaxSpeed = RelaxSpeed.w;
			}
			_float4 Speed = _float4(m_tInstanceEffectDesc.KeyFrame_1_Speed, m_tInstanceEffectDesc.KeyFrame_2_Speed
				, m_tInstanceEffectDesc.KeyFrame_3_Speed, m_tInstanceEffectDesc.KeyFrame_4_Speed);
			if (ImGui::DragFloat4("KeyFrame Speed", &Speed.x, 0.01f, -10.f, 13.f))
			{
				m_tInstanceEffectDesc.KeyFrame_1_Speed = Speed.x;
				m_tInstanceEffectDesc.KeyFrame_2_Speed = Speed.y;
				m_tInstanceEffectDesc.KeyFrame_3_Speed = Speed.z;
				m_tInstanceEffectDesc.KeyFrame_4_Speed = Speed.w;
			}
			ImGui::DragFloat("KeyFram_1_Time", &m_tInstanceEffectDesc.KeyFrame_1_TimeEnd, 0.1f, 0.f, 10000.f);
			ImGui::DragFloat4("KeyFram_1_Movement", &m_tInstanceEffectDesc.KeyFrame_1_Movement.x, 0.001f, -1.f, 1.f);

			ImGui::DragFloat("KeyFram_2_Time", &m_tInstanceEffectDesc.KeyFrame_2_TimeEnd, 0.1f, 0.f, 10000.f);
			ImGui::DragFloat4("KeyFram_2_Movement", &m_tInstanceEffectDesc.KeyFrame_2_Movement.x, 0.001f, -1.f, 1.f);

			ImGui::DragFloat("KeyFram_3_Time", &m_tInstanceEffectDesc.KeyFrame_3_TimeEnd, 0.1f, 0.f, 10000.f);
			ImGui::DragFloat4("KeyFram_3_Movement", &m_tInstanceEffectDesc.KeyFrame_3_Movement.x, 0.001f, -1.f, 1.f);

			ImGui::DragFloat("KeyFram_4_Time", &m_tInstanceEffectDesc.KeyFrame_4_TimeEnd, 0.1f, 0.f, 10000.f);
			ImGui::DragFloat4("KeyFram_4_Movement", &m_tInstanceEffectDesc.KeyFrame_4_Movement.x, 0.001f, -1.f, 1.f);

			ImGui::PopItemWidth();

			static_cast<CInstanceEffect*>(pEffect)->Set_InstanceEffectDesc(m_tInstanceEffectDesc);

			ImGui::TreePop();
		}

		ImGui::PopItemWidth();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Tool::Get_EffectObjectFrame(_uint iCount)
{
	m_fMaxFrame = static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectObjectDesc().fMaxFrame;
	m_fCurFrame = static_cast<CEffectObject*>(m_vecEffect[iCount])->Get_EffectObjectDesc().fCurFrame;
}

HRESULT CEffect_Tool::LoadTexture(vector<FILEDATA>& vecTextureData)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 > vecTextureData.size())
		return E_FAIL;

	for (auto& iter : vecTextureData)
	{
		_wstring strProtoTag = TEXT("Prototype_Component_Texture_");
		strProtoTag += iter.strFileName;
		_tchar* szProtoTag = new _tchar[MAX_PATH];
		lstrcpy(szProtoTag, strProtoTag.c_str());

		_tchar* szPullPath = new _tchar[MAX_PATH];
		lstrcpy(szPullPath, iter.strFilePath.c_str());

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STATIC, szProtoTag, CTexture::Create(m_pDevice, m_pContext, szPullPath))))
			return E_FAIL;

		m_vecTextureTag.push_back(strProtoTag);

		m_StrList.push_back(szProtoTag);
		m_StrList.push_back(szPullPath);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect_Tool::InitEffectDesc(EFFECTDESC* tEffectDesc)
{
	tEffectDesc->bGravity = false;
	tEffectDesc->bLoop = false;
	tEffectDesc->vPos = _float3(0.f, 0.f, 0.f);
	tEffectDesc->vRot = _float3(0.f, 0.f, 0.f);
	tEffectDesc->vScl = _float3(1.f, 1.f, 1.f);
	tEffectDesc->vScaleDelta = _float3(0.f, 0.f, 0.f);
	tEffectDesc->vDir = _float3(0.f, 0.f, 0.f);
	tEffectDesc->vColor = _float4(1.f, 1.f, 1.f, 1.f);
	tEffectDesc->vSliceUV = _float2(0.f, 0.f);
	tEffectDesc->vUV_Mul = _float2(1.f, 1.f);
	tEffectDesc->fForce = 0.f;
	tEffectDesc->fMaxFrame = 0.f;
	tEffectDesc->fCurFrame = 0.f;
	tEffectDesc->fDelayTime = 0.f;
	tEffectDesc->fGravity = 0.f;
	tEffectDesc->fAcceleration = 0.f;
	tEffectDesc->fAccMaxTime = 0.f;
	tEffectDesc->fAccSpeed = 0.f;
	tEffectDesc->fRotSpeed = 0.f;
	tEffectDesc->fJumpPower = 0.f;
	tEffectDesc->fAngle = 0.f;
	tEffectDesc->iInstanceCount = 0;

	tEffectDesc->vScrollSpeeds = _float3(1.3f, 10.f, 2.3f);
	tEffectDesc->vNoiseScale = _float3(1.f, 2.f, 3.f);
	tEffectDesc->vDistortion1 = _float2(0.1f, 0.2f);
	tEffectDesc->vDistortion2 = _float2(0.1f, 0.3f);
	tEffectDesc->vDistortion3 = _float2(0.1f, 0.1f);
	tEffectDesc->fDistortionScale = 0.8f;
	tEffectDesc->fDistortionBias = 0.5f;
}

void CEffect_Tool::DeleteEffect(wstring _strName, CEffect* pEffect, _uint iEffectObjectCount, _uint iEffectCount)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ����Ʈ ����
	wstring strDeleteW = L"Delete" + _strName;
	wstring strNameW = L"PopUp" + _strName;
	//strDeleteW += to_wstring(iEffectIndex);
	//strNameW += to_wstring(iEffectIndex);

	string strDeleteName = CMethod::ConvertWToS(strDeleteW);
	string strNameS = CMethod::ConvertWToS(strNameW);

	if (ImGui::Button(strDeleteName.c_str()))
		ImGui::OpenPopup(strNameS.c_str());

	static int iIndex = -1;
	const char* ChoiseEffect[] = { "YESSSS", "NOOOO" };

	if (ImGui::BeginPopup(strNameS.c_str()))
	{
		for (int iSelect = 0; iSelect < IM_ARRAYSIZE(ChoiseEffect); iSelect++)
			if (ImGui::Selectable(ChoiseEffect[iSelect]))
				iIndex = iSelect;

		// YES �� �����ٸ� ����
		if (0 == iIndex)
		{
			if (FAILED(pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_EFFECT, pEffect)))
				return;

			vector<CEffect*>& vecEffect = static_cast<CEffectObject*>(m_vecEffect[iEffectObjectCount])->Get_vecEffect();

			auto iter = vecEffect.begin();
			_uint iCount = 0;
			for (; iter != vecEffect.end();)
			{
				if (iEffectCount == iCount)
				{
					Safe_Release(*iter);
					vecEffect.erase(iter);
					m_iCurEffectObjectIndex--;
					break;
				}
				else
				{
					iter++;
					iCount++;
				}
			}
		}

		iIndex = -1;
		ImGui::EndPopup();
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CEffect_Tool::SaveEffect(CGameObject* pEffectObject, _wstring _strFileName)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	EFFECTDESC	tEffectObjectDesc;
	EFFECTDESC	tEffectDesc;
	INSTANCEDESC tInstanceEffectDesc;
	ZeroMemory(&tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tInstanceEffectDesc, sizeof(INSTANCEDESC));

	_tchar szFileName[MAX_PATH] = L"";
	_int iFileNameSize = 0;

	_tchar szFullPath[MAX_PATH] = L"";
	_int iPathSize = 0;
	_int iTextureIndex = 0;

	_tchar szNoiseFullPath[MAX_PATH] = L"";
	_int iNoisePathSize = 0;
	_int iNoiseTextureIndex = 0;

	_tchar szMeshFullPath[MAX_PATH] = L"";
	_int iMeshPathSize = 0;
	_int iMeshIndex = 0;

	_bool bNoise = false;

	_wstring strFileName = _strFileName;

	_wstring strFilePath = L"../../Resources/Data/Effect/";
	strFilePath += strFileName;
	strFilePath += L".bin";

	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte = 0;

	// �����̸�, �����̸� ������ ����
	lstrcpy(szFileName, strFileName.c_str());
	iFileNameSize = lstrlen(szFileName) + 1;
	WriteFile(hFile, &iFileNameSize, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &szFileName, iFileNameSize * 2, &dwByte, nullptr);

	// ����Ʈ ���� ����
	_int iEffectCount = static_cast<CEffectObject*>(pEffectObject)->Get_EffectCount();
	WriteFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr);

	// ����Ʈ ������Ʈ ������ ����
	tEffectObjectDesc = static_cast<CEffectObject*>(pEffectObject)->Get_EffectObjectDesc();
	WriteFile(hFile, &tEffectObjectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

	for (auto& iter : static_cast<CEffectObject*>(pEffectObject)->Get_vecEffect())
	{
		// ����Ʈ �ؽ��� ������Ÿ�� �±� ������ �ʱ�ȭ
		iPathSize = 0;
		// ����Ʈ �޽� ������Ÿ�� �±� ������ �ʱ�ȭ
		iMeshIndex = 0;
		// ����Ʈ ������ ���� �ʱ�ȭ
		ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));

		// ����Ʈ ������ ����
		EFFECTDESC tEffectDesc = iter->Get_EffectDesc();
		WriteFile(hFile, &tEffectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

		// �켱������ ����Ʈ Ÿ�� ����
		_int iEffectType = iter->Get_EffectType();
		WriteFile(hFile, &iEffectType, sizeof(_uint), &dwByte, nullptr);

		// �� Ÿ�Կ� ���缭 ����Ʈ �����͸� �����Ѵ�.
		if (CEffect::EFFECT_IMAGE == iEffectType)
		{
			// ����Ʈ �ؽ����� ������Ÿ�� �±׸� �޾ƿͼ� _tchar �迭�� ����
			//lstrcpy(szFullPath, static_cast<CImageEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());
			// ������Ÿ�� �±� ������ ���
			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			// ����Ʈ �ؽ����� ������Ÿ�� �±� ����
			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			// ����Ʈ �ؽ��� �ε����� �޾ƿ�
			//iTextureIndex = static_cast<CImageEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			// �޾ƿ� ����Ʈ �ؽ��� �ε����� ����
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}
		}
		else if (CEffect::EFFECT_STATICMESH == static_cast<CEffect*>(iter)->Get_EffectType())
		{
			// �ؽ��� ������ ����
			//lstrcpy(szFullPath, static_cast<CStaticMeshEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());

			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);

			//iTextureIndex = static_cast<CStaticMeshEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}

			// �޽� ������ ����
			lstrcpy(szMeshFullPath, static_cast<CStaticMeshEffect*>(iter)->Get_MeshPrototypePath().c_str());

			iMeshPathSize = lstrlen(szMeshFullPath) + 1;
			WriteFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);

			iMeshIndex = static_cast<CStaticMeshEffect*>(iter)->Get_MeshIndex();
			WriteFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
		}
		else if (CEffect::EFFECT_ANIMMESH == static_cast<CEffect*>(iter)->Get_EffectType())
		{
			// �ִ� �޽� ������ ����
			lstrcpy(szMeshFullPath, static_cast<CAnimMeshEffect*>(iter)->Get_MeshPrototypePath().c_str());

			iMeshPathSize = lstrlen(szMeshFullPath) + 1;
			WriteFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);

			iMeshIndex = static_cast<CAnimMeshEffect*>(iter)->Get_MeshIndex();
			WriteFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
		}
		else
		{
			INSTANCEDESC tInstanceEffectDesc = static_cast<CInstanceEffect*>(iter)->Get_InstanceEffectDesc();
			WriteFile(hFile, &tInstanceEffectDesc, sizeof(INSTANCEDESC), &dwByte, nullptr);

			//lstrcpy(szFullPath, static_cast<CInstanceEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());

			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);

			//iTextureIndex = static_cast<CInstanceEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


HRESULT CEffect_Tool::LoadEffect(wstring _strFileName)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	EFFECTDESC	tEffectObjectDesc;
	EFFECTDESC	tEffectDesc;
	INSTANCEDESC tInstanceEffectDesc;
	ZeroMemory(&tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tInstanceEffectDesc, sizeof(INSTANCEDESC));

	PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };
	FILEGROUP* pTextureFolder = pGameInstance->Find_Folder(&vecTexturePath);

	PATHS vecNoisePath = { L"NoiseTexture" };
	FILEGROUP* pNoiseFolder = pGameInstance->Find_Folder(&vecNoisePath);

	PATHS vecStaticMeshEffectPath = { L"Model",L"EffectModel",L"FBX_Converter" };
	FILEGROUP* pStaticMeshFolder = pGameInstance->Find_Folder(&vecStaticMeshEffectPath);

	PATHS vecAnimMeshEffectPath = { L"AnimationModel",L"EffectModel",L"FBX_Converter" };
	FILEGROUP* pAnimMeshFolder = pGameInstance->Find_Folder(&vecAnimMeshEffectPath);

	_tchar szFileName[MAX_PATH] = L"";
	_int iFileNameSize = 0;

	_uint iEffectObjectCount = 0;
	_int iEffectCount = 0;
	_int iEffectType = CEffect::EFFECT_END;

	_tchar* pFullPath = nullptr;
	_tchar szFullPath[MAX_PATH] = L"";
	_int iPathSize = 0;
	_int iTextureIndex = 0;

	_tchar* pNoiseFullPath = nullptr;
	_tchar szNoiseFullPath[MAX_PATH] = L"";
	_int iNoisePathSize = 0;
	_int iNoiseIndex = 0;

	_tchar* pMeshFullPath = nullptr;
	_tchar szMeshFullPath[MAX_PATH] = L"";
	_int iMeshPathSize = 0;
	_int iMeshIndex = 0;


	_wstring strFileName = _strFileName;

	_wstring strFilePath = L"../../Resources/Data/Effect/";
	strFilePath += strFileName;
	strFilePath += L".bin";

	//Release();		// ������ �ִ� Ÿ�� ������ �Ҹ��ϰ� �ҷ����̴� �����ͷ� ������ϱ� ����

	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD		dwByte = 0;

	// ���� �̸� ������, ���� �̸� �ҷ�����
	ReadFile(hFile, &iFileNameSize, sizeof(_int), &dwByte, nullptr);
	ReadFile(hFile, &szFileName, iFileNameSize * 2, &dwByte, nullptr);

	// ����Ʈ ������Ʈ ���� ����Ʈ�� ������ �ε�
	ReadFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr);

	// ����Ʈ ������Ʈ�� ������ �ε�
	ReadFile(hFile, &tEffectObjectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

	// ����Ʈ ������Ʈ ��ü ����
	CGameObject* pEffectObject = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, TEXT("Layer_Effect"), PROTO_GOB_EFFECT_TOOL, &tEffectObjectDesc);
	if (nullptr == pEffectObject)
	{
		MSG_BOX("����Ʈ ������Ʈ ���� ����");
		return E_FAIL;
	}
	else
	{
		static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_TOOL);
		static_cast<CEffectObject*>(pEffectObject)->Set_Name(szFileName);
		pEffectObject->Set_Active(true);
		m_vecEffect.push_back(pEffectObject);
		Safe_AddRef(pEffectObject);
	}


	for (_int i = 0; i < iEffectCount; ++i)
	{
		// ����Ʈ ������ �ε�
		ReadFile(hFile, &tEffectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

		// ����Ʈ Ÿ�� �ε�
		ReadFile(hFile, &iEffectType, sizeof(_int), &dwByte, nullptr);

		_wstring strEffectName = L"";


		if (CEffect::EFFECT_IMAGE == iEffectType)
		{
			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_IMAGE_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				strEffectName = L"_ImageEffect";
				//strEffectName += to_wstring(static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount() + 1);
				pEffect->Set_EffectName(strEffectName);
				pEffect->Init_Texture();
				pEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, szFullPath);

				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				// ���� ����� ����Ǿ��� ����Ʈ��� ����
				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					pEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, szNoiseFullPath);

					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
			}
		}
		else if (CEffect::EFFECT_STATICMESH == iEffectType)
		{
			// �ؽ��� ������
			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_STATICMESH_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				strEffectName = L"_MeshEffect";
				//strEffectName += to_wstring(static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount() + 1);
				pEffect->Set_EffectName(strEffectName);
				pEffect->Init_Texture();
				pEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, szFullPath);
				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				// ���� ����� ����Ǿ��� ����Ʈ��� ����
				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					pEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, szNoiseFullPath);

					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				// �޽� ������
				ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
				ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
				ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);

				for (int i = 0; i < pStaticMeshFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pStaticMeshFolder->vecFileDataList[i].strFilePath.c_str(), szMeshFullPath))
					{
						_matrix PivotMatrix = XMMatrixIdentity();
						if (!lstrcmp(L"PlaneCustomX", pStaticMeshFolder->vecFileDataList[i].strFileName.c_str()))
							PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.27f, 0.f, 0.f);
						else if (!lstrcmp(L"PlaneCustomY", pStaticMeshFolder->vecFileDataList[i].strFileName.c_str()))
							PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, 1.27f, 0.f);
						else
							PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

						_wstring strProtoTag = L"Prototype_Component_Model_";
						strProtoTag += pStaticMeshFolder->vecFileDataList[i].strFileName;
						pEffect->Set_MeshProtoTag(strProtoTag);
						CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
						//Safe_AddRef(pModel);
						//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pStaticMeshFolder, &pStaticMeshFolder->vecFileDataList[i], PivotMatrix);
						static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, i);
						//Safe_Release(pModel);
						break;
					}
				}

				static_cast<CStaticMeshEffect*>(pEffect)->Set_MeshPrototypePath(szMeshFullPath);
				static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
			}
		}
		else if (CEffect::EFFECT_ANIMMESH == iEffectType)
		{
			ReadFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_ANIMMESH_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				strEffectName = L"_AnimEffect";
				//strEffectName += to_wstring(static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount() + 1);
				pEffect->Set_EffectName(strEffectName);
				_matrix PivotMatrix = XMMatrixIdentity();
				PivotMatrix *= XMMatrixScaling(0.001f, 0.001f, 0.001f);
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pAnimMeshFolder, &pAnimMeshFolder->vecFileDataList[iMeshIndex], PivotMatrix);
				if (nullptr != pModel)
				{
					static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel, iMeshIndex);
					static_cast<CAnimMeshEffect*>(pEffect)->Set_MeshPrototypePath(szMeshFullPath);
					static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
					//Safe_Release(pModel);
				}
			}
		}
		else
		{
			// ����Ʈ ������ �ε�
			ReadFile(hFile, &tInstanceEffectDesc, sizeof(INSTANCEDESC), &dwByte, nullptr);

			ReadFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			ReadFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			ReadFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			CEffect* pEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_INSTANCE_TOOL, &tEffectDesc);
			if (nullptr != pEffect)
			{
				strEffectName = L"_InstanceEffect";
				//strEffectName += to_wstring(static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount() + 1);
				pEffect->Set_EffectName(strEffectName);
				pEffect->Init_Texture();
				pEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, szFullPath);
				for (int i = 0; i < pTextureFolder->vecFileDataList.size(); ++i)
				{
					if (!lstrcmp(pTextureFolder->vecFileDataList[i].strFilePath.c_str(), szFullPath))
					{
						pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, i);
						break;
					}
				}

				// ���� ����� ����Ǿ��� ����Ʈ��� ����
				if (tEffectDesc.bNoise)
				{
					ReadFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
					ReadFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
					ReadFile(hFile, &iNoiseIndex, sizeof(_int), &dwByte, nullptr);
					pEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, szNoiseFullPath);

					for (int i = 0; i < pNoiseFolder->vecFileDataList.size(); ++i)
					{
						if (!lstrcmp(pNoiseFolder->vecFileDataList[i].strFilePath.c_str(), szNoiseFullPath))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, i);
							break;
						}
					}
				}

				static_cast<CInstanceEffect*>(pEffect)->ChangeInstanceCount(tEffectDesc.iInstanceCount);
				static_cast<CInstanceEffect*>(pEffect)->Set_InstanceEffectDesc(tInstanceEffectDesc);
				static_cast<CEffectObject*>(pEffectObject)->Add_Effect(pEffect);
			}
		}
	}

	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEffect_Tool::ShowEffectObject()
{
	// ���� ó��
	if (0 > m_iCurEffectObjectIndex || 0 >= m_vecEffect.size() || m_iCurEffectObjectIndex == m_vecEffect.size())
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// EffectObject �� ������ �����ͼ� Tool �� EffectObjectDesc �� �Ҵ�����
	Get_EffectObjectDesc(m_iCurEffectObjectIndex);

	ImGui::PushItemWidth(150.f);
	ImGui::InputText("SaveFileName", m_szSaveName, sizeof(char) * MAX_PATH);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	string strSaveFileName = m_szSaveName;
	_wstring strSaveFileNameW = CMethod::ConvertSToW(strSaveFileName);

	// ���� ��ư
	if (ImGui::Button("Save"))
		ImGui::OpenPopup("SavePopUp");

	if (ImGui::BeginPopup("SavePopUp"))
	{
		int iSelectIndex = -1;
		const char* Choise[] = { "Real???", "Nob!" };

		for (int iSelect = 0; iSelect < IM_ARRAYSIZE(Choise); iSelect++)
			if (ImGui::Selectable(Choise[iSelect]))
				iSelectIndex = iSelect;

		if (0 == iSelectIndex)
		{
			SaveEffect(m_vecEffect[m_iCurEffectObjectIndex], strSaveFileNameW);
			ZeroMemory(m_szSaveName, MAX_PATH);
		}

		ImGui::EndPopup();
	}

	// ����Ʈ �����
	RatioBar();

	// �÷��� ��ư
	if (ImGui::Button("Play"))
	{
		m_tEffectObjectDesc.bPlay = true;
	}
	else
	{
		m_bPause = false;
		m_tEffectObjectDesc.bPause = m_bPause;
	}


	ImGui::SameLine();
	ImGui::Checkbox("Loop", &m_tEffectObjectDesc.bLoop);
	if (m_tEffectObjectDesc.bLoop)
		m_tEffectObjectDesc.bPlay = true;
	ImGui::SameLine();
	if (ImGui::Button("Pause"))
	{
		if (m_tEffectObjectDesc.bPlay)
		{
			m_bPause = !m_bPause;
			m_tEffectObjectDesc.bPause = m_bPause;
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox(u8"��ü������", &m_tEffectObjectDesc.bBillBoardAll);
	if (m_tEffectObjectDesc.bBillBoardAll)
		m_tEffectObjectDesc.bBillBoardExceptY = false;
	ImGui::SameLine();
	ImGui::Checkbox(u8"Y�� ����������", &m_tEffectObjectDesc.bBillBoardExceptY);
	if (m_tEffectObjectDesc.bBillBoardExceptY)
		m_tEffectObjectDesc.bBillBoardAll = false;

	ImGui::NewLine();

	if (ImGui::TreeNode("Transform"))
	{
		ImGui::PushItemWidth(200.f);
		ImGui::DragFloat3("Position", m_fEffectObjectPosArr, 0.01f);
		ImGui::DragFloat3("Rotation", m_fEffectObjectRotArr, 0.1f);
		ImGui::DragFloat3("Scale", m_fEffectObjectSclArr, 0.01f, 0.1f, 100.f);
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	ImGui::NewLine();

	ImGui::PushItemWidth(100.f);
	ImGui::Text("CurFrame : %.3f", m_tEffectObjectDesc.fCurFrame);
	ImGui::DragFloat("MaxFrame", &m_tEffectObjectDesc.fMaxFrame);
	ImGui::SameLine();
	ImGui::DragFloat("DelayTime", &m_tEffectObjectDesc.fDelayTime);
	ImGui::PopItemWidth();

	_int iEffectObjectRotAxis = m_tEffectObjectDesc.eRotAxisType;
	ImGui::RadioButton(u8"X�� ȸ��", &iEffectObjectRotAxis, 0);
	ImGui::RadioButton(u8"Y�� ȸ��", &iEffectObjectRotAxis, 1);
	ImGui::RadioButton(u8"Z�� ȸ��", &iEffectObjectRotAxis, 2);
	ImGui::RadioButton(u8"��ü ȸ��", &iEffectObjectRotAxis, 3);
	if (0 == iEffectObjectRotAxis)
		m_tEffectObjectDesc.eRotAxisType = EFFECTDESC::Axis_X;
	else if (1 == iEffectObjectRotAxis)
		m_tEffectObjectDesc.eRotAxisType = EFFECTDESC::Axis_Y;
	else if (2 == iEffectObjectRotAxis)
		m_tEffectObjectDesc.eRotAxisType = EFFECTDESC::Axis_Z;
	else
		m_tEffectObjectDesc.eRotAxisType = EFFECTDESC::Axis_ALL;

	ImGui::PushItemWidth(100.f);
	ImGui::InputFloat("EffectObjectDestAngle", &m_tEffectObjectDesc.fDestAngle);
	ImGui::PopItemWidth();

	// ����Ʈ ���� ��ư
	EffectButtonDraw(m_iCurEffectObjectIndex);

	// EffectObject Desc �� ������ ������ ������ ����
	Set_EffectObjectDesc(m_iCurEffectObjectIndex);

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Tool::CreateEffect()
{
	if (ImGui::Button("Create"))
	{
		EFFECTDESC tEffectDesc;
		ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
		InitEffectDesc(&tEffectDesc);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject* pEffectObject = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_TOOL, &tEffectDesc);
		RELEASE_INSTANCE(CGameInstance);
		if (nullptr == pEffectObject)
		{
			MSG_BOX("����Ʈ ������Ʈ ���� ����");
		}
		else
		{
			static_cast<CEffectObject*>(pEffectObject)->Set_ProjectType(CEffectObject::PROJECT_TOOL);
			//_wstring strEffectObjectName = L"EffectObject_";
			//strEffectObjectName += to_wstring(m_vecEffect.size());
			//static_cast<CEffectObject*>(pEffectObject)->Set_Name(strEffectObjectName);
			m_vecEffect.push_back(pEffectObject);
			Safe_AddRef(pEffectObject);
		}
	}
}

void CEffect_Tool::Load()
{
	ImGui::SameLine();
	string strLoadFileName = m_szLoadName;
	_wstring strLoadFileNameW = CMethod::ConvertSToW(strLoadFileName);
	ImGui::PushItemWidth(150.f);
	ImGui::InputText("LoadFileName", m_szLoadName, sizeof(char) * MAX_PATH);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		LoadEffect(strLoadFileNameW);
		ZeroMemory(m_szLoadName, MAX_PATH);
	}
}

void CEffect_Tool::DeleteEffect(_int iType)
{
	if (0 > iType || 1 < iType)
		return;

	ImGui::SameLine();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ���� ���
	if (ImGui::Button("Delete"))
		ImGui::OpenPopup("DeletePopUp");

	if (ImGui::BeginPopup("DeletePopUp"))
	{
		int iSelectIndex = -1;
		const char* Choise[] = { "YESSSS", "NOOOO" };

		for (int iSelect = 0; iSelect < IM_ARRAYSIZE(Choise); iSelect++)
			if (ImGui::Selectable(Choise[iSelect]))
				iSelectIndex = iSelect;

		if (0 == iSelectIndex)
		{
			if (0 == iType)
			{
				static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_Play(false);
				if (FAILED(pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_EFFECT, m_vecEffect[m_iCurEffectObjectIndex])))
				{
					MSG_BOX("���� ����");
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				auto iter = m_vecEffect.begin();
				_uint iCount = 0;
				for (; iter != m_vecEffect.end();)
				{
					if (m_iCurEffectObjectIndex == iCount)
					{
						for (auto& Effect : static_cast<CEffectObject*>(*iter)->Get_vecEffect())
						{
							if (FAILED(pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_EFFECT, Effect)))
							{
								MSG_BOX("���� ����");
								RELEASE_INSTANCE(CGameInstance);
								return;
							}
						}

						Safe_Release(*iter);
						m_vecEffect.erase(iter);
						m_iCurEffectObjectIndex--;
						if (0 >= m_iCurEffectObjectIndex)
							m_iCurEffectObjectIndex = 0;
						m_iCurEffectIndex = 0;
						break;
					}
					else
					{
						iter++;
						iCount++;
					}
				}
			}
			else if (1 == iType)
			{
				vector<CEffect*>& vecEffect = static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_vecEffect();

				if (FAILED(pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_EFFECT, vecEffect[m_iCurEffectIndex])))
				{
					MSG_BOX("���� ����");
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				auto iter = vecEffect.begin();
				_uint iCount = 0;
				for (; iter != vecEffect.end();)
				{
					if (m_iCurEffectIndex == iCount)
					{
						Safe_Release(*iter);
						vecEffect.erase(iter);
						m_iCurEffectIndex--;
						if (0 >= m_iCurEffectIndex)
							m_iCurEffectIndex = 0;
						static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Set_ToolCurEffect(m_iCurEffectIndex);
						break;
					}
					else
					{
						iter++;
						iCount++;
					}
				}
			}
		}

		ImGui::EndPopup();
	}

	RELEASE_INSTANCE(CGameInstance);
}


void CEffect_Tool::Find_Texture(PATHS _Paths, CEffect* pEffect, _uint iType)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	FILEGROUP* pTextureFolder = pGameInstance->Find_Folder(&_Paths);

	_bool bInput = false;

	if (CEffect::TEXTURE_DIFFUSE == iType)
	{
		_char INPUT[MAX_PATH] = "";
		strcpy_s(INPUT, m_strDiffuseText.c_str());
		ImGui::PushItemWidth(150.f);
		bInput = ImGui::InputText("Name", INPUT, sizeof(_char) * MAX_PATH);
		ImGui::PopItemWidth();
		m_strDiffuseText = INPUT;
	}
	else if (CEffect::TEXTURE_NOISE == iType)
	{
		_char INPUT[MAX_PATH] = "";
		strcpy_s(INPUT, m_strNoiseText.c_str());
		ImGui::PushItemWidth(150.f);
		bInput = ImGui::InputText("Name", INPUT, sizeof(_char) * MAX_PATH);
		ImGui::PopItemWidth();
		m_strNoiseText = INPUT;
	}

	if (CEffect::TEXTURE_DIFFUSE == iType)
	{
		if (!strcmp(m_strDiffuseText.c_str(), ""))
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectDiffuse).c_str());

			if (ImGui::BeginListBox("Diffuse", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < pTextureFolder->vecFileDataList.size(); n++)
				{
					const bool is_selected = (pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE) == pTextureFolder->vecFileDataList[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(pTextureFolder->vecFileDataList[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						// �ؽ��� �ε��� ����
						if (n != pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, n);
							pEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, pTextureFolder->vecFileDataList[n].strFilePath);
						}

						m_strSelectDiffuse = pTextureFolder->vecFileDataList[n].strFileName;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
		else if (true == bInput)
		{
			m_vecDiffuseFileData.clear();
			for (auto& iter : pTextureFolder->vecFileDataList)
			{
				if (CMethod::Is_Same_Text(CMethod::ConvertWToS(iter.strFileName), m_strDiffuseText.c_str()))
					m_vecDiffuseFileData.push_back(iter);
			}
		}
		else
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectDiffuse).c_str());
			if (ImGui::BeginListBox("Diffuse", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < m_vecDiffuseFileData.size(); n++)
				{
					const bool is_selected = (pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE) == m_vecDiffuseFileData[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(m_vecDiffuseFileData[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						// �ؽ��� �ε��� ����
						if (n != pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, m_vecDiffuseFileData[n].iIndex);
							pEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, m_vecDiffuseFileData[n].strFilePath);
						}

						m_strSelectDiffuse = m_vecDiffuseFileData[n].strFileName;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}
	else if (CEffect::TEXTURE_NOISE == iType)
	{
		if (!strcmp(m_strNoiseText.c_str(), ""))
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectNoise).c_str());
			if (ImGui::BeginListBox("Noise", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < pTextureFolder->vecFileDataList.size(); n++)
				{
					const bool is_selected = (pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE) == pTextureFolder->vecFileDataList[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(pTextureFolder->vecFileDataList[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						// �ؽ��� �ε��� ����
						if (n != pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, n);
							pEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, pTextureFolder->vecFileDataList[n].strFilePath);
						}

						m_strSelectNoise = pTextureFolder->vecFileDataList[n].strFileName;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
		else if (true == bInput)
		{
			m_vecNoiseFileData.clear();
			for (auto& iter : pTextureFolder->vecFileDataList)
			{
				if (CMethod::Is_Same_Text(CMethod::ConvertWToS(iter.strFileName), m_strNoiseText.c_str()))
					m_vecNoiseFileData.push_back(iter);
			}
		}
		else
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectNoise).c_str());
			if (ImGui::BeginListBox("Noise", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < m_vecNoiseFileData.size(); n++)
				{
					const bool is_selected = (pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE) == m_vecNoiseFileData[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(m_vecNoiseFileData[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						// �ؽ��� �ε��� ����
						if (n != pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE))
						{
							pEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, m_vecNoiseFileData[n].iIndex);
							pEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, m_vecNoiseFileData[n].strFilePath);
						}

						m_strSelectNoise = m_vecNoiseFileData[n].strFileName;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}


void CEffect_Tool::Find_Mesh(PATHS _Paths, CEffect* pEffect, _uint iType)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	FILEGROUP* pMeshFolder = pGameInstance->Find_Folder(&_Paths);

	_char INPUT[MAX_PATH] = "";
	strcpy_s(INPUT, m_strMeshText.c_str());
	_bool bInput = false;
	ImGui::PushItemWidth(150.f);
	bInput = ImGui::InputText("Name", INPUT, sizeof(_char) * MAX_PATH);
	ImGui::PopItemWidth();
	m_strMeshText = INPUT;

	if (!strcmp(m_strMeshText.c_str(), ""))
	{
		if (CEffect::EFFECT_STATICMESH == iType)
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectStaticMesh).c_str());
			if (ImGui::BeginListBox("Mesh", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < pMeshFolder->vecFileDataList.size(); n++)
				{
					const bool is_selected = (pEffect->Get_MeshIndex() == pMeshFolder->vecFileDataList[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(pMeshFolder->vecFileDataList[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						if (n != pEffect->Get_MeshIndex())
						{
							_matrix PivotMatrix = XMMatrixIdentity();
							if (!lstrcmp(L"PlaneCustomX", pMeshFolder->vecFileDataList[n].strFileName.c_str()))
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.27f, 0.f, 0.f);
							else if (!lstrcmp(L"PlaneCustomY", pMeshFolder->vecFileDataList[n].strFileName.c_str()))
							{
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, 1.27f, 0.f);
							}
							else
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

							_wstring strProtoTag = L"Prototype_Component_Model_";
							strProtoTag += pMeshFolder->vecFileDataList[n].strFileName;
							pEffect->Set_MeshProtoTag(strProtoTag);
							CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
							//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pMeshFolder, &pMeshFolder->vecFileDataList[n], PivotMatrix);
							pEffect->Set_MeshPrototypePath(pMeshFolder->vecFileDataList[n].strFilePath);
							pEffect->Set_MeshIndex(n);

							static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, n);

							m_strSelectStaticMesh = pMeshFolder->vecFileDataList[n].strFileName;
						}
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
		else if (CEffect::EFFECT_ANIMMESH == iType)
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectAnimMesh).c_str());
			if (ImGui::BeginListBox("AnimMesh", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < pMeshFolder->vecFileDataList.size(); n++)
				{
					const bool is_selected = (pEffect->Get_MeshIndex() == pMeshFolder->vecFileDataList[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(pMeshFolder->vecFileDataList[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						if (n != pEffect->Get_MeshIndex())
						{
							_matrix PivotMatrix = XMMatrixIdentity();
							PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
							_wstring strProtoTag = L"Prototype_Component_Model_";
							strProtoTag += pMeshFolder->vecFileDataList[n].strFileName;
							pEffect->Set_MeshProtoTag(strProtoTag);
							CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
							//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pMeshFolder, &pMeshFolder->vecFileDataList[n], PivotMatrix);
							pEffect->Set_MeshPrototypePath(pMeshFolder->vecFileDataList[n].strFilePath);
							pEffect->Set_MeshIndex(n);

							static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel, n);

							m_strSelectAnimMesh = pMeshFolder->vecFileDataList[n].strFileName;
						}
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}
	else if (true == bInput)
	{
		m_vecMeshFileData.clear();
		for (auto& iter : pMeshFolder->vecFileDataList)
		{
			if (CMethod::Is_Same_Text(CMethod::ConvertWToS(iter.strFileName), m_strMeshText.c_str()))
				m_vecMeshFileData.push_back(iter);
		}
	}
	else
	{
		if (CEffect::EFFECT_STATICMESH == iType)
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectStaticMesh).c_str());
			if (ImGui::BeginListBox("Mesh", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < m_vecMeshFileData.size(); n++)
				{
					const bool is_selected = (pEffect->Get_MeshIndex() == m_vecMeshFileData[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(m_vecMeshFileData[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						if (n != pEffect->Get_MeshIndex())
						{
							_matrix PivotMatrix = XMMatrixIdentity();
							if (!lstrcmp(L"PlaneCustomX", m_vecMeshFileData[n].strFileName.c_str()))
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.27f, 0.f, 0.f);
							else if (!lstrcmp(L"PlaneCustomY", m_vecMeshFileData[n].strFileName.c_str()))
							{
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, 1.27f, 0.f);
							}
							else
								PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

							_wstring strProtoTag = L"Prototype_Component_Model_";
							strProtoTag += m_vecMeshFileData[n].strFileName;
							pEffect->Set_MeshProtoTag(strProtoTag);
							CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
							//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pMeshFolder, &m_vecMeshFileData[n], PivotMatrix);
							pEffect->Set_MeshPrototypePath(m_vecMeshFileData[n].strFilePath);
							pEffect->Set_MeshIndex(m_vecMeshFileData[n].iIndex);

							static_cast<CStaticMeshEffect*>(pEffect)->Set_Model(pModel, m_vecMeshFileData[n].iIndex);

							m_strSelectStaticMesh = m_vecMeshFileData[n].strFileName;
						}
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
		else if (CEffect::EFFECT_ANIMMESH == iType)
		{
			ImGui::Text(CMethod::ConvertWToS(m_strSelectAnimMesh).c_str());
			if (ImGui::BeginListBox("AnimMesh", ImVec2(500.f, 200.f)))
			{
				for (int n = 0; n < m_vecMeshFileData.size(); n++)
				{
					const bool is_selected = (pEffect->Get_MeshIndex() == m_vecMeshFileData[n].iIndex);
					string strSelectName = CMethod::ConvertWToS(m_vecMeshFileData[n].strFileName);

					if (ImGui::Selectable(strSelectName.c_str(), is_selected))
					{
						if (n != pEffect->Get_MeshIndex())
						{
							_matrix PivotMatrix = XMMatrixIdentity();
							PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
							_wstring strProtoTag = L"Prototype_Component_Model_";
							strProtoTag += m_vecMeshFileData[n].strFileName;
							pEffect->Set_MeshProtoTag(strProtoTag);
							CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
							//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pMeshFolder, &m_vecMeshFileData[n], PivotMatrix);
							pEffect->Set_MeshPrototypePath(m_vecMeshFileData[n].strFilePath);
							pEffect->Set_MeshIndex(m_vecMeshFileData[n].iIndex);

							static_cast<CAnimMeshEffect*>(pEffect)->Set_Model(pModel, m_vecMeshFileData[n].iIndex);

							m_strSelectAnimMesh = m_vecMeshFileData[n].strFileName;
						}
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Tool::RatioBar()
{
	ImGui::NewLine();

	_float fPlayRatio = 0.f;
	ImGui::Checkbox(u8"����� ���", &m_bPercentage);
	if (m_bPercentage)
	{
		if (0.f < m_fMaxFrame)
			fPlayRatio = m_fCurFrame / m_fMaxFrame;
	}

	ImGui::Text(u8"����Ʈ �����");
	if (m_bPercentage)
	{
		ImGui::SliderFloat("##_RatioPercentage", &fPlayRatio, 0.f, 1.f);
		/*if (ImGui::SliderFloat("##_RatioPercentage", &fPlayRatio, 0.f, 1.f))
		{
			fPlayRatio = fPlayRatio;
		}*/
		ImGui::SameLine();
		ImGui::Text("%.3f", 1.f);
	}
	else
	{
		ImGui::SliderFloat("##_Ratio", &m_fCurFrame, 0.f, m_fMaxFrame);
		/*if (ImGui::SliderFloat("##_Ratio", &m_fCurFrame, 0.f, m_fMaxFrame))
		{
			m_tEffectObjectDesc.fCurFrame = m_fCurFrame;
		}*/
		ImGui::SameLine();
		ImGui::Text("%.3f", m_fMaxFrame);
	}

	ImGui::NewLine();
}

void CEffect_Tool::AllLoad()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	PATHS vecPath = { L"Data",L"Effect" };
	FILEGROUP* pEffectFolder = pGameInstance->Find_Folder(&vecPath);

	for (auto& iter : pEffectFolder->vecFileDataList)
		LoadEffect(iter.strFileName);

	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Tool::AllSave()
{
	for (auto& iter : m_vecEffect)
		NewSave(iter, static_cast<CEffectObject*>(iter)->Get_Name());
}

void CEffect_Tool::NewSave(CGameObject* pEffectObject, _wstring _strFileName)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	EFFECTDESC	tEffectObjectDesc;
	EFFECTDESC	tEffectDesc;
	INSTANCEDESC tInstanceEffectDesc;
	INSTANCEDESC2 tNewInstanceDesc;
	ZeroMemory(&tEffectObjectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));
	ZeroMemory(&tInstanceEffectDesc, sizeof(INSTANCEDESC));
	ZeroMemory(&tNewInstanceDesc, sizeof(INSTANCEDESC2));

	_tchar szFileName[MAX_PATH] = L"";
	_int iFileNameSize = 0;

	_tchar szFullPath[MAX_PATH] = L"";
	_int iPathSize = 0;
	_int iTextureIndex = 0;

	_tchar szNoiseFullPath[MAX_PATH] = L"";
	_int iNoisePathSize = 0;
	_int iNoiseTextureIndex = 0;

	_tchar szMeshFullPath[MAX_PATH] = L"";
	_int iMeshPathSize = 0;
	_int iMeshIndex = 0;

	_bool bNoise = false;

	_wstring strFileName = _strFileName;

	_wstring strFilePath = L"../../Resources/Data/Effect/";
	strFilePath += strFileName;
	strFilePath += L".bin";

	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte = 0;

	// �����̸�, �����̸� ������ ����
	lstrcpy(szFileName, strFileName.c_str());
	iFileNameSize = lstrlen(szFileName) + 1;
	WriteFile(hFile, &iFileNameSize, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &szFileName, iFileNameSize * 2, &dwByte, nullptr);

	// ����Ʈ ���� ����
	_int iEffectCount = static_cast<CEffectObject*>(pEffectObject)->Get_EffectCount();
	WriteFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr);

	// ����Ʈ ������Ʈ ������ ����
	tEffectObjectDesc = static_cast<CEffectObject*>(pEffectObject)->Get_EffectObjectDesc();
	WriteFile(hFile, &tEffectObjectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

	for (auto& iter : static_cast<CEffectObject*>(pEffectObject)->Get_vecEffect())
	{
		// ����Ʈ �ؽ��� ������Ÿ�� �±� ������ �ʱ�ȭ
		iPathSize = 0;
		// ����Ʈ �޽� ������Ÿ�� �±� ������ �ʱ�ȭ
		iMeshIndex = 0;
		// ����Ʈ ������ ���� �ʱ�ȭ
		ZeroMemory(&tEffectDesc, sizeof(EFFECTDESC));



		// ����Ʈ ������ ����
		EFFECTDESC tEffectDesc = iter->Get_EffectDesc();
		tEffectDesc.t1 = _float3(0.f, 0.f, 0.f);
		tEffectDesc.t2 = _float3(0.f, 0.f, 0.f);
		tEffectDesc.t3 = 0.f;
		tEffectDesc.iSamplerMode = 0;
		tEffectDesc.vUV_Mul = _float2(1.f, 1.f);
		WriteFile(hFile, &tEffectDesc, sizeof(EFFECTDESC), &dwByte, nullptr);

		// �켱������ ����Ʈ Ÿ�� ����
		_int iEffectType = iter->Get_EffectType();
		WriteFile(hFile, &iEffectType, sizeof(_uint), &dwByte, nullptr);

		// �� Ÿ�Կ� ���缭 ����Ʈ �����͸� �����Ѵ�.
		if (CEffect::EFFECT_IMAGE == iEffectType)
		{
			// ����Ʈ �ؽ����� ������Ÿ�� �±׸� �޾ƿͼ� _tchar �迭�� ����
			//lstrcpy(szFullPath, static_cast<CImageEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());
			// ������Ÿ�� �±� ������ ���
			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);
			// ����Ʈ �ؽ����� ������Ÿ�� �±� ����
			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);
			// ����Ʈ �ؽ��� �ε����� �޾ƿ�
			//iTextureIndex = static_cast<CImageEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			// �޾ƿ� ����Ʈ �ؽ��� �ε����� ����
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}
		}
		else if (CEffect::EFFECT_STATICMESH == static_cast<CEffect*>(iter)->Get_EffectType())
		{
			// �ؽ��� ������ ����
			//lstrcpy(szFullPath, static_cast<CStaticMeshEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());

			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);

			//iTextureIndex = static_cast<CStaticMeshEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}

			// �޽� ������ ����
			lstrcpy(szMeshFullPath, static_cast<CStaticMeshEffect*>(iter)->Get_MeshPrototypePath().c_str());

			iMeshPathSize = lstrlen(szMeshFullPath) + 1;
			WriteFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);

			iMeshIndex = static_cast<CStaticMeshEffect*>(iter)->Get_MeshIndex();
			WriteFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
		}
		else if (CEffect::EFFECT_ANIMMESH == static_cast<CEffect*>(iter)->Get_EffectType())
		{
			// �ִ� �޽� ������ ����
			lstrcpy(szMeshFullPath, static_cast<CAnimMeshEffect*>(iter)->Get_MeshPrototypePath().c_str());

			iMeshPathSize = lstrlen(szMeshFullPath) + 1;
			WriteFile(hFile, &iMeshPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szMeshFullPath, iMeshPathSize * 2, &dwByte, nullptr);

			iMeshIndex = static_cast<CAnimMeshEffect*>(iter)->Get_MeshIndex();
			WriteFile(hFile, &iMeshIndex, sizeof(_int), &dwByte, nullptr);
		}
		else
		{
			tInstanceEffectDesc = static_cast<CInstanceEffect*>(iter)->Get_InstanceEffectDesc();

			/*tNewInstanceDesc.eInstanceType = (INSTANCEDESC2::INSTANCETYPE)tInstanceEffectDesc.eInstanceType;
			tNewInstanceDesc.eAxisType = (INSTANCEDESC2::AxisType)tInstanceEffectDesc.eAxisType;
			tNewInstanceDesc.vMinScl = tInstanceEffectDesc.vScl;
			tNewInstanceDesc.vMinDir = tInstanceEffectDesc.vDir;
			tNewInstanceDesc.vColor = tInstanceEffectDesc.vColor;
			tNewInstanceDesc.fMinForce = tInstanceEffectDesc.fForce;
			tNewInstanceDesc.fAcceleration = tInstanceEffectDesc.fAcceleration;
			tNewInstanceDesc.fAccMaxTime = tInstanceEffectDesc.fAccMaxTime;
			tNewInstanceDesc.fAngle = tInstanceEffectDesc.fAngle;
			tNewInstanceDesc.bRandomSpeed = tInstanceEffectDesc.bRandomSpeed;
			tNewInstanceDesc.bRandomPos = tInstanceEffectDesc.bRandomPos;
			tNewInstanceDesc.bTemp1 = tInstanceEffectDesc.bTemp1;
			tNewInstanceDesc.bTemp2 = tInstanceEffectDesc.bTemp2;*/

			WriteFile(hFile, &tInstanceEffectDesc, sizeof(INSTANCEDESC), &dwByte, nullptr);

			//lstrcpy(szFullPath, static_cast<CInstanceEffect*>(iter)->Get_PrototypePath().c_str());
			lstrcpy(szFullPath, iter->Get_TexturePath(CEffect::TEXTURE_DIFFUSE).c_str());

			iPathSize = lstrlen(szFullPath) + 1;
			WriteFile(hFile, &iPathSize, sizeof(_int), &dwByte, nullptr);

			WriteFile(hFile, &szFullPath, iPathSize * 2, &dwByte, nullptr);

			//iTextureIndex = static_cast<CInstanceEffect*>(iter)->Get_TextureIndex();
			iTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE);
			WriteFile(hFile, &iTextureIndex, sizeof(_int), &dwByte, nullptr);

			// ������ �ؽ��ĸ� ����ߴ��� �˻�
			if (tEffectDesc.bNoise)
			{
				lstrcpy(szNoiseFullPath, iter->Get_TexturePath(CEffect::TEXTURE_NOISE).c_str());
				iNoisePathSize = lstrlen(szNoiseFullPath) + 1;
				WriteFile(hFile, &iNoisePathSize, sizeof(_int), &dwByte, nullptr);
				WriteFile(hFile, &szNoiseFullPath, iNoisePathSize * 2, &dwByte, nullptr);
				iNoiseTextureIndex = iter->Get_TextureIndex(CEffect::TEXTURE_NOISE);
				WriteFile(hFile, &iNoiseTextureIndex, sizeof(_int), &dwByte, nullptr);
			}
		}
	}

	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return;
}

void CEffect_Tool::EffectCopy()
{
	if (0 >= static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_EffectCount())
		return;

	CEffect* pEffect = static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Get_Effect(m_iCurEffectIndex);
	if (nullptr == pEffect)
		return;

	EFFECTDESC tEffectDesc = static_cast<CEffect*>(pEffect)->Get_EffectDesc();
	_wstring strEffectName = L"";

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (0 == static_cast<CEffect*>(pEffect)->Get_EffectType())
	{
		CEffect* pNewEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_IMAGE_TOOL, &tEffectDesc);
		if (nullptr != pNewEffect)
		{
			strEffectName = L"_ImageEffect";
			static_cast<CEffect*>(pNewEffect)->Set_EffectName(strEffectName);
			// 0 ��° �̹����� �ʱ�ȭ
			pNewEffect->Init_Texture();
			pNewEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TexturePath(CEffect::TEXTURE_DIFFUSE));
			pNewEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE));
			if (tEffectDesc.bNoise)
			{
				pNewEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, pEffect->Get_TexturePath(CEffect::TEXTURE_NOISE));
				pNewEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE));
			}
			static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Add_Effect(pNewEffect);
		}
	}
	else if (1 == static_cast<CEffect*>(pEffect)->Get_EffectType())
	{
		CEffect* pNewEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_STATICMESH_TOOL, &tEffectDesc);
		pNewEffect->Set_Parent(m_vecEffect[m_iCurEffectObjectIndex]);
		if (nullptr != pNewEffect)
		{
			strEffectName = L"_MeshEffect";
			// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����
			static_cast<CEffect*>(pNewEffect)->Set_EffectName(strEffectName);
			// 0 ��° �̹����� �ʱ�ȭ
			pNewEffect->Init_Texture();
			pNewEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TexturePath(CEffect::TEXTURE_DIFFUSE));
			pNewEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE));
			if (tEffectDesc.bNoise)
			{
				pNewEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, pEffect->Get_TexturePath(CEffect::TEXTURE_NOISE));
				pNewEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE));
			}

			// ó�� �����ϸ� ������ 0 ��° �ε����� ���ҽ��� �� �ʱ�ȭ
			_matrix PivotMatrix = XMMatrixIdentity();
			PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
			_wstring strProtoTag = pEffect->Get_MeshProtoTag();
			pNewEffect->Set_MeshProtoTag(strProtoTag);
			CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
			//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pEffectFolder, &pEffectFolder->vecFileDataList[0], PivotMatrix);
			if (nullptr != pModel)
			{
				static_cast<CStaticMeshEffect*>(pNewEffect)->Set_Model(pModel, 0);
				static_cast<CStaticMeshEffect*>(pNewEffect)->Set_MeshIndex(pEffect->Get_MeshIndex());
				static_cast<CStaticMeshEffect*>(pNewEffect)->Set_MeshPrototypePath(pEffect->Get_MeshPrototypePath());
				static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Add_Effect(pNewEffect);
			}
		}
	}
	else if (2 == static_cast<CEffect*>(pEffect)->Get_EffectType())
	{
		CEffect* pNewEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_ANIMMESH_TOOL, &tEffectDesc);
		if (nullptr != pNewEffect)
		{
			strEffectName = L"_AnimEffect";
			// ����Ʈ �� ����Ʈ�ڽ� ��Ͽ� ǥ���� �̸� ����
			static_cast<CEffect*>(pNewEffect)->Set_EffectName(strEffectName);

			// ó�� �����ϸ� ������ 0 ��° �ε����� ���ҽ��� �� �ʱ�ȭ
			_matrix PivotMatrix = XMMatrixIdentity();
			PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);
			_wstring strProtoTag = pEffect->Get_MeshProtoTag();
			pNewEffect->Set_MeshProtoTag(strProtoTag);
			CModel* pModel = (CModel*)pGameInstance->Clone_Component(0, strProtoTag.c_str());
			//CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pEffectFolder, &pEffectFolder->vecFileDataList[0], PivotMatrix);
			if (nullptr != pModel)
			{
				static_cast<CAnimMeshEffect*>(pNewEffect)->Set_Model(pModel, 0);
				static_cast<CAnimMeshEffect*>(pNewEffect)->Set_MeshIndex(pEffect->Get_MeshIndex());
				static_cast<CAnimMeshEffect*>(pNewEffect)->Set_MeshPrototypePath(pEffect->Get_MeshPrototypePath());
				static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Add_Effect(pNewEffect);
			}
		}
	}
	else
	{
		CEffect* pNewEffect = (CEffect*)pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_EFFECT, PROTO_GOB_EFFECT_INSTANCE_TOOL, &tEffectDesc);
		if (nullptr != pNewEffect)
		{
			strEffectName = L"_InstanceEffect";
			static_cast<CEffect*>(pNewEffect)->Set_EffectName(strEffectName);
			// 0 ��° �̹����� �ʱ�ȭ
			pNewEffect->Init_Texture();
			pNewEffect->Set_TexturePath(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TexturePath(CEffect::TEXTURE_DIFFUSE));
			pNewEffect->Set_TextureIndex(CEffect::TEXTURE_DIFFUSE, pEffect->Get_TextureIndex(CEffect::TEXTURE_DIFFUSE));
			if (tEffectDesc.bNoise)
			{
				pNewEffect->Set_TexturePath(CEffect::TEXTURE_NOISE, pEffect->Get_TexturePath(CEffect::TEXTURE_NOISE));
				pNewEffect->Set_TextureIndex(CEffect::TEXTURE_NOISE, pEffect->Get_TextureIndex(CEffect::TEXTURE_NOISE));
			}
			static_cast<CEffectObject*>(m_vecEffect[m_iCurEffectObjectIndex])->Add_Effect(pNewEffect);

			INSTANCEDESC tInstanceDesc = static_cast<CInstanceEffect*>(pEffect)->Get_InstanceEffectDesc();
			static_cast<CInstanceEffect*>(pNewEffect)->Set_InstanceEffectDesc(tInstanceDesc);

			if (FAILED(static_cast<CInstanceEffect*>(pNewEffect)->ChangeInstanceCount(tEffectDesc.iInstanceCount)))
				MSG_BOX("�ν��Ͻ� ���Ҵ� ����");
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

CEffect_Tool* CEffect_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect_Tool* pInstance = new CEffect_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Create Failed : CEffect_Tool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Tool::Free()
{
	__super::Free();

	m_vecDiffuseFileData.clear();
	m_vecNoiseFileData.clear();
	m_vecMeshFileData.clear();

	for (auto& iter : m_StrList)
		Safe_Delete_Array(iter);

	for (auto& iter : m_vecEffect)
		Safe_Release(iter);
}
