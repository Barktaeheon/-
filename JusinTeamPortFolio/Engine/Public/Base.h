#pragma once
#include "SuperBase.h"

BEGIN(Engine)
class CRefDebugActor;

/* ���� Ŭ���� .*/
/* ���۷��� ī��Ʈ�� �����Ѵ�*/
class ENGINE_DLL  CBase abstract  : public CSuperBase
{
protected:	
	CBase();
	virtual ~CBase();
#ifdef _DEBUG
public:
	virtual unsigned long AddRef(const _char* FILE,
		const _uint& LINE, const _char* FUNC) override;
	virtual unsigned long Release(const _char* FILE,
		const _uint& LINE, const _char* FUNC) override;
private:
	EXPLAINS Make_Explain(const _string& _strName );
	CRefDebugActor*						m_pRefActor = nullptr;
	DEBUGREMAINDESC*				m_pRemainDesc = nullptr;
#endif
public:
	virtual void Free();

};

END



