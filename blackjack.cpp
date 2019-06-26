// Немодальные окна.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "blackjack.h"

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hDlg;
HINSTANCE hInst;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	hInst = hInstance;
	hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	ShowWindow(hDlg, nCmdShow);
	UpdateWindow(hDlg);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

struct Cards
{
	HBITMAP hBitmap;
	int score;
	bool used;
}card;

HBITMAP GetCard(std::vector<Cards>&, int&);
void ReturnTheCard(std::vector<Cards>&);

BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBackground, hCard_back, hTemp;
	static std::vector<Cards> hCards;
	static std::vector<HBITMAP> player1, player2;
	static bool closed_card = true;
	static int x = 2, score_player1 = 0, score_player2 = 0, i = 0;
	static HDC hdc, hMemDC;
	static HWND hEdit1, hEdit2, hPic, hPic1, hPic2;
	static HGDIOBJ hbmOld;
	static TCHAR buff[255];
	int res;
	switch (msg)
	{
		case WM_INITDIALOG:
			hBackground = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACKGROUND));
			hCard_back = (HBITMAP)LoadImage(NULL, L"Data\\card_back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			hEdit1 = GetDlgItem(hWnd, IDC_EDIT1);
			hEdit2 = GetDlgItem(hWnd, IDC_EDIT2);
			hPic = GetDlgItem(hWnd, IDC_STATIC);
			hPic1 = GetDlgItem(hWnd, IDC_STATIC1);
			hPic2 = GetDlgItem(hWnd, IDC_STATIC2);
			for (int i = 1; i < 37; i++)
			{
				wsprintf(buff, TEXT("Data\\%d.bmp"), i);
				hTemp = (HBITMAP)LoadImage(NULL, buff, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				if (!hTemp)
				{
					MessageBox(NULL, TEXT("Error get file"), TEXT(""), MB_OK);
					break;
				}
				card.hBitmap = hTemp;
				card.score = x;
				card.used = false;
				hCards.push_back(card);
				if (!(i % 4))
					x++;
				if (x == 5)
					x++;
			}
			srand(time(NULL));
			std::random_shuffle(hCards.begin(), hCards.end());
			player1.push_back(GetCard(hCards, score_player1));
			player1.push_back(GetCard(hCards, score_player1));
			player2.push_back(GetCard(hCards, score_player2));
			player2.push_back(GetCard(hCards, score_player2));
			wsprintf(buff, TEXT("%d"), score_player2);
			SetWindowText(hEdit2, buff);
			if (score_player2 > 21)
				PostMessage(hWnd, WM_COMMAND, IDC_BUTTON2, 0);
			return TRUE;
		case WM_PAINT:
			BITMAP bm;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hPic, &ps);
			hMemDC = CreateCompatibleDC(hdc);
			hbmOld = SelectObject(hMemDC, hBackground);
			GetObject(hBackground, sizeof(bm), &bm);
			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
			SelectObject(hMemDC, hbmOld);
			DeleteDC(hMemDC);
			EndPaint(hPic, &ps);

			hdc = BeginPaint(hPic1, &ps);
			hMemDC = CreateCompatibleDC(hdc);
			for (int i = 0; i < player1.size(); i++)
			{
				if (!i && closed_card)
					hbmOld = SelectObject(hMemDC, hCard_back);
				else
				{
					if (!i)
					{
						wsprintf(buff, TEXT("%d"), score_player1);
						SetWindowText(hEdit1, buff);
					}
					hbmOld = SelectObject(hMemDC, player1[i]);
				}
				BitBlt(hdc, 30 * i, 0, 75, 109, hMemDC, 0, 0, SRCCOPY);
				SelectObject(hMemDC, hbmOld);
			}
			DeleteDC(hMemDC);
			EndPaint(hPic1, &ps);


			hdc = BeginPaint(hPic2, &ps);
			hMemDC = CreateCompatibleDC(hdc);
			for (int i = 0; i < player2.size(); i++)
			{
				hbmOld = SelectObject(hMemDC, player2[i]);
				BitBlt(hdc, 30 * i, 0, 75, 109, hMemDC, 0, 0, SRCCOPY);
				SelectObject(hMemDC, hbmOld);
			}
			DeleteDC(hMemDC);
			EndPaint(hPic2, &ps);

			return 0;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON1:
				player2.push_back(GetCard(hCards, score_player2));
				wsprintf(buff, TEXT("%d"), score_player2);
				SetWindowText(hEdit2, buff);
				InvalidateRect(hWnd, 0, false);
				if (score_player2 > 21)
					PostMessage(hWnd, WM_COMMAND, IDC_BUTTON2, 0);
				return TRUE;
			case IDC_BUTTON2:
				closed_card = false;
				InvalidateRect(hWnd, 0, false);
				if (score_player1 < 17)
				{
					/*Sleep(500);*/
					player1.push_back(GetCard(hCards, score_player1));
					PostMessage(hWnd, WM_COMMAND, IDC_BUTTON2, 0);
				}
				else
				{
					if (score_player1 < 22 && (score_player1 > score_player2 || score_player2 > 21))
						res = MessageBox(hWnd, TEXT("Вы проиграли\nИграем ещё?"), TEXT(""), MB_YESNO);
					else if (score_player2 < 22 && (score_player2 > score_player1 || score_player1 > 21))
						res = MessageBox(hWnd, TEXT("Вы выиграли\nИграем ещё?"), TEXT(""), MB_YESNO);
					else if (score_player1 > 21 && score_player2 > 21)
						res = MessageBox(hWnd, TEXT("У обоих перебор, ничья\nИграем ещё?"), TEXT(""), MB_YESNO);
					else if (score_player1 == score_player2)
						res = MessageBox(hWnd, TEXT("Одинаковое колличество очков, ничья\nИграем ещё?"), TEXT(""), MB_YESNO);
					switch (res)
					{
					case IDYES:
						score_player1 = 0;
						score_player2 = 0;
						ReturnTheCard(hCards);
						player1.clear();
						player2.clear();
						std::random_shuffle(hCards.begin(), hCards.end());
						player1.push_back(GetCard(hCards, score_player1));
						player1.push_back(GetCard(hCards, score_player1));
						player2.push_back(GetCard(hCards, score_player2));
						player2.push_back(GetCard(hCards, score_player2));
						wsprintf(buff, TEXT("%d"), score_player2);
						SetWindowText(hEdit2, buff);
						SetWindowText(hEdit1, L"");
						InvalidateRect(hWnd, 0, false);
						closed_card = true;
						break;
					case IDNO:
						DestroyWindow(hWnd);
						PostQuitMessage(0);
						break;
					}
				}
				return TRUE;
			}
			return TRUE;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;
}

HBITMAP GetCard(std::vector<Cards> &Cards, int& sum)
{
	for (int i = 0; i < Cards.size(); i++)
	{
		if (!(Cards[i].used))
		{
			Cards[i].used = true;
			sum += Cards[i].score;
			return Cards[i].hBitmap;
		}
	}
}

void ReturnTheCard(std::vector<Cards>& Cards)
{
	for (int i = 0; i < Cards.size(); i++)
	{
		if (Cards[i].used)
		{
			Cards[i].used = false;

		}
	}
}