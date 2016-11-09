#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include "Screen.h"

typedef enum _Card_Status { OPEN, CLOSE, MATCH } Card_Status; // ī�� ���� ����

typedef struct _CARD_INFO		// ī�� ����
{
	Card_Status nCardState;  // 
	int  nUse;				// ī�忡 ���ڰ� ���� ����
	char Munja;				// ī�忡 ������ ����
	int  nX, nY;				// ī���� x, y ��ǥ
} CARD_INFO;					// CARD_INFO�� �����Ѵ�.

typedef struct _DATA			// ���� ����
{
	int   nUse;				// ���� ��� ����
	char Munja;			// ���� ����
} DATA;							// DATA�� �����Ѵ�.

typedef struct _MATCH_CARD_DATA // ī�� ¦ ����
{
	int nCardCount;
	int nCardIndex[2];		// ������ 2���� ī�� �迭	
	clock_t OldTime;			// ī�� ���� ���� �ð� ����
} MATCH_CARD_DATA;				// MATCH_CARD_DATA�� �����Ѵ�.

								/*
								typedef struct _EFFECT			// ȿ�� ����
								{
								clock_t E_StartTime;		// ȿ�� �߻� �ð�
								clock_t E_StayTime;			// ȿ�� ���� �ð�
								} EFFECT;						// EFFECT�� �����Ѵ�.
								*/

DATA g_sAplhaData[94];			// DATA = g_sAplhaData, 94���� 1���� �迭, �������� ����� ������ ��
CARD_INFO g_sGameCard[36];		// CARD_INFO = g_sGameCard, 36���� 1���� �迭, ī���� ��
MATCH_CARD_DATA g_sMatchCardInfo;	// MATCH_CARD_DATA = g_sMatchCardinfo, ¦�� �´� ī���� ����
									//EFFECT g_sEffect;				// EFFECT = g_sEffect, ȭ��� ��Ÿ�� ȿ���� �ð� �Ӽ�

int g_nRow, g_nCol;
int g_nExit;					// ���� ����
int k_CardCount = 0;			// ¦���� �� ����

clock_t g_StartTime, g_Time, g_LimitTime;	// ���� �ð�, �ʷ� ��Ÿ�� �ð�, ���� �ð�

void OpenCard(int x, int y, char Munja)	// ���ڿ� ��� �Լ�
{
	char string[100];

	SetColor(14);
	ScreenPrint(x, y, "������");
	ScreenPrint(x, y + 1, "��  ��");
	ScreenPrint(x, y + 2, "������");
	SetColor(15);
	sprintf_s(string, 80, " %c", Munja);
	ScreenPrint(x + 2, y + 1, string);
	SetColor(7);
}

void MatchCard(int x, int y, char Munja)
{
	char string[100];

	SetColor(10);
	ScreenPrint(x, y, "�֦���");
	ScreenPrint(x, y + 1, "��  ��");
	ScreenPrint(x, y + 2, "�ئ���");
	SetColor(15);
	sprintf_s(string, 80, " %c", Munja);
	ScreenPrint(x + 2, y + 1, string);
	SetColor(7);
}

void CloseCard(int x, int y)
{
	ScreenPrint(x, y + 1, "  ��  ");
}

void Init()	// �ʱ�ȭ �Լ�
{
	int i, j;
	int nCardIndex, nAlphaIndex;
	int k_CardCount = 0;
	g_StartTime = clock();
	g_LimitTime = 180;

	for (i = 0; i < 94; i++)	// ���� �迭 �ʱ�ȭ
	{
		g_sAplhaData[i].nUse = 0;
		g_sAplhaData[i].Munja = '!' + i;
	}

	for (i = 0; i < 36; i++)	// ī���� ���ڸ� �ʱ�ȭ
		g_sGameCard[i].nUse = 0;

	srand((unsigned)time(NULL));

	for (i = 0; i < 18; i++)		// ī�� ¦�� �°� 18���� ������
	{
		while (1)
		{
			nAlphaIndex = rand() % 94;		// ������ ���� ����

			if (g_sAplhaData[nAlphaIndex].nUse == 0)		// ī�忡 �ƹ��͵� �Ƚ��� �ִٸ�
			{
				g_sAplhaData[nAlphaIndex].nUse = 1;		// ����
				break;										// ���ѹݺ����� ������
			}
		}

		for (j = 0; j < 2; j++)		// ī�� �迭
		{
			while (1)
			{
				nCardIndex = rand() % 36;		// ������ ī�� �ڸ�
				if (g_sGameCard[nCardIndex].nUse == 0)		// ����, �� �ڸ��� ī�尡 ���ٸ�
				{
					g_sGameCard[nCardIndex].nUse = 1;			// ���� �־��.
					g_sGameCard[nCardIndex].Munja = g_sAplhaData[nAlphaIndex].Munja;		// ���Ƿ� ����� ���ڸ� ī�忡 ���.
					break;
				}
			}
		}
	}

	for (i = 0; i < 36; i++)		// ī�� ��ǥ ������ ��Ÿ����
	{
		g_sGameCard[i].nX = (i % 6) * 8 + 3;
		g_sGameCard[i].nY = (i / 6) * 4 + 1;
		g_sGameCard[i].nCardState = OPEN;		// ���ڸ� ���̰� �϶�.
	}
}

