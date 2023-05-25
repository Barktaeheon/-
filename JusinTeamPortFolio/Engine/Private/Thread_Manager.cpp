#include "..\Public\Thread_Manager.h"

IMPLEMENT_SINGLETON(CThread_Manager)

CThread_Manager::CThread_Manager() 
{

}
// �����带 �����ϰ�  JOIN �ϴ� �Լ�
HRESULT CThread_Manager::Add_Thread(function<void(void*)> _pData, void* _pArg)
{
	m_Threads.push_back(thread(_pData, _pArg));
	return S_OK;
}
HRESULT CThread_Manager::Join_Thread()
{
	for (auto& iter : m_Threads)
	{
		m_Mutex.lock();
		iter.join();
		m_Mutex.unlock();
	}
	return S_OK;
}
HRESULT CThread_Manager::Detach_Thread()
{
	for (auto& iter : m_Threads)
	{
		m_Mutex.lock();
		iter.detach();
		m_Mutex.unlock();
	}
	return S_OK;
}

HRESULT CThread_Manager::End_Threads()
{
	// ������ ��� ���� 
	m_Threads.clear();
	return S_OK;
}


void CThread_Manager::Free()
{
	m_Threads.clear();
}

