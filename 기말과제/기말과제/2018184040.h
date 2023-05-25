#include<iostream>
#include<conio.h>
#include<time.h>
#include<Windows.h>
#include<fstream>
#include<string>

using namespace std;
// 메모리 삭제 매크로
#define DELETE_MEMORY_2018184040(x) if(x != nullptr){ delete x; x = nullptr;}
#define DELETE_MEMORY_2018184040_ARR_2018184040(x) if(x != nullptr) {delete[] x; x = nullptr;}
// 맵의 최대 길이
#define MAX_LENGTH_2018184040 19

// 커스텀 리스트
template<typename T>
class CustomList_2018184040
{
	template<typename T>
	struct STNode
	{
	public:
		STNode<T>* next;
		STNode<T>* prev;
		T data;
		int index;
	};
public:
	CustomList_2018184040()
	{
		stHead = new STNode<T>();
		stHead->next = stHead;
		stHead->prev = stHead;
	}
	~CustomList_2018184040()
	{
		Clear();
		DELETE_MEMORY_2018184040(stHead);
	}
	void Insert(T data)
	{
		STNode<T>* newNode = new STNode<T>();
		newNode->data = data;
		if (stHead->next == stHead)
		{
			stHead->next = newNode; stHead->prev = newNode;
			newNode->next = stHead; newNode->prev = stHead;
			newNode->index = 0;
		}
		else
		{
			newNode->next = stHead; newNode->prev = stHead->prev;
			newNode->index = stHead->prev->index + 1;
			stHead->prev->next = newNode; stHead->prev = newNode;
		}
	}
	int GetLength() const
	{
		if (stHead->next == stHead) return 0;
		return stHead->prev->index + 1;
	}
	T GetData(int index)
	{
		T data = T();
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->index == index)
				{
					data = findNode->data;
					break;
				}
			}
		}
		return data;
	}
	void DeleteToIndex(int index)
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->index == index)
				{
					findNode->prev->next = findNode->next;
					findNode->next->prev = findNode->prev;
					DELETE_MEMORY_2018184040(findNode);
					findNode = stHead->next;
					int index = 0;
					for (; findNode != stHead; findNode = findNode->next)
						findNode->index = index++;
					break;
				}
			}
		}
	}
	void DeleteToData(T data)
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			for (; findNode != stHead; findNode = findNode->next)
			{
				if (findNode->data == data)
				{
					findNode->prev->next = findNode->next;
					findNode->next->prev = findNode->prev;
					DELETE_MEMORY_2018184040(findNode);
					findNode = stHead->next;
					int index = 0;
					for (; findNode != stHead; findNode = findNode->next)
						findNode->index = index++;

					break;
				}
			}
		}
	}
	void Clear()
	{
		if (stHead->next != stHead)
		{
			STNode<T>* findNode = stHead->next;
			while (findNode != stHead)
			{
				STNode<T>* tmp = findNode;
				findNode = findNode->next;
				tmp->prev->next = tmp->next;
				DELETE_MEMORY_2018184040(tmp);
			}
			stHead->next = stHead;
			stHead->prev = stHead;
		}
	}
protected:
	STNode<T>* stHead;
};
// 게임에 필요한 문양들
enum EMuni_2018184040 : int
{
	CROSS = 0, WHITE_ROCK, BLACK_ROCK, NONE
};
// 게임에 필요한 좌표
typedef struct Vector2_2018184040
{
public:
	Vector2_2018184040() { x = 0; y = 0; }
	Vector2_2018184040(int x, int y) { this->x = x; this->y = y; }
	int x, y;
}Vector2_2018184040;
// 게임에 필요한 정보들
// 좌표와 무늬값 저장
typedef struct STSpace_2018184040
{
public:
	STSpace_2018184040()
	{
		v2Pos = Vector2_2018184040{}; eMuni = EMuni_2018184040::NONE;  iWeight = 0; iID = 0;
	}
	STSpace_2018184040(Vector2_2018184040 pos, EMuni_2018184040 muni)
	{
		this->v2Pos = pos; this->eMuni = muni;  iWeight = 0; iID = 0;
	}
	Vector2_2018184040 v2Pos;
	EMuni_2018184040 eMuni;
	double iWeight;
	int iID;
}STSpace_2018184040;

class URock_2018184040
{
public:
	URock_2018184040() { eRockType = EMuni_2018184040::NONE; }
	~URock_2018184040() {}

