#pragma once
#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL CDebugActor abstract : public CSuperBase
{
protected:
	explicit CDebugActor();
	virtual ~CDebugActor() = default;
public:
	// �ʱ�ȭ
	virtual HRESULT NativeConstruct();
	virtual HRESULT Clear();
private:
	/* ���� ������ ��� �����Ѵ�. */
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free() override;
};

END