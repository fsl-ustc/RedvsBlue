#include<Windows.h>
#include<stdlib.h>
#include<time.h>
#include "resource.h"

#define GAMENUM 2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK StartWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ShopWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ClickFastWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RPSWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szStartClass[] = TEXT("Start");
TCHAR szShopClass[] = TEXT("Shop");
TCHAR szClickFastClass[] = TEXT("ClickFast");
TCHAR szRPSClass[] = TEXT("RPS");

struct status{
	int rMoney=200, bMoney=200, rFlag=0, bFlag, rTricks=1, bTricks=1, mode, rPhase=0,bPhase = 14, rReady, bReady, rlvl=2, blvl=2,rDice,bDice,dice;
}game;

int map[28] = {0,100,50,-50,100,0,50,-50,100,-50,50,100,50,-50,0,100,50,-50,100,0,50,-50,100,50,-50,100,-50,50};

int ClickFastTmp,ShopSelect,rRPS,bRPS,ShowHelp;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdline, int iCmdShow)
{
	srand(time(NULL));
	game.mode = -1;
	static TCHAR szAppName[] = TEXT("Main");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("ERROR!"), szAppName, MB_ICONERROR);
		return 0;
	}

	wndclass.lpfnWndProc = StartWndProc;
	wndclass.cbWndExtra = sizeof(long);
	wndclass.hIcon = NULL;
	wndclass.lpszClassName = szStartClass;

	RegisterClass(&wndclass);

	wndclass.lpfnWndProc = ShopWndProc;
	wndclass.lpszClassName = szShopClass;

	RegisterClass(&wndclass);

	wndclass.lpfnWndProc = RPSWndProc;
	wndclass.lpszClassName = szRPSClass;

	RegisterClass(&wndclass);
	
	wndclass.lpfnWndProc = ClickFastWndProc;
	wndclass.lpszClassName = szClickFastClass;

	RegisterClass(&wndclass);

	hwnd = CreateWindow(
		szAppName,
		TEXT("Red vs Blue"),
		WS_OVERLAPPEDWINDOW,
		20,
		20,
		1500,
		810,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void gamereset() {
	game.rMoney = game.bMoney = 200;
	game.rFlag = game.bFlag = game.rReady = game.bReady = game.rPhase = game.rDice=game.bDice=game.dice=0;
	game.rlvl = game.blvl = 2;
	game.bPhase = 14;
	game.rTricks = game.bTricks = 1;
}

int cal_X(int phase) {
	if (phase > 5 && phase <= 14 ) 
		return phase-5;
	if (phase >= 19 && phase <= 27)
		return 28 - phase;
	if (phase <= 5) return 0;
	if (phase > 14 && phase < 19) return 9;
}

int cal_Y(int phase) {
	if (phase <= 5) return 5 - phase;
	if (phase > 5 && phase <= 14)
		return 0;
	if (phase > 14 && phase <= 19) return phase - 14;
	if (phase >= 19 && phase <= 27) return 5;
}

LRESULT CALLBACK StartWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HBITMAP hbmp;
	HDC hdc,hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	BITMAP BitMap;
	static int  cxBmp, cyBmp;
	switch (message) {
	case WM_CREATE:
		SetWindowLongPtr(hwnd, 0, 0);
		return 0;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case 'J':
			game.rReady = 1 - game.rReady;
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			if (game.rReady && game.bReady) {
				Sleep(2000);
				game.rReady = game.bReady = 0;
				game.mode = 0;
				MoveWindow(hwnd, 0, 0, 0, 0,TRUE);
			}
			return 0;
		case VK_NUMPAD3:
			game.bReady = 1 - game.bReady;
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			if (game.rReady && game.bReady) {
				Sleep(2000);
				game.rReady = game.bReady = 0;
				game.mode = 0;
				MoveWindow(hwnd, 0, 0, 0, 0,TRUE);
			}
			return 0;
		case VK_SPACE:
			ShowHelp = 1-ShowHelp;
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP3));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 300, 100, cxBmp, cyBmp, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		if (ShowHelp == 0) {
			hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP7));
			GetObject(hbmp, sizeof(BITMAP), &BitMap);
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hbmp);
			BitBlt(hdc, 575, 150, 160, 100, hdcMem, 0, 200, SRCCOPY);
			DeleteObject(hbmp);
			DeleteObject(hdcMem);
		}
		
		if (game.rReady == 1) {
			hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
			GetObject(hbmp, sizeof(BITMAP), &BitMap);
			cxBmp = BitMap.bmWidth;
			cyBmp = BitMap.bmHeight;
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hbmp);
			BitBlt(hdc, 542, 442, 15, 15, hdcMem, 245, 4, SRCCOPY);
			DeleteObject(hbmp);
			DeleteObject(hdcMem);
		}
		if (game.bReady == 1) {
			hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
			GetObject(hbmp, sizeof(BITMAP), &BitMap);
			cxBmp = BitMap.bmWidth;
			cyBmp = BitMap.bmHeight;
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hbmp);
			BitBlt(hdc, 842, 442, 15, 15, hdcMem, 248,32, SRCCOPY);
			DeleteObject(hbmp);
			DeleteObject(hdcMem);
		
		}
		if (ShowHelp == 1) {
			hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP7));
			GetObject(hbmp, sizeof(BITMAP), &BitMap);
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hbmp);
			BitBlt(hdc, 350, 550, 1200, 200, hdcMem, 0, 0, SRCCOPY);
			DeleteObject(hbmp);
			DeleteObject(hdcMem);
		}


		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ShopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HBITMAP hbmp;
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	BITMAP BitMap;
	static int  cxBmp, cyBmp;
	switch (message) {
	case WM_CREATE:
		SetWindowLongPtr(hwnd, 0, 0);
		return 0;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case 'J':
			if (game.mode == -2 || game.mode == -4) {
				game.mode = 0;
				MoveWindow(hwnd, 0, 0, 0, 0, TRUE);
			}
			return 0;
		case VK_NUMPAD3:
			if (game.mode == -3 || game.mode == -5) {
				game.mode = 0;
				MoveWindow(hwnd, 0, 0, 0, 0, TRUE);
			}
			return 0;
		case 'S':
			if (game.mode == -2 || game.mode == -4) {
				ShopSelect = min(ShopSelect + 1, 1);
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_DOWN:
			if (game.mode == -3 || game.mode == -5) {
				ShopSelect = min(ShopSelect + 1, 1);
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case 'W':
			if (game.mode == -2 || game.mode == -4) {
				ShopSelect = max(ShopSelect - 1, 0);
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_UP:
			if (game.mode == -3 || game.mode == -5) {
				ShopSelect = max(ShopSelect - 1, 0);
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case 'D':
			if (game.mode==-2) {
				if (ShopSelect==0) {
					if (game.rMoney >= 100 * game.rlvl) {
						game.rMoney -= 100 * game.rlvl;
						game.rlvl++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
				if (ShopSelect == 1) {
					if (game.rMoney >= 100) {
						game.rMoney -= 100;
						game.rTricks++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
			}
			if (game.mode==-4) {
				if (ShopSelect == 0) {
					if (game.rMoney >= 100 * game.rlvl) {
						game.rMoney -= 100 * game.rlvl;
						game.rlvl++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
				if (ShopSelect == 1) {
					if (game.rMoney >= 500) {
						game.rMoney -= 500;
						game.rFlag++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
			}
			return 0;
		case VK_RIGHT:
			if (game.mode==-3) {
				if (ShopSelect == 0) {
					if (game.bMoney >= 100 * game.blvl) {
						game.bMoney -= 100 * game.blvl;
						game.blvl++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
				if (ShopSelect == 1) {
					if (game.bMoney >= 100) {
						game.bMoney -= 100;
						game.bTricks++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
			}
			if (game.mode == -5) {
				if (ShopSelect == 0) {
					if (game.bMoney >= 100 * game.blvl) {
						game.bMoney -= 100 * game.blvl;
						game.blvl++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
				if (ShopSelect == 1) {
					if (game.bMoney >= 500) {
						game.bMoney -= 500;
						game.bTricks++;
						InvalidateRect(GetParent(hwnd), NULL, TRUE);
						UpdateWindow(GetParent(hwnd));
						InvalidateRect(hwnd, NULL, TRUE);
						UpdateWindow(hwnd);
					}
				}
			}
			return 0;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP4));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 0, 0, cxBmp, cyBmp, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 485 , 25+100*ShopSelect , 24 , 50 , hdcMem, 0, 50 , SRCCOPY);

		if (game.mode == -2 || game.mode == -3) {
			BitBlt(hdc, 100, 25 , 170, 50, hdcMem, 0,100 , SRCCOPY);
			if (game.mode == -2) {
				BitBlt(hdc, 360, 25, 24, 50, hdcMem, game.rlvl*24, 0, SRCCOPY);
				BitBlt(hdc, 385, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
				BitBlt(hdc, 410, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
			}
			if (game.mode == -3) {
				BitBlt(hdc, 360, 25, 24, 50, hdcMem, game.blvl*24, 0, SRCCOPY);
				BitBlt(hdc, 385, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
				BitBlt(hdc, 410, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
			}
			BitBlt(hdc, 360, 125, 24, 50, hdcMem, 1 * 24, 0, SRCCOPY);
			BitBlt(hdc, 385, 125, 24, 50, hdcMem, 0, 0, SRCCOPY);
			BitBlt(hdc, 410, 125, 24, 50, hdcMem, 0, 0, SRCCOPY);

			BitBlt(hdc, 100, 125,  170, 50, hdcMem, 0, 150, SRCCOPY);
		}

		if (game.mode == -4 || game.mode == -5) {
			BitBlt(hdc, 100, 25, 170, 50, hdcMem, 0, 100, SRCCOPY);
			if (game.mode == -4) {
				BitBlt(hdc, 360, 25, 24, 50, hdcMem, game.rlvl * 24, 0, SRCCOPY);
				BitBlt(hdc, 385, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
				BitBlt(hdc, 410, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
			}
			if (game.mode == -5) {
				BitBlt(hdc, 360, 25, 24, 50, hdcMem, game.blvl * 24, 0, SRCCOPY);
				BitBlt(hdc, 385, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
				BitBlt(hdc, 410, 25, 24, 50, hdcMem, 0, 0, SRCCOPY);
			}
			BitBlt(hdc, 360, 125, 24, 50, hdcMem, 5 * 24, 0, SRCCOPY);
			BitBlt(hdc, 385, 125, 24, 50, hdcMem, 0, 0, SRCCOPY);
			BitBlt(hdc, 410, 125, 24, 50, hdcMem, 0, 0, SRCCOPY);

			BitBlt(hdc, 100, 125, 70, 50, hdcMem, 0, 200, SRCCOPY);
		}

		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ClickFastWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HBITMAP hbmp;
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	BITMAP BitMap;
	static int  cxBmp, cyBmp;
	switch (message) {
	case WM_CREATE:
		SetWindowLongPtr(hwnd, 0, 0);
		return 0;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		ClickFastTmp = 10;
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case 'W':
			if (game.rTricks > 0) {
				game.rTricks--;
				ClickFastTmp += 3;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_UP:
			if (game.bTricks > 0) {
				game.bTricks--;
				ClickFastTmp -= 3;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case 'J':
			ClickFastTmp++;
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			if (ClickFastTmp >= 20) {
				hdc = BeginPaint(hwnd, &ps);
				ClickFastTmp = 10;
				EndPaint(hwnd, &ps);
				Sleep(2000);
				game.mode = 0;
				game.rMoney += 125;
				game.bMoney += 25;
				MoveWindow(hwnd, 0, 0, 0, 0, TRUE);
			}
			return 0;
		case VK_NUMPAD3:
			ClickFastTmp--;
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
			if (ClickFastTmp <= 0) {
				hdc = BeginPaint(hwnd, &ps);
				ClickFastTmp = 10;
				EndPaint(hwnd, &ps);
				Sleep(2000);
				game.mode = 0;
				game.rMoney += 25;
				game.bMoney += 125;
				MoveWindow(hwnd, 0, 0, 0, 0, TRUE);
			}
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP5));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 150, 300, 1200, 200, hdcMem, 0, 200, SRCCOPY);
		BitBlt(hdc, 150, 300, 60 * ClickFastTmp, 200, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK RPSWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HBITMAP hbmp;
	HDC hdc, hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	BITMAP BitMap;
	static int  cxBmp, cyBmp;
	switch (message) {
	case WM_CREATE:
		SetWindowLongPtr(hwnd, 0, 0);
		return 0;
	case WM_SIZE:
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		rRPS = bRPS = 0;
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case 'A':
			if (rRPS == 0) {
				rRPS = 1;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case 'W':
			if (rRPS == 0) {
				rRPS = 2;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case 'D':
			if (rRPS == 0) {
				rRPS = 3;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_LEFT:
			if (bRPS == 0) {
				bRPS = 1;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_UP:
			if (bRPS == 0) {
				bRPS = 2;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		case VK_RIGHT:
			if (bRPS == 0) {
				bRPS = 3;
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
			}
			return 0;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP3));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 300, 100, cxBmp, cyBmp, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		if (rRPS > 0 && bRPS == 0)
			BitBlt(hdc, 530, 425, 40, 50, hdcMem, 0, 250 , SRCCOPY);
		if (rRPS == 0 && bRPS > 0)
			BitBlt(hdc, 830, 425, 40, 50, hdcMem, 0, 250, SRCCOPY);
		if (rRPS > 0 && bRPS > 0) {
			BitBlt(hdc, 530, 425, 40, 50, hdcMem, rRPS*40 , 250, SRCCOPY);
			BitBlt(hdc, 830, 425, 40, 50, hdcMem, bRPS*40 , 250, SRCCOPY);
		}
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		EndPaint(hwnd, &ps);
		if (rRPS > 0 && bRPS > 0) {
			Sleep(2000);
			game.mode = 0;
			int res = (rRPS - bRPS + 3) % 3;
			if (res == 0) {
				game.rMoney += 75;
				game.bMoney += 75;
			}
			if (res == 1) {
				game.rMoney += 125;
				game.bMoney += 25;
			}
			if (res == 2) {
				game.rMoney += 25;
				game.bMoney += 125;
			}
			MoveWindow(hwnd, 0, 0, 0, 0, TRUE);
		}
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndChild[GAMENUM+1],hwndShop;
	HBITMAP hbmp;
	HDC hdc,hdcMem;
	PAINTSTRUCT ps;
	RECT rect;
	BITMAP BitMap;
	static int cxClient = LOWORD(lParam), cyClient = HIWORD(lParam), cxBmp, cyBmp;

	switch (message)
	{
	case WM_CREATE:
		hwndShop = CreateWindow(
			szShopClass,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)GAMENUM+1,
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL
		);
		hwndChild[0] = CreateWindow(
			szStartClass,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)0,
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL
		);
		hwndChild[1] = CreateWindow(
			szClickFastClass,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)1,
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL
		);
		hwndChild[2] = CreateWindow(
			szRPSClass,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)1,
			(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
			NULL
		);
		return 0;
	case WM_KEYDOWN:
		if (game.mode == 0)
			switch (wParam) {
			case 'P':
				MessageBeep(0);
				if (MessageBox(NULL, TEXT("请问是否要重置游戏"), TEXT(""), MB_YESNO) == IDYES) {
					gamereset();
					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);
				}
				return 0;
			case 'J':
				if (game.rReady == 2||game.rReady>=game.bReady+1) return 0;
				game.rReady += 1;
				if (game.rReady == 1) {
					game.rDice = rand() % game.rlvl + 1;
					game.rPhase = (game.rPhase + game.rDice) % 28;
					game.rMoney = max(0, game.rMoney + map[game.rPhase]);
					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);
				}
				if (game.rReady == 1 && game.bReady == 1) {
					game.dice = rand() % GAMENUM + 1;
				}
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
				if (game.rReady == 2 && game.bReady == 2) {
					game.mode = game.dice;
					MoveWindow(hwndChild[game.dice], 0, 0, cxClient, cyClient, TRUE);
					game.rReady = game.bReady = 0;
				}
				return 0;
			case VK_NUMPAD3:
				if (game.bReady == 2||game.bReady>=game.rReady+1)return 0;
				game.bReady += 1;
				if (game.bReady == 1) {
					game.bDice = rand() % game.blvl + 1;
					game.bPhase = (game.bPhase + game.bDice) % 28;
					game.bMoney = max(0, game.bMoney + map[game.bPhase]);
					InvalidateRect(hwnd, NULL, TRUE);
					UpdateWindow(hwnd);
				}
				if (game.rReady == 1 && game.bReady == 1) {
					game.dice = rand() % GAMENUM + 1;
				}
				InvalidateRect(hwnd, NULL, TRUE);
				UpdateWindow(hwnd);
				if (game.rReady == 2 && game.bReady == 2) {
					game.mode = game.dice;
					MoveWindow(hwndChild[game.dice], 0, 0, cxClient, cyClient, TRUE);
					game.rReady = game.bReady = 0;
				}
				return 0;
			case 'W':
				if (game.rReady == 0) {
					int tmp=game.rPhase;
					for (int i = 1;i <= game.rlvl;i++) {
						tmp = (game.rPhase + i) % 28;
						if (tmp == 5 ||	tmp==19) {
							game.rPhase = tmp;
							game.mode = -2;
							game.rReady = 1;
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							MoveWindow(hwndShop, 225, 250, 600, 200, TRUE);
							break;
						}
						if (tmp == 14) {
							game.rPhase = tmp;
							game.mode = -4;
							game.rReady = 1;
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							MoveWindow(hwndShop, 225, 250, 600, 200, TRUE);
							break;
						}
						if (tmp == 0) {
							game.rPhase = tmp;
							game.rMoney += 500;
							game.rReady = 1;
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							break;
						}
					}
				}
				return 0;
			case VK_UP:
				if (game.bReady == 0) {
					int tmp = game.bPhase;
					for (int i = 1;i <= game.blvl;i++) {
						tmp = (game.bPhase + i) % 28;
						if (tmp == 5 || tmp == 19) {
							game.bPhase = tmp;
							game.mode = -3;
							game.bReady = 1;
							MoveWindow(hwndShop, 225, 250, 600, 200, TRUE);
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							break;
						}
						if (tmp == 0) {
							game.rPhase = tmp;
							game.mode = -5;
							game.bReady = 1;
							MoveWindow(hwndShop, 225, 250, 600, 200, TRUE);
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							break;
						}
						if (tmp == 14) {
							game.bPhase = tmp;
							game.bMoney += 500;
							game.bReady = 1;
							InvalidateRect(hwnd, NULL, TRUE);
							UpdateWindow(hwnd);
							break;
						}
					}
				}
				return 0;
			default: return 0;
			}
		else
			if (game.mode <= -2)
				SendMessage(hwndShop, message, wParam, lParam);
		else 
			if (game.mode == -1)
				SendMessage(hwndChild[0], message, wParam, lParam);
		else 
				SendMessage(hwndChild[game.mode],message,wParam,lParam);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		MoveWindow(hwndChild[0], 0, 0, cxClient, cyClient, TRUE);
		MoveWindow(hwndShop,0 , 0 , 0 , 0 , FALSE);
		MoveWindow(hwndChild[1], 0, 0, 0, 0, FALSE);
		MoveWindow(hwndChild[2], 0, 0, 0, 0, FALSE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
	    hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP2));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 25, 25, cxBmp, cyBmp, hdcMem, 0, 0, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP3));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc,1050, 50, 400, 200, hdcMem, 200, 200, SRCCOPY);
		DeleteObject(hbmp);
		DeleteObject(hdcMem);

		hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hbmp, sizeof(BITMAP), &BitMap);
		cxBmp = BitMap.bmWidth;
		cyBmp = BitMap.bmHeight;
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, hbmp);
		BitBlt(hdc, 50+100*cal_X(game.rPhase), 95+100*cal_Y(game.rPhase), 15, 15, hdcMem, 245, 4, SRCCOPY);
		BitBlt(hdc, 100 + 100 * cal_X(game.bPhase), 95 + 100 * cal_Y(game.bPhase), 15, 15, hdcMem, 248, 32, SRCCOPY);
		if (game.rReady == 2) 
			BitBlt(hdc, 1092, 192, 15, 15, hdcMem, 245, 4, SRCCOPY);
		if (game.bReady == 2) 
			BitBlt(hdc, 1392, 192, 15, 15, hdcMem, 248, 32, SRCCOPY);

		{
			BitBlt(hdc, 150, 625, 24, 50, hdcMem, (game.rMoney /1000%10)* 24, 0, SRCCOPY);
			BitBlt(hdc, 175, 625, 24, 50, hdcMem, (game.rMoney /100 %10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 200, 625, 24, 50, hdcMem, (game.rMoney / 10 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 225, 625, 24, 50, hdcMem, (game.rMoney / 1 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 150, 675, 24, 50, hdcMem, (game.rFlag %10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 150, 725, 24, 50, hdcMem, (game.rTricks /10% 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 175, 725, 24, 50, hdcMem, (game.rTricks /1 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 450, 625, 24, 50, hdcMem, game.rlvl*24, 0, SRCCOPY);
			BitBlt(hdc, 450, 675, 24, 50, hdcMem,game.rDice*24 , 0, SRCCOPY);
			BitBlt(hdc, 650, 625, 24, 50, hdcMem, (game.bMoney / 1000 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 675, 625, 24, 50, hdcMem, (game.bMoney / 100 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 700, 625, 24, 50, hdcMem, (game.bMoney / 10 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 725, 625, 24, 50, hdcMem, (game.bMoney / 1 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 650, 675, 24, 50, hdcMem, (game.bFlag % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 650, 725, 24, 50, hdcMem, (game.bTricks / 10 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 675, 725, 24, 50, hdcMem, (game.bTricks / 1 % 10) * 24, 0, SRCCOPY);
			BitBlt(hdc, 950, 625, 24, 50, hdcMem, game.blvl * 24, 0, SRCCOPY);
			BitBlt(hdc, 950, 675, 24, 50, hdcMem, game.bDice * 24, 0, SRCCOPY);
		}

		DeleteObject(hbmp);
		DeleteObject(hdcMem);


		if (game.rReady >= 1 && game.bReady >= 1) {
			hbmp = (HBITMAP)LoadBitmap((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDB_BITMAP6));
			GetObject(hbmp, sizeof(BITMAP), &BitMap);
			cxBmp = BitMap.bmWidth;
			cyBmp = BitMap.bmHeight;
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hbmp);

			BitBlt(hdc, 1092, 450, 200, 200, hdcMem, (game.dice - 1) * 200, 0, SRCCOPY);

			DeleteObject(hbmp);
			DeleteObject(hdcMem);
		}


		if (game.rFlag == 6) {
			gamereset();
			MoveWindow(hwndShop, 0, 0, 0, 0, FALSE);
			MessageBox(hwnd, TEXT("红方旗帜胜利"), TEXT("游戏结束！"), MB_OK);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}

		if (game.bFlag == 6) {
			gamereset();
			MoveWindow(hwndShop, 0, 0, 0, 0, FALSE);
			MessageBox(hwnd, TEXT("蓝方旗帜胜利"), TEXT("游戏结束！"), MB_OK);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}

		if (game.rlvl == 9) {
			gamereset();
			MoveWindow(hwndShop, 0, 0, 0, 0, FALSE);
			MessageBox(hwnd, TEXT("红方科技胜利"), TEXT("游戏结束！"), MB_OK);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}

		if (game.blvl == 9) {
			gamereset();
			MoveWindow(hwndShop, 0, 0, 0, 0, FALSE);
			MessageBox(hwnd, TEXT("蓝方科技胜利"), TEXT("游戏结束！"), MB_OK);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}

		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

