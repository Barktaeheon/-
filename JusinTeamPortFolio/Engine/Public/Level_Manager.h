#pragma once

#include "Base.h"
/* ���� ���ӳ��� �����ִ� ������ �����Ѵ�. */
/* ������ ��ü �۾��� �����Ѵ�. (���������� ��ü���� �����Ѵ�.) */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	explicit CLevel_Manager();
	virtual ~CLevel_Manager() = default;
public:
	const _float& Get_DeltaTime() const { return m_fDeltaTime;}
public:
	HRESULT Change_Level(const _uint& _iCurrentLevelIndex, class CLevel* pNextLevel);
	const _uint& Get_CurLevel() const { return m_iCurrentLevelIndex; }
	HRESULT Tick(const _double& _dTimeDelta);
	HRESULT Late_Tick(const _double& _dTimeDelta);
	HRESULT Render();
private:
	class CLevel* m_pCurrentLevel = nullptr;
	_uint					m_iCurrentLevelIndex = 0;
	_float					m_fDeltaTime = 0.f;
public:
	virtual void Free() override;
};

END