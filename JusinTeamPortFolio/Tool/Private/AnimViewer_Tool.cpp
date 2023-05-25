#include "../../Tool/Public/AnimViewer_Tool.h"
#include "GameInstance.h"
#include "AnimViewerModel.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "MeshContainer.h"
#include "Channel.h"
#include "AnimationClip.h"
#include "Animator.h"
#include "Item_Tool.h"
#include "HierarchyNode.h"

const ImGuiTableSortSpecs* CAnimViewer_Tool::MyItem::s_current_sort_specs = NULL;

CAnimViewer_Tool::CAnimViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CImGuiClass_Tool(_pDevice, _pContext)
{
}

HRESULT CAnimViewer_Tool::NativeConstruct(void)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_pName = "AnimViewer";
	m_tWinDesc = WINDESC("AnimViewer", ImGuiWindowFlags_MenuBar, ImVec2(5.f, 5.f), ImVec2(600.f, 720.f), true);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// ĳ���� ������ �ִ� ���������� ��θ� ��� ����ü ����
	PATHS paths = { L"AnimationModel", L"PlayerCharacter" };
	// ĳ���� �� ������ �ִ� ����
	FILEGROUP* pCharacterFolder = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//// ĳ���� �� ���� ����Ʈ
	list<FILEGROUP*> pUnderFolderList = pCharacterFolder->lsUnderList;

	paths = { L"AnimationModel", L"OniCharacter" };
	// ĳ���� �� ������ �ִ� ����
	FILEGROUP* pOniCharacter = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	for (auto& pFile : pOniCharacter->lsUnderList)
		pUnderFolderList.push_back(pFile);

	//// ���� ���� ����Ʈ
	paths = { L"AnimationModel", L"BossCharacter" };
	// ĳ���� �� ������ �ִ� ����
	FILEGROUP* pBossCharacter = pGameInstance->Find_Folder(&paths);
	if (nullptr == pCharacterFolder)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	for (auto& pFile : pBossCharacter->lsUnderList)
		pUnderFolderList.push_back(pFile);


	// ���� ���� ����Ʈ���� ���̳ʸ� ���ϵ��� ã�ƿ´�.
	if (0 != pUnderFolderList.size())
	{
		for (auto& iter : pUnderFolderList)
		{
			// ���̳ʸ� ������ �޾ƿ´�.
			FILEGROUP* pBinaryFolder = iter->Find_Group(L"FBX_Converter");
			// ���̳ʸ� ������ ���ٸ� continue;
			if (nullptr == pBinaryFolder)
				continue;

			// ���̳ʸ� ���ϵ��� binaryList�� ����ش�.
			if (0 != pBinaryFolder->vecFileDataList.size())
			{
				for (auto& pBinaryFile : pBinaryFolder->vecFileDataList)
				{
					// ���̳ʸ� ���ϵ��� �����Ѵ�.
					m_BinaryList.push_back(pBinaryFile);
					// �ǹ� ��Ʈ������ ������ش�.
					_matrix		PivotMatrix = XMMatrixIdentity();
					PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
					// ���� �����.
					CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pBinaryFolder, &pBinaryFile, PivotMatrix);
					if (nullptr == pModel)
						return E_FAIL;
					// ������� ���� ����Ʈ�� �־��ش� ( ������Ÿ������ ����� )
					m_ModelList.push_back(pModel);
				}
			}
		}
	}

	// ����
	{
		map<const _tchar*, FMOD_SOUND*>* SoundMap = pGameInstance->Get_SoundMap();
		if (nullptr != SoundMap)
		{
			for (auto& Pair : *SoundMap)
			{
				// ��� ������� �̸��� ��´�.
				_string strSoundName = CMethod::ConvertWToS(Pair.first);

				m_SoundNameList.push_back(strSoundName);
			}
		}
	}


	RELEASE_INSTANCE(CGameInstance);

	{
		paths = { L"Model", L"Item" };
		FILEGROUP* ItemFileGroup = pGameInstance->Find_Folder(&paths);
		for (auto& iter : ItemFileGroup->lsUnderList)
		{
			// ���̳ʸ� ������ �޾ƿ´�.
			FILEGROUP* pBinaryFolder = iter->Find_Group(L"FBX_Converter");
			// ���̳ʸ� ������ ���ٸ� continue;
			if (nullptr == pBinaryFolder)
				continue;

			for (auto& Data : pBinaryFolder->vecFileDataList)
			{
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, pBinaryFolder, &Data,
					XMMatrixScaling(0.1f, 0.1f, 0.1f));
				m_ItemModelList.push_back(pModel);
			}
		}
	}

	{
		paths = { L"Data", L"Animator" };
		FILEGROUP* AnimatorFileGroup = pGameInstance->Find_Folder(&paths);
		for (auto& iter : AnimatorFileGroup->lsUnderList)
		{
			// ���̳ʸ� ������ �־��ش�.
			for (auto& file : iter->vecFileDataList)
				m_SavedAnimatorList.push_back(file);
		}
	}

	return S_OK;
}

HRESULT CAnimViewer_Tool::NativeConstruct_InnerValue(void)
{
	if (FAILED(__super::NativeConstruct_InnerValue()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pSwordTool = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
		Safe_AddRef(m_pSwordTool);
	}

	{
		m_pShelteTool = static_cast<CItem_Tool*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_TEST, PROTO_GOB_TEST_ITEM));
		Safe_AddRef(m_pShelteTool);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CAnimViewer_Tool::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pAnimatorCom && nullptr != m_pAnimatorCom->Get_CurrentAnimationClip())
	{
		if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
			m_pAnimatorCom->Set_Play(!m_pAnimatorCom->Is_Play());
	}


	/*if (pGameInstance->Get_DIKeyDown(DIK_COMMA))
		m_pAnimatorCom->Play_OneFrame(-_dTimeDelta);

	if (pGameInstance->Get_DIKeyDown(DIK_PERIOD))
		m_pAnimatorCom->Play_OneFrame(_dTimeDelta);*/


	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pSwordTool)
		m_pSwordTool->Tick(_dTimeDelta);

	if (nullptr != m_pShelteTool)
		m_pShelteTool->Tick(_dTimeDelta);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

	ImVec2 imVec2 = ImVec2(static_cast<_float>(g_iWinSizeX - 180.f), 10.f);
	ImGui::SetNextWindowPos(ImVec2(imVec2.x + m_tWinDesc.v2Pos.x, 5.f + imVec2.y + m_tWinDesc.v2Pos.y), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_tWinDesc.v2Size, ImGuiCond_Appearing);

	ImGui::Begin(m_tWinDesc.strName.c_str(), &m_tWinDesc.bEnable, m_tWinDesc.imGuiWindowFlags);

	Show_Window();

	ImGui::End();

	if (nullptr != m_pSwordTool)
		m_pSwordTool->Late_Tick(_dTimeDelta);

	if (nullptr != m_pShelteTool)
		m_pShelteTool->Late_Tick(_dTimeDelta);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Release_InnerValue(void)
{
	if (FAILED(__super::Release_InnerValue()))
		return E_FAIL;

	// ���� �����ϴ� ��ü�� ���۷��� ī��Ʈ�� �����Ѵ�.
	Safe_Release(m_pAnimObject);
	Safe_Release(m_pSwordTool);
	Safe_Release(m_pShelteTool);
	// �׽�Ʈ�� Animator ������Ʈ ��ü�� ���۷��� ī��Ʈ�� ���ҽ�Ų��.
	Safe_Release(m_pAnimatorCom);
	// �׽�Ʈ�� Animation Ŭ���� ���ش�.
	Safe_Release(m_pAnimationClip);

	m_pAnimObject = nullptr;
	m_pSwordTool = nullptr;
	m_pShelteTool = nullptr;
	m_pAnimatorCom = nullptr;
	m_pAnimationClip = nullptr;
	// ���� ������ �ִϸ��̼� �� �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
	m_iModelIndex = -1;
	// ���� ������ ���� ���� �ִϸ��̼� �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
	m_iAnimationIndex = -1;
	// ���� ����ǰ��ִ� �ִϸ��̼��� �����ִ� Channel�� ���� ������ ä�� -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
	m_iChannelIndex = -1;
	// ���� ������ ���� ���� Bone �ε��� ��ȣ -> �ƹ��͵� ���õ��� �ʾҴٸ� -1�� �ʱ�ȭ
	m_iBoneIndex = -1;
	// �ִϸ��̼� �� ���� ����ִ� �̺�Ʈ ����� ���� ������ �̺�Ʈ �ε���
	m_iEventIndex = -1;
	// ���� ���õ� Ʈ������ �ε���
	m_iTransitionIndex = -1;
	// Channel�� KeyFrame�� 2�� ��츦 ������ �� ����ϱ����� bool ����
	m_bIsFiltered = false;

	return S_OK;
}