	void Initialize()
	{
		// 이기는 위치
		ROCK_PUT_WIN = 900;
		// 보통  상황일 때 NS
		// 돌을 놓으면 4개의 연속된 돌 완성
		ROCK_PUT_FOUR_NS = 1000;
		// 돌을 놓으면 3개의 연속된 돌 완성
		ROCK_PUT_THREE_NS = 500;
		// 돌을 놓으면 2개의 연속된 돌 완성
		ROCK_PUT_TWO_NS = 160;
		// 한쪽이 막혀있을 상황일 때 SS
		// 돌을 놓으면 4개의 연속된 돌이 완성되는데 한쪽이 막혀있는 경우
		ROCK_PUT_FOUR_PS = 20;
	}
	// 돌이 놓일 위치를 정해주는 함수
	void PutofStone(EMuni_2018184040 eMap[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040])
	{
		UpdateWeight(eMap);

		PutofStone_Vertical(eRockType);
		PutofStone_Horizontal(eRockType);
		PutofStone_LeftUp(eRockType);
		PutofStone_RightUp(eRockType);

		Ban2X2_Black();
		Ban3X3_Black();
	}
protected:
	void UpdateWeight(EMuni_2018184040 eMap[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040])
	{
		for (int i = 0; i < MAX_LENGTH_2018184040; i++)
		{
			for (int j = 0; j < MAX_LENGTH_2018184040; j++)
			{
				stMap[j][i].iWeight = 0;
				stMap[j][i].iID = 0;
			}
		}
		SettingWeight_LeftUp();
		SettingWeight_RightUp();
		for (int i = 0; i < MAX_LENGTH_2018184040; i++)
		{
			for (int j = 0; j < MAX_LENGTH_2018184040; j++)
			{
				stMap[j][i].eMuni = eMap[j][i];
				if (eMap[j][i] == BLACK_ROCK || eMap[j][i] == WHITE_ROCK)
				{
					stMap[j][i].iWeight = ROCK_PUT_NONE;
					stMap[j][i].iID = -100;
				}
			}
		}
	}
	// 무게를 세팅하는 함수
	void SettingWeight_LeftUp()
	{
		const int Length = MAX_LENGTH_2018184040 * 2 - 1;
		int k = 0, i = 0, length = 0, x = 0, y = 0, pmValue = 1, index = 0, save = 0;
		EMuni_2018184040 muni = NONE; int tmpCount = 0;
		// 연속으로 되어있는지 체크
		while (index != Length)
		{
			for (i = 0; i <= length; i++)
			{
				stMap[y][x].iWeight += (length * 1);
				x--; y++;
			}
			tmpCount = 0;	muni = NONE;
			if (i >= length)
			{
				if (length == MAX_LENGTH_2018184040 - 1) { pmValue = -1; }
				index++; length += pmValue;
				if (pmValue == 1) { x = length;	if (length >= MAX_LENGTH_2018184040) x -= 1;	y = 0; }
				else if (pmValue == -1) { x = MAX_LENGTH_2018184040 - 1;	save += 1;	y = save; }
			}
		}
	}
	void SettingWeight_RightUp()
	{
		const int Length = MAX_LENGTH_2018184040 * 2 - 1;
		int k = 0, i = 0, length = 0, x = MAX_LENGTH_2018184040 - 1, y = 0, pmValue = 1, index = 0, save = 0;
		EMuni_2018184040 muni = NONE;
		int tmpCount = 0;
		// 각 열마다 검은색, 휜색 돌이 얼마나 연속으로 놓여있는지에 대한 코드
		while (index != Length)
		{
			for (i = k; i <= length; i++)
			{
				stMap[y][x].iWeight += (length * 1);
				x--;	y--;
			}
			muni = NONE;
			if (i >= length)
			{
				if (length == MAX_LENGTH_2018184040 - 1) { pmValue = -1; }	index++; length += pmValue;
				if (pmValue == 1) { x = MAX_LENGTH_2018184040 - 1;	save += 1;	y = save; }
				else if (pmValue == -1) { x = length;	y = save; }
				k = 0;
			}
		}
	}
	// 돌이 놓일 위치를 정해주는 함수_가로축 검사
	void PutofStone_Vertical(EMuni_2018184040 eRockType)
	{
		int index = 0, k = 0, i = 0, mValue = 0, pValue = 0;
		EMuni_2018184040 muni = NONE;
		int tmpCount = 0;
		while (index != MAX_LENGTH_2018184040)
		{
			for (i = k; i < MAX_LENGTH_2018184040; i++)
				if (stMap[index][i].eMuni == eRockType) { muni = stMap[index][i].eMuni;	k = i; break; }
			if (muni == eRockType)
			{
				int n = 0, pos = k; tmpCount = 0;
				for (int j = pos; j < MAX_LENGTH_2018184040; j++)
					if (stMap[index][j].eMuni == muni) { tmpCount++; ; }
					else	break;

				if (tmpCount >= 0) k += tmpCount;
				// 1인 경우
				if (tmpCount == 1)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 2, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[index][j].eMuni == muni) _index++; else break;
					}
					// 연속된 3개의 돌이 있을 때
					if (_index == 3)
					{
						if (stMap[index][pos + 1].eMuni == CROSS)
							stMap[index][pos + 1].iWeight += ROCK_PUT_WIN;
					}
					// 연속된 2개의 돌이 있을 때
					else if (_index == 2)
					{
						int next = pos + 4; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if ((stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni != CROSS) || (stMap[index][prev].eMuni != CROSS && stMap[index][next].eMuni == CROSS))
							{
								if (stMap[index][pos + 1].eMuni == CROSS)
								{
									if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4_4;
									}
									else	if (stMap[index][pos + 1].iID == 0)
									{
										stMap[index][pos + 1].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4;
									}
								}
							}
							else
							{
								if (stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni == CROSS)
								{
									if (stMap[index][pos + 1].eMuni == CROSS)
									{
										if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[index][pos + 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[index][pos + 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[index][pos + 1].iID == 0)
										{
											stMap[index][pos + 1].iWeight += ROCK_PUT_FOUR_NS;
											stMap[index][pos + 1].iID = MY_ROCK_ID_4;
										}
									}
								}

							}
						}
						// 맨 끝 점중 한 곳일 때
						else if (prev == -100 && next != -100)
						{
							// 양끝이 막혀있지 않을 때
							if (stMap[index][next].eMuni == CROSS)
								if (stMap[index][pos + 1].eMuni == CROSS)
								{
									if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[index][pos + 1].iID == 0)
									{
										stMap[index][pos + 1].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4;
									}
								}
						}
						else if (next == -100 && prev != -100)
						{
							// 양끝이 막혀있지 않을 때
							if (stMap[index][prev].eMuni == CROSS)
								if (stMap[index][pos + 1].eMuni == CROSS)
								{
									if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[index][pos + 1].iID == 0)
									{
										stMap[index][pos + 1].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 1].iID = MY_ROCK_ID_4;
									}
								}
						}
					}
					// 연속된 1개의 돌이 있을 때
					else if (_index == 1)
					{
						int next = pos + 3; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if ((stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni != CROSS) || (stMap[index][prev].eMuni != CROSS && stMap[index][next].eMuni == CROSS))
							{
								if (stMap[index][pos + 1].eMuni == CROSS)
								{
									if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_2;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_3;
									}
									else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 1].iID == 0)
									{
										stMap[index][pos + 1].iWeight += ROCK_PUT_TWO_NS;
										stMap[index][pos + 1].iID = MY_ROCK_ID_2;
									}
								}
							}
							else
							{
								if (stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni == CROSS)
								{
									// 양쪽 다 뚫려 있을 때
									if (stMap[index][pos + 1].eMuni == CROSS)
									{
										if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
											stMap[index][pos + 1].iID = MY_ROCK_ID_3_3;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[index][pos + 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][pos + 1].iID == 0)
										{
											stMap[index][pos + 1].iWeight += ROCK_PUT_THREE_NS;
											stMap[index][pos + 1].iID = MY_ROCK_ID_3;
										}
									}
								}
							}
						}
						else if (prev != -100 && next == -100)
						{
							if (stMap[prev][index].eMuni == CROSS)
								if (prev - 1 >= 0)
									if (stMap[prev - 1][index].eMuni == CROSS)
									{
										if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2_2;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
										{
											stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
										}
										else	if (stMap[index][pos + 1].iID == 0)
										{
											stMap[index][pos + 1].iWeight += ROCK_PUT_TWO_NS;
											stMap[index][pos + 1].iID = MY_ROCK_ID_2;
										}
									}
						}
					}
					// 연속된 0개의 돌이 있을 때
					else if (_index == 0)
					{
						int next = pos + 1; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if (stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni == CROSS)
							{
								if (pos + 2 <= MAX_LENGTH_2018184040 - 1 && pos - 2 >= 0)
								{
									if ((stMap[index][pos + 2].eMuni == CROSS || stMap[index][pos + 2].eMuni == eRockType)
										&& (stMap[index][pos - 2].eMuni == CROSS || stMap[index][pos - 2].eMuni == eRockType))
									{
										if (pos + 3 <= MAX_LENGTH_2018184040 - 1)
										{
											if (stMap[index][pos + 3].eMuni == CROSS || stMap[index][pos + 3].eMuni == eRockType)
												if (stMap[index][pos + 1].eMuni == CROSS)
												{
													if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
													{
														stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
														stMap[index][pos + 1].iID = MY_ROCK_ID_2_2;
													}
													else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
													{
														stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
														stMap[index][pos + 1].iID = MY_ROCK_ID_2_3;
													}
													else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
													{
														stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
														stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
													}
													else	if (stMap[index][pos + 1].iID == 0)
													{
														stMap[index][pos + 1].iWeight += ROCK_PUT_TWO_NS;
														stMap[index][pos + 1].iID = MY_ROCK_ID_2;
													}
												}
										}
										if (pos - 3 >= 0)
										{
											if (stMap[index][pos - 3].eMuni == CROSS || stMap[index][pos - 3].eMuni == eRockType)
												if (stMap[index][pos + 1].iID == MY_ROCK_ID_2)
												{
													stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
													stMap[index][pos + 1].iID = MY_ROCK_ID_2_2;
												}
												else if (stMap[index][pos + 1].iID == MY_ROCK_ID_3)
												{
													stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
													stMap[index][pos + 1].iID = MY_ROCK_ID_2_3;
												}
												else if (stMap[index][pos + 1].iID == MY_ROCK_ID_4)
												{
													stMap[index][pos + 1].iWeight = (stMap[index][pos + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
													stMap[index][pos + 1].iID = MY_ROCK_ID_2_4;
												}
												else if (stMap[index][pos + 1].iID == 0)
												{
													stMap[index][pos + 1].iWeight += ROCK_PUT_TWO_NS;
													stMap[index][pos + 1].iID = MY_ROCK_ID_2;
												}
										}
									}
								}
							}
						}
					}
				}
				// 2개 연속됬을 때
				else if (tmpCount == 2)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 3, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[index][j].eMuni == muni) _index++; else break;
					}
					// 두개 연속된 돌일 때
					if (_index == 2)
					{
						if (stMap[index][pos + 2].eMuni == CROSS)
							stMap[index][pos + 2].iWeight += ROCK_PUT_WIN;
					}
					// 한 개 연속된 될일 때
					else if (_index == 1)
					{
						int next = pos + 4; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if ((stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni != CROSS) || (stMap[index][prev].eMuni != CROSS && stMap[index][next].eMuni == CROSS))
							{
								if (stMap[index][pos + 2].eMuni == CROSS)
								{
									if (stMap[index][pos + 2].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[index][pos + 2].iID == 0)
									{
										stMap[index][pos + 2].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 2].iID = MY_ROCK_ID_4;
									}
								}
							}
							// 양쪽이 뚫려있을 때
							else
							{
								if (stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni == CROSS)
								{
									if (stMap[index][pos + 2].eMuni == CROSS)
									{
										if (stMap[index][pos + 2].iID == MY_ROCK_ID_2)
										{
											stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[index][pos + 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[index][pos + 2].iID == MY_ROCK_ID_3)
										{
											stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[index][pos + 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][pos + 2].iID == MY_ROCK_ID_4)
										{
											stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[index][pos + 2].iID = MY_ROCK_ID_4_4;
										}
										else	 if (stMap[index][pos + 2].iID == 0)
										{
											stMap[index][pos + 2].iWeight += ROCK_PUT_FOUR_NS;
											stMap[index][pos + 2].iID = MY_ROCK_ID_4;
										}

									}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[index][next].iID == MY_ROCK_ID_4_4 || stMap[index][prev].iID == MY_ROCK_ID_4_4 ||
											stMap[index][next].iID == MY_ROCK_ID_3_4 || stMap[index][prev].iID == MY_ROCK_ID_3_4)
										{
											stMap[index][next].iWeight = -100;
											stMap[index][prev].iWeight = -100;
										}
									}
								}
							}
						}
						else if (prev == -100 && next != -100)
						{
							if (stMap[index][pos + 4].eMuni == CROSS)
								if (stMap[index][pos + 2].eMuni == CROSS)
								{
									if (stMap[index][pos + 2].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos + 2].iWeight = (stMap[index][pos + 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos + 2].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[index][pos + 2].iID == 0)
									{
										stMap[index][pos + 2].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 2].iID = MY_ROCK_ID_4;
										// 여기까지 
									}
								}
						}
						else if (prev != -100 && next == -100)
						{
							// 다음이 막혀있다면
							if (stMap[index][prev].eMuni == CROSS)
								if (stMap[index][pos - 2].eMuni == CROSS)
								{
									if (stMap[index][pos - 2].iID == MY_ROCK_ID_2)
									{
										stMap[index][pos - 2].iWeight = (stMap[index][pos - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[index][pos - 2].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos - 2].iID == MY_ROCK_ID_3)
									{
										stMap[index][pos - 2].iWeight = (stMap[index][pos - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[index][pos - 2].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos - 2].iID == MY_ROCK_ID_4)
									{
										stMap[index][pos - 2].iWeight = (stMap[index][pos - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[index][pos - 2].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[index][pos - 2].iID == 0)
									{
										stMap[index][pos - 2].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos - 2].iID = MY_ROCK_ID_4;
									}
								}
						}
					}
					else if (_index == 0)
					{
						int next = pos + 2; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if (stMap[index][next].eMuni == CROSS && stMap[index][prev].eMuni == CROSS)
							{
								// 맨 끝이 아닐 때
								if (pos + 3 <= MAX_LENGTH_2018184040 - 1)
									if (stMap[index][next].eMuni == CROSS)
										if (stMap[index][next + 1].eMuni == CROSS)
										{
											if (stMap[index][next].iID == MY_ROCK_ID_2)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[index][next].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[index][next].iID == MY_ROCK_ID_3)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[index][next].iID = MY_ROCK_ID_3_3;
											}
											else if (stMap[index][next].iID == MY_ROCK_ID_4)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[index][next].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[index][next].iID == 0)
											{
												stMap[index][next].iWeight += ROCK_PUT_THREE_NS;
												stMap[index][next].iID = MY_ROCK_ID_3;
											}
										}
								// 처음이 아닐 때
								if (pos - 2 >= 0)
									if (stMap[index][prev - 1].eMuni == CROSS)
										if (stMap[index][prev].eMuni == CROSS)
										{
											if (stMap[index][prev].iID == MY_ROCK_ID_2)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[index][prev].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[index][prev].iID == MY_ROCK_ID_3)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[index][prev].iID = MY_ROCK_ID_3_3;
											}
											else if (stMap[index][prev].iID == MY_ROCK_ID_4)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[index][prev].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[index][prev].iID == 0)
											{
												stMap[index][prev].iWeight += ROCK_PUT_THREE_NS;
												stMap[index][prev].iID = MY_ROCK_ID_3;
											}
										}

								if (eRockType == BLACK_ROCK)
								{
									if (stMap[index][next].iID == MY_ROCK_ID_3_3 || stMap[index][prev].iID == MY_ROCK_ID_3_3 )
									{
										stMap[index][next].iWeight = -100;
										stMap[index][prev].iWeight = -100;
									}
								}
							}
						}
					}
				}
				// 3개가 연속됬을 때
				else if (tmpCount == 3)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 4, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[index][j].eMuni == muni) _index++; else break;
					}
					// 연속된 돌이 1개 일 때
					if (_index == 1)
					{
						if (stMap[index][pos + 3].eMuni == CROSS)
							stMap[index][pos + 3].iWeight += ROCK_PUT_WIN;
					}
					else if (_index == 0)
					{
						int next = pos + 3; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if ((stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni != CROSS)
								|| stMap[index][prev].eMuni != CROSS && stMap[index][next].eMuni == CROSS)
							{
								if (stMap[index][next].eMuni == CROSS)
									if (next + 1 <= MAX_LENGTH_2018184040 - 1)
										if (stMap[index][next + 1].eMuni == CROSS || stMap[index][next + 1].eMuni == eRockType)
										{
											if (stMap[index][next].iID == MY_ROCK_ID_2)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[index][next].iID = MY_ROCK_ID_2_4;
											}
											else	if (stMap[index][next].iID == MY_ROCK_ID_3)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[index][next].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[index][next].iID == MY_ROCK_ID_4)
											{
												stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[index][next].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[index][next].iID == 0)
											{
												stMap[index][next].iWeight += ROCK_PUT_FOUR_PS;
												stMap[index][next].iID = MY_ROCK_ID_4;
											}
										}

								if (stMap[index][prev].eMuni == CROSS)
									if (prev - 1 >= 0)
										if (stMap[index][prev - 1].eMuni == CROSS || stMap[index][prev - 1].eMuni == eRockType)
										{
											if (stMap[index][prev].iID == MY_ROCK_ID_2)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[index][prev].iID = MY_ROCK_ID_2_4;
											}
											else	if (stMap[index][prev].iID == MY_ROCK_ID_3)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[index][prev].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[index][prev].iID == MY_ROCK_ID_4)
											{
												stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[index][prev].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[index][prev].iID == 0)
											{
												stMap[index][prev].iWeight += ROCK_PUT_FOUR_PS;
												stMap[index][prev].iID = MY_ROCK_ID_4;
											}
										}
							}
							else if (stMap[index][prev].eMuni == CROSS && stMap[index][next].eMuni == CROSS)
							{
								if (prev - 1 >= 0)
								{
									if (stMap[index][prev].eMuni == CROSS)
									{
										if (stMap[index][prev].iID == MY_ROCK_ID_2)
										{
											stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[index][prev].iID = MY_ROCK_ID_2_4;
										}
										else	if (stMap[index][prev].iID == MY_ROCK_ID_3)
										{
											stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[index][prev].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][prev].iID == MY_ROCK_ID_4)
										{
											stMap[index][prev].iWeight = (stMap[index][prev].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[index][prev].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[index][prev].iID == 0)
										{
											stMap[index][prev].iWeight += ROCK_PUT_FOUR_NS;
											stMap[index][prev].iID = MY_ROCK_ID_4;
										}
									}
								}
								if (next + 1 <= MAX_LENGTH_2018184040 - 1)
								{
									if (stMap[index][next].eMuni == CROSS)
									{
										if (stMap[index][next].iID == MY_ROCK_ID_2)
										{
											stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[index][next].iID = MY_ROCK_ID_2_4;
										}
										else	if (stMap[index][next].iID == MY_ROCK_ID_3)
										{
											stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[index][next].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][next].iID == MY_ROCK_ID_4)
										{
											stMap[index][next].iWeight = (stMap[index][next].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[index][next].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[index][next].iID == 0)
										{
											stMap[index][next].iWeight += ROCK_PUT_FOUR_NS;
											stMap[index][next].iID = MY_ROCK_ID_4;
										}
									}
								}
							}
							if (eRockType == BLACK_ROCK)
							{
								if (stMap[index][next].iID == MY_ROCK_ID_4_4 || stMap[index][prev].iID == MY_ROCK_ID_4_4 ||
									stMap[index][next].iID == MY_ROCK_ID_3_4 || stMap[index][prev].iID == MY_ROCK_ID_3_4)
								{
									stMap[index][next].iWeight = -100;
									stMap[index][prev].iWeight = -100;
								}
							}
						}
					}
				}
				// 4개 연속
				else if (tmpCount == 4)
				{
					int next = pos + 4, prev = pos - 1;
					if (next > (MAX_LENGTH_2018184040 - 1)) next = -100;
					if (prev < 0) prev = -100;

					if (prev != -100)
						if (stMap[index][prev].eMuni == CROSS) stMap[index][prev].iWeight += ROCK_PUT_WIN;
					if (next != -100)
						if (stMap[index][next].eMuni == CROSS) stMap[index][next].iWeight += ROCK_PUT_WIN;
				}
			}
			muni = NONE;
			if (i >= MAX_LENGTH_2018184040 - 1) { index++; k = 0; }
		}
	}
	// 돌이 놓일 위치를 정해주는 함수_세로축 검사
	void PutofStone_Horizontal(EMuni_2018184040 eRockType)
	{
		int index = 0, k = 0, i = 0;
		EMuni_2018184040 muni = NONE;
		int tmpCount = 0;
		while (index != MAX_LENGTH_2018184040)
		{
			for (i = k; i < MAX_LENGTH_2018184040; i++)
				if (stMap[i][index].eMuni == eRockType) { k = i; muni = stMap[i][index].eMuni; break; }

			if (muni == eRockType)
			{
				int n = 0, pos = k;
				tmpCount = 0;
				for (int j = pos; j < MAX_LENGTH_2018184040; j++)
				{
					if (stMap[j][index].eMuni == muni) { tmpCount++; }
					else	break;
				}
				if (tmpCount > 0) k += tmpCount;
				// 1인 경우
				if (tmpCount == 1)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 2, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[j][index].eMuni == muni) _index++; else break;
					}
					// 연속된 3개의 돌이 있을 때
					if (_index == 3)
					{
						if (stMap[pos + 1][index].eMuni == CROSS)
							stMap[pos + 1][index].iWeight += ROCK_PUT_WIN;
					}
					// 연속된 2개의 돌이 있을 때
					else if (_index == 2)
					{
						int next = pos + 4; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if ((stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni != CROSS) || (stMap[prev][index].eMuni != CROSS && stMap[next][index].eMuni == CROSS))
							{
								if (stMap[pos + 1][index].eMuni == CROSS)
								{
									if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[pos + 1][index].iID == 0)
									{
										stMap[pos + 1][index].iWeight += ROCK_PUT_FOUR_PS;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4;
									}
								}
							}
							else
							{
								if (stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni == CROSS)
								{
									if (stMap[pos + 1][index].eMuni == CROSS)
									{
										if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[pos + 1][index].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[pos + 1][index].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[pos + 1][index].iID == 0)
										{
											stMap[pos + 1][index].iWeight += ROCK_PUT_FOUR_NS;
											stMap[pos + 1][index].iID = MY_ROCK_ID_4;
										}
									}
								}
								if (eRockType == BLACK_ROCK)
								{
									if (stMap[next][index].iID == MY_ROCK_ID_4_4 || stMap[prev][index].iID == MY_ROCK_ID_4_4
										|| stMap[next][index].iID == MY_ROCK_ID_3_4 || stMap[prev][index].iID == MY_ROCK_ID_3_4)
									{
										stMap[next][index].iWeight = -100;
										stMap[prev][index].iWeight = -100;
									}
								}
							}
						}
						// 맨 끝 점중 한 곳일 때
						else if (prev == -100 && next != -100)
						{
							// 양끝이 막혀있지 않을 때
							if (stMap[next][index].eMuni == CROSS)
								if (stMap[pos + 1][index].eMuni == CROSS)
								{
									if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[pos + 1][index].iID == 0)
									{
										stMap[pos + 1][index].iWeight += ROCK_PUT_FOUR_PS;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4;
									}
								}
						}
						else if (next == -100 && prev != -100)
						{
							// 양끝이 막혀있지 않을 때
							if (stMap[prev][index].eMuni == CROSS)
								if (stMap[pos + 1][index].eMuni == CROSS)
								{
									if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[pos + 1][index].iID == 0)
									{
										stMap[pos + 1][index].iWeight += ROCK_PUT_FOUR_PS;
										stMap[pos + 1][index].iID = MY_ROCK_ID_4;
									}
								}
						}
					}
					// 연속된 1개의 돌이 있을 때
					else if (_index == 1)
					{
						int next = pos + 3; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if ((stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni != CROSS) || (stMap[prev][index].eMuni != CROSS && stMap[next][index].eMuni == CROSS))
							{
								// 양쪽 다 뚫려 있을 때
								if (stMap[pos + 1][index].eMuni == CROSS)
								{
									if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_2;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_3;
									}
									else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 1][index].iID == 0)
									{
										stMap[pos + 1][index].iWeight += ROCK_PUT_TWO_NS;
										stMap[pos + 1][index].iID = MY_ROCK_ID_2;
									}
								}
							}
							else
								if (stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni == CROSS)
								{
									// 양쪽 다 뚫려 있을 때
									if (stMap[pos + 1][index].eMuni == CROSS)
									{
										if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[pos + 1][index].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
											stMap[pos + 1][index].iID = MY_ROCK_ID_3_3;
										}
										else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
										{
											stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[pos + 1][index].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[pos + 1][index].iID == 0)
										{
											stMap[pos + 1][index].iWeight += ROCK_PUT_THREE_NS;
											stMap[pos + 1][index].iID = MY_ROCK_ID_3;
										}
									}
								}
						}
						else if (prev != -100 && next == -100)
						{
							if (stMap[prev][index].eMuni == CROSS)
								if (prev - 1 >= 0)
									if (stMap[prev - 1][index].eMuni == CROSS)
									{
										if (stMap[prev][index].iID == MY_ROCK_ID_2)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
											stMap[prev][index].iID = MY_ROCK_ID_2_2;
										}
										else if (stMap[prev][index].iID == MY_ROCK_ID_3)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[prev][index].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[prev][index].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[prev][index].iID == 0)
										{
											stMap[prev][index].iWeight += ROCK_PUT_TWO_NS;
											stMap[prev][index].iID = MY_ROCK_ID_2;
										}
									}
						}
					}
					// 연속된 0개의 돌이 있을 때
					else if (_index == 0)
					{
						int next = pos + 1; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						// 맨 끝위치 지점이 아닐 때
						if (prev != -100 && next != -100)
						{
							if (stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni == CROSS)
							{
								if (pos + 2 <= MAX_LENGTH_2018184040 - 1 && pos - 2 >= 0)
								{
									if ((stMap[pos + 2][index].eMuni == CROSS || stMap[pos + 2][index].eMuni == eRockType)
										&& (stMap[pos - 2][index].eMuni == CROSS || stMap[pos - 2][index].eMuni == eRockType))
									{
										if (pos + 3 <= MAX_LENGTH_2018184040 - 1)
										{
											if (stMap[pos + 3][index].eMuni == CROSS || stMap[pos + 3][index].eMuni == eRockType)
												if (stMap[pos + 1][index].eMuni == CROSS)
												{
													if (stMap[pos + 1][index].iID == MY_ROCK_ID_2)
													{
														stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
														stMap[pos + 1][index].iID = MY_ROCK_ID_2_2;
													}
													else if (stMap[pos + 1][index].iID == MY_ROCK_ID_3)
													{
														stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
														stMap[pos + 1][index].iID = MY_ROCK_ID_2_3;
													}
													else if (stMap[pos + 1][index].iID == MY_ROCK_ID_4)
													{
														stMap[pos + 1][index].iWeight = (stMap[pos + 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
														stMap[pos + 1][index].iID = MY_ROCK_ID_2_4;
													}
													else if (stMap[pos + 1][index].iID == 0)
													{
														stMap[pos + 1][index].iWeight += ROCK_PUT_TWO_NS;
														stMap[pos + 1][index].iID = MY_ROCK_ID_2;
													}
												}
										}
										if (pos - 3 >= 0)
										{
											if (stMap[pos - 3][index].eMuni == CROSS || stMap[pos - 3][index].eMuni == eRockType)
												if (stMap[pos - 1][index].eMuni == CROSS)
												{
													if (stMap[pos - 1][index].iID == MY_ROCK_ID_2)
													{
														stMap[pos - 1][index].iWeight = (stMap[pos - 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
														stMap[pos - 1][index].iID = MY_ROCK_ID_2_2;
													}
													else if (stMap[pos - 1][index].iID == MY_ROCK_ID_3)
													{
														stMap[pos - 1][index].iWeight = (stMap[pos - 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
														stMap[pos - 1][index].iID = MY_ROCK_ID_2_3;
													}
													else if (stMap[pos - 1][index].iID == MY_ROCK_ID_4)
													{
														stMap[pos - 1][index].iWeight = (stMap[pos - 1][index].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
														stMap[pos - 1][index].iID = MY_ROCK_ID_2_4;
													}
													else if (stMap[pos - 1][index].iID == 0)
													{
														stMap[pos - 1][index].iWeight += ROCK_PUT_TWO_NS;
														stMap[pos - 1][index].iID = MY_ROCK_ID_2;
													}
												}
										}
									}
								}
							}
						}
					}
				}
				// 2개 연속됬을 때
				else if (tmpCount == 2)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 3, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[j][index].eMuni == muni) _index++; else break;
					}
					// 두개 연속된 돌일 때
					if (_index == 2)
					{
						if (stMap[pos + 2][index].eMuni == CROSS)
							stMap[pos + 2][index].iWeight += ROCK_PUT_WIN;
					}
					// 한 개 연속된 될일 때
					else if (_index == 1)
					{
						int next = pos + 4; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if ((stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni != CROSS) || (stMap[prev][index].eMuni != CROSS && stMap[next][index].eMuni == CROSS))
							{
								if (stMap[pos + 2][index].eMuni == CROSS)
								{
									if (stMap[pos + 2][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[index][pos + 2].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[pos + 2][index].iID == 0)
									{
										stMap[pos + 2][index].iWeight += ROCK_PUT_FOUR_PS;
										stMap[pos + 2][index].iID = MY_ROCK_ID_4;
									}
								}
							}
							// 양쪽이 뚫려있을 때
							else
							{
								if (stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni == CROSS)
								{
									if (stMap[pos + 2][index].eMuni == CROSS)
									{
										if (stMap[pos + 2][index].iID == MY_ROCK_ID_2)
										{
											stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[pos + 2][index].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[pos + 2][index].iID == MY_ROCK_ID_3)
										{
											stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[pos + 2][index].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[index][pos + 2].iID == MY_ROCK_ID_4)
										{
											stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[pos + 2][index].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[index][pos + 2].iID == 0)
										{
											stMap[pos + 2][index].iWeight += ROCK_PUT_FOUR_NS;
											stMap[pos + 2][index].iID = MY_ROCK_ID_4;
										}
									}
								}
								if (eRockType == BLACK_ROCK)
								{
									if (stMap[next][index].iID == MY_ROCK_ID_4_4 || stMap[prev][index].iID == MY_ROCK_ID_4_4
										|| stMap[next][index].iID == MY_ROCK_ID_3_4 || stMap[prev][index].iID == MY_ROCK_ID_3_4)
									{
										stMap[next][index].iWeight = -100;
										stMap[prev][index].iWeight = -100;
									}
								}
							}
						}
						else if (prev == -100 && next != -100)
						{
							if (stMap[pos + 4][index].eMuni == CROSS)
								if (stMap[pos + 4][index].eMuni == CROSS)
								{
									if (stMap[pos + 2][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 2][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[pos + 2][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_4;
									}
									else if (stMap[pos + 2][index].iID == 0)
									{
										stMap[index][pos + 2].iWeight += ROCK_PUT_FOUR_PS;
										stMap[index][pos + 2].iID = MY_ROCK_ID_4;
									}
								}
						}
						else if (prev != -100 && next == -100)
						{
							// 다음이 막혀있다면
							if (stMap[prev][index].eMuni == CROSS)
								if (stMap[index][pos - 2].eMuni == CROSS)
								{
									if (stMap[pos + 2][index].iID == MY_ROCK_ID_2)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_2_4;
									}
									else if (stMap[pos + 2][index].iID == MY_ROCK_ID_3)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_3_4;
									}
									else if (stMap[pos + 2][index].iID == MY_ROCK_ID_4)
									{
										stMap[pos + 2][index].iWeight = (stMap[pos + 2][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
										stMap[pos + 2][index].iID = MY_ROCK_ID_4_4;
									}
									else if (stMap[pos + 2][index].iID == 0)
									{
										stMap[pos + 2][index].iWeight += ROCK_PUT_FOUR_PS;
										stMap[pos + 2][index].iID = MY_ROCK_ID_4;
									}
								}
						}
					}
					else if (_index == 0)
					{
						int next = pos + 2; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if (stMap[index][next].eMuni == CROSS && stMap[index][prev].eMuni == CROSS)
							{
								// 맨 끝이 아닐 때
								if (pos + 3 <= MAX_LENGTH_2018184040 - 1)
									if (stMap[next + 1][index].eMuni == CROSS)
										if (stMap[next][index].eMuni == CROSS)
										{
											if (stMap[next][index].iID == MY_ROCK_ID_2)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[next][index].iID = MY_ROCK_ID_2_3;
											}
											else	if (stMap[next][index].iID == MY_ROCK_ID_3)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[next][index].iID = MY_ROCK_ID_3_3;
											}
											else	if (stMap[next][index].iID == MY_ROCK_ID_4)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[next][index].iID = MY_ROCK_ID_3_4;
											}
											else	if (stMap[next][index].iID == 0)
											{
												stMap[next][index].iWeight += ROCK_PUT_THREE_NS;
												stMap[next][index].iID += MY_ROCK_ID_3;
											}
										}
								// 처음이 아닐 때
								if (pos - 2 >= 0)
									if (stMap[prev - 1][index].eMuni == CROSS)
										if (stMap[prev][index].eMuni == CROSS)
										{
											if (stMap[prev][index].iID == MY_ROCK_ID_2)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[prev][index].iID = MY_ROCK_ID_2_2;
											}
											else	if (stMap[prev][index].iID == MY_ROCK_ID_3)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[prev][index].iID = MY_ROCK_ID_3_3;
											}
											else	if (stMap[prev][index].iID == MY_ROCK_ID_4)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[prev][index].iID = MY_ROCK_ID_3_4;
											}
											else	if (stMap[prev][index].iID == 0)
											{
												stMap[prev][index].iWeight += ROCK_PUT_THREE_NS;
												stMap[prev][index].iID += MY_ROCK_ID_3;
											}
										}
								if (eRockType == BLACK_ROCK)
								{
									if (stMap[next][index].iID == MY_ROCK_ID_3_3 || stMap[prev][index].iID == MY_ROCK_ID_3_3)
									{
										stMap[next][index].iWeight = -100;
										stMap[prev][index].iWeight = -100;
									}
								}
							}
						}
					}
				}
				// 3개가 연속됬을 때
				else if (tmpCount == 3)
				{
					int _index = -1, prev = pos - 1; if (prev < 0) prev = -100;
					int startPos = pos + 4, nextPos = pos + 4;
					if (pos <= (MAX_LENGTH_2018184040 - 1))
					{
						_index = 0;
						if (nextPos > MAX_LENGTH_2018184040 - 1) nextPos = MAX_LENGTH_2018184040 - 1;
						if (startPos <= MAX_LENGTH_2018184040 - 1)
							for (int j = startPos; j <= nextPos; j++)
								if (stMap[j][index].eMuni == muni) _index++; else break;
					}
					// 연속된 돌이 1개 일 때
					if (_index == 1)
					{
						if (stMap[pos + 3][index].eMuni == CROSS)
							stMap[pos + 3][index].iWeight += ROCK_PUT_WIN;
					}
					else if (_index == 0)
					{
						int next = pos + 3; if (next > MAX_LENGTH_2018184040 - 1) next = -100;
						if (prev != -100 && next != -100)
						{
							if ((stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni != CROSS)
								|| stMap[prev][index].eMuni != CROSS && stMap[next][index].eMuni == CROSS)
							{
								if (stMap[next][index].eMuni == CROSS)
									if (next + 1 <= MAX_LENGTH_2018184040 - 1)
										if (stMap[next + 1][index].eMuni == CROSS || stMap[next + 1][index].eMuni == eRockType)
										{
											if (stMap[next][index].iID == MY_ROCK_ID_2)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[next][index].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[next][index].iID == MY_ROCK_ID_3)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[next][index].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[next][index].iID == MY_ROCK_ID_4)
											{
												stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[next][index].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[next][index].iID == 0)
											{
												stMap[next][index].iWeight += ROCK_PUT_FOUR_PS;
												stMap[next][index].iID = MY_ROCK_ID_4;
											}
										}

								if (stMap[prev][index].eMuni == CROSS)
									if (prev - 1 >= 0)
										if (stMap[prev - 1][index].eMuni == CROSS || stMap[prev - 1][index].eMuni == eRockType)
										{
											if (stMap[prev][index].iID == MY_ROCK_ID_2)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[prev][index].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[prev][index].iID == MY_ROCK_ID_3)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[prev][index].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[prev][index].iID == MY_ROCK_ID_4)
											{
												stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[prev][index].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[prev][index].iID == 0)
											{
												stMap[prev][index].iWeight += ROCK_PUT_FOUR_PS;
												stMap[prev][index].iID = MY_ROCK_ID_4;
											}
										}
							}
							else if (stMap[prev][index].eMuni == CROSS && stMap[next][index].eMuni == CROSS)
							{
								if (prev - 1 >= 0)
								{
									if (stMap[prev][index].eMuni == CROSS)
									{
										if (stMap[prev][index].iID == MY_ROCK_ID_2)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[prev][index].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[prev][index].iID == MY_ROCK_ID_3)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[prev][index].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[prev][index].iID == MY_ROCK_ID_4)
										{
											stMap[prev][index].iWeight = (stMap[prev][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[prev][index].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[prev][index].iID == 0)
										{
											stMap[prev][index].iWeight += ROCK_PUT_FOUR_NS;
											stMap[prev][index].iID = MY_ROCK_ID_4;
										}
									}
								}
								if (next + 1 <= MAX_LENGTH_2018184040 - 1)
								{
									if (stMap[next][index].eMuni == CROSS)
									{
										if (stMap[next][index].iID == MY_ROCK_ID_2)
										{
											stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[next][index].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[next][index].iID == MY_ROCK_ID_3)
										{
											stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[next][index].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[next][index].iID == MY_ROCK_ID_4)
										{
											stMap[next][index].iWeight = (stMap[next][index].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[next][index].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[next][index].iID == 0)
										{
											stMap[next][index].iWeight += ROCK_PUT_FOUR_NS;
											stMap[next][index].iID = MY_ROCK_ID_4;
										}
									}
								}
							}
						}
						if (eRockType == BLACK_ROCK)
						{
							if (stMap[next][index].iID == MY_ROCK_ID_4_4 || stMap[prev][index].iID == MY_ROCK_ID_4_4
								|| stMap[next][index].iID == MY_ROCK_ID_3_4 || stMap[prev][index].iID == MY_ROCK_ID_3_4)
							{
								stMap[next][index].iWeight = -100;
								stMap[prev][index].iWeight = -100;
							}
						}
					}
				}
				// 4개 연속
				else if (tmpCount == 4)
				{
					int next = pos + 4, prev = pos - 1;
					if (next > (MAX_LENGTH_2018184040 - 1)) next = -100;
					if (prev < 0) prev = -100;

					if (prev != -100)
						if (stMap[prev][index].eMuni == CROSS) stMap[prev][index].iWeight += ROCK_PUT_WIN;
					if (next != -100)
						if (stMap[next][index].eMuni == CROSS) stMap[next][index].iWeight += ROCK_PUT_WIN;
				}
			}
			muni = NONE;
			if (i >= MAX_LENGTH_2018184040 - 1) {
				index++;
				k = 0;
			}
		}
	}
	// 돌이 놓일 위치를 정해주는함수_좌상향 검사
	void PutofStone_LeftUp(EMuni_2018184040 eRockType)
	{
		const int Length = MAX_LENGTH_2018184040 * 2 - 1;
		int k = 0, i = 0, length = 0, x = 0, y = 0, pmValue = 1, index = 0, save = 0;
		EMuni_2018184040 muni = NONE;
		int tmpCount = 0;
		// 연속으로 되어있는지 체크
		while (index != Length)
		{
			for (i = k; i <= length; i++)
			{
				if (stMap[y][x].eMuni == eRockType) { muni = stMap[y][x].eMuni; k = i; break; }
				else	x--; y++;
			}
			if (length >= 4)
			{
				if (muni == eRockType)
				{
					int pos = k, _x = x, _y = y;
					tmpCount = 0;
					for (int j = pos; j <= length; j++)
					{
						if (stMap[y][x].eMuni == muni) { tmpCount++; }
						else  break;
						x--; y++;
					}
					if (tmpCount > 0) k += tmpCount;
					// 1개
					if (tmpCount == 1)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1;
						tmpX -= 1; tmpY += 1;
						if (prev < 0) prev = -100;
						_index = 0;
						int value = pos + 1;
						for (int j = 0; j < 4; j++)
						{
							if (value > length) break;
							if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY++;
						}
						// 앞에 연속된 돌이 3개 있을 경우
						if (_index == 3)
						{
							if (stMap[_y + 1][_x - 1].eMuni == CROSS)
								stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_WIN;
						}
						// 앞에 연속된 돌이 2개 있을 경우
						else if (_index == 2)
						{
							int next = pos + 4;
							if (next > length) next = -100;
							// 양쪽이 모두 끝쪽이 아닐 때
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 4][_x - 4].eMuni != CROSS) || (stMap[_y - 1][_x + 1].eMuni != CROSS && stMap[_y + 4][_x - 4].eMuni == CROSS))
								{
									if (stMap[_y + 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == 0)
										{
											stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
								}
								else
								{
									if (stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 4][_x - 4].eMuni == CROSS)
									{
										if (stMap[_y + 1][_x - 1].eMuni == CROSS)
										{
											if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y + 1][_x - 1].iID == 0)
											{
												stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4;
											}
										}
									}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_3_4
											|| stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y - 1][_x + 1].iWeight = -100;
											stMap[_y + 3][_x - 3].iWeight = -100;
										}
									}
								}
							}
							// 맨 끝 점중 한 곳일 때
							else if (prev == -100 && next != -100)
							{
								// 양끝이 막혀있지 않을 때
								if (stMap[_y + 4][_x - 4].eMuni == CROSS)
									if (stMap[_y + 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == 0)
										{
											stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
							}
							else if (next == -100 && prev != -100)
							{
								// 양끝이 막혀있지 않을 때
								if (stMap[_y - 1][_x + 1].eMuni == CROSS)
									if (stMap[_y + 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 1][_x - 1].iID == 0)
										{
											stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
							}
						}
						// 앞에 연속된 돌이 1개 있을 경우
						else if (_index == 1)
						{
							int next = pos + 3; if (next > length) next = -100;
							// 맨 끝위치 지점이 아닐 때
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 3][_x - 3].eMuni != CROSS) || (stMap[_y - 1][_x + 1].eMuni != CROSS && stMap[_y + 3][_x - 3].eMuni == CROSS))
								{
									if (stMap[_y - 1][_x + 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
											stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_2;
										}
										else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 1][_x + 1].iID == 0)
										{
											stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_TWO_NS;
											stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2;
										}
									}
								}
								else
								{
									if (stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 3][_x - 3].eMuni == CROSS)
									{
										// 양쪽 다 뚫려 있을 때
										if (stMap[_y - 1][_x + 1].eMuni == CROSS)
										{
											if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3_3;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 1][_x + 1].iID == 0)
											{
												stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_THREE_NS;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3;
											}
										}
									}
								}
							}
							else if (prev != -100 && next == -100)
							{
								if (stMap[_y - 1][_x + 1].eMuni == CROSS)
									if (prev - 1 >= 0)
										if (stMap[_y - 2][_x + 2].eMuni == CROSS)
										{
											if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_2;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_4;
											}
											else if (stMap[_y - 1][_x + 1].iID == 0)
											{
												stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_TWO_NS;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2;
											}
										}
							}
						}
						else if (_index == 0)
						{
							int next = pos + 1; if (next > length) next = -100;
							// 맨 끝위치 지점이 아닐 때
							if (prev != -100 && next != -100)
							{
								if (stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 1][_x - 1].eMuni == CROSS)
								{
									if (pos + 2 <= length && pos - 2 >= 0)
									{
										if ((stMap[_y + 2][_x - 2].eMuni == CROSS || stMap[_y + 2][_x - 2].eMuni == eRockType) &&
											(stMap[_y - 2][_x + 2].eMuni == CROSS || stMap[_y - 2][_x + 2].eMuni == eRockType))
										{
											if (pos + 3 <= length)
												if (stMap[_y + 3][_x - 3].eMuni == CROSS || stMap[_y + 3][_x - 3].eMuni == eRockType)
													if (stMap[_y + 1][_x - 1].eMuni == CROSS)
													{
														if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_2)
														{
															stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_2;
														}
														else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_3)
														{
															stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_3;
														}
														else if (stMap[_y + 1][_x - 1].iID == MY_ROCK_ID_4)
														{
															stMap[_y + 1][_x - 1].iWeight = (stMap[_y + 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
														}
														else if (stMap[_y + 1][_x - 1].iID == 0)
														{
															stMap[_y + 1][_x - 1].iWeight += ROCK_PUT_TWO_NS;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2;
														}
													}

											if (pos - 3 >= 0)
												if (stMap[_y - 3][_x + 3].eMuni == CROSS || stMap[_y - 3][_x + 3].eMuni == eRockType)
													if (stMap[_y - 1][_x + 1].eMuni == CROSS)
													{
														if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
														{
															stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_2;
														}
														else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
														{
															stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_3;
														}
														else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
														{
															stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
															stMap[_y + 1][_x - 1].iID = MY_ROCK_ID_2_4;
														}
														else if (stMap[_y - 1][_x + 1].iID == 0)
														{
															stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_TWO_NS;
															stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2;
														}
													}
										}
									}
								}
							}
						}
					}
					// 2개 연속됬을 때
					else if (tmpCount == 2)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1;
						tmpX -= 2; tmpY += 2;
						if (prev < 0) prev = -100;	_index = 0;
						int value = pos + 2;
						for (int j = 0; j < 3; j++)
						{
							if (value > length) break;	if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY++;
						}
						// 두개 연속된 돌일 때
						if (_index == 2)
						{
							if (stMap[_y + 2][x - 2].eMuni == CROSS)
								stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_WIN;
						}
						// 한 개 연속된 될일 때
						else if (_index == 1)
						{
							int next = pos + 4; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 4][_x - 4].eMuni != CROSS) || (stMap[_y - 1][_x + 1].eMuni != CROSS && stMap[_y + 4][_x - 4].eMuni == CROSS))
								{
									if (stMap[_y + 2][_x - 2].eMuni == CROSS)
									{
										if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == 0)
										{
											stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4;
										}
									}
								}
								// 양쪽이 뚫려있을 때
								else
								{
									if (stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 4][_x - 4].eMuni == CROSS)
									{
										if (stMap[_y + 2][_x - 2].eMuni == CROSS)
										{
											if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_2)
											{
												stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3)
											{
												stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_4)
											{
												stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y + 2][_x - 2].iID == 0)
											{
												stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4;
											}
										}
									}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_3_4
											|| stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y - 1][_x + 1].iWeight = -100;
											stMap[_y + 3][_x - 3].iWeight = -100;
										}
									}
								}
							}
							else if (prev == -100 && next != -100)
							{
								if (stMap[_y + 4][_x - 4].eMuni == CROSS)
									if (stMap[_y + 2][_x - 2].eMuni == CROSS)
									{
										if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == 0)
										{
											stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4;
										}
									}
							}
							else if (prev != -100 && next == -100)
								// 다음이 막혀있다면
								if (stMap[_y - 1][_x + 1].eMuni == CROSS)
									if (stMap[_y + 2][_x - 2].eMuni == CROSS)
									{
										if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y + 2][_x - 2].iID == 0)
										{
											stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_4;
										}
									}
						}
						else if (_index == 0)
						{
							int next = pos + 2; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if (stMap[_y + 2][_x - 2].eMuni == CROSS && stMap[_y - 1][_x + 1].eMuni == CROSS)
								{
									// 맨 끝이 아닐 때
									if (pos + 3 <= length)
										if (stMap[_y + 3][_x - 3].eMuni == CROSS)
											if (stMap[_y + 2][_x - 2].eMuni == CROSS)
											{
												if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_2)
												{
													stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
													stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_2_3;
												}
												else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3)
												{
													stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
													stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_3;
												}
												else if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_4)
												{
													stMap[_y + 2][_x - 2].iWeight = (stMap[_y + 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y + 2][_x - 2].iID == 0)
												{
													stMap[_y + 2][_x - 2].iWeight += ROCK_PUT_THREE_NS;
													stMap[_y + 2][_x - 2].iID = MY_ROCK_ID_3;
												}
											}
									// 처음이 아닐 때
									if (pos - 2 >= 0)
										if (stMap[_y - 2][_x + 2].eMuni == CROSS)
											if (stMap[_y - 1][_x + 1].eMuni == CROSS)
											{
												if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_3;
												}
												else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
													stMap[_y - 2][_x + 1].iID = MY_ROCK_ID_3_3;
												}
												else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y - 1][_x + 1].iID == 0)
												{
													stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_THREE_NS;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3;
												}
											}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y + 2][_x - 2].iID == MY_ROCK_ID_3_3 || stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3_3)
										{
											stMap[_y + 2][_x - 2].iWeight = -100;
											stMap[_y - 1][_x + 1].iWeight = -100;
										}
									}
								}
							}
						}
					}
					else if (tmpCount == 3)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1, value = pos + 3;
						tmpX -= 3; tmpY += 3;
						if (prev < 0) prev = -100;	_index = 0;
						for (int j = 0; j < 2; j++)
						{
							if (value > length - 1) break;
							if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY++;
						}
						// 연속된 돌이 1개 일 때
						if (_index == 1)
						{
							if (stMap[_y + 3][_x - 3].eMuni == CROSS)
								stMap[_y + 3][_x - 3].iWeight += ROCK_PUT_WIN;
						}
						else if (_index == 0)
						{
							int next = pos + 3; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 3][_x - 3].eMuni != CROSS)
									|| stMap[_y - 1][_x + 1].eMuni != CROSS && stMap[_y + 3][_x - 3].eMuni == CROSS)
								{

									if (stMap[_y + 3][_x - 3].eMuni == CROSS)
										if (next + 1 <= MAX_LENGTH_2018184040 - 1)
											if (stMap[_y + 4][_x - 4].eMuni == CROSS || stMap[_y + 4][_x - 4].eMuni == eRockType)
											{
												if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_2)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_2_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_3)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_4)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_4_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == 0)
												{
													stMap[_y + 3][_x - 3].iWeight += ROCK_PUT_FOUR_PS;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_4;
												}
											}

									if (stMap[_y - 1][_x + 1].eMuni == CROSS)
										if (prev - 1 >= 0)
											if (stMap[_y - 2][_x + 2].eMuni == CROSS || stMap[_y - 2][_x + 2].eMuni == eRockType)
											{
												if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_4;
												}
												else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
												{
													stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_4_4;
												}
												else if (stMap[_y - 1][_x + 1].iID == 0)
												{
													stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_FOUR_PS;
													stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_4;
												}
											}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_3_4
											|| stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y - 1][_x + 1].iWeight = -100;
											stMap[_y + 3][_x - 3].iWeight = -100;
										}
									}
								}
								else if (stMap[_y - 1][_x + 1].eMuni == CROSS && stMap[_y + 3][_x - 3].eMuni == CROSS)
								{
									if (prev - 1 >= 0)
									{
										if (stMap[_y - 2][_x + 2].eMuni == CROSS)
										{
											if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 1][_x + 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 1][_x + 1].iWeight = (stMap[_y - 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y - 1][_x + 1].iID == 0)
											{
												stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_4;
											}
										}
									}
									if (next + 1 <= length)
										if (stMap[_y + 40][_x - 4].eMuni == CROSS)
										{
											if (stMap[_y + 3][_x - 3].eMuni == CROSS)
											{
												if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_2)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_2_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_3)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == MY_ROCK_ID_4)
												{
													stMap[_y + 3][_x - 3].iWeight = (stMap[_y + 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_4_4;
												}
												else if (stMap[_y + 3][_x - 3].iID == 0)
												{
													stMap[_y + 3][_x - 3].iWeight += ROCK_PUT_FOUR_NS;
													stMap[_y + 3][_x - 3].iID = MY_ROCK_ID_4;
												}
											}
										}
								}
							}
						}
					}
					// 4개 연속
					else if (tmpCount == 4)
					{
						int next = pos + 4, prev = pos - 1;
						if (next > (length)) next = -100;
						if (prev < 0) prev = -100;

						if (prev != -100)  if (stMap[_y - 1][_x + 1].eMuni == CROSS) stMap[_y - 1][_x + 1].iWeight += ROCK_PUT_WIN;
						if (next != -100)  if (stMap[_y + 4][_x - 4].eMuni == CROSS) stMap[_y + 4][_x - 4].iWeight += ROCK_PUT_WIN;
					}
				}
			}
			else length = i;

			tmpCount = 0;
			muni = NONE;
			if (i >= length)
			{
				k = 0;
				if (length == MAX_LENGTH_2018184040 - 1) { pmValue = -1; }	index++;	length += pmValue;
				if (pmValue == 1) { x = length;	if (length >= MAX_LENGTH_2018184040) x -= 1;	y = 0; }
				else if (pmValue == -1) { x = MAX_LENGTH_2018184040 - 1;	save += 1;	y = save; }
			}
		}
	}
	// 돌이 놓일 위치를 정해주는함수_우상향 검사
	void PutofStone_RightUp(EMuni_2018184040 eRockType)
	{
		const int Length = MAX_LENGTH_2018184040 * 2 - 1;
		int k = 0, i = 0, length = 0, x = MAX_LENGTH_2018184040 - 1, y = 0, pmValue = 1, index = 0, save = 0;
		EMuni_2018184040 muni = NONE;
		int tmpCount = 0;
		// 각 열마다 검은색, 휜색 돌이 얼마나 연속으로 놓여있는지에 대한 코드
		while (index != Length)
		{
			for (i = k; i <= length; i++)
			{
				if (stMap[y][x].eMuni == eRockType) { muni = eRockType; k = i; break; }
				x--;	y--;
			}
			if (muni == eRockType)
			{
				int pos = k, _x = x, _y = y;
				tmpCount = 0;
				for (int j = k; j <= length; j++)
				{
					if (stMap[y][x].eMuni == muni) { tmpCount++; }
					else  break;
					x--;	y--;
				}
				if (tmpCount >= 0) k += tmpCount;
				if (length >= 4)
				{
					// 1개
					if (tmpCount == 1)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1;
						tmpX -= 1; tmpY -= 1;
						if (prev < 0) prev = -100;	_index = 0;
						int value = pos + 1;
						for (int j = 0; j < 4; j++)
						{
							if (value > length - 1) break;
							if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY--;
						}
						// 앞에 연속된 돌이 3개 있을 경우
						if (_index == 3)
						{
							if (stMap[_y - 1][_x - 1].eMuni == CROSS)
								stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_WIN;
						}
						// 앞에 연속된 돌이 2개 있을 경우
						else if (_index == 2)
						{
							int next = pos + 4; if (next > length) next = -100;
							// 양쪽이 모두 끝쪽이 아닐 때
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 4][_x - 4].eMuni != CROSS) || (stMap[_y + 1][_x + 1].eMuni != CROSS && stMap[_y - 4][_x - 4].eMuni == CROSS))
								{
									if (stMap[_y - 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == 0)
										{
											stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
								}
								else
								{
									if (stMap[_y - 1][_x - 1].eMuni == CROSS && stMap[_y + 1][_x + 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == 0)
										{
											stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_FOUR_NS;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_3_4 ||
											stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y + 1][_x + 1].iWeight = -100;
											stMap[_y - 3][_x - 3].iWeight = -100;
										}
									}
								}
							}
							// 맨 끝 점중 한 곳일 때
							else if (prev == -100 && next != -100)
							{
								// 양끝이 막혀있지 않을 때
								if (stMap[_y - 4][_x - 4].eMuni == CROSS)
									if (stMap[_y - 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == 0)
										{
											stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
							}
							else if (next == -100 && prev != -100)
							{
								// 양끝이 막혀있지 않을 때
								if (stMap[_y + 1][_x + 1].eMuni == CROSS)
									if (stMap[_y - 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == 0)
										{
											stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4;
										}
									}
							}
						}
						// 앞에 연속된 돌이 1개 있을 경우
						else if (_index == 1)
						{
							int next = pos + 3; if (next > length) next = -100;
							// 맨 끝위치 지점이 아닐 때
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 3][_x - 3].eMuni != CROSS) || (stMap[_y + 1][_x + 1].eMuni != CROSS && stMap[_y - 3][_x - 3].eMuni == CROSS))
								{
									if (stMap[_y - 1][_x - 1].eMuni == CROSS)
									{
										if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_2;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
											stMap[_y - 1][_x + 1].iID = MY_ROCK_ID_2_3;
										}
										else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 1][_x - 1].iID == 0)
										{
											stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_TWO_NS;
											stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2;
										}
									}
								}
								else
								{
									if (stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 3][_x - 3].eMuni == CROSS)
									{
										// 양쪽 다 뚫려 있을 때
										if (stMap[_y - 1][_x - 1].eMuni == CROSS)
										{
											if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
												stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_3;
											}
											else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 1][_x - 1].iID == 0)
											{
												stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_THREE_NS;
												stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_3;
											}
										}
									}
								}
							}
							else if (prev == -100 && next != -100)
							{
								if (stMap[_y + 1][_x + 1].eMuni == CROSS)
									if (prev - 1 >= 0)
										if (stMap[_y + 2][_x + 2].eMuni == CROSS)
										{
											if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_2;
											}
											else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_3;
											}
											else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y - 1][_x - 1].iID == 0)
											{
												stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_TWO_NS;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2;
											}
										}
							}
						}
						else if (_index == 0)
						{
							int next = pos + 1; if (next > length) next = -100;
							// 맨 끝위치 지점이 아닐 때
							if (prev != -100 && next != -100)
							{
								if (stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 1][_x - 1].eMuni == CROSS)
								{
									if (pos + 2 <= length && pos - 2 >= 0)
									{
										if ((stMap[_y + 2][_x + 2].eMuni == CROSS || stMap[_y + 2][_x + 2].eMuni == eRockType) &&
											(stMap[_y - 2][_x - 2].eMuni == CROSS || stMap[_y - 2][_x - 2].eMuni == eRockType))
										{
											if (pos + 3 <= length)
												if (stMap[_y - 3][_x - 3].eMuni == CROSS || stMap[_y - 3][_x - 3].eMuni == eRockType)
													if (stMap[_y - 1][_x - 1].eMuni == CROSS)
													{
														if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_2)
														{
															stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
															stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_2;
														}
														else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_3)
														{
															stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
															stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_3;
														}
														else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
														{
															stMap[_y - 1][_x - 1].iWeight = (stMap[_y - 1][_x - 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
															stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2_4;
														}
														else if (stMap[_y - 1][_x - 1].iID == 0)
														{
															stMap[_y - 1][_x - 1].iWeight += ROCK_PUT_TWO_NS;
															stMap[_y - 1][_x - 1].iID = MY_ROCK_ID_2;
														}
													}

											if (pos - 3 >= 0)
												if (stMap[_y + 3][_x + 3].eMuni == CROSS || stMap[_y + 3][_x + 3].eMuni == eRockType)
													if (stMap[_y + 1][_x + 1].eMuni == CROSS)
													{
														if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_2)
														{
															stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_2;
															stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_2;
														}
														else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3)
														{
															stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_3;
															stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_3;
														}
														else if (stMap[_y - 1][_x - 1].iID == MY_ROCK_ID_4)
														{
															stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_TWO_NS) * ROCK_MULTIPLE_SITUATION_2_4;
															stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_4;
														}
														else if (stMap[_y - 1][_x - 1].iID == 0)
														{
															stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_TWO_NS;
															stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2;
														}
													}
										}
									}
								}
							}
						}
					}
					else if (tmpCount == 2)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1;
						tmpX -= 2; tmpY -= 2;
						if (prev < 0) prev = -100;	_index = 0;
						int value = pos + 2;
						for (int j = 0; j < 3; j++)
						{
							if (value > length) break;	if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY--;
						}
						// 두개 연속된 돌일 때
						if (_index == 2)
						{
							if (stMap[_y - 2][_x - 2].eMuni == CROSS)
							{
								stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_WIN;
							}
						}
						// 한 개 연속된 될일 때
						else if (_index == 1)
						{
							int next = pos + 4; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 4][_x - 4].eMuni != CROSS) || (stMap[_y + 1][_x + 1].eMuni != CROSS && stMap[_y - 4][_x - 4].eMuni == CROSS))
								{
									if (stMap[_y - 2][_x - 2].eMuni == CROSS)
									{
										if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == 0)
										{
											stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4;
										}
									}
								}
								// 양쪽이 뚫려있을 때
								else
								{
									if (stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 4][_x - 4].eMuni == CROSS)
									{
										if (stMap[_y - 2][_x - 2].eMuni == CROSS)
										{
											if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y - 2][_x - 2].iID == 0)
											{
												stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4;
											}
										}
									}
									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_3_4 ||
											stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y + 1][_x + 1].iWeight = -100;
											stMap[_y - 3][_x - 3].iWeight = -100;
										}
									}
								}
							}
							else if (prev == -100 && next != -100)
							{
								if (stMap[_y - 4][_x - 4].eMuni == CROSS)
									if (stMap[_y - 2][_x - 2].eMuni == CROSS)
									{
										if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == 0)
										{
											stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4;
										}
									}
							}
							else if (prev != -100 && next == -100)
								// 다음이 막혀있다면
								if (stMap[_y + 1][_x + 1].eMuni == CROSS)
									if (stMap[_y - 2][_x - 2].eMuni == CROSS)
										if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_2)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_2_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_4)
										{
											stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4_4;
										}
										else if (stMap[_y - 2][_x - 2].iID == 0)
										{
											stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_FOUR_PS;
											stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_4;
										}
						}
						else if (_index == 0)
						{
							int next = pos + 2; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if (stMap[_y - 2][_x - 2].eMuni == CROSS && stMap[_y + 1][_x + 1].eMuni == CROSS)
								{
									// 맨 끝이 아닐 때
									if (pos + 3 <= length)
										if (stMap[_y - 3][_x - 3].eMuni == CROSS)
											if (stMap[_y - 2][_x - 2].eMuni == CROSS)
											{
												if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_2)
												{
													stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
													stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_2_3;
												}
												else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3)
												{
													stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
													stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_3;
												}
												else if (stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_4)
												{
													stMap[_y - 2][_x - 2].iWeight = (stMap[_y - 2][_x - 2].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y - 2][_x - 2].iID == 0)
												{
													stMap[_y - 2][_x - 2].iWeight += ROCK_PUT_THREE_NS;
													stMap[_y - 2][_x - 2].iID = MY_ROCK_ID_3;
												}
											}
									// 처음이 아닐 때
									if (pos - 2 >= 0)
										if (stMap[_y + 2][_x + 2].eMuni == CROSS)
											if (stMap[_y + 1][_x + 1].eMuni == CROSS)
											{
												if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_2)
												{
													stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_2_3;
													stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_3;
												}
												else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3)
												{
													stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_3;
													stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_3_3;
												}
												else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4)
												{
													stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_THREE_NS) * ROCK_MULTIPLE_SITUATION_3_4;
													stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_3_4;
												}
												else if (stMap[_y + 1][_x + 1].iID == 0)
												{
													stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_THREE_NS;
													stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_3;
												}
											}

									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3_3 || stMap[_y - 2][_x - 2].iID == MY_ROCK_ID_3_3)
										{
											stMap[_y + 1][_x + 1].iWeight = -100;
											stMap[_y - 2][_x - 2].iWeight = -100;
										}
									}
								}
							}
						}
					}
					else if (tmpCount == 3)
					{
						int _index = -1, tmpX = _x, tmpY = _y, prev = pos - 1, value = pos + 3;
						tmpX -= 3; tmpY -= 3;
						if (prev < 0) prev = -100;	_index = 0;
						for (int j = 0; j < 2; j++)
						{
							if (value > length - 1) break;
							if (stMap[tmpY][tmpX].eMuni == muni) _index++;
							value++;
							tmpX--; tmpY--;
						}
						// 연속된 돌이 1개 일 때
						if (_index == 1)
						{
							if (stMap[_y - 3][_x - 3].eMuni == CROSS)
								stMap[_y - 3][_x - 3].iWeight += ROCK_PUT_WIN;
						}
						else if (_index == 0)
						{
							int next = pos + 3; if (next > length) next = -100;
							if (prev != -100 && next != -100)
							{
								if ((stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 3][_x - 3].eMuni != CROSS)
									|| stMap[_y + 1][_x + 1].eMuni != CROSS && stMap[_y - 3][_x - 3].eMuni == CROSS)
								{
									if (next + 1 <= length)
										if (stMap[_y - 3][_x - 3].eMuni == CROSS)
										{
											if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == 0)
											{
												stMap[_y - 3][_x - 3].iWeight += ROCK_PUT_FOUR_PS;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_4;
											}
										}
									if (prev - 1 >= 0)
										if (stMap[_y + 1][_x + 1].eMuni == CROSS)
										{
											if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_PS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == 0)
											{
												stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_FOUR_PS;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_4;
											}
										}

									if (eRockType == BLACK_ROCK)
									{
										if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_3_4 || 
											stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4_4 || stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_4_4)
										{
											stMap[_y + 1][_x + 1].iWeight = -100;
											stMap[_y - 3][_x - 3].iWeight = -100;
										}
									}
								}
								else if (stMap[_y + 1][_x + 1].eMuni == CROSS && stMap[_y - 3][_x - 3].eMuni == CROSS)
								{
									if (prev - 1 >= 0)
									{
										if (stMap[_y + 1][_x + 1].eMuni == CROSS)
										{
											if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_2)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_3)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == MY_ROCK_ID_4)
											{
												stMap[_y + 1][_x + 1].iWeight = (stMap[_y + 1][_x + 1].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y + 1][_x + 1].iID == 0)
											{
												stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y + 1][_x + 1].iID = MY_ROCK_ID_4;
											}
										}
									}
									if (next + 1 <= MAX_LENGTH_2018184040 - 1)
									{
										if (stMap[_y - 3][_x - 3].eMuni == CROSS)
										{
											if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_2)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_2_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_2_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_3)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_3_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_3_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == MY_ROCK_ID_4)
											{
												stMap[_y - 3][_x - 3].iWeight = (stMap[_y - 3][_x - 3].iWeight + ROCK_PUT_FOUR_NS) * ROCK_MULTIPLE_SITUATION_4_4;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_4_4;
											}
											else if (stMap[_y - 3][_x - 3].iID == 0)
											{
												stMap[_y - 3][_x - 3].iWeight += ROCK_PUT_FOUR_NS;
												stMap[_y - 3][_x - 3].iID = MY_ROCK_ID_4;
											}
										}
									}
								}

							}
						}
					}
					else if (tmpCount == 4)
					{
						int next = pos + 4, prev = pos - 1;
						if (next > (length)) next = -100;
						if (prev < 0) prev = -100;

						if (prev != -100)
							if (stMap[_y + 1][_x + 1].eMuni == CROSS) stMap[_y + 1][_x + 1].iWeight += ROCK_PUT_WIN;
						if (next != -100)
							if (stMap[_y - 4][_x - 4].eMuni == CROSS) stMap[_y - 4][_x - 4].iWeight += ROCK_PUT_WIN;
					}
				}
				else i = length;
			}
			muni = NONE;
			if (i >= length)
			{
				if (length == MAX_LENGTH_2018184040 - 1) { pmValue = -1; }	index++; length += pmValue;
				if (pmValue == 1) { x = MAX_LENGTH_2018184040 - 1;	save += 1;	y = save; }
				else if (pmValue == -1) { x = length;	y = save; }
				k = 0;
			}
		}
	}

	// Ban 2X2 Black
	void Ban2X2_Black()
	{
		if (eRockType == BLACK_ROCK)
		{
			for (int i = 0; i < MAX_LENGTH_2018184040; i++)
				for (int j = 0; j < MAX_LENGTH_2018184040; j++)
					if (stMap[j][i].iID == MY_ROCK_ID_3_3 )
						stMap[j][i].iWeight = -100;
		}
	}
	// Ban 3X3 Black
	void Ban3X3_Black()
	{
		if (eRockType == BLACK_ROCK)
		{
			for (int i = 0; i < MAX_LENGTH_2018184040; i++)
				for (int j = 0; j < MAX_LENGTH_2018184040; j++)
					if (stMap[j][i].iID == MY_ROCK_ID_4_4 || stMap[j][i].iID == MY_ROCK_ID_3_4)
						stMap[j][i].iWeight = -100;
		}
	}

