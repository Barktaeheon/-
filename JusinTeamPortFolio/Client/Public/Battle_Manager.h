#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CCollider;
END

BEGIN(Client)
class CCharacter;

class CBattle_Manager : public CBase
{
	DECLARE_SINGLETON(CBattle_Manager);
public:
	enum CHARACTER_TYPE
	{
		CHAR_PLAYER, CHAR_AI, CHAR_END
	};

	enum BATTLEMAP_TYPE
	{
		MAP_UROKODAKI, MAP_HUJIKASANE, MAP_TAMAYO, MAP_ASAKUSA
	};

	typedef struct tagBattleInfo
	{
		// ��Ʋ ��
		BATTLEMAP_TYPE		eBattleMap = MAP_UROKODAKI;
		// ���� �ʱ�ȭ
		CUSTIMER			tBattleTimer = CUSTIMER(99.f);
		CUSTIMER			tStartTimer = CUSTIMER(0.f);
		_bool				bBattleInit = false;
		_bool				bBattleStart = false;
		// ��Ʋ ����
		_bool				bBattleEnd = false;
		// ��Ʋ ����
		_uint				iRound = 0;
		// ����� AI�� �ο�� ������ true�� �ٲٸ� �˾Ƽ� �����
		_bool				bSabitoAI = false;
		// ���ο�
		_float				fUpdateRatio = 1.f;
	}BATTLEINFO;

protected:
	explicit CBattle_Manager();
	virtual ~CBattle_Manager() = default;

public:
	CCharacter*				Get_BattleCharacter(const CHARACTER_TYPE& _eCharType);
	vector<CCharacter*>		Get_OniArr() { return m_pOniArr; }
	const _uint&			Get_CurrentOniLayer() const { return m_iCurrentOniLayer; }
	const _bool&			IsStoryBattle() const { return m_bIsStoryBattle; }
	const BATTLEINFO&		Get_BattleInfo() const { return m_tBattleInfo; }
	const _bool& Get_BattleStart() const { return m_bInit; }
	
	_bool Is_MotionBlurOn();
	// ��Ʋ �������� üũ
	void	Set_BattleEnd(_bool bBattleEnd) { m_tBattleInfo.bBattleEnd = bBattleEnd; }
	// ��Ʋ ����� ���� �������ش�. 
	void	Set_BattleMapType(BATTLEMAP_TYPE eMapType) { m_tBattleInfo.eBattleMap = eMapType; }
	// ���丮 ��忡�� ��Ʋ ���¸� �����Ѵ�.
	void	Set_IsStoryBattle(_bool bIsStoryBattle) { m_bIsStoryBattle = bIsStoryBattle; }
	// �����̸� �����Ѵ�.
	void	Set_DelayTime(_float _fDelayTime) { m_tDelayTimer.Reset_Timer(); m_tDelayTimer.fStandardTime = _fDelayTime; }

public:
	HRESULT NativeConstruct_StartCutScene();
	void Start_Battle(CCharacter* _pCharacter, const CHARACTER_TYPE& _eCharType);
	// Update
	void Update(const _double& _dTimeDelta);

private:
	// ���� ���� Layer�� �����ϴ� ���ϵ��� �����´�.
	void SetUp_OniLayer();
	// ���� �浹�� �о��
	void Push();
	// ���� Ÿ�̸�
	void InitBattle(const _double& _dTimeDelta);
	void Change_UpdateRatio(_float fUpdateRatio);
	// ������ ����
	void Check_BattleDelayTimer(const _double& _dTimeDelta);
	void Check_StoryDelayTimer(const _double& _dTimeDelta);

private:
	// Player
	CCharacter* m_pCharacterArr[CHAR_END] = { nullptr };

	// Oni
	vector<CCharacter*> m_pOniArr;
	_uint				m_iCurrentOniLayer = 1;
	_bool				m_bIsStoryBattle = false;
	// ����
	_bool				m_bSuccessDash = false;
	// Ư�� ���� �̺�Ʈ�� �������� ���ߴµ��� �ൿ�� �Ѵ�
	CUSTIMER			m_tDelayTimer;
	
private:
	BATTLEINFO			m_tBattleInfo;

	_bool				m_bFadeOut = false;
	_bool				m_bInit = false;

protected:
	virtual void Free() override;
};

END