void CAnimViewer_Tool::Show_Window(void)
{
	__super::Show_Window();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("AnimViewer TabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Object"))
		{
			// �ִϸ��̼� �� ����� ��ư
			if (nullptr == m_pAnimObject)
			{
				if (ImGui::Button("Create Animation Model"))
				{
					if (FAILED(Create_AnimObject()))
						return;
				}
			}
			// �ִϸ��̼� ���� ��������ٸ� ��ġ �����̳� ���⸦ ���̴µ��� �۾��� �� �� ����.
			else
			{
				ImGui::Text("Animation Model List");
				// ���̳ʸ�ȭ �� �ִϸ��̼� ĳ���� �𵨵��� �����ִ� ����Ʈ�� ����. ( �� �ܰ迡�� ������ �̸��� �ܼ��� ���ϸ��ϻ��� )
				if (ImGui::BeginListBox("Animation Model List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (int i = 0; i < m_BinaryList.size(); i++)
					{
						const bool is_selected = (m_iModelIndex == i);
						if (ImGui::Selectable(CMethod::ConvertWToS(m_BinaryList[i].strFileName).c_str(), is_selected))
						{
							m_iModelIndex = i;

							// ���� �����Ѵ� ( �׽�Ʈ ���� �ִϸ����� ������Ʈ���� ���� �־��ش�. )
							if (FAILED(Set_Model()))
								return;

							//// �ִϸ��̼� �׽�Ʈ ���� �ִϸ����͸� �޾ƿ´�.
							//m_pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();
							//if (nullptr == m_pAnimatorCom)
							//	return;

							// �ִԸ��� ���� �ִϸ����Ϳ� ��� ���� �ִϸ��̼��� �����´�.
							vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

							// �ִϸ����� ������Ʈ�� CurrentClip�� Test�� �ٲ۴�.
							if (nullptr == m_pAnimatorCom->Get_AnimationClip("Test"))
							{
								// �� �ִϸ��̼� Ŭ�� �ϳ��� �����Ѵ�.
								CAnimationClip* pClip = CAnimationClip::Create();

								// �̸��� Test�� �����Ѵ�.
								pClip->Set_Name("Test");

								// �ִϸ����� ������Ʈ���� �ִϸ��̼� Ŭ���� �־��ش�.
								if (nullptr != pClip)
									m_pAnimatorCom->Add_AnimationClip(pClip);
							}
							m_pAnimatorCom->Set_CurrentAnimationClip("Test");

							// ���� �ִϸ��̼� Ŭ���� ���� �ִϸ��̼��� ���ٸ� ���� �⺻���� �������ش�.
							if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation())
							{
								m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);
							}

							// �ִԸ��� ���� �ִϸ����Ϳ� ���� Ŭ���� �ִϸ��̼��� ����ش�.
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);

							// ������ ������Ʈ�Ѵ�.
							m_pAnimatorCom->Update_InitBoneState();
						}
					}
					ImGui::EndListBox();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation Clip"))
		{
			// �ִϸ��̼� ���� ���ٸ� ���ٴ� ������ ����.
			if (nullptr == m_pAnimObject)
				ImGui::Text("Please Select Model");
			else
			{
				// ������ �ִϸ��̼� ���� ModelCom�� ä������ �ʾҴٸ� ä���޶�� ������ ����.
				if (-1 == m_iModelIndex)
					ImGui::Text("Please Set Animation Model");
				// ����Ʈ���� �ִϸ��̼� ���� ���õƴٸ� �ִϸ��̼� ���� ���� �ִϸ��̼� ����� �����ش�.
				else
				{
					vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

					// �ִϸ����� ������Ʈ�� Test��� Ŭ���� �������� �ʴٸ� ������ش�.
					if (nullptr == m_pAnimatorCom->Get_AnimationClip("Test"))
					{
						// �� �ִϸ��̼� Ŭ�� �ϳ��� �����Ѵ�.
						CAnimationClip* pClip = CAnimationClip::Create();

						// �̸��� Test�� �����Ѵ�.
						pClip->Set_Name("Test");

						// �ִϸ����� ������Ʈ���� �ִϸ��̼� Ŭ���� �־��ش�.
						if (nullptr != pClip)
							m_pAnimatorCom->Add_AnimationClip(pClip);
					}
					// �ִϸ������� ���� Ŭ���� Test Ŭ���� ���� �ʴٸ� Test�� �������ش�.			
					if (m_pAnimatorCom->Get_AnimationClip("Test") != m_pAnimatorCom->Get_CurrentAnimationClip())
						m_pAnimatorCom->Set_CurrentAnimationClip("Test");

					if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation())
					{
						m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[0]);
					}

					// ���� AnimViewerModel ��ü�� ����ǰ��ִ� �ִϸ��̼� �ε��� ��ȣ
					string strCurAnimIndex = "Current Playing Animation Index : ";
					if (-1 == m_iAnimationIndex)
						strCurAnimIndex += "Please Select Animation";
					else
						strCurAnimIndex += CMethod::Convert_Number_S(m_iAnimationIndex);
					ImGui::Text(strCurAnimIndex.c_str());

					CAnimator* pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();

					// ���� AnimViewerModel ��ü�� ���صǰ��ִ� �ִϸ��̼��� �̸�
					string strCurAnimName = "Current Playing Animation Name : ";

					_int iStart = (_int)m_pAnimatorCom->Get_MainAnimationName().find("(", 0) + 1;
					_int iEnd = (_int)m_pAnimatorCom->Get_MainAnimationName().find(")", 0) - iStart;
					strCurAnimName += m_pAnimatorCom->Get_MainAnimationName().substr(iStart, iEnd);
					ImGui::Text(strCurAnimName.c_str());

					// �ִϸ��̼��� ���� �÷��� Ÿ��
					string strTimeAcc = "TimeAcc : ";
					strTimeAcc += CMethod::Convert_Number_S(m_pAnimatorCom->Get_TimeAcc(), 2);
					ImGui::PushItemWidth(1.f);
					ImGui::Text(strTimeAcc.c_str());
					ImGui::PopItemWidth();

					ImGui::SameLine();

					// �ִϸ��̼��� ����
					string strDuration = "Duration : ";
					strDuration += CMethod::Convert_Number_S(m_pAnimatorCom->Get_Duration(), 2);
					ImGui::Text(strDuration.c_str());

					ImGui::SameLine();

					// �ִϸ��̼��� TickPerSecond
					string strTickPerSecond = "Tps : ";
					strTickPerSecond += CMethod::Convert_Number_S(m_pAnimatorCom->Get_TickPerSecond(), 2);
					ImGui::Text(strTickPerSecond.c_str());

					ImGui::SameLine();

					_string strAnimSpeed = "Speed : ";
					strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

					ImGui::Text(strAnimSpeed.c_str());

					ImGui::SameLine();

					// �ִϸ��̼� �ӵ���
					if (ImGui::SmallButton("Set"))
					{
						ImGui::OpenPopup("Set");
					}

					if (ImGui::BeginPopup("Set"))
					{
						ImGui::Text("Set Speed");

						ImGui::SameLine();

						if (ImGui::SmallButton("Reset"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

						ImGui::Separator();

						if (ImGui::Selectable("0.5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

						if (ImGui::Selectable("2x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

						if (ImGui::Selectable("3x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

						if (ImGui::Selectable("4x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

						if (ImGui::Selectable("5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

						if (ImGui::Selectable("6x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

						_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

						if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

						ImGui::EndPopup();
					}

#pragma region �ִϸ��̼� ������� ���
					// �ִϸ��̼��� ���൵�� �޾ƿ´�.
					_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
					if (ImGui::SliderFloat("Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
					{
						// �ִϸ��̼��� ���൵�� �����Ѵ�.
						static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
					}

					// �÷��� ���϶��� Pause��ư Ȱ��ȭ, �÷����ϰ����� �ʴ� ��쿡�� Play ��ư Ȱ��ȭ
					if (true == m_pAnimatorCom->Is_Play())
					{
						if (ImGui::Button("Pause"))
						{
							m_pAnimatorCom->Set_Play(false);
						}
					}
					else
					{
						if (ImGui::Button("Play"))
						{
							m_pAnimatorCom->Set_Play(true);
						}
					}

					ImGui::SameLine();
					// Stop ��ư�� ������ Play�� false�� �����ϰ� �ִϸ��̼��� ���� �ִϸ��̼����� �ٽ� �־��ش�.
					if (ImGui::Button("Stop"))
					{
						m_pAnimatorCom->Set_Stop();
					}

					ImGui::SameLine();

					// �������ο� ���� üũ���Ѵ�.
					_bool isLoop = m_pAnimatorCom->Get_CurrentAnimationClip()->Is_Loop();
					if (ImGui::Checkbox("Loop", &isLoop))
					{
						m_pAnimatorCom->Get_CurrentAnimationClip()->Set_Loop(isLoop);
					}

					ImGui::SameLine();

					// RootBone�� ���� üũ���Ѵ�.
					_bool isRootBone = m_pAnimatorCom->Is_RootBone();
					if (ImGui::Checkbox("RootBone", &isRootBone))
					{
						m_pAnimatorCom->Set_RootBone(isRootBone);
					}
#pragma endregion

#pragma region �ִϸ��̼� ����Ʈ
					if (ImGui::TreeNode("Animation List"))
					{
						const int COLUMNS_COUNT = 5;
						// �ִϸ��̼� ������ ���ڷ� �������� string ����
						string strAnimSize = "Animation Count : ";
						strAnimSize += CMethod::Convert_Number_S((_uint)AnimationList.size());
						ImGui::Text(strAnimSize.c_str());

						static ImVector<MyItem> items;
						// �������� ����ִٸ� �������� ������ش�.
						if (items.Size == 0)
						{
							// ����� �缳���Ѵ�.
							items.resize((_int)AnimationList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)AnimationList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)AnimationList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = AnimationList[n]->Get_Name().substr(iStart, iEnd);
								// �ִϸ��̼��� �̸� ����
								strcpy_s(item.Name, szFileName.c_str());
								// �ִϸ��̼��� Duration ����
								item.Duration = AnimationList[n]->Get_Duration();
								// �ִϸ��̼��� TimePerSecond ����
								item.Tps = AnimationList[n]->Get_TickPerSecond();
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Duration);
							ImGui::TableSetupColumn("Tps", ImGuiTableColumnFlags_PreferSortDescending | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_TPS);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// ���� ���� ���ؿ� ���� �����Ѵ�.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// �ִϸ��̼� �̸�
									const bool is_selected = (m_iAnimationIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iAnimationIndex = item->Index;
										Set_Animation();
									}
									ImGui::TableNextColumn();
									// Duration
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Duration, 2).c_str());
									ImGui::TableNextColumn();
									// TPS
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Tps, 2).c_str());
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region ä�� ����Ʈ
					// ���� ���� ä�� ��� ���
					if (ImGui::TreeNode("Channel List"))
					{
						// ���� �ִϸ��̼��� �����ִ� ä�� ����Ʈ
						vector<CChannel*> m_ChannelList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_ChannelList();

						const int COLUMNS_COUNT = 4;
						// ä�� ������ ���´�.
						string strChannelSize = "All Channel Count : ";
						strChannelSize += CMethod::Convert_Number_S((_uint)m_ChannelList.size());
						ImGui::Text(strChannelSize.c_str());

						ImGui::SameLine();

						static ImVector<MyItem> items;
						// �������� ����ִٸ� �������� ������ش�.
						if (items.Size == 0)
						{
							// ����� �缳���Ѵ�.
							items.resize((_int)m_ChannelList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)m_ChannelList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_ChannelList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_ChannelList[n]->Get_Name().substr(iStart, iEnd);
								// ä���� �̸� ����
								strcpy_s(item.Name, szFileName.c_str());
								// ä���� Ű������ ����
								item.FrameCount = m_ChannelList[n]->Get_NumMaxKeyFrames();
							}
						}

						// Ȱ��ȭ �� ä�� ������ ���´�.
						string strActiveChannelSize = " / Activate Channel Count : ";
						_uint iActiveCount = 0;
						for (auto& pChannel : m_ChannelList)
						{
							if (2 < pChannel->Get_NumMaxKeyFrames())
								iActiveCount++;
						}
						strActiveChannelSize += CMethod::Convert_Number_S(iActiveCount);
						ImGui::Text(strActiveChannelSize.c_str());

						if (ImGui::Checkbox("Filter DeActive", &m_bIsFiltered))
						{
							// ������ ��� �����
							items.clear();

							// ä�θ���Ʈ
							vector<CChannel*> ChannelList;

							// ���͸��� �����ٸ� FrameCount�� 2���� ū�ֵ鸸 ����Ѵ�.
							if (true == m_bIsFiltered)
							{
								for (auto& pChannel : m_ChannelList)
								{
									if (2 < pChannel->Get_NumMaxKeyFrames())
										ChannelList.push_back(pChannel);
								}
							}
							else
								ChannelList = m_ChannelList;

							// ����� �缳���Ѵ�.
							items.resize((_int)ChannelList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)ChannelList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)ChannelList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = ChannelList[n]->Get_Name().substr(iStart, iEnd);
								// ä���� �̸� ����
								strcpy_s(item.Name, szFileName.c_str());
								// ä���� Ű������ ����
								item.FrameCount = ChannelList[n]->Get_NumMaxKeyFrames();
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Channel_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("KeyFrames", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_FrameCount);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// ���� ���� ���ؿ� ���� �����Ѵ�.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// �̸�
									const bool is_selected = (m_iChannelIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iChannelIndex = item->Index;
									}
									ImGui::TableNextColumn();
									// KeyFrames
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].FrameCount, 2).c_str());
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region ���̾��Ű ��� ����Ʈ
					if (ImGui::TreeNode("Hierarchy Node List"))
					{
						vector<CHierarchyNode*> m_HierarchyNodeList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_HierarchyNodeList();

						const int COLUMNS_COUNT = 5;
						// �ִϸ��̼� ������ ���ڷ� �������� string ����
						string strChannelSize = "All Bone Count : ";
						strChannelSize += CMethod::Convert_Number_S((_uint)m_HierarchyNodeList.size());
						ImGui::Text(strChannelSize.c_str());

						ImGui::SameLine();

						// ���� ���õ� ���� �˷��ش�.
						string strCurrentBone = "/ Current Bone : ";
						if (-1 == m_iBoneIndex)
							strCurrentBone += "None Selected";
						else
							strCurrentBone += m_HierarchyNodeList[m_iBoneIndex]->Get_Name();

						ImGui::Text(strCurrentBone.c_str());

						// ���õ� ���� �ִٸ� ���� ������ �� �ִ� ��ư�� ������ش�.
						if (-1 != m_iBoneIndex)
						{
							ImGui::SameLine();

							if (ImGui::SmallButton("Unselect Bone"))
							{
								m_pAnimatorCom->Set_RootBoneName("Root");
								m_iBoneIndex = -1;
							}

							_float4 vBonePos;

							if (-1 != m_iBoneIndex)
								vBonePos = CMethod::XMMatrixGetFloat4(m_HierarchyNodeList[m_iBoneIndex]->Get_CombinedTransformationMatrix(), 3);

							_float4 vPlayerPos = CMethod::XMMatrixGetFloat4(m_pAnimObject->Get_Transform()->Get_WorldMatrix(), 3);

							ImGui::Text("Current BonePos [ x: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.x, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / y: ");
							ImGui::SameLine();
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.y, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / z: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vBonePos.z, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" ]");

							ImGui::Text("Test Model Pos [ x: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.x, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / y: ");
							ImGui::SameLine();
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.y, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" / z: ");
							ImGui::SameLine();
							ImGui::Text(CMethod::Convert_Number_S(vPlayerPos.z, 2).c_str());
							ImGui::SameLine();
							ImGui::Text(" ]");
						}

						// ��ġ�� ������ ����� ���� �ִ� RootBoneã��
						if (ImGui::Button("Find RootBone"))
						{
							int iIndexCount = 0;

							for (auto& pBone : m_HierarchyNodeList)
							{
								// Root��� �̸��� Bone�� ã�Ҵٸ� BoneIndex�� �����ϰ� �ݺ����� Ż���Ѵ�.
								if (!strcmp(pBone->Get_Name().c_str(), "Root"))
								{
									m_iBoneIndex = iIndexCount;
									m_pAnimatorCom->Set_RootBoneName("Root");
									break;
								}
								iIndexCount++;
							}
						}

						ImGui::SameLine();

						// ����� ��ġ�� ������ ����� RootBone ���
						if (ImGui::Button("Add Bone To Animator"))
							m_pAnimatorCom->Add_WeaponBone(m_HierarchyNodeList[m_iBoneIndex]);

						static ImVector<MyItem> items;
						// �������� ����ִٸ� �������� ������ش�.
						if (items.Size == 0)
						{
							// ����� �缳���Ѵ�.
							items.resize((_int)m_HierarchyNodeList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)m_HierarchyNodeList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_HierarchyNodeList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_HierarchyNodeList[n]->Get_Name().substr(iStart, iEnd);
								// ���� �̸� ����
								strcpy_s(item.Name, szFileName.c_str());
								// ���� Depth ����
								item.Depth = (_uint)m_HierarchyNodeList[n]->Get_Depth();
								// ���� ���� �θ���� �̸�
								iStart = (_int)m_HierarchyNodeList[n]->Get_ParentName().find("(", 0) + 1;
								iEnd = (_int)m_HierarchyNodeList[n]->Get_ParentName().find(")", 0) - iStart;
								_string szParentName = m_HierarchyNodeList[n]->Get_ParentName().substr(iStart, iEnd);
								strcpy_s(item.ParentName, szParentName.c_str());
							}
						}

						// ���� item����� �θ���� �̸��� ���� ��ü�� �ִٸ� �ε�����ȣ�� �θ�� �̸��� �տ� �ٿ��ش�.
						for (auto& pParent : items)
						{
							for (auto& pChild : items)
							{
								if (!strcmp(pParent.Name, pChild.ParentName))
								{
									_string szParentName = "[";
									szParentName += CMethod::Convert_Number_S(pParent.Index);
									szParentName += "]";
									strcpy_s(pChild.ParentName, (szParentName + pChild.ParentName).c_str());
									pChild.ParentIndex = pParent.Index;
									continue;
								}
							}
						}

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Depth", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Depth);
							ImGui::TableSetupColumn("ParentNode", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Parent);
							ImGui::TableSetupColumn("Selected", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// ���� ���� ���ؿ� ���� �����Ѵ�.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// ���� �̸�
									const bool is_selected = (m_iBoneIndex == item->Index);

									if (ImGui::Selectable(item->Name, is_selected))
									{
										m_iBoneIndex = item->Index;
										m_pAnimatorCom->Set_RootBoneName(item->Name);
									}
									ImGui::TableNextColumn();
									// Depth
									ImGui::Text(CMethod::Convert_Number_S(items[item->Index].Depth, 2).c_str());
									ImGui::TableNextColumn();
									// �θ� ���� �̸�
									//static int iParentIndex = 0;
									//const bool isParent_selected = (iParentIndex == row_n);

									if (ImGui::Selectable(item->ParentName, false))
									{
										m_iBoneIndex = item->ParentIndex;
									}
									ImGui::TableNextColumn();
									// Selected
									if (is_selected)
										ImGui::SmallButton("Selected");
									ImGui::PopID();
								}
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region �޽� �����̳� ����Ʈ
					if (ImGui::TreeNode("MeshContainer List"))
					{
						vector<CMeshContainer*> m_MeshList = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_MeshContainerList();

						const int COLUMNS_COUNT = 3;
						// �ִϸ��̼� ������ ���ڷ� �������� string ����
						string strMeshSize = "All Meshes Count : ";
						strMeshSize += CMethod::Convert_Number_S((_uint)m_MeshList.size());
						ImGui::Text(strMeshSize.c_str());

						static ImVector<MyItem> items;
						// �������� ����ִٸ� �������� ������ش�.
						if (items.Size == 0)
						{
							// ����� �缳���Ѵ�.
							items.resize((_int)m_MeshList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)m_MeshList[n]->Get_Name().find("(", 0) + 1;
								iEnd = (_int)m_MeshList[n]->Get_Name().find(")", 0) - iStart;
								_string szFileName = m_MeshList[n]->Get_Name().substr(iStart, iEnd);
								// �޽��� �̸� ����
								strcpy_s(item.Name, szFileName.c_str());
								// �޽��� Ȱ��ȭ ���� ( ó�� �����ô� ������ true )
								item.Action = true;
							}
						}

						// ������ ����� ��ȸ�ϸ� Action�� True�� ��ü�� ã�� ��ü�� Index�� �־��ش�.
						m_MeshContainerList.clear();
						for (auto& pMesh : items)
						{
							if (true == pMesh.Action)
								m_MeshContainerList.push_back(pMesh.Index);
						}

						ImGui::SameLine();

						// ���� Ȱ��ȭ�� �޽��� ������ �˷��ش�.
						string strMeshCount = "/ Active Meshes Count : ";
						strMeshCount += CMethod::Convert_Number_S((_uint)m_MeshContainerList.size());

						ImGui::Text(strMeshCount.c_str());

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY;

						if (ImGui::BeginTable("Animation_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Action);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// ���� ���� ���ؿ� ���� �����Ѵ�.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									// Index
									ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
									ImGui::TableNextColumn();
									// Name
									ImGui::Text(item->Name);
									ImGui::TableNextColumn();
									// Active
									ImGui::Checkbox("", &item->Action);
									ImGui::PopID();
								}

							static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_MeshIndexList(m_MeshContainerList);

							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region �̺�Ʈ ����Ʈ
					// �ִϸ��̼� Ŭ���� �����ִ� �̺�Ʈ ����Ʈ 
					if (ImGui::TreeNode("Events List"))
					{
						static _int iSelectEvent = -1;

						// �ִϸ����� ������Ʈ�� �����ִ� Test Clip�� �޾ƿ´�.
						CAnimationClip* pClip = m_pAnimatorCom->Get_AnimationClip("Test");
						if (nullptr == pClip)
							return;

						CModel* pModel = m_pAnimatorCom->Get_Model();
						if (nullptr == pModel)
							return;

						// �̺�Ʈ ����� �޾ƿ´�.
						vector<CAnimationClip::ANIMEVENTDESC> eventList = pClip->Get_EventList();

						const int COLUMNS_COUNT = 5;
						// ���� Ŭ���� ���� �̺�Ʈ�� ����
						string strEventSize = "All Event Count : ";
						strEventSize += CMethod::Convert_Number_S((_uint)eventList.size());
						ImGui::Text(strEventSize.c_str());

						ImGui::SameLine();

						if (ImGui::Button("Add Event"))
						{
							CAnimationClip::ANIMEVENTDESC animDesc;
							// ���� Ratio�� ��´�.
							animDesc.dStartRatio = pClip->Get_Ratio();
							// �̺�Ʈ ����Ʈ�� push_back �Ѵ�.
							eventList.push_back(animDesc);
						}

						ImGui::SameLine();

						if (ImGui::Button("Remove Event"))
						{
							// ����Ʈ���� �����.
							eventList.erase(eventList.begin() + iSelectEvent);

							// -1�� ������ش�.
							iSelectEvent = -1;
						}

						static ImVector<MyItem> items;
						// �������� ����ִٸ� �������� ������ش�.
						if (items.Size == 0 || eventList.size() != items.Size || m_bIsClipUpdate)
						{
							// �̺�Ʈ ����Ʈ�� ������� ����� �缳���Ѵ�.
							items.resize((_int)eventList.size(), MyItem());
							for (int n = 0; n < items.Size; n++)
							{
								MyItem& item = items[n];
								// �ִϸ��̼� Ŭ���� �ε��� ���� ����
								item.Index = n;
								// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
								iStart = (_int)eventList[n].strName.find("(", 0) + 1;
								iEnd = (_int)eventList[n].strName.find(")", 0) - iStart;
								_string szEventName = eventList[n].strName.substr(iStart, iEnd);
								// �ִϸ��̼� Ŭ���� �̸� ����
								strcpy_s(item.Name, szEventName.c_str());
								// �̺�Ʈ Ÿ��
								item.iEventType = (_uint)eventList[n].tEventType;
								// �̺�Ʈ ���� ����
								item.dStartRatio = eventList[n].dStartRatio;
								// �̺�Ʈ ���� ����
								item.dEndRatio = eventList[n].dEndRatio;
								// �̺�Ʈ ���� �ð�
								item.dEndTime = eventList[n].dEndTime;
								// ���� ��ġ 
								item.vEventPos = eventList[n].vEventPos;
								// �ɼ� ���� �̸�
								strcpy_s(item.EventText, eventList[n].strEventText.c_str());
							}
						}

						static const char* eventTypes[] =
						{
							"Effect","UI", "Sound", "Collider", "WeaponBone", "Camera", "Hide", "SpeedSection","BoneTransform","Not_Selected"
						};

						// Options
						static ImGuiTableFlags flags =
							ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
							| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
							| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

						if (ImGui::BeginTable("Event_Table_AniClipTest", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
						{
							// - ImGuiTableColumnFlags_DefaultSort
							// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
							// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
							ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
							ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort, 0.0f, MyItemColumnID_Name);
							ImGui::TableSetupColumn("BoneName", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
							ImGui::TableSetupColumn("EventType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventType);
							ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
							ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
							ImGui::TableHeadersRow();

							// Sort our data if sort specs have been changed!
							if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
								if (sorts_specs->SpecsDirty)
								{
									MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
									// ���� ���� ���ؿ� ���� �����Ѵ�.
									if (items.Size > 1)
										qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
									MyItem::s_current_sort_specs = NULL;
									sorts_specs->SpecsDirty = false;
								}

							// Demonstrate using clipper for large vertical lists
							ImGuiListClipper clipper;
							clipper.Begin(items.Size);
							while (clipper.Step())
								for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
								{
									MyItem* item = &items[row_n];
									ImGui::PushID(item->Index);
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									// ���õƴٸ� �Ķ������� �׸���.
									if (iSelectEvent == item->Index)
									{
										ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.3f, 0.5f));
										ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
									}

									// Index
									if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
									{
										iSelectEvent = item->Index;
									}
									ImGui::TableNextColumn();
									// Name
									char strText[MAX_PATH] = "";
									strcpy_s(strText, item->Name);
									ImGui::SetNextItemWidth(-FLT_MIN);
									if (ImGui::InputText("##10", strText, sizeof(char) * MAX_PATH))
									{
										strcpy_s(item->Name, strText);
										eventList[item->Index].strName = strText;
									}
									ImGui::TableNextColumn();

									static _string strInput;
									static vector<CHierarchyNode*> BoneList;
									// BoneName
									if (ImGui::Selectable(eventList[item->Index].strEventText.c_str()))
										ImGui::OpenPopup("BoneName");

									if (ImGui::BeginPopup("BoneName"))
									{
										ImGui::Text(u8"�� �̸��� �Է��Ͻÿ�");
										_char INPUT[MAX_PATH] = "";
										strcpy_s(INPUT, strInput.c_str());
										_bool bInput = false;
										bInput = ImGui::InputText("BoneName", INPUT, sizeof(_char) * MAX_PATH);
										strInput = INPUT;

										if (!strcmp(strInput.c_str(), ""))
										{
											for (auto& iter : pModel->Get_HierachyNodeList())
											{
												if (ImGui::Selectable(iter->Get_Name().c_str()))
												{
													eventList[item->Index].strEventText = iter->Get_Name();
												}
											}
										}
										else if (true == bInput)
										{
											BoneList.clear();
											for (auto& iter : pModel->Get_HierachyNodeList())
											{
												if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strInput.c_str()))
													BoneList.push_back(iter);
											}
										}
										else
										{
											for (auto& iter : BoneList)
											{
												if (ImGui::Selectable(iter->Get_Name().c_str()))
												{
													eventList[item->Index].strEventText = iter->Get_Name();
												}
											}
										}

										ImGui::EndPopup();
									}
									ImGui::TableNextColumn();

									// Event
									if (ImGui::Selectable(eventTypes[item->iEventType]))
										ImGui::OpenPopup("EventType");

									if (ImGui::BeginPopup("EventType"))
									{
										for (int i = 0; i < IM_ARRAYSIZE(eventTypes); ++i)
										{
											if (ImGui::Selectable(eventTypes[i]))
											{
												eventList[item->Index].tEventType = (CAnimationClip::ANIMEVENTTYPE)i;
												item->iEventType = i;
											}
										}
										ImGui::EndPopup();
									}
									ImGui::TableNextColumn();

									// Value ����
									if (ImGui::Selectable("##Values"))
										ImGui::OpenPopup("##EventValues");

									if (ImGui::BeginPopup("##EventValues"))
									{
										ImGui::Text(u8"�Ⱦ��� ���� ���θ� ��");

										ImGui::SameLine();

										if (ImGui::SmallButton("Reset"))
										{
											eventList[item->Index].dStartRatio = pClip->Get_Ratio();
											eventList[item->Index].dEndRatio = 0.0;
											eventList[item->Index].dEndTime = 0.0;
											eventList[item->Index].vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
											eventList[item->Index].fValue = 0.f;
											eventList[item->Index].fAddValue = 0.f;
										}

										ImGui::Separator();

										ImGui::Text(u8"��������");

										_float fRatio = (_float)eventList[item->Index].dStartRatio;
										if (ImGui::DragFloat("##1", &fRatio, 0.01f))
											eventList[item->Index].dStartRatio = fRatio;

										ImGui::SameLine();

										if (ImGui::Button("CurTime"))
											eventList[item->Index].dStartRatio = pClip->Get_Ratio();

										ImGui::Text(u8"��������");

										_float fEndRatio = (_float)eventList[item->Index].dEndRatio;
										if (ImGui::DragFloat("##2", &fEndRatio, 0.01f))
											eventList[item->Index].dEndRatio = fEndRatio;

										ImGui::Text(u8"����ð�");

										_float fEndTime = (_float)eventList[item->Index].dEndTime;
										if (ImGui::DragFloat("##3", &fEndTime, 0.01f))
											eventList[item->Index].dEndTime = (_double)fEndTime;

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"�ִϸ��̼� ��� �ӵ�");
											break;

										case 4:
											ImGui::Text(u8"���� �� �ε���");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"�ִϸ��̼� ��� �ӵ�");
											break;

										case 8:
											ImGui::Text(u8"x = Right, y = Up, z = Look 1 �Ǵ� -1 �����ÿ�");
											break;

										default:
											ImGui::Text(u8"������ ��ġ");
											break;
										}

										_float4 vPos = eventList[item->Index].vEventPos;
										_float vEventPos[4] = { vPos.x, vPos.y, vPos.z, vPos.w };
										if (ImGui::DragFloat4("##4", vEventPos, 0.01f))
											memcpy(&eventList[item->Index].vEventPos, &vEventPos, sizeof(_float4));


										switch (item->iEventType)
										{
										case 3:
											ImGui::Text(u8"�ݶ��̴��� ���Ⱚ�Դϴ�.");
											break;

										default:
											ImGui::Text(u8"������ ��ġ");
											break;
										}

										_float4 vDir = eventList[item->Index].vEventDir;
										_float vEventDir[4] = { vDir.x, vDir.y, vDir.z, vDir.w };
										if (ImGui::DragFloat4("##EventDirValues", vEventDir, 0.01f))
											memcpy(&eventList[item->Index].vEventDir, &vEventDir, sizeof(_float4));


										switch (item->iEventType)
										{
										case 3:
											ImGui::Text(u8"�ݶ��̴��� ũ���Դϴ�.");
											break;

										default:
											ImGui::Text(u8"Value���� �־��ּ���");
											break;
										}

										_float4 vValues = eventList[item->Index].vEventValues;
										_float vEventValues[4] = { vValues.x, vValues.y, vValues.z, vValues.w };
										if (ImGui::DragFloat4("##EventValueList", vEventValues, 0.01f))
											memcpy(&eventList[item->Index].vEventValues, &vEventValues, sizeof(_float4));

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"�̴� ���� �Է����ּ���");
											break;

										case 4:
											ImGui::Text(u8"���� �� �ε����� �־��ּ���");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"�ִϸ��̼� ��� �ӵ��� �Է��ϼ���");
											break;

										case 8:
											ImGui::Text(u8"�̵����� �־��ּ���");
											break;

										default:
											ImGui::Text(u8"Value���� �־��ּ���");
											break;
										}
										_float fValue = eventList[item->Index].fValue;
										if (ImGui::DragFloat("##EventFValue", &fValue, 0.01f))
											eventList[item->Index].fValue = fValue;

										switch (item->iEventType)
										{
										case 0:
											break;

										case 1:
											break;

										case 2:
											break;

										case 3:
											ImGui::Text(u8"�������� ���̸� �Է����ּ���");
											break;

										case 4:
											ImGui::Text(u8"���� �� �ε����� �־��ּ���");
											break;

										case 5:
											break;

										case 6:
											break;

										case 7:
											ImGui::Text(u8"�ִϸ��̼� ��� �ӵ��� �Է��ϼ���");
											break;

										case 8:
											ImGui::Text(u8"�̵����� �־��ּ���");
											break;

										default:
											ImGui::Text(u8"�߰� Value���� �־��ּ���");
											break;
										}
										_float fAddValue = eventList[item->Index].fAddValue;
										if (ImGui::DragFloat("##EventFAddValue", &fAddValue, 0.01f))
											eventList[item->Index].fAddValue = fAddValue;
										ImGui::EndPopup();
									}
									ImGui::PopID();
								}
							// �̺�Ʈ�� �����Ѵ�.
							pClip->Set_Event(eventList);
							ImGui::EndTable();
						}
						ImGui::TreePop();
					}
#pragma endregion
					// �ִϸ����Ϳ� Ŭ���� �߰��մϴ�.
					if (ImGui::Button("Add Clip"))
					{
						Add_AnimationClip();
						// �ִϸ����� �ʿ��� Ŭ���� ������ ������Ʈ���ش�.
						m_bIsClipUpdate = true;
					}

					ImGui::SameLine();

					if (ImGui::Button("Add AllClip"))
					{
						Add_AllAnimationClip();
						m_bIsClipUpdate = true;
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animator"))
		{
			if (-1 == m_iModelIndex)
				ImGui::Text("Please Select Model");
			else
			{
				if (ImGui::Button("Save Data"))
					Save_Animator();


				if (ImGui::TreeNode("Animator List"))
				{
					ImGui::Text("Animator Component List");
					// ���̳ʸ�ȭ �� �ִϸ��̼� ĳ���� �𵨵��� �����ִ� ����Ʈ�� ����. ( �� �ܰ迡�� ������ �̸��� �ܼ��� ���ϸ��ϻ��� )
					if (ImGui::BeginListBox("Animator Component List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						for (int i = 0; i < m_SavedAnimatorList.size(); i++)
						{
							const bool is_selected = (m_iAnimatorIndex == i);
							if (ImGui::Selectable(CMethod::ConvertWToS(m_SavedAnimatorList[i].strFileName).c_str(), is_selected))
							{
								m_iAnimatorIndex = i;

								// �ִϸ��̼� �׽�Ʈ ���� �ִϸ����͸� �־��ش�..
								Set_Animator(m_SavedAnimatorList[i].strFileName);
								m_bIsClipUpdate = true;
							}
						}
						ImGui::EndListBox();
					}
					ImGui::TreePop();
				}

				ImGui::Text("Merge Into Current Animator");
				ImGui::SameLine();
				if (ImGui::Button("Select"))
				{
					ImGui::OpenPopup("Animator List");
				}

				if (ImGui::BeginPopup("Animator List"))
				{
					ImGui::Text("Select Animator");

					ImGui::SameLine();

					static _string strText = "";
					// �ִϸ��̼� Ŭ���� ���� ���� ����Ʈ
					static vector<FILEDATA> SaveToFindAnimatorList;

					_char INPUT[MAX_PATH] = "";
					strcpy_s(INPUT, strText.c_str());
					_bool bInput = false;
					bInput = ImGui::InputText("Animator", INPUT, sizeof(_char) * MAX_PATH);
					strText = INPUT;

					ImGui::Separator();

					if (!strcmp(strText.c_str(), ""))
					{
						for (int i = 0; i < m_SavedAnimatorList.size(); ++i)
						{
							if (ImGui::Selectable(CMethod::ConvertWToS(m_SavedAnimatorList[i].strFileName).c_str()))
							{
								Merge_Animator(m_SavedAnimatorList[i].strFileName);
							}
						}
					}
					else if (true == bInput)
					{
						SaveToFindAnimatorList.clear();
						for (auto& iter : m_SavedAnimatorList)
						{
							if (CMethod::Is_Same_Text(CMethod::ConvertWToS(iter.strFileName).c_str(), strText.c_str()))
								SaveToFindAnimatorList.push_back(iter);
						}
					}
					else
					{
						for (int i = 0; i < SaveToFindAnimatorList.size(); ++i)
						{
							if (ImGui::Selectable(CMethod::ConvertWToS(SaveToFindAnimatorList[i].strFileName).c_str()))
							{
								Merge_Animator(SaveToFindAnimatorList[i].strFileName);
							}
						}
					}
					ImGui::EndPopup();
				}

				// �������� ����ִٸ� �������� ������ش�.
				m_ClipList = m_pAnimatorCom->Get_AnimationClipList();

				// ���� ������Ʈ�� �����ִ� �ִϸ��̼� Ŭ�� ����Ʈ
				if (ImGui::TreeNode("AnimationClip List"))
				{
					vector<_string> EnterNameList;
					for (auto& clip : m_ClipList)
						EnterNameList.push_back(clip->Get_Name());

					//// Ŭ���� �ٲ������ üũ�Ѵ�.
					//static _bool m_bIsClipUpdate = false;

					const int COLUMNS_COUNT = 9;
					// Ʈ������ ������ ���ڷ� �������� string ����
					string strMeshSize = "All Clips Count : ";
					strMeshSize += CMethod::Convert_Number_S((_uint)m_ClipList.size());
					ImGui::Text(strMeshSize.c_str());

					ImGui::Text("Current Playing Clip : ");

					ImGui::SameLine();

					if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip())
						ImGui::Text("None");
					else
						// ���� ������� �ִϸ��̼� Ŭ���� �̸��� �־��ش�.
						ImGui::Text(m_pAnimatorCom->Get_CurrentAnimationClip()->Get_Name().c_str());

					// �ִϸ��̼��� ���൵�� �޾ƿ´�.
					_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
					if (ImGui::SliderFloat("##Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
					{
						// �ִϸ��̼��� ���൵�� �����Ѵ�.
						static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
					}

					ImGui::SameLine();

					_string strAnimSpeed = "Speed : ";
					strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

					ImGui::Text(strAnimSpeed.c_str());

					ImGui::SameLine();

					// �ִϸ��̼� �ӵ���
					if (ImGui::SmallButton("Set"))
					{
						ImGui::OpenPopup("Set");
					}

					if (ImGui::BeginPopup("Set"))
					{
						ImGui::Text("Set Speed");

						ImGui::SameLine();

						if (ImGui::SmallButton("Reset"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

						ImGui::Separator();

						if (ImGui::Selectable("0.5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

						if (ImGui::Selectable("2x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

						if (ImGui::Selectable("3x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

						if (ImGui::Selectable("4x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

						if (ImGui::Selectable("5x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

						if (ImGui::Selectable("6x Speed"))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

						_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

						if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
							m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

						m_bIsClipUpdate = true;

						ImGui::EndPopup();
					}

					ImGui::Separator();

					// �ִϸ��̼� Ÿ�� 
					static const char* AnimTypes[]{ "IDLE","MOVE","PRE_JUMP","JUMP","ATTACK","AIR_ATTACK","CHARGE_ATTACK","GUARD",
					"THROW","PRE_DASH","DASH","AIR_DASH","PARING","HIT","AIR_HIT","DOWN","AIR_DOWN","STEP","AIR_STEP","PUSH",
					"SKILL_1","SKILL_2","SKILL_3","STRIKE","CHARGE","BOUND","DIE","AWAKE","ULTIMATE","SPIN","PASSIVE","ROLL","BLOW", "TURN", "BOSS_HEAL"
					, "INIT" };

					static ImVector<MyItem> items;
					_int iStart = 0;
					_int iEnd = 0;

					if (items.Size == 0 || true == m_bIsClipUpdate || items.Size != m_ClipList.size())
					{
						// ����� �缳���Ѵ�.
						items.resize((_int)m_ClipList.size(), MyItem());
						for (int n = 0; n < items.Size; n++)
						{
							MyItem& item = items[n];
							// �ε��� ���� ����
							item.Index = n;

							// �̸� �޾ƿ���
							strcpy_s(item.Name, m_ClipList[n]->Get_Name().c_str());

							_string szMainAnimName;
							// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
							if (nullptr == m_ClipList[n]->Get_MainAnimation())
								szMainAnimName = "None";
							else
							{
								iStart = (_int)m_ClipList[n]->Get_MainAnimationName().find("(", 0) + 1;
								iEnd = (_int)m_ClipList[n]->Get_MainAnimationName().find(")", 0) - iStart;
								szMainAnimName = m_ClipList[n]->Get_MainAnimationName().substr(iStart, iEnd);
							}
							// ���� �ִϸ��̼��� �̸� ����
							strcpy_s(item.MainAnim, szMainAnimName.c_str());

							_string szSubAnimName;
							// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
							if (nullptr == m_ClipList[n]->Get_SubAnimation())
								szSubAnimName = "None";
							else
							{
								iStart = (_int)m_ClipList[n]->Get_SubAnimationName().find("(", 0) + 1;
								iEnd = (_int)m_ClipList[n]->Get_SubAnimationName().find(")", 0) - iStart;
								szSubAnimName = m_ClipList[n]->Get_SubAnimationName().substr(iStart, iEnd);
							}
							// ���� Ŭ���� �̸� ����
							strcpy_s(item.SubAnim, szSubAnimName.c_str());

							// �������� �������� �޾ƿ���
							item.Loop = m_ClipList[n]->Is_Loop();

							// ��Ʈ�� ���� �޾ƿ���
							item.RootBone = m_ClipList[n]->Is_RootBone();

							// �ִϸ��̼� Ŭ���� �ӵ�
							item.fAnimSpeed = m_ClipList[n]->Get_AnimationSpeed();

							// �ִϸ��̼��� Ÿ��
							strcpy_s(item.AnimationType, m_ClipList[n]->Get_AnimationType().c_str());
							//for (int i = 0; i < IM_ARRAYSIZE(AnimTypes); ++i)
							//{
								//if ("" == m_ClipList[n]->Get_AnimationType())
								//{
								//	strcpy_s(item.AnimationType, "None");
								//}
								//else
								//{
										//strcpy_s(item.AnimationType, AnimTypes[i]);
									//if (!strcmp(AnimTypes[i], m_ClipList[n]->Get_AnimationType().c_str()))
								//}
							//}

							// Event����
							item.EventCount = (_int)m_ClipList[n]->Get_EventList().size();
						}
						m_bIsClipUpdate = false;
					}

					static _int iClipIndex = -1;

					// Options
					static ImGuiTableFlags flags =
						ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
						| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
						| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

					if (ImGui::BeginTable("Clip_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
					{
						// - ImGuiTableColumnFlags_DefaultSort
						// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
						// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
						ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
						ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterState);
						ImGui::TableSetupColumn("MainAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
						ImGui::TableSetupColumn("SubAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
						ImGui::TableSetupColumn("Loop", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("RootBone", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("Speed", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
						ImGui::TableSetupColumn("AnimType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_AnimationType);
						ImGui::TableSetupColumn("EventCount", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventCount);
						ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
						ImGui::TableHeadersRow();

						// Sort our data if sort specs have been changed!
						if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
							if (sorts_specs->SpecsDirty)
							{
								MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
								// ���� ���� ���ؿ� ���� �����Ѵ�.
								if (items.Size > 1)
									qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
								MyItem::s_current_sort_specs = NULL;
								sorts_specs->SpecsDirty = false;
							}

						// Demonstrate using clipper for large vertical lists
						ImGuiListClipper clipper;
						clipper.Begin(items.Size);
						while (clipper.Step())
							for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
							{
								MyItem* item = &items[row_n];
								ImGui::PushID(item->Index);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();

								// ���õ� Ŭ���� �Ķ������� ó����.
								if (item->Index == iClipIndex)
								{
									ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.4f, 0.7f, 0.5f));
									ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
								}

								// Index
								ImGui::Text(CMethod::Convert_Number_S(item->Index).c_str());
								ImGui::TableNextColumn();

								// Clip�� �̸� �����ϱ�
								if (ImGui::Selectable(item->Name))
								{
									iClipIndex = item->Index;
									m_bIsClipUpdate = true;
									m_bIsEventUpdate = true;
								}
								ImGui::TableNextColumn();

								// ���� �ִϸ��̼� �̸�
								ImGui::Text(item->MainAnim);
								ImGui::TableNextColumn();

								// ���� �ִϸ��̼� �̸�
								ImGui::Text(item->SubAnim);
								ImGui::TableNextColumn();

								// Loop ����
								_bool isLoop = item->Loop;
								ImGui::SetNextItemWidth(-FLT_MIN);
								ImGui::PushID(0);
								if (ImGui::Checkbox("", &isLoop))
								{
									item->Loop = isLoop;
									m_ClipList[item->Index]->Set_Loop(isLoop);
								}
								ImGui::PopID();

								ImGui::TableNextColumn();
								// RootBone ����
								ImGui::PushID(1);
								_bool isRootBone = item->RootBone;
								ImGui::SetNextItemWidth(-FLT_MIN);
								if (ImGui::Checkbox("", &isRootBone))
								{
									item->RootBone = isRootBone;
									m_ClipList[item->Index]->Set_RootBone(isRootBone);
								}
								ImGui::TableNextColumn();
								ImGui::PopID();

								// �ִϸ��̼� �ӵ�
								if (ImGui::Selectable(CMethod::Convert_Number_S(item->fAnimSpeed, 1).c_str()))
									ImGui::OpenPopup("AnimSpeed");

								if (ImGui::BeginPopup("AnimSpeed"))
								{
									ImGui::Text("Adjust AnimSpeed");
									ImGui::SameLine();
									if (ImGui::SmallButton("Reset"))
									{
										m_ClipList[item->Index]->Set_AnimationSpeed(1.f);
										m_bIsClipUpdate = true;
									}
									ImGui::Separator();

									_float fSpeed = item->fAnimSpeed;
									if (ImGui::DragFloat("##15", &fSpeed, 0.1f))
									{
										m_ClipList[item->Index]->Set_AnimationSpeed(fSpeed);
										m_bIsClipUpdate = true;
									}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();

								char AnimationType[MAX_PATH] = "##";

								strcat_s(AnimationType, item->AnimationType);
								// �ִϸ��̼��� Ÿ��
								if (ImGui::Selectable(item->AnimationType))
									ImGui::OpenPopup("AnimType");

								if (ImGui::BeginPopup("AnimType"))
								{
									ImGui::Text("Choose AnimType");
									ImGui::SameLine();
									if (ImGui::SmallButton(u8"�����"))
									{
										m_ClipList[item->Index]->Set_AnimationType(_string(""));
										m_bIsClipUpdate = true;
									}

									char strAnimType[MAX_PATH] = "";
									
									strcpy_s(strAnimType, m_strAnimType.c_str());

									ImGui::Text(u8"Ÿ�� �Է�â�� �ǵ��� �ʹ� ���� ������� ������");
									
									// �ݶ��̴� Ÿ�� ��ǲâ
									if(ImGui::InputText("##AnimTypeInput", strAnimType, sizeof(_char) * MAX_PATH))
										m_strAnimType = strAnimType;	

									ImGui::SameLine();

									// �ݶ��̴� Ÿ���� �ִ´�.
									if (true == ImGui::SmallButton("Input"))
									{
										m_ClipList[item->Index]->Set_AnimationType(strAnimType);																		
										m_bIsClipUpdate = true;
									}

									ImGui::Separator();

									for (_uint i = 0; i < IM_ARRAYSIZE(AnimTypes); ++i)
									{
										if (ImGui::Selectable(AnimTypes[i]))
										{
											m_ClipList[item->Index]->Set_AnimationType(AnimTypes[i]);
											m_bIsClipUpdate = true;
										}
									}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();

								// �̺�Ʈ ����
								ImGui::Text(CMethod::Convert_Number_S(item->EventCount).c_str());

								// �ִϸ��̼� Ŭ�� ����Ʈ�� ������Ʈ�Ѵ�.
								m_pAnimatorCom->Set_AnimationClipList(m_ClipList);

								ImGui::PopID();
							}
						ImGui::EndTable();
					}
					ImGui::Text("Selected Clip : ");

					ImGui::SameLine();

					if (-1 != iClipIndex)
						ImGui::Text(m_ClipList[iClipIndex]->Get_Name().c_str());
					else
						ImGui::Text("Please Select Clip");

					ImGui::SameLine();

					if (ImGui::Button("Delete"))
					{
						if (-1 != iClipIndex)
						{
							Delete_AnimationClip(m_ClipList[iClipIndex]->Get_Name());
							iClipIndex = -1;
							m_ClipList = m_pAnimatorCom->Get_AnimationClipList();
							m_bIsClipUpdate = true;
						}
					}

					// ������ Ŭ���� �̸��� ���� Ŭ������ �����Ѵ�.
					if (ImGui::Button("Set_Clip"))
						if (-1 != iClipIndex)
							m_pAnimatorCom->Set_CurrentAnimationClip(m_pAnimatorCom->Get_AnimationClip(items[iClipIndex].Name));

					ImGui::SameLine();

					if (ImGui::Button("Play"))
						m_pAnimatorCom->Set_Play(true);

					if (nullptr != m_pAnimObject)
					{
						_vector vPos = m_pAnimObject->Get_Transform()->Get_Pos();

						ImGui::Text("Test Model Pos [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetX(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetY(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(XMVectorGetZ(vPos), 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");

						_float4 vRootRight = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_RIGHT);

						_float4 vRootUp = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_UP);

						_float4 vRootLook = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_Model()->
							Get_HierarchyNodes("Root")->Get_CombinedTransformationMatrix(), MATROW_LOOK);

						ImGui::Text("Root Bone Look [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootRight.x, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootUp.y, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootLook.z, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");

						
						_float4 vRootMatrixRight = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_RIGHT);
						_float4 vRootMatrixUp = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_UP);
						_float4 vRootMatrixLook = CMethod::XMMatrixGetFloat4(m_pAnimatorCom->Get_RootBoneMatrix(), MATROW_LOOK);

						ImGui::Text("Root Matrix [ x: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixRight.x, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / y: ");
						ImGui::SameLine();
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixUp.y, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" / z: ");
						ImGui::SameLine();
						ImGui::Text(CMethod::Convert_Number_S(vRootMatrixLook.z, 2).c_str());
						ImGui::SameLine();
						ImGui::Text(" ]");
					}

#pragma region �ִϸ����Ͱ� ����ִ� �� ������
					if (ImGui::TreeNode("Animator WeaponList"))
					{
						static _int iWeaponBoneIndex = -1;

						vector<CHierarchyNode*> weaponBoneList = m_pAnimatorCom->Get_WeaponBoneList();

						if (-1 != iWeaponBoneIndex)
						{
							_string strWeaponBone = "Current Bone : ";
							strWeaponBone += weaponBoneList[iWeaponBoneIndex]->Get_Name();
							ImGui::Text(strWeaponBone.c_str());
						}

						for (int i = 0; i < m_pAnimatorCom->Get_WeaponBoneList().size(); ++i)
						{
							if (ImGui::Selectable(weaponBoneList[i]->Get_Name().c_str()))
								iWeaponBoneIndex = i;
						}

						if (ImGui::Button("Remove WeaponBone"))
						{
							if (-1 != iWeaponBoneIndex)
							{
								m_pAnimatorCom->Remove_WeaponBone(iWeaponBoneIndex);
								iWeaponBoneIndex = -1;
							}
						}
						ImGui::TreePop();
					}
#pragma endregion

#pragma region ���� ���õ� Ŭ���� �̺�Ʈ ���
					if (-1 != iClipIndex)
					{
						if (ImGui::TreeNode("Current Clip Events"))
						{
							static _int iSelectEvent = -1;

							// �ִϸ����� ������Ʈ�� �����ִ� Test Clip�� �޾ƿ´�.
							CAnimationClip* pClip = m_pAnimatorCom->Get_AnimationClip(items[iClipIndex].Name);
							if (nullptr == pClip)
								return;

							CModel* pModel = m_pAnimatorCom->Get_Model();
							if (nullptr == pModel)
								return;

							// �̺�Ʈ ����� �޾ƿ´�.
							vector<CAnimationClip::ANIMEVENTDESC> eventList = pClip->Get_EventList();

							const int eventColumn_COUNT = 5;
							// ���� Ŭ���� ���� �̺�Ʈ�� ����
							string strEventSize = "All Event Count : ";
							strEventSize += CMethod::Convert_Number_S((_uint)eventList.size());
							ImGui::Text(strEventSize.c_str());

							ImGui::SameLine();

							if (ImGui::Button("Add Event"))
							{
								CAnimationClip::ANIMEVENTDESC animDesc;
								// ���� Ratio�� ��´�.
								animDesc.dStartRatio = pClip->Get_Ratio();
								// �̺�Ʈ ����Ʈ�� push_back �Ѵ�.
								eventList.push_back(animDesc);
								m_bIsClipUpdate = true;
								m_bIsEventUpdate = true;
							}

							ImGui::SameLine();

							if (ImGui::Button("Remove Event"))
							{
								if (iSelectEvent <= eventList.size())
								{
									// ����Ʈ���� �����.
									eventList.erase(eventList.begin() + iSelectEvent);

									// -1�� ������ش�.
									iSelectEvent = -1;
									m_bIsClipUpdate = true;
									m_bIsEventUpdate = true;
								}
							}

							static ImVector<MyItem> eventItems;
							// �������� ����ִٸ� �������� ������ش�.
							if (eventItems.Size == 0 || true == m_bIsEventUpdate || pClip->Get_EventList().size() != eventItems.Size)
							{
								// �̺�Ʈ ����Ʈ�� ������� ����� �缳���Ѵ�.
								eventItems.resize((_int)eventList.size(), MyItem());
								for (int n = 0; n < eventItems.Size; n++)
								{
									MyItem& item = eventItems[n];
									// �ִϸ��̼� Ŭ���� �ε��� ���� ����
									item.Index = n;
									// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
									iStart = (_int)eventList[n].strName.find("(", 0) + 1;
									iEnd = (_int)eventList[n].strName.find(")", 0) - iStart;
									_string szEventName = eventList[n].strName.substr(iStart, iEnd);
									// �ִϸ��̼� Ŭ���� �̸� ����
									strcpy_s(item.Name, szEventName.c_str());
									// �̺�Ʈ Ÿ��
									item.iEventType = (_uint)eventList[n].tEventType;
									// �̺�Ʈ ���� ����
									item.dStartRatio = eventList[n].dStartRatio;
									// �̺�Ʈ ���� ����
									item.dEndRatio = eventList[n].dEndRatio;
									// �̺�Ʈ ���� �ð�
									item.dEndTime = eventList[n].dEndTime;
									// ���� ��ġ 
									item.vEventPos = eventList[n].vEventPos;
									// ���� �̸����� ��� �̺�Ʈ �ؽ�Ʈ
									strcpy_s(item.EventText, eventList[n].strEventText.c_str());
								}

								m_bIsEventUpdate = false;
							}

							static const char* eventTypes[] =
							{
								"Effect","UI", "Sound", "Collider", "WeaponBone", "Camera", "Hide", "SpeedSection","BoneTransform","Gravity","TimeCheck","Trail","Not_Selected"
							};

							// Options
							static ImGuiTableFlags flags =
								ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
								| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
								| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

							if (ImGui::BeginTable("Event_Table_Animator", eventColumn_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
							{
								// - ImGuiTableColumnFlags_DefaultSort
								// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
								// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
								ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Index);
								ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort, 0.0f, MyItemColumnID_Name);
								ImGui::TableSetupColumn("EventText", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
								ImGui::TableSetupColumn("EventType", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EventType);
								ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
								ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
								ImGui::TableHeadersRow();

								// Sort our data if sort specs have been changed!
								if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
									if (sorts_specs->SpecsDirty)
									{
										MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
										// ���� ���� ���ؿ� ���� �����Ѵ�.
										if (eventItems.Size > 1)
											qsort(&eventItems[0], (size_t)eventItems.Size, sizeof(eventItems[0]), MyItem::CompareWithSortSpecs);
										MyItem::s_current_sort_specs = NULL;
										sorts_specs->SpecsDirty = false;
									}

								// Demonstrate using clipper for large vertical lists
								ImGuiListClipper clipper;
								clipper.Begin(eventItems.Size);
								while (clipper.Step())
									for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
									{
										MyItem* item = &eventItems[row_n];
										ImGui::PushID(item->Index);
										ImGui::TableNextRow();
										ImGui::TableNextColumn();

										// ���õƴٸ� �Ķ������� �׸���.
										if (iSelectEvent == item->Index)
										{
											ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.3f, 0.5f));
											ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
										}

										// Index
										if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
										{
											iSelectEvent = item->Index;
										}
										ImGui::TableNextColumn();
										// Name
										char strText[MAX_PATH] = "";
										strcpy_s(strText, item->Name);
										ImGui::SetNextItemWidth(-FLT_MIN);
										if (ImGui::InputText("##EventName", strText, sizeof(char) * MAX_PATH))
										{
											strcpy_s(item->Name, strText);
											eventList[item->Index].strName = strText;
										}
										ImGui::TableNextColumn();

										static _string strInput;
										static vector<CHierarchyNode*> BoneList;
										static vector<_string> ColliderList;

										const char* colliderTypes[] =
										{
											"HIT_FRONT","HIT_LEFT","HIT_RIGHT","HIT_UP","STRIKE","BLOW",
											"SPIN","HIT_UPPER","HIT_DOWN","SKILL_1","SKILL_2","SKILL_3",
											"PUSH","THROW","ULTIMATE","HIT_AWAKE"
										};

										if (ImGui::Selectable(item->EventText))
											ImGui::OpenPopup("##EventText");

										if (ImGui::BeginPopup("##EventText"))
										{
											if (3 == item->iEventType)
												ImGui::Text(u8"�ݶ��̴� Ÿ���� ����ּ���");
											else
												ImGui::Text(u8"�� �̸��� �Է��ϼ���");

											if (ImGui::SmallButton("Reset"))
											{
												eventList[item->Index].strEventText = "";
												m_bIsEventUpdate = true;
											}


											_char INPUT[MAX_PATH] = "";
											strcpy_s(INPUT, strInput.c_str());
											_bool bInput = false;
											bInput = ImGui::InputText("FindText", INPUT, sizeof(_char) * MAX_PATH);
											strInput = INPUT;

											// �ݶ��̴��� ���
											if (3 == item->iEventType)
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : colliderTypes)
													{
														if (ImGui::Selectable(iter))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													ColliderList.clear();
													for (auto& iter : colliderTypes)
													{
														if (CMethod::Is_Same_Text(iter, strInput.c_str()))
															ColliderList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : ColliderList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
											}
											// ������ ���
											else if (2 == item->iEventType)
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : m_SoundNameList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													m_ResearchSoundList.clear();
													for (auto& iter : m_SoundNameList)
													{
														if (CMethod::Is_Same_Text(iter, strInput))
															m_ResearchSoundList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : m_ResearchSoundList)
													{
														if (ImGui::Selectable(iter.c_str()))
														{
															eventList[item->Index].strEventText = iter;
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}												
											}
											else
											{
												if (!strcmp(strInput.c_str(), ""))
												{
													for (auto& iter : pModel->Get_HierachyNodeList())
													{
														if (ImGui::Selectable(iter->Get_Name().c_str()))
														{
															eventList[item->Index].strEventText = iter->Get_Name();
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
												else if (true == bInput)
												{
													BoneList.clear();
													for (auto& iter : pModel->Get_HierachyNodeList())
													{
														if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strInput.c_str()))
															BoneList.push_back(iter);
													}
												}
												else
												{
													for (auto& iter : BoneList)
													{
														if (ImGui::Selectable(iter->Get_Name().c_str()))
														{
															eventList[item->Index].strEventText = iter->Get_Name();
															m_bIsClipUpdate = true;
															m_bIsEventUpdate = true;
														}
													}
												}
											}

											ImGui::EndPopup();
										}
										ImGui::TableNextColumn();

										// 4��°���� ��� ( ���� ���� ) NoneSelect�� �����.
										if (4 == item->Index)
										{
											if (IM_ARRAYSIZE(eventTypes) - 1 != item->iEventType)
											{
												eventList[item->Index].tEventType = CAnimationClip::ANIMEVENTTYPE(IM_ARRAYSIZE(eventTypes) - 1);
												m_bIsEventUpdate = true;
											}
										}

										// Event
										if (ImGui::Selectable(eventTypes[item->iEventType]))
											ImGui::OpenPopup("EventType");

										if (ImGui::BeginPopup("EventType"))
										{
											for (int i = 0; i < IM_ARRAYSIZE(eventTypes); ++i)
											{
												if (ImGui::Selectable(eventTypes[i]))
												{
													eventList[item->Index].tEventType = (CAnimationClip::ANIMEVENTTYPE)i;
													item->iEventType = i;
													m_bIsClipUpdate = true;
													m_bIsEventUpdate = true;
												}
											}
											ImGui::EndPopup();
										}
										ImGui::TableNextColumn();

										// Value ����
										if (ImGui::Selectable("Values"))
											ImGui::OpenPopup("Values");

										if (ImGui::BeginPopup("Values"))
										{
											ImGui::Text(u8"�Ⱦ��� ���� ���θ� ��");

											ImGui::SameLine();

											if (ImGui::SmallButton("Reset"))
											{
												eventList[item->Index].dStartRatio = pClip->Get_Ratio();
												eventList[item->Index].dEndRatio = 0.0;
												eventList[item->Index].dEndTime = 0.0;
												eventList[item->Index].vEventPos = _float4(0.f, 0.f, 0.f, 1.f);
												eventList[item->Index].fValue = 0.f;
												eventList[item->Index].fAddValue = 0.f;
												m_bIsEventUpdate = true;
											}

											ImGui::Separator();

											ImGui::Text(u8"��������");

											_float fRatio = (_float)eventList[item->Index].dStartRatio;
											if (ImGui::DragFloat("##StartRatio", &fRatio, 0.01f))
											{
												eventList[item->Index].dStartRatio = fRatio;
												m_bIsEventUpdate = true;
											}

											ImGui::SameLine();

											if (ImGui::Button("CurTime"))
											{
												eventList[item->Index].dStartRatio = pClip->Get_Ratio();
												m_bIsEventUpdate = true;
											}

											ImGui::Text(u8"��������");

											_float fEndRatio = (_float)eventList[item->Index].dEndRatio;
											if (ImGui::DragFloat("##EndRatio", &fEndRatio, 0.01f))
											{
												eventList[item->Index].dEndRatio = fEndRatio;
												m_bIsEventUpdate = true;
											}

											ImGui::Text(u8"����ð�");

											_float fEndTime = (_float)eventList[item->Index].dEndTime;
											if (ImGui::DragFloat("##EndTime", &fEndTime, 0.01f))
											{
												eventList[item->Index].dEndTime = (_double)fEndTime;
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"�ݶ��̴��� ��ġ���Դϴ�.");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_WEAPONBONE:
												ImGui::Text(u8"���� �� �ε���");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_SPEEDSECTION:
												ImGui::Text(u8"�ִϸ��̼� ��� �ӵ�");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_BONETRANSFORM:
												ImGui::Text(u8"x = Right, y = Up, z = Look 1 �Ǵ� -1 �����ÿ�");
												break;
											default:
												ImGui::Text(u8"�̺�Ʈ�� ��ġ ��");
												break;
											}

											_float4 vPos = eventList[item->Index].vEventPos;
											_float vEventPos[4] = { vPos.x, vPos.y, vPos.z, vPos.w };
											if (ImGui::DragFloat4("##SpawnPos", vEventPos, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventPos, &vEventPos, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"x = ������ , y = ���� �ð� , z = ����ũ �ð� w = ����ũ ����");
												break;
											default:
												ImGui::Text(u8"�̺�Ʈ�� Dir��");
												break;
											}

											_float4 vDir = eventList[item->Index].vEventDir;
											_float vEventDir[4] = { vDir.x, vDir.y, vDir.z, vDir.w };
											if (ImGui::DragFloat4("##EventDir", vEventDir, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventDir, &vEventDir, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"�ݶ��̴��� ũ���Դϴ�.");
												break;
											default:
												ImGui::Text(u8"AddValue���� �ʿ信 ���� �־��ּ���");
												break;
											}
											
											_float4 vValues = eventList[item->Index].vEventValues;
											_float vEventValues[4] = { vValues.x, vValues.y, vValues.z, vValues.w };
											if (ImGui::DragFloat4("##EventValues", vEventValues, 0.01f))
											{
												memcpy(&eventList[item->Index].vEventValues, &vEventValues, sizeof(_float4));
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_SOUND:
												ImGui::Text(u8"������ ũ�⸦ 0 ~ 1 ������ ������ �־��ּ���");
												break;
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"�˹� ��ġ�� �Է����ּ���");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_WEAPONBONE:
												ImGui::Text(u8"���� �� �ε����� �־��ּ���");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_SPEEDSECTION:
												ImGui::Text(u8"�ִϸ��̼� ��� �ӵ��� �Է��ϼ���");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_BONETRANSFORM:
												ImGui::Text(u8"�̵����� �־��ּ���");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_TRAIL:
												ImGui::Text(u8"Ʈ���� ���۰��� �־��ּ���");
												break;

											default:
												ImGui::Text(u8"Value���� �־��ּ���");
												break;
											}
											_float fValue = eventList[item->Index].fValue;
											if (ImGui::DragFloat("##EventValue", &fValue, 0.01f))
											{
												eventList[item->Index].fValue = fValue;
												m_bIsEventUpdate = true;
											}

											switch (item->iEventType)
											{
											case CLIPEVENTTYPE::EVENTTYPE_COLLIDER:
												ImGui::Text(u8"�������� ���̸� �Է����ּ���");
												break;

											case CLIPEVENTTYPE::EVENTTYPE_TRAIL:
												ImGui::Text(u8"Ʈ���� ������ �־��ּ���");
												break;

											default:
												ImGui::Text(u8"AddValue���� �ʿ信 ���� �־��ּ���");
												break;
											}
											_float fAddValue = eventList[item->Index].fAddValue;
											if (ImGui::DragFloat("##EventAddValue", &fAddValue, 0.01f))
											{
												eventList[item->Index].fAddValue = fAddValue;
												m_bIsEventUpdate = true;
											}
											ImGui::EndPopup();
										}
										ImGui::PopID();
									}
								// �̺�Ʈ�� �����Ѵ�.
								pClip->Set_Event(eventList);
								ImGui::EndTable();
							}
							ImGui::TreePop();
						}
					}
#pragma endregion
					ImGui::TreePop();
				}

				// Ʈ������ ����Ʈ.
				if (ImGui::TreeNode("Transition List"))
				{
					vector<CAnimator::TRANSITION>transitionList = m_pAnimatorCom->Get_TransitionList();

					vector<_string> EnterNameList;
					for (auto& clip : m_ClipList)
						EnterNameList.push_back(clip->Get_Name());

					const int COLUMNS_COUNT = 5;
					// Ʈ������ ������ ���ڷ� �������� string ����
					string strMeshSize = "All Transitions Count : ";
					strMeshSize += CMethod::Convert_Number_S((_uint)transitionList.size());
					ImGui::Text(strMeshSize.c_str());

					ImGui::SameLine();

					static _bool isTransitionChange = false;

					if (ImGui::Button("Add Transition"))
					{
						m_pAnimatorCom->Make_Transition();
						transitionList = m_pAnimatorCom->Get_TransitionList();
						isTransitionChange = true;
					}

					static ImVector<MyItem> items;
					_int iStart = 0;
					_int iEnd = 0;

					if (items.Size == 0 || true == isTransitionChange || items.Size != transitionList.size())
					{
						// ����� �缳���Ѵ�.
						items.resize((_int)transitionList.size(), MyItem());
						for (int n = 0; n < items.Size; n++)
						{
							MyItem& item = items[n];
							// �ε��� ���� ����
							item.Index = n;

							// EnterState
							item.EnterState = (_int)transitionList[n].tEnterState;

							_string szCurAnimName;
							// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
							if (nullptr == transitionList[n].pEnterClip)
								szCurAnimName = "None";
							else
							{
								iStart = (_int)transitionList[n].pEnterClip->Get_Name().find("(", 0) + 1;
								iEnd = (_int)transitionList[n].pEnterClip->Get_Name().find(")", 0) - iStart;
								szCurAnimName = transitionList[n].pEnterClip->Get_Name().substr(iStart, iEnd);
							}
							// ���� Ŭ���� �̸� ����
							strcpy_s(item.EnterAnim, szCurAnimName.c_str());

							_string szNextAnimName;
							// ( ) �ȿ� �ִ� ���ڸ� �޾ƿ���
							if (nullptr == transitionList[n].pExitClip)
								szNextAnimName = "None";
							else
							{
								iStart = (_int)transitionList[n].pExitClip->Get_Name().find("(", 0) + 1;
								iEnd = (_int)transitionList[n].pExitClip->Get_Name().find(")", 0) - iStart;
								szNextAnimName = transitionList[n].pExitClip->Get_Name().substr(iStart, iEnd);
							}
							// ���� Ŭ���� �̸� ����
							strcpy_s(item.ExitAnim, szNextAnimName.c_str());

							// Enter Clip�� ���� ���� ����
							item.EnterRatio = transitionList[n].dEnterRatio;

							// Exit Clip�� ���� ���� ����
							item.ExitRatio = transitionList[n].dExitRatio;

							// Ʈ���Ÿ� �ν��ϴ� ����
							item.TriggerRatio = transitionList[n].InputTrigger.dAnimRatio;

							// Ʈ������ �̸�
							strcpy_s(item.Trigger, transitionList[n].InputTrigger.strInput.c_str());

							// ���� �ӵ�
							item.LerpSpeed = (_float)transitionList[n].dLerpSpeed;
						}
						isTransitionChange = false;
					}

					// Options
					static ImGuiTableFlags flags =
						ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
						| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
						| ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX;

					if (ImGui::BeginTable("Transition_Table_Sorting", COLUMNS_COUNT, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 15), 0.0f))
					{
						// - ImGuiTableColumnFlags_DefaultSort
						// - ImGuiTableColumnFlags_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
						// - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
						ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.f, MyItemColumnID_Index);
						ImGui::TableSetupColumn("EnterState", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterState);
						ImGui::TableSetupColumn("EnterAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_EnterClip);
						ImGui::TableSetupColumn("ExitAnim", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_ExitClip);
						ImGui::TableSetupColumn("Trigger", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Trigger);
						ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
						ImGui::TableHeadersRow();

						// Sort our data if sort specs have been changed!
						if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
							if (sorts_specs->SpecsDirty)
							{
								MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
								// ���� ���� ���ؿ� ���� �����Ѵ�.
								if (items.Size > 1)
									qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
								MyItem::s_current_sort_specs = NULL;
								sorts_specs->SpecsDirty = false;
							}

						static _bool isDelete = false;
						static _int	 iDeleteIndex = -1;

						// Demonstrate using clipper for large vertical lists
						ImGuiListClipper clipper;
						clipper.Begin(items.Size);
						while (clipper.Step())
							for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
							{
								MyItem* item = &items[row_n];
								ImGui::PushID(item->Index);
								ImGui::TableNextRow();
								ImGui::TableNextColumn();

								if (-1 != m_iTransitionIndex)
								{
									if (item->Index == m_iTransitionIndex)
									{
										ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0.f, 0.4f, 0.7f, 0.5f));
										ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, cell_bg_color);
									}
								}

								// Index								
								if (ImGui::Selectable(CMethod::Convert_Number_S(item->Index).c_str()))
									m_iTransitionIndex = item->Index;

								ImGui::TableNextColumn();

								// EnterState
								const char* enterStates[] = { "ENTRY", "ANYSTATE", "DEFAULT", "NONE_SELECT" };

								int iSelectedState = item->EnterState;

								if (ImGui::Selectable(enterStates[iSelectedState]))
									ImGui::OpenPopup("EnterState");

								if (ImGui::BeginPopup("EnterState"))
								{
									ImGui::Text("Select EnterState");
									ImGui::Separator();
									for (int i = 0; i < IM_ARRAYSIZE(enterStates); i++)
										if (ImGui::Selectable(enterStates[i]))
										{
											item->EnterState = i;
											transitionList[item->Index].tEnterState = CAnimator::ENTERSTATE(i);
										}
									ImGui::EndPopup();
								}
								ImGui::TableNextColumn();


								ImGui::PushID("##1");
								// EnterAnimation �����ϱ�
								if (ImGui::Selectable(item->EnterAnim))
									ImGui::OpenPopup("EnterClip");

								if (ImGui::BeginPopup("EnterClip"))
								{
									ImGui::Text("Select EnterClip");

									ImGui::SameLine();

									if (ImGui::SmallButton("Unselect"))
									{
										transitionList[item->Index].pEnterClip = nullptr;
										strcpy_s(item->EnterAnim, "None");
									}

									static _string strText = "";
									// �ִϸ��̼� Ŭ���� ���� ���� ����Ʈ
									static vector<CAnimationClip*> SaveToFindClipList;

									_char INPUT[MAX_PATH] = "";
									strcpy_s(INPUT, strText.c_str());
									_bool bInput = false;
									bInput = ImGui::InputText("Anim", INPUT, sizeof(_char) * MAX_PATH);
									strText = INPUT;

									ImGui::Separator();

									if (!strcmp(strText.c_str(), ""))
									{
										for (int i = 0; i < m_ClipList.size(); ++i)
										{
											if (ImGui::Selectable(m_ClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)m_ClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)m_ClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = m_ClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pEnterClip = m_ClipList[i];
												strcpy_s(item->EnterAnim, strClipName.c_str());
											}
										}
									}
									else if (true == bInput)
									{
										SaveToFindClipList.clear();
										for (auto& iter : m_ClipList)
										{
											if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strText.c_str()))
												SaveToFindClipList.push_back(iter);
										}
									}
									else
									{
										for (int i = 0; i < SaveToFindClipList.size(); ++i)
										{
											if (ImGui::Selectable(SaveToFindClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)SaveToFindClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)SaveToFindClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = SaveToFindClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pEnterClip = SaveToFindClipList[i];
												strcpy_s(item->EnterAnim, strClipName.c_str());
											}
										}
									}
									ImGui::EndPopup();
								}
								ImGui::PopID();
								ImGui::TableNextColumn();

								// ExitClip �ε����� ���ϱ�

								ImGui::PushID("##2");
								// ExitAnimation �����ϱ�
								if (ImGui::Selectable(item->ExitAnim))
								{
									ImGui::OpenPopup("ExitClip");
								}

								if (ImGui::BeginPopup("ExitClip"))
								{
									ImGui::Text("Select ExitClip");

									ImGui::SameLine();

									if (ImGui::SmallButton("Unselect"))
									{
										transitionList[item->Index].pExitClip = nullptr;
										strcpy_s(item->ExitAnim, "None");
									}

									static _string strText = "";
									// �ִϸ��̼� Ŭ���� ���� ���� ����Ʈ
									static vector<CAnimationClip*> SaveToFindClipList;

									_char INPUT[MAX_PATH] = "";
									strcpy_s(INPUT, strText.c_str());
									_bool bInput = false;
									bInput = ImGui::InputText("Anim", INPUT, sizeof(_char) * MAX_PATH);
									strText = INPUT;

									ImGui::Separator();

									if (!strcmp(strText.c_str(), ""))
									{
										for (int i = 0; i < m_ClipList.size(); ++i)
										{
											if (ImGui::Selectable(m_ClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)m_ClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)m_ClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = m_ClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pExitClip = m_ClipList[i];
												strcpy_s(item->ExitAnim, strClipName.c_str());
											}
										}
									}
									else if (true == bInput)
									{
										SaveToFindClipList.clear();
										for (auto& iter : m_ClipList)
										{
											if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strText.c_str()))
												SaveToFindClipList.push_back(iter);
										}
									}
									else
									{
										for (int i = 0; i < SaveToFindClipList.size(); ++i)
										{
											if (ImGui::Selectable(SaveToFindClipList[i]->Get_Name().c_str()))
											{
												_int iStart = (_int)SaveToFindClipList[i]->Get_Name().find("(", 0) + 1;
												_int iEnd = (_int)SaveToFindClipList[i]->Get_Name().find(")", 0) - iStart;
												string strClipName = SaveToFindClipList[i]->Get_Name().substr(iStart, iEnd);

												transitionList[item->Index].pExitClip = SaveToFindClipList[i];
												strcpy_s(item->ExitAnim, strClipName.c_str());
											}
										}
									}
									ImGui::EndPopup();
								}
								ImGui::PopID();
								ImGui::TableNextColumn();

								// InputTrigger ���� ä���ش�.
								ImGui::SetNextItemWidth(-FLT_MIN);
								if (ImGui::InputText("", item->Trigger, sizeof(item->Trigger)))
								{
									transitionList[item->Index].InputTrigger.strInput = item->Trigger;
								}

								ImGui::PopID();
							}
						ImGui::EndTable();

						if (-1 != m_iTransitionIndex)
						{
							ImGui::Text("Current Clip : ");

							ImGui::SameLine();

							if (nullptr == m_pAnimatorCom->Get_CurrentAnimationClip())
								ImGui::Text("None Clip");
							else
								ImGui::Text(m_pAnimatorCom->Get_CurrentAnimationClip()->Get_Name().c_str());

							ImGui::SameLine();

							// �ش� Ʈ���������� �̵��Ѵ�.
							if (ImGui::Button("Go"))
							{
								if (nullptr != transitionList[m_iTransitionIndex].pEnterClip)
									m_pAnimatorCom->Set_CurrentAnimationClip(transitionList[m_iTransitionIndex].pEnterClip);
							}

							ImGui::SameLine();

							if (ImGui::ArrowButton("##Up", ImGuiDir_Up))
							{
								if (-1 < m_iTransitionIndex - 1)
								{
									_uint iIndex = m_iTransitionIndex;

									CAnimator::TRANSITION transition = transitionList[iIndex];

									transitionList.erase(transitionList.begin() + iIndex);
									transitionList.insert(transitionList.begin() + iIndex - 1, transition);
									if (-1 < m_iTransitionIndex - 1)
										m_iTransitionIndex -= 1;
									isTransitionChange = true;
								}
							}

							ImGui::SameLine();

							if (ImGui::ArrowButton("##Down", ImGuiDir_Down))
							{
								if (transitionList.size() > m_iTransitionIndex + 1)
								{
									_uint iIndex = m_iTransitionIndex;

									CAnimator::TRANSITION transition = transitionList[iIndex];

									transitionList.erase(transitionList.begin() + iIndex);
									transitionList.insert(transitionList.begin() + iIndex + 1, transition);
									if (transitionList.size() > m_iTransitionIndex + 1)
										m_iTransitionIndex += 1;
									isTransitionChange = true;
								}
							}

							// �ִϸ��̼��� ���൵�� �޾ƿ´�.
							_float fRatio = (_float)m_pAnimatorCom->Get_Ratio();
							if (ImGui::SliderFloat("##Animation Progress Ratio", &fRatio, 0.0f, 1.0f))
							{
								// �ִϸ��̼��� ���൵�� �����Ѵ�.
								static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_ProgressRatio(fRatio);
							}

							ImGui::SameLine();

							_string strAnimSpeed = "Speed : ";
							strAnimSpeed += CMethod::Convert_Number_S(m_pAnimatorCom->Get_AnimationSpeed(), 2);

							ImGui::Text(strAnimSpeed.c_str());

							ImGui::SameLine();

							// �ִϸ��̼� �ӵ���
							if (ImGui::SmallButton("Set"))
							{
								ImGui::OpenPopup("Set");
							}

							if (ImGui::BeginPopup("Set"))
							{
								ImGui::Text("Set Speed");

								ImGui::SameLine();

								if (ImGui::SmallButton("Reset"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(1.f);

								ImGui::Separator();

								if (ImGui::Selectable("0.5x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(0.5f);

								if (ImGui::Selectable("2x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(2.f);

								if (ImGui::Selectable("3x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(3.f);

								if (ImGui::Selectable("4x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(4.f);

								if (ImGui::Selectable("5x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(5.f);

								if (ImGui::Selectable("6x Speed"))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(6.f);

								_float fSpeed = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_FastValue();

								if (ImGui::DragFloat("Current Speed", &fSpeed, 0.1f, 0.f, 10.f))
									m_pAnimatorCom->Get_CurrentAnimationClip()->Set_AnimationSpeed(fSpeed);

								m_bIsClipUpdate = true;

								ImGui::EndPopup();
							}

							ImGui::Separator();

							_float fEnterRatio = (_float)transitionList[m_iTransitionIndex].dEnterRatio;
							if (ImGui::DragFloat("EnterRatio", &fEnterRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dEnterRatio = (_double)fEnterRatio;

							_float fExitRatio = (_float)transitionList[m_iTransitionIndex].dExitRatio;
							if (ImGui::DragFloat("ExitRatio", &fExitRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dExitRatio = (_double)fExitRatio;

							_float fChangeRatio = (_float)transitionList[m_iTransitionIndex].dChangeRatio;
							if (ImGui::DragFloat("ChangeRatio", &fChangeRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dChangeRatio = (_double)fChangeRatio;

							ImGui::SameLine();

							_bool isOver = transitionList[m_iTransitionIndex].bIsOverChangeRatio;
							if (ImGui::Checkbox("OverChangeRatio", &isOver))
								transitionList[m_iTransitionIndex].bIsOverChangeRatio = isOver;

							_float fLerpSpeed = (_float)transitionList[m_iTransitionIndex].dLerpSpeed;
							if (ImGui::DragFloat("LerpSpeed", &fLerpSpeed, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].dLerpSpeed = (_double)fLerpSpeed;

							_float fInputRatio = (_float)transitionList[m_iTransitionIndex].InputTrigger.dAnimRatio;
							if (ImGui::DragFloat("InputRatio", &fInputRatio, 0.001f, 0.f, 1.f))
								transitionList[m_iTransitionIndex].InputTrigger.dAnimRatio = (_double)fInputRatio;

							ImGui::SameLine();

							_bool isOverRatio = transitionList[m_iTransitionIndex].InputTrigger.bIsOverRatio;
							if (ImGui::Checkbox(u8"�̻�, ����", &isOverRatio))
								transitionList[m_iTransitionIndex].InputTrigger.bIsOverRatio = isOverRatio;

							if (ImGui::Button("UnSelect"))
								m_iTransitionIndex = -1;

							ImGui::SameLine();

							if (ImGui::Button("Remove"))
							{
								isDelete = true;
								iDeleteIndex = m_iTransitionIndex;
								m_iTransitionIndex = -1;
							}
						}

						// ����ٴ� bool���� true��� ����Ʈ���� �����.
						if (true == isDelete)
						{
							transitionList.erase(transitionList.begin() + iDeleteIndex);
							isDelete = false;
							iDeleteIndex = -1;
							isTransitionChange = true;
						}

						m_pAnimatorCom->Set_TransitionList(transitionList);

						static char szTrigger[MAX_PATH];
						ImGui::SetNextItemWidth(200.f);
						ImGui::InputText("Input Trigger", szTrigger, sizeof(szTrigger));

						ImGui::SameLine();

						// �ִϸ����ͷ� Trigger�� �����մϴ�.
						if (ImGui::Button("Set Trigger"))
							m_pAnimatorCom->Input_Trigger(_string(szTrigger));

					}
					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::CollapsingHeader("SwordViewer_TabBar"))
	{
		Attach_SwordModel();
	}
	if (ImGui::CollapsingHeader("ShelteViewer_TabBar"))
	{
		Attach_ShelteModel();
	}
	// �� ��ü�� Ʈ�������� �ʱ�ȭ�մϴ�.
	if (ImGui::Button("Reset Transform"))
	{
		if (nullptr != m_pAnimObject)
		{
			m_pAnimObject->Get_Transform()->Set_Pos(XMVectorZero());
			if (nullptr != m_pAnimatorCom)
				m_pAnimatorCom->Initialize_BeginTransform();
		}
	}
}

HRESULT CAnimViewer_Tool::Create_AnimObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pAnimObject = pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_ANIMVIEWER, PROTO_GOB_ANIMVIEWERMODEL);

	if (nullptr == m_pAnimObject)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	// �ִ� ������Ʈ�� ���۷��� ī��Ʈ�� �ø���.
	Safe_AddRef(m_pAnimObject);

	// ���� �ִϸ����� ��ü�� �ϳ� �����
	m_pAnimatorCom = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR);

	// �� �ִϸ��̼� Ŭ�� �ϳ��� �����Ѵ�.
	CAnimationClip* pClip = CAnimationClip::Create();

	// �̸��� Test�� �����Ѵ�.
	pClip->Set_Name("Test");

	// �ִϸ����� ������Ʈ���� �ִϸ��̼� Ŭ���� �־��ش�.
	if (nullptr != pClip)
		m_pAnimatorCom->Add_AnimationClip(pClip);

	if (nullptr != m_pAnimObject)
		m_pAnimatorCom->Set_Transform(m_pAnimObject->Get_Transform());

	// AnimObject���� Animator�� �־��ش�.
	static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Animator(m_pAnimatorCom);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Model()
{
	if (nullptr == m_pAnimObject)
		return E_FAIL;

	// �� ��ü�� �ִϸ��̼� �׽�Ʈ �𵨿� Set ���ش�. ( RefCount�� ���ο��� �����ȴ�. )
	CModel* pModel = static_cast<CModel*>(m_ModelList[m_iModelIndex]->Clone(m_ModelList[m_iModelIndex]));
	if (FAILED(static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Model(pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Animation()
{
	if (nullptr == m_pAnimObject)
		return E_FAIL;

	// ������ �ִϸ��̼� �ε����� �𵨸���Ʈ�� �ִϸ��̼� �������� ũ�ų� ���ٸ� �����Ѵ�.
	if (m_iAnimationIndex >= m_pAnimatorCom->Get_Model()->Get_Animations().size())
		return E_FAIL;

	// �ִϸ��̼� �׽�Ʈ ���� ���� �����ִ� �𵨰�ü�� �ִϸ��̼��� �޾ƿ´�.
	vector<CAnimation*> AnimationList = m_pAnimatorCom->Get_Model()->Get_Animations();

	// �ִϸ��̼� Ŭ���� ���� �ִϸ��̼��� �ٲ۴�.
	m_pAnimatorCom->Get_CurrentAnimationClip()->Set_MainAnimation(AnimationList[m_iAnimationIndex]);

	// �� ������ ������Ʈ�Ѵ�.
	m_pAnimatorCom->Update_InitBoneState();

	return S_OK;
}

HRESULT CAnimViewer_Tool::Add_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// () �ȿ� ����ִ� �̸��� �ִϸ��̼��� �̸����� �޾ƿ´�.
	_int iStart = (_int)m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().find("(", 0) + 1;
	_int iEnd = (_int)m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().find(")", 0) - iStart;
	_string szClipName = m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName().substr(iStart, iEnd);

	// ������ ����ϱ����� Test Ŭ���� �޾ƿ´�.
	CAnimationClip* pTestClip = m_pAnimatorCom->Get_AnimationClip("Test");

	CAnimationClip* pClip = pTestClip->Clone();
	pClip->Set_Name(szClipName);
	m_pAnimatorCom->Add_AnimationClip(pClip);

	// �̺�Ʈ ����Ʈ�� ����ش�.
	vector<CAnimationClip::ANIMEVENTDESC> eventList;

	pTestClip->Set_Event(eventList);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Add_AllAnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	for (auto& pAnimation : m_pAnimatorCom->Get_Model()->Get_Animations())
	{
		// () �ȿ� ����ִ� �̸��� �ִϸ��̼��� �̸����� �޾ƿ´�.
		_int iStart = (_int)pAnimation->Get_Name().find("(", 0) + 1;
		_int iEnd = (_int)pAnimation->Get_Name().find(")", 0) - iStart;
		_string szClipName = pAnimation->Get_Name().substr(iStart, iEnd);

		CAnimationClip* pClip = CAnimationClip::Create();
		pClip->Set_Name(szClipName);
		pClip->Set_MainAnimation(pAnimation);
		m_pAnimatorCom->Add_AnimationClip(pClip);
	}

	return S_OK;
}

HRESULT CAnimViewer_Tool::Set_Animator(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	// �����ִ� �ִϸ����͸� ���ֱ� ���� Safe_Release�� ���ش�.
	static_cast<CAnimViewerModel*>(m_pAnimObject)->Set_Animator(pAnimator);

	Safe_Release(m_pAnimatorCom);

	// AnimObject�� ���� �ִϸ����Ͱ� �ٲ���ٸ� m_pAnimatorCom�� �ٽ� �־��ش�.
	m_pAnimatorCom = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Animator();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Delete_AnimationClip(_string strName)
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	m_pAnimatorCom->Delete_AnimationClip(strName);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Save_Animator()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// ���� ���
	_wstring strFilePath = L"..\\..\\Resources\\Data\\Animator\\";
	// ��������
	_wmkdir(strFilePath.c_str());
	// ���õ� �� �޾ƿ���
	strFilePath += m_ModelList[m_iModelIndex]->Get_FileData()->strFileName;
	strFilePath += L"\\";
	// ���̸����� ���� ����L"..\\..\\Resources\\Animator\\�𵨸�"
	_wmkdir(strFilePath.c_str());
	// ���̸����� �ִϸ����� ���� ���� L"..\\..\\Resources\\Animator\\�𵨸�\\�𵨸�.bin"
	strFilePath += m_ModelList[m_iModelIndex]->Get_FileData()->strFileName;
	strFilePath += L".bin";

	_ulong dwByte = 0;

	HANDLE hFile = CMethod::Begin_Save(strFilePath.c_str());
	if (0 == hFile)
		return E_FAIL;

	// �ִϸ��̼� Ŭ�� ������ ����ش�.
	_uint iClipSize = (_uint)m_pAnimatorCom->Get_AnimationClipList().size();
	WriteFile(hFile, &iClipSize, sizeof(_uint), &dwByte, nullptr);

	// �ҷ��� �� �ִϸ��̼� Ŭ�� ����Ʈ�� ��Ƶ� �ִϸ��̼� Ŭ����
	for (auto& pClip : m_pAnimatorCom->Get_AnimationClipList())
	{
		// �ִϸ��̼� Ŭ���� �̸��� �����Ѵ�.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_Name());
		// ���� �ִϸ��̼��� �̸��� �����մϴ�.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_MainAnimationName());
		// ���� �ִϸ��̼��� �̸��� �����մϴ�.
		if (nullptr == pClip->Get_SubAnimation())
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, pClip->Get_SubAnimationName());

		// ���� ���θ� �����Ѵ�.
		WriteFile(hFile, &pClip->Is_Loop(), sizeof(_bool), &dwByte, nullptr);
		// ��Ʈ�� ���θ� �����Ѵ�
		_bool IsRootBone = pClip->Is_RootBone();
		WriteFile(hFile, &IsRootBone, sizeof(_bool), &dwByte, nullptr);
		// ��Ʈ�� �̸��� �����Ѵ�.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_RootBoneName());
		// �ִϸ��̼��� �ӵ��� �����Ѵ�.
		WriteFile(hFile, &pClip->Get_AnimationSpeed(), sizeof(_float), &dwByte, nullptr);
		// �ִϸ��̼� Ÿ���� �����Ѵ�.
		CMethod::Save_File_String(hFile, dwByte, pClip->Get_AnimationType());

		// �̺�Ʈ ������ �����Ѵ�.
		_uint iEventSize = (_uint)pClip->Get_EventList().size();
		WriteFile(hFile, &iEventSize, sizeof(_uint), &dwByte, nullptr);
		// �̺�Ʈ�� �����Ѵ�.
		for (auto& event : pClip->Get_EventList())
		{
			// �̺�Ʈ Ÿ���� �����Ѵ�.
			WriteFile(hFile, &event.tEventType, sizeof(_uint), &dwByte, nullptr);
			// �̺�Ʈ �̸��� �����Ѵ�
			CMethod::Save_File_String(hFile, dwByte, event.strName);
			// �̺�Ʈ�� ���۵Ǵ� �ð�����
			WriteFile(hFile, &event.dStartRatio, sizeof(_double), &dwByte, nullptr);
			// �̺�Ʈ�� ������ �ð�����
			WriteFile(hFile, &event.dEndRatio, sizeof(_double), &dwByte, nullptr);
			// �̺�Ʈ�� ���� �ð� ( ���� �Ŀ� �ð��ʷ� ������ ������ )
			WriteFile(hFile, &event.dEndTime, sizeof(_double), &dwByte, nullptr);
			// Value�� ����
			WriteFile(hFile, &event.fValue, sizeof(_float), &dwByte, nullptr);
			// AddValue�� ����
			WriteFile(hFile, &event.fAddValue, sizeof(_float), &dwByte, nullptr);
			// ���� ��ġ
			WriteFile(hFile, &event.vEventPos, sizeof(_float4), &dwByte, nullptr);
			// �̺�Ʈ�� ���� Dir��
			WriteFile(hFile, &event.vEventDir, sizeof(_float4), &dwByte, nullptr);
			// �̺�Ʈ�� ���� Value�� ( �پ��ϰ� ��밡�� )
			WriteFile(hFile, &event.vEventValues, sizeof(_float4), &dwByte, nullptr);
			// ����� ���� �̸�
			CMethod::Save_File_String(hFile, dwByte, event.strEventText);
		}
	}

	// ����� ����� �����Ѵ�.
	_uint iWeaponBoneSize = (_uint)m_pAnimatorCom->Get_WeaponBoneList().size();
	WriteFile(hFile, &iWeaponBoneSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& pWeaponBone : m_pAnimatorCom->Get_WeaponBoneList())
		CMethod::Save_File_String(hFile, dwByte, pWeaponBone->Get_Name());

	// �ε�� ������ �ִϸ��̼� Ŭ����� ������ TransitionList
	for (auto& transition : m_pAnimatorCom->Get_TransitionList())
	{
		// Ʈ�������� EnterState
		WriteFile(hFile, &transition.tEnterState, sizeof(_uint), &dwByte, nullptr);
		// Ʈ�������� EnterClip�̸��� �����Ѵ�
		if (nullptr == transition.pEnterClip)
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, transition.pEnterClip->Get_Name());
		// Ʈ�������� ExitClip�̸��� �����Ѵ�.
		if (nullptr == transition.pExitClip)
			CMethod::Save_File_String(hFile, dwByte, "");
		else
			CMethod::Save_File_String(hFile, dwByte, transition.pExitClip->Get_Name());
		// Ʈ�������� EnterRatio
		WriteFile(hFile, &transition.dEnterRatio, sizeof(_double), &dwByte, nullptr);
		// Ʈ�������� ExitRatio
		WriteFile(hFile, &transition.dExitRatio, sizeof(_double), &dwByte, nullptr);
		// Ʈ�������� ChangeRatio
		WriteFile(hFile, &transition.dChangeRatio, sizeof(_double), &dwByte, nullptr);
		// Ʈ�������� OverChange �Ұ�
		WriteFile(hFile, &transition.bIsOverChangeRatio, sizeof(_bool), &dwByte, nullptr);
		// Ʈ�������� LerpSpeed
		WriteFile(hFile, &transition.dLerpSpeed, sizeof(_double), &dwByte, nullptr);
		// Ʈ�������� Trigger		
		CMethod::Save_File_String(hFile, dwByte, transition.InputTrigger.strInput);
		// Ʈ�������� Trigger Input Ratio
		WriteFile(hFile, &transition.InputTrigger.dAnimRatio, sizeof(_double), &dwByte, nullptr);
		// Ʈ�������� InputRatio�� �̻�, ���� ����
		WriteFile(hFile, &transition.InputTrigger.bIsOverRatio, sizeof(_bool), &dwByte, nullptr);
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Save_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	// �ִϸ��̼� Ŭ�� ����Ʈ
	vector<CAnimationClip*>animm_ClipList = m_pAnimatorCom->Get_AnimationClipList();

	return S_OK;
}

HRESULT CAnimViewer_Tool::Load_Animator()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Load_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewer_Tool::Initialize_ToolData()
{
	return S_OK;
}

HRESULT CAnimViewer_Tool::Merge_Animator(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	vector<CAnimationClip*> FilterList = pAnimator->Get_AnimationClipList();

	// ó���� �̻��Ѱ͵��� �� �ɷ��ش�.
	for (_uint i = 0; i < FilterList.size();)
	{
		if (nullptr == FilterList[i]->Get_MainAnimation() && nullptr == FilterList[i]->Get_SubAnimation())
		{
			Safe_Release(FilterList[i]);
			FilterList.erase(FilterList.begin() + i);
		}
		else
			i++;
	}

	pAnimator->Set_AnimationClipList(FilterList);

	// Ŭ������Ʈ�� ��ģ��.
	vector<CAnimationClip*> ClipList;

	for (auto& iter : pAnimator->Get_AnimationClipList())
	{
		CAnimationClip* pClip = CAnimationClip::Create();
		if (nullptr == pClip)
			return E_FAIL;

		if ("Test" == iter->Get_Name())
		{
			Safe_Release(pClip);
			continue;
		}

		// �ִϸ��̼� Ŭ���� �̸��� �޾ƿ´�.
		pClip->Set_Name(iter->Get_Name());

		// ���ΰ� ���갡 ��� ����ִٸ� for���� �ѱ��.
		if (nullptr == iter->Get_MainAnimation() && nullptr == iter->Get_SubAnimation())
		{
			Safe_Release(pClip);		
			continue;		
		}

		// ���� �ִϸ��̼��� �޾Ƽ� �ֽ��ϴ�.
		_string strMainAnim = iter->Get_MainAnimationName();

		pClip->Set_MainAnimation(m_pAnimatorCom->Get_Model()->Get_Animation(strMainAnim));
		// ���� �ִϸ��̼��� �޾Ƽ� �ֽ��ϴ�.
		//_string strSubAnim = iter->Get_SubAnimationName();
		//pClip->Set_SubAnimation(m_pAnimatorCom->Get_Model()->Get_Animation(strSubAnim));
		// ������ �����Ѵ�.
		pClip->Set_Loop(iter->Is_Loop());
		// ��Ʈ�� ���θ� �����Ѵ�
		pClip->Set_RootBone(iter->Is_RootBone());
		// ��Ʈ�� �̸��� �����Ѵ�.
		pClip->Set_RootBoneName(iter->Get_RootBoneName());
		// ���ǵ尪�� �ҷ��´�.
		pClip->Set_AnimationSpeed(iter->Get_AnimationSpeed());
		// �ִϸ��̼� Ÿ�԰��� �����´�.
		pClip->Set_AnimationType(iter->Get_AnimationType());

		vector<CAnimationClip::ANIMEVENTDESC> eventList;

		vector<CAnimationClip::ANIMEVENTDESC> ClipEventList = iter->Get_EventList();

		// �̺�Ʈ�� �����Ѵ�.
		for (_uint k = 0; k < ClipEventList.size(); ++k)
		{
			CAnimationClip::ANIMEVENTDESC event;

			// �̺�Ʈ Ÿ���� �����Ѵ�
			event.tEventType = ClipEventList[k].tEventType;
			// �̺�Ʈ �̸��� �ҷ��´�
			event.strName = ClipEventList[k].strEventText;
			// �̺�Ʈ�� ���۵Ǵ� �ð�����
			event.dStartRatio = ClipEventList[k].dStartRatio;
			// �̺�Ʈ�� ������ �ð�����
			event.dEndRatio = ClipEventList[k].dEndRatio;
			// �̺�Ʈ�� ���� �ð� ( ���� �Ŀ� �ð��ʷ� ������ ������ )
			event.dEndTime = ClipEventList[k].dEndTime;
			// Value�� ����
			event.fValue = ClipEventList[k].fValue;
			// AddValue�� ����
			event.fAddValue = ClipEventList[k].fAddValue;
			// ���� ��ġ
			event.vEventPos = ClipEventList[k].vEventPos;
			// ����� ���� �̸�
			event.strEventText = ClipEventList[k].strEventText;
			// �̺�Ʈ�� ����ִ� Dir��
			event.vEventDir = ClipEventList[k].vEventDir;
			// �̺�Ʈ�� ����ִ� Value��
			event.vEventValues = ClipEventList[k].vEventValues;

			eventList.push_back(event);
		}
		// ������ Ŭ������ �̺�Ʈ����Ʈ�� �־��ش�.
		pClip->Set_Event(eventList);
		// ������ Ŭ������ ���� �� ��ü�� ��� ����Ʈ�� �־��ش�.
		pClip->Set_HierarchyNodeList(m_pAnimatorCom->Get_Model()->Get_HierachyNodeList());
		// Ŭ���� Ŭ�� ����Ʈ�� �ִ´�.
		ClipList.push_back(pClip);
	}
	// ���������ִ� �ִϸ��̼� Ŭ���� ����Ʈ�� �ִ´�.
	for (auto& pAnimatorClip : m_pAnimatorCom->Get_AnimationClipList())
	{
		_uint iCount = 0;

		for (auto& pMergeClip : pAnimator->Get_AnimationClipList())
		{
			if (pMergeClip->Get_Name() == pAnimatorClip->Get_Name())
				iCount++;
		}
		if (0 == iCount)
		{
			// ���۷��� ���� ��Ų��.
			Safe_AddRef(pAnimatorClip);
			ClipList.push_back(pAnimatorClip);
		}
		// �ߺ��Ǵ°��� �ִٸ� �ش� �ִϸ��̼� Ŭ���� �����ִ� Ʈ�������� �����Ѵ�.
		else
		{
			vector<CAnimator::TRANSITION> newTransitionList;
			vector<CAnimator::TRANSITION> transitionList = m_pAnimatorCom->Get_TransitionList();
			for (auto& transition : m_pAnimatorCom->Get_TransitionList())
			{
				if (pAnimatorClip == transition.pEnterClip || pAnimatorClip == transition.pExitClip)
					continue;

				newTransitionList.push_back(transition);
			}
			m_pAnimatorCom->Set_TransitionList(newTransitionList);
		}
	}

	for (auto& pPreviousClip : m_pAnimatorCom->Get_AnimationClipList())
		Safe_Release(pPreviousClip);

	// �ִϸ��̼� Ŭ�� ����Ʈ�� ��ü�Ѵ�.
	m_pAnimatorCom->Set_AnimationClipList(ClipList);

	vector<CAnimator::TRANSITION> transitionList;

	// Ʈ������ ����� �޾ƿ´�.
	for (auto& mergeTransition : pAnimator->Get_TransitionList())
	{
		CAnimator::TRANSITION transition;

		// Ʈ�������� EnterState
		transition.tEnterState = mergeTransition.tEnterState;
		// EnterClip�� Ʈ�����ǿ� �־��ش�.
		if (nullptr != mergeTransition.pEnterClip)
			transition.pEnterClip = m_pAnimatorCom->Get_AnimationClip(mergeTransition.pEnterClip->Get_Name());
		// ExitClip�� Ʈ�����ǿ� �־��ش�.
		if (nullptr != mergeTransition.pExitClip)
			transition.pExitClip = m_pAnimatorCom->Get_AnimationClip(mergeTransition.pExitClip->Get_Name());

		// Ʈ�������� EnterRatio
		transition.dEnterRatio = mergeTransition.dEnterRatio;
		// Ʈ�������� ExitRatio
		transition.dExitRatio = mergeTransition.dExitRatio;
		// Ʈ�������� ChangeRatio
		transition.dChangeRatio = mergeTransition.dChangeRatio;
		// Ʈ�������� OverChange �Ұ�
		transition.bIsOverChangeRatio = mergeTransition.bIsOverChangeRatio;
		// Ʈ�������� LerpSpeed
		transition.dLerpSpeed = mergeTransition.dLerpSpeed;
		// Ʈ�������� Trigger		
		transition.InputTrigger.strInput = mergeTransition.InputTrigger.strInput;
		// Ʈ�������� Trigger Input Ratio
		transition.InputTrigger.dAnimRatio = mergeTransition.InputTrigger.dAnimRatio;
		// Ʈ�������� InputRatio �̻�, ���� ���� �Ǵ�
		transition.InputTrigger.bIsOverRatio = mergeTransition.InputTrigger.bIsOverRatio;

		// ����Ʈ�� ����ش�.
		transitionList.push_back(transition);
	}

	for (auto& animatorTransition : m_pAnimatorCom->Get_TransitionList())
	{
		transitionList.push_back(animatorTransition);
	}

	m_pAnimatorCom->Set_TransitionList(transitionList);

	Safe_Release(pAnimator);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CAnimViewer_Tool::Merge_AnimatorEvent(_wstring _strPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator::ANIMATORDESC animatorDesc;
	animatorDesc.pModel = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Model();
	animatorDesc.strFilePath = _strPath;
	animatorDesc.pTransform = static_cast<CAnimViewerModel*>(m_pAnimObject)->Get_Transform();

	CAnimator* pAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, &animatorDesc);

	vector<CAnimationClip*> FilterList = pAnimator->Get_AnimationClipList();

	// ó���� �̻��Ѱ͵��� �� �ɷ��ش�.
	for (_uint i = 0; i < FilterList.size();)
	{
		if (nullptr == FilterList[i]->Get_MainAnimation() && nullptr == FilterList[i]->Get_SubAnimation())
		{
			Safe_Release(FilterList[i]);
			FilterList.erase(FilterList.begin() + i);
		}
		else
			i++;
	}

	pAnimator->Set_AnimationClipList(FilterList);

	for (auto& pClip : pAnimator->Get_AnimationClipList())
	{
		for (auto& pMyClip : m_pAnimatorCom->Get_AnimationClipList())
		{
			if (pClip->Get_Name() == pMyClip->Get_Name())
			{
				pMyClip->Set_Event(pClip->Get_EventList());
			}
		}
	}

	Safe_Release(pAnimator);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

// ================== ���� �߰� ==================
HRESULT CAnimViewer_Tool::Attach_SwordModel()
{
	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), u8"�� ����");
	for (auto& iter : m_ItemModelList)
	{
		if (nullptr != iter)
		{
			FILEGROUP* pFileGroup = iter->Get_FileGroup();
			FILEDATA* pFileData = iter->Get_FileData();

			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				_wstring str = pFileGroup->strUppderName;
				str.append(L" ");
				str.append(pFileData->strFileName);
				if (ImGui::Selectable(CMethod::ConvertWToS(str).c_str()))
				{
					if (nullptr != m_pSwordTool)
						m_pSwordTool->Bine_Model(iter);
				}
			}
		}
	}
	ImGui::NewLine();
	if (nullptr != m_pSwordTool)
	{
		if (-1 == m_iModelIndex)
			return E_FAIL;
		// Anim Model
		m_pSwordTool->Find_Bones(m_pAnimObject);
	}
	return S_OK;
}

HRESULT CAnimViewer_Tool::Attach_ShelteModel()
{
	ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f), u8"�� ����");
	for (auto& iter : m_ItemModelList)
	{
		if (nullptr != iter)
		{
			FILEGROUP* pFileGroup = iter->Get_FileGroup();
			FILEDATA* pFileData = iter->Get_FileData();

			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				_wstring str = pFileGroup->strUppderName;
				str.append(L" ");
				str.append(pFileData->strFileName);
				if (ImGui::Selectable(CMethod::ConvertWToS(str).c_str()))
				{
					if (nullptr != m_pShelteTool)
						m_pShelteTool->Bine_Model(iter);
				}
			}
		}
	}
	ImGui::NewLine();
	if (nullptr != m_pShelteTool)
	{
		if (-1 == m_iModelIndex)
			return E_FAIL;
		// Anim Model
		m_pShelteTool->Find_Bones(m_pAnimObject);
	}
	return S_OK;
}

CAnimViewer_Tool* CAnimViewer_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CAnimViewer_Tool* pInstance = new CAnimViewer_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimViewer_Tool::Create To Failed");
	}
	return pInstance;
}

void CAnimViewer_Tool::Free(void)
{
	// ���� �����ϴ� ��ü�� ���۷��� ī��Ʈ�� �����Ѵ�.
	Safe_Release(m_pAnimObject);
	// �׽�Ʈ�� Animator ������Ʈ ��ü�� ���۷��� ī��Ʈ�� ���ҽ�Ų��.
	Safe_Release(m_pAnimatorCom);
	// �׽�Ʈ�� Animation Ŭ���� ���ش�.
	Safe_Release(m_pAnimationClip);

	for (auto& pModel : m_ModelList)
		Safe_Release(pModel);

	for (auto& pModel : m_ItemModelList)
		Safe_Release(pModel);

	m_ModelList.clear();

	Safe_Release(m_pSwordTool);
	Safe_Release(m_pShelteTool);

	m_ItemModelList.clear();
	__super::Free();
}