public:
	STSpace_2018184040 stMap[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040] = { STSpace_2018184040() };
	EMuni_2018184040 eRockType;

	// 이기는 위치
	double ROCK_PUT_WIN = 0;
	// 보통  상황일 때 NS
	// 돌을 놓으면 4개의 연속된 돌 완성
	double ROCK_PUT_FOUR_NS = 0;
	// 돌을 놓으면 3개의 연속된 돌 완성
	double ROCK_PUT_THREE_NS = 0;
	// 돌을 놓으면 2개의 연속된 돌 완성
	double  ROCK_PUT_TWO_NS = 0;
	// 한쪽이 막혀있을 상황일 때 SS
	// 돌을 놓으면 4개의 연속된 돌이 완성되는데 한쪽이 막혀있는 경우
	double  ROCK_PUT_FOUR_PS = 0;

	// 지는 위치
	double ROCK_BLOCK_LOSE = 0;
	double ROCK_BLOCK_FOUR_NS = 0;
	double ROCK_BLOCK_THREE_NS = 0;

	const double ROCK_EXIST = -1000000;
	// 절대 놓으면 안되는 위치
	const double ROCK_PUT_NONE = -1000000;

	const int MY_ROCK_ID_2 = 2;
	const int MY_ROCK_ID_3 = 3;
	const int MY_ROCK_ID_4 = 4;
	const int MY_ROCK_ID_2_2 = 5;
	const int MY_ROCK_ID_2_3 = 6;
	const int MY_ROCK_ID_2_4 = 7;
	const int MY_ROCK_ID_3_3 = 8;
	const int MY_ROCK_ID_3_4 = 9;
	const int MY_ROCK_ID_4_4 = 10;

	const double ROCK_MULTIPLE_SITUATION_2_2 = 1.1;
	const double ROCK_MULTIPLE_SITUATION_2_3 = 1.2;
	const double ROCK_MULTIPLE_SITUATION_2_4 = 1.3;
	const double ROCK_MULTIPLE_SITUATION_3_3 = 1.4;
	const double ROCK_MULTIPLE_SITUATION_3_4 = 1.5;
	const double ROCK_MULTIPLE_SITUATION_4_4 = 1.6;
};

class URockController_2018184040
{
public:
	URockController_2018184040() {}
	URockController_2018184040(EMuni_2018184040 ownerRock)
	{
		this->eOwnerRock = ownerRock;
		if (eOwnerRock == BLACK_ROCK)
		{
			clOwner.eRockType = BLACK_ROCK;
			clEnemy.eRockType = WHITE_ROCK;
		}
		else
		{
			clOwner.eRockType = WHITE_ROCK;
			clEnemy.eRockType = BLACK_ROCK;
		}
		clOwner.Initialize();
		clEnemy.Initialize();
	}

	void RecieveMap(EMuni_2018184040 eMap[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040], int* x, int* y)
	{
		clOwner.PutofStone(eMap); clEnemy.PutofStone(eMap);
		STSpace_2018184040 map[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040];
		for (int i = 0; i < MAX_LENGTH_2018184040; i++)
		{
			for (int j = 0; j < MAX_LENGTH_2018184040; j++)
			{
				map[j][i].eMuni = eMap[j][i];
				if (clOwner.stMap[j][i].iWeight > 0)
					map[j][i].iWeight += clOwner.stMap[j][i].iWeight;
				if (clEnemy.stMap[j][i].iWeight > 0)
					map[j][i].iWeight += clEnemy.stMap[j][i].iWeight;
			}
		}
		double num = map[0][0].iWeight;
		for (int i = 0; i < MAX_LENGTH_2018184040; i++)
		{
			for (int j = 0; j < MAX_LENGTH_2018184040; j++)
			{
				if (num < map[j][i].iWeight)
					num = map[j][i].iWeight;
			}
		}
		CustomList_2018184040<Vector2_2018184040> r;
		for (int i = 0; i < MAX_LENGTH_2018184040; i++)
		{
			for (int j = 0; j < MAX_LENGTH_2018184040; j++)
			{
				if (num == map[j][i].iWeight)
				{
					r.Insert(Vector2_2018184040{ i,j });
					break;
				}
			}
		}
		if (r.GetLength() > 0)
		{
			srand(time(NULL));
			int n = rand() % r.GetLength();
			Vector2_2018184040 pos = r.GetData(n);
			*x = pos.x; *y = pos.y;
			r.Clear();
		}
	}
	URock_2018184040 clOwner,  clEnemy;
	EMuni_2018184040 eOwnerRock = EMuni_2018184040::NONE;
};


URockController_2018184040 uRockController_2018184040_white = URockController_2018184040(WHITE_ROCK);
URockController_2018184040 uRockController_2018184040_black = URockController_2018184040(BLACK_ROCK);
EMuni_2018184040 eMap_2018184040_white[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040] = { EMuni_2018184040::CROSS };
EMuni_2018184040 eMap_2018184040_black[MAX_LENGTH_2018184040][MAX_LENGTH_2018184040] = { EMuni_2018184040::CROSS };

void WhiteAttack_2018184040(int* x, int* y)
{
	uRockController_2018184040_white.RecieveMap(eMap_2018184040_white, x, y);
	eMap_2018184040_white[*y][*x] = WHITE_ROCK;
}

void WhiteDefence_2018184040(int x, int y)
{
	eMap_2018184040_white[y][x] = BLACK_ROCK;
}

void BlackAttack_2018184040(int* x, int* y)
{
	uRockController_2018184040_black.RecieveMap(eMap_2018184040_black, x, y);
	eMap_2018184040_black[*y][*x] = BLACK_ROCK;
}

void BlackDefence_2018184040(int x, int y)
{
	eMap_2018184040_black[y][x] = WHITE_ROCK;
}