void Update()	// ������ ���� �Լ�
{
	int stop = 0;
	clock_t CurTime = clock();						// ���α׷� ���� �ð��� CurTime�� �־��.
	g_Time = (clock() - g_StartTime) / 1000;		// �ʷ� ��Ÿ���ִ� ����

	if (g_Time > g_LimitTime)						// ���� �ð��� �Ѿ��ٸ�,
		g_nExit = 1;								// ������.

	if (g_sMatchCardInfo.nCardCount == 2)			// ������ ����
	{
		if (CurTime - g_sMatchCardInfo.OldTime > 1000)		// 1�ʰ� ī��� �� ���� ����
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
	char string[100];	// ���ڿ� �ִ� �迭

	ScreenClear();		// ȭ���� ������

	if (g_nExit)
		return;

	sprintf_s(string, 80, "�־��� �ð� : %d��", g_LimitTime);
	ScreenPrint(58, 8, string);
	sprintf_s(string, 80, "����� �ð� : %d��", g_Time - 5);
	ScreenPrint(58, 10, string);
	sprintf_s(string, 80, "���纻 Ƚ�� : %d��", k_CardCount);
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
	}		// ���ڿ� ���

	nIndex = g_nRow * 6 + g_nCol;
	SetColor(15);
	ScreenPrint(g_sGameCard[nIndex].nX, g_sGameCard[nIndex].nY, "������");
	ScreenPrint(g_sGameCard[nIndex].nX, g_sGameCard[nIndex].nY + 2, "������");
	SetColor(7);

	// �����ϴ� ����Ʈ

	ScreenFlipping();
}

void Release()
{
}

void PauseScreen()	// ó���� ī�� �����ִ� �Լ�
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

	ScreenInit();	// ȭ�� ���� 2��
	Init();        // �ʱ�ȭ

	while (1)
	{
		if (g_nExit)
			break;

		if (_kbhit())
		{
			nKey = _getch();

			switch (nKey)
			{
			case 75:  // ����
				if (g_nCol - 1 < 0)
					g_nCol = 5;
				else
					g_nCol--;
				break;
			case 77:  // ������
				if (g_nCol + 1 > 5)
					g_nCol = 0;
				else
					g_nCol++;
				break;
			case 72:  // ����
				if (g_nRow - 1 < 0)
					g_nRow = 5;
				else
					g_nRow--;
				break;
			case 80:	// �Ʒ���
				if (g_nRow + 1 > 5)
					g_nRow = 0;
				else
					g_nRow++;
				break;
			case 32: // ī�� ����
				nIndex = g_nRow * 6 + g_nCol;
				switch (g_sMatchCardInfo.nCardCount)
				{
				case 0:
					g_sMatchCardInfo.nCardIndex[0] = nIndex;	//1��° ����
					g_sGameCard[nIndex].nCardState = MATCH;
					g_sMatchCardInfo.nCardCount++;
					break;
				case 1:
					if (g_sMatchCardInfo.nCardIndex[0] != nIndex) // 2��° ����
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
			case 'R':		// r�� ������ �ٽ��ϱ�
				stop = 0;
				k_CardCount = 0;
				ScreenInit();
				Init();
				break;
			}
		}
		Update();    // ������ ����
		Render();    // ȭ�� ���
		if (stop == 0)
		{
			PauseScreen();
			stop = 1;
		}
	}

	Release();   // ����
	ScreenRelease();
	return 0;
}