#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "Screen.h"

typedef enum _Card_Status { OPEN, CLOSE, MATCH } Card_Status; // 카드 상태 정의

typedef struct _CARD_INFO		// 카드 정보
{
	Card_Status nCardState;  // 
	int  nUse;				// 카드에 문자가 적힌 유무
	char Munja;				// 카드에 쓰여진 문자
	int  nX, nY;				// 카드의 x, y 좌표
} CARD_INFO;					// CARD_INFO로 정의한다.

typedef struct _DATA			// 문자 정보
{
	int   nUse;				// 문자 사용 유무
	char Munja;			// 문자 정의
} DATA;							// DATA로 정의한다.

typedef struct _MATCH_CARD_DATA // 카드 짝 정보
{
	int nCardCount;
	int nCardIndex[2];		// 선택할 2개의 카드 배열	
	clock_t OldTime;			// 카드 선택 이전 시간 정의
} MATCH_CARD_DATA;				// MATCH_CARD_DATA로 정의한다.

								/*
								typedef struct _EFFECT			// 효과 정보
								{
								clock_t E_StartTime;		// 효과 발생 시각
								clock_t E_StayTime;			// 효과 지속 시간
								} EFFECT;						// EFFECT로 정의한다.
								*/

DATA g_sAplhaData[94];			// DATA = g_sAplhaData, 94개의 1차원 배열, 랜덤으로 골라질 문자의 수
CARD_INFO g_sGameCard[36];		// CARD_INFO = g_sGameCard, 36개의 1차원 배열, 카드의 수
MATCH_CARD_DATA g_sMatchCardInfo;	// MATCH_CARD_DATA = g_sMatchCardinfo, 짝이 맞는 카드의 정보
									//EFFECT g_sEffect;				// EFFECT = g_sEffect, 화면상에 나타날 효과의 시간 속성

int g_nRow, g_nCol;
int g_nExit;					// 종료 변수
int k_CardCount = 0;			// 짝으로 고른 갯수

clock_t g_StartTime, g_Time, g_LimitTime;	// 시작 시간, 초로 나타낸 시간, 제한 시간

void OpenCard(int x, int y, char Munja)	// 문자열 출력 함수
{
	char string[100];

	SetColor(14);
	ScreenPrint(x, y, "┌─┐");
	ScreenPrint(x, y + 1, "│  │");
	ScreenPrint(x, y + 2, "└─┘");
	SetColor(15);
	sprintf_s(string, 80, " %c", Munja);
	ScreenPrint(x + 2, y + 1, string);
	SetColor(7);
}

void MatchCard(int x, int y, char Munja)
{
	char string[100];

	SetColor(10);
	ScreenPrint(x, y, "↗━↘");
	ScreenPrint(x, y + 1, "┃  ┃");
	ScreenPrint(x, y + 2, "↖━↙");
	SetColor(15);
	sprintf_s(string, 80, " %c", Munja);
	ScreenPrint(x + 2, y + 1, string);
	SetColor(7);
}

void CloseCard(int x, int y)
{
	ScreenPrint(x, y + 1, "  ♣  ");
}

void Init()	// 초기화 함수
{
	int i, j;
	int nCardIndex, nAlphaIndex;
	int k_CardCount = 0;
	g_StartTime = clock();
	g_LimitTime = 180;

	for (i = 0; i < 94; i++)	// 문자 배열 초기화
	{
		g_sAplhaData[i].nUse = 0;
		g_sAplhaData[i].Munja = '!' + i;
	}

	for (i = 0; i < 36; i++)	// 카드의 문자를 초기화
		g_sGameCard[i].nUse = 0;

	srand((unsigned)time(NULL));

	for (i = 0; i < 18; i++)		// 카드 짝에 맞게 18개를 택하자
	{
		while (1)
		{
			nAlphaIndex = rand() % 94;		// 임의의 문자 고르기

			if (g_sAplhaData[nAlphaIndex].nUse == 0)		// 카드에 아무것도 안써져 있다면
			{
				g_sAplhaData[nAlphaIndex].nUse = 1;		// 쓰고
				break;										// 무한반복문을 나가라
			}
		}

		for (j = 0; j < 2; j++)		// 카드 배열
		{
			while (1)
			{
				nCardIndex = rand() % 36;		// 임의의 카드 자리
				if (g_sGameCard[nCardIndex].nUse == 0)		// 만약, 그 자리에 카드가 없다면
				{
					g_sGameCard[nCardIndex].nUse = 1;			// 집어 넣어라.
					g_sGameCard[nCardIndex].Munja = g_sAplhaData[nAlphaIndex].Munja;		// 임의로 골랐던 문자를 카드에 써라.
					break;
				}
			}
		}
	}

	for (i = 0; i < 36; i++)		// 카드 좌표 일정히 나타내기
	{
		g_sGameCard[i].nX = (i % 6) * 8 + 3;
		g_sGameCard[i].nY = (i / 6) * 4 + 1;
		g_sGameCard[i].nCardState = OPEN;		// 문자를 보이게 하라.
	}
}

