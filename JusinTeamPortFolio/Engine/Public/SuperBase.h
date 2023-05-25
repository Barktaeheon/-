#pragma once
#include "Engine_Defines.h"

BEGIN(Engine)

/* ���۷��� ī��Ʈ�� �����Ѵ�*/
class ENGINE_DLL CSuperBase abstract
{
protected:
	CSuperBase();
	virtual ~CSuperBase();
public:
#ifdef _DEBUG
	virtual unsigned long AddRef(const _char* FILE, const _uint& LINE, const _char* TIME); /* ���� ������ ������Ų��, ������Ų ���۷���ī��Ʈ�� �����Ѵ�. */
	virtual unsigned long Release(const _char* FILE, const _uint& LINE, const _char* TIME); /* ���ҽ�Ų��. or �����Ѵ�. ���ҽ�Ű�� ������ ���۷��� ī��Ʈ�� �����Ѵ�. */
#else
	unsigned long AddRef();
	unsigned long Release();
#endif

protected:
	/* ���� ������ ��� �����Ѵ�. */
	unsigned long								m_dwRefCnt = 0;
public:
	virtual void Free();
};

END
