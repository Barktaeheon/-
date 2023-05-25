#pragma once
#include "CManager.h"

class CSoundMgr : public CManager
{
public:
	explicit CSoundMgr();
	explicit CSoundMgr(CGameMgr*& pGameMgr);
	virtual ~CSoundMgr();
protected:
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;
public:
	virtual void Initialize() override;
	virtual void Release() override;
	// CManager��(��) ���� ��ӵ�
	virtual int Update() override { return 0; }
public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

private:
	void LoadSoundFile(const char* szPath);
};