void Update()	// 데이터 갱신 함수
{
	int stop = 0;
	clock_t CurTime = clock();						// 프로그램 시작 시간을 CurTime에 넣어라.
	g_Time = (clock() - g_StartTime) / 1000;		// 초로 나타내주는 공식

	if (g_Time > g_LimitTime)						// 제한 시간이 넘었다면,
		g_nExit = 1;								// 나가라.

	if (g_sMatchCardInfo.nCardCount == 2)			// 데이터 갱신
	{
		if (CurTime - g_sMatchCardInfo.OldTime > 1000)		// 1초간 카드는 고른 상태 유지
		{
			g_sMatchCardInfo.nCardCount = 0;
			if (g_sGameCard[g_sMatchCardInfo.nCardIndex[0]].Munja == g_sGameCard[g_sMatchCardInfo.nCardIndex[1]].Munja)
			{
				g_sGameCard[g_sMatchCardInfo.nCardIndex[0]].nCardState = OPEN;
				g_sGameCard[g_sMatchCardInfo.nCardIndex[1]].nCardState = OPEN;
				k_CardCount++;
			}
			else
			{
				g_sGameCard[g_sMatchCardInfo.nCardIndex[0]].nCardState = CLOSE;
				g_sGameCard[g_sMatchCardInfo.nCardIndex[1]].nCardState = CLOSE;
				k_CardCount++;
			}
		}
	}
}

void Render()
{
	int i, nIndex;
	char string[100];	// 문자열 넣는 배열

	ScreenClear();		// 화면을 지워라

	if (g_nExit)
		return;

	sprintf_s(string, 80, "주어진 시간 : %d초", g_LimitTime);
	ScreenPrint(58, 8, string);
	sprintf_s(string, 80, "경과된 시간 : %d초", g_Time - 5);
	ScreenPrint(58, 10, string);
	sprintf_s(string, 80, "맞춰본 횟수 : %d번", k_CardCount);
	ScreenPrint(58, 12, string);

	for (i = 0; i < 36; i++)
	{
		switch (g_sGameCard[i].nCardState)
		{
		case OPEN:
			OpenCard(g_sGameCard[i].nX, g_sGameCard[i].nY, g_sGameCard[i].Munja);
			break;
		case CLOSE:
			CloseCard(g_sGameCard[i].nX, g_sGameCard[i].nY);
			break;
		case MATCH:
			MatchCard(g_sGameCard[i].nX, g_sGameCard[i].nY, g_sGameCard[i].Munja);
			break;
		}
	}		// 문자열 출력

	nIndex = g_nRow * 6 + g_nCol;
	SetColor(15);
	ScreenPrint(g_sGameCard[nIndex].nX, g_sGameCard[nIndex].nY, "┏━┓");
	ScreenPrint(g_sGameCard[nIndex].nX, g_sGameCard[nIndex].nY + 2, "┗━┛");
	SetColor(7);

	// 선택하는 이펙트

	ScreenFlipping();
}

void Release()
{
}

void PauseScreen()	// 처음에 카드 보여주는 함수
{
	int i;

	Sleep(5000);

	for (i = 0; i < 36; i++)
	{
		g_sGameCard[i].nX = (i % 6) * 8 + 3;
		g_sGameCard[i].nY = (i / 6) * 4 + 1;
		g_sGameCard[i].nCardState = CLOSE;
	}
}

int main(void)
{
	int nKey, nIndex, stop = 0;

	ScreenInit();	// 화면 버퍼 2개
	Init();        // 초기화

	while (1)
	{
		if (g_nExit)
			break;

		if (_kbhit())
		{
			nKey = _getch();

			switch (nKey)
			{
			case 75:  // 왼쪽
				if (g_nCol - 1 < 0)
					g_nCol = 5;
				else
					g_nCol--;
				break;
			case 77:  // 오른쪽
				if (g_nCol + 1 > 5)
					g_nCol = 0;
				else
					g_nCol++;
				break;
			case 72:  // 위쪽
				if (g_nRow - 1 < 0)
					g_nRow = 5;
				else
					g_nRow--;
				break;
			case 80:	// 아래쪽
				if (g_nRow + 1 > 5)
					g_nRow = 0;
				else
					g_nRow++;
				break;
			case 32: // 카드 선택
				nIndex = g_nRow * 6 + g_nCol;
				switch (g_sMatchCardInfo.nCardCount)
				{
				case 0:
					g_sMatchCardInfo.nCardIndex[0] = nIndex;	//1번째 선택
					g_sGameCard[nIndex].nCardState = MATCH;
					g_sMatchCardInfo.nCardCount++;
					break;
				case 1:
					if (g_sMatchCardInfo.nCardIndex[0] != nIndex) // 2번째 선택
					{
						g_sMatchCardInfo.OldTime = clock();
						g_sMatchCardInfo.nCardIndex[1] = nIndex;
						g_sGameCard[nIndex].nCardState = MATCH;
						g_sMatchCardInfo.nCardCount++;
					}
					break;
				}
				break;
			case 'r':
			case 'R':		// r을 누르면 다시하기
				stop = 0;
				k_CardCount = 0;
				ScreenInit();
				Init();
				break;
			}
		}
		Update();    // 데이터 갱신
		Render();    // 화면 출력
		if (stop == 0)
		{
			PauseScreen();
			stop = 1;
		}
	}

	Release();   // 해제
	ScreenRelease();
	return 0;
}