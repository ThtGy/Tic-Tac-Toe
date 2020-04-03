// Tic Tac Toe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsx.h>
#include "Tic Tac Toe.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//custom global
const int CELL_SIZE = 100;

class CTicTacToe
{
private:
	HBRUSH hbr1, hbr2;
	HICON hIcon1, hIcon2;
	int playerTurn = 1;
	int gameBoard[9] = { 0 };
	int winner = 0;
	int wins[3];

public:
	//Getters
	HBRUSH getBrush(int select)
	{
		return select == 2 ? hbr2 : hbr1;
	}

	HICON getIcon(int select)
	{
		return select == 2 ? hIcon2 : hIcon1;
	}

	int getTurn()
	{
		return playerTurn;
	}

	int getBoardSize()
	{
		return ARRAYSIZE(gameBoard);
	}

	int getBoardIndex(int index)
	{
		return gameBoard[index];
	}

	int getWinner()
	{
		return winner;
	}

	int* getWins()
	{
		return wins;
	}

	//Setters
	void BrushCreate()
	{
		hbr1 = CreateSolidBrush(RGB(255, 0, 0));
		hbr2 = CreateSolidBrush(RGB(0, 0, 255));
	}

	void IconCreate()
	{
		hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
		hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
	}

	void BrushDestroy()
	{
		DeleteObject(hbr1);
		DeleteObject(hbr2);
	}

	void IconDestroy()
	{
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
	}
	void setBoardIndex(int index, int data)
	{
		gameBoard[index] = data;
	}

	void setTurn(int player)
	{
		playerTurn = player;
	}

	void Restart()
	{
		playerTurn = 1;
		winner = 0;
		ZeroMemory(gameBoard, sizeof(gameBoard));
	}

	bool GetGameBoardRect(HWND hwnd, RECT* pRect)
	{
		RECT rc;
		if (GetClientRect(hwnd, &rc))
		{
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			pRect->left = (width - CELL_SIZE * 3) / 2;
			pRect->top = (height - CELL_SIZE * 3) / 2;

			pRect->right = pRect->left + CELL_SIZE * 3;
			pRect->bottom = pRect->top + CELL_SIZE * 3;

			return true;
		}

		SetRectEmpty(pRect);
		return false;
	}

	void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
	{
		MoveToEx(hdc, x1, y1, NULL);
		LineTo(hdc, x2, y2);
	}

	int GetCellNumberFromPoint(HWND hwnd, int x, int y)
	{
		RECT rc;
		POINT pt = { x, y };

		if (GetGameBoardRect(hwnd, &rc))
		{
			if (PtInRect(&rc, pt))
			{ //User clicked inside game board
			  //Normalize x and y (0 to 3 * CELL_SIZE)
				x = pt.x - rc.left;
				y = pt.y - rc.top;

				int column = x / CELL_SIZE;
				int row = y / CELL_SIZE;

				//Convert to index
				return column + row * 3;
			}
		}

		return -1; //Outside game board or failure
	}

	bool GetCellRect(HWND hWnd, int index, RECT *pRect)
	{
		RECT rcBoard;

		SetRectEmpty(pRect);
		if (index < 0 || index > 8)
			return false;

		if (GetGameBoardRect(hWnd, &rcBoard))
		{
			//Convert index from 0 to 8 into x,y
			int y = index / 3; //Row number
			int x = index % 3; //Column number

			pRect->left = rcBoard.left + x * CELL_SIZE + 1;
			pRect->top = rcBoard.top + y * CELL_SIZE + 1;

			pRect->right = pRect->left + CELL_SIZE - 1;
			pRect->bottom = pRect->top + CELL_SIZE - 1;

			return true;
		}

		return false;
	}

	//Returns
	//0 - No winner
	//1 - Player 1 wins
	//2 - Player 2 wins
	//3 - Cat game
	/*
	0, 1, 2
	3, 4, 5
	6, 7, 8
	*/
	void CheckWinner()
	{
		int cells[] = { 0, 1, 2,   3, 4, 5,   6, 7, 8,   0, 3, 6,   1, 4, 7,   2, 5, 8,   0, 4, 8,   2, 4, 6 };

		for (int i = 0; i < ARRAYSIZE(cells); i += 3)
		{
			if (gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]] && 0 != gameBoard[cells[i]])
			{
				//Winner
				wins[0] = cells[i];
				wins[1] = cells[i + 1];
				wins[2] = cells[i + 2];

				winner = gameBoard[cells[i]];
				return;
			}
		}

		//Check if any cells are left
		for (int i = 0; i < ARRAYSIZE(gameBoard); i++)
		{
			if (0 == gameBoard[i])
			{
				winner = 0;
				return;
			}
		}

		//Draw
		winner = 3;
		return;
	}

	void ShowTurn(HWND hwnd, HDC hdc)
	{
		RECT rc;
		const WCHAR szTurn1[] = L"Player 1 Turn";
		const WCHAR szTurn2[] = L"Player 2 Turn";
		const WCHAR* pszTurnText = NULL;

		switch (winner)
		{
		case 0: //Continue game
			pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
			break;
		case 1: //Player one wins
			pszTurnText = L"Player 1 wins!";
			break;
		case 2: //Player two wins
			pszTurnText = L"Player 2 wins!";
			break;
		case 3: //Draw
			pszTurnText = L"Cat's game!";
			break;
		}

		if (NULL != pszTurnText && GetClientRect(hwnd, &rc))
		{
			rc.top = rc.bottom - 48;
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
			SetTextColor(hdc, RGB(255, 255, 255));
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
		}
	}

	void DrawIconCentered(HDC hdc, RECT * pRect, HICON hIcon)
	{
		const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
		const int ICON_HEIGHT = GetSystemMetrics(SM_CYICON);
		if (NULL != pRect)
		{
			int left = pRect->left + (pRect->right - pRect->left - ICON_WIDTH) / 2;
			int top = pRect->top + (pRect->bottom - pRect->top - ICON_HEIGHT) / 2;
			DrawIcon(hdc, left, top, hIcon);
		}
	}

	void HighlightWinner(HWND hWnd, HDC hdc)
	{
		RECT rcWin;

		for (int i = 0; i < 3; i++)
		{
			if (GetCellRect(hWnd, wins[i], &rcWin))
			{
				FillRect(hdc, &rcWin, winner == 2 ? hbr2 : hbr1);
				DrawIconCentered(hdc, &rcWin, winner == 2 ? hIcon2 : hIcon1);
			}
		}
	}
};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//class variable declaration
CTicTacToe game;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			game.BrushCreate();

			//Load player icon
			game.IconCreate();
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
			{
				int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);
				if (IDYES == ret)
				{
					//Reset and start new game
					game.Restart();

					//Force a paint message
					InvalidateRect(hWnd, NULL, true);
					UpdateWindow(hWnd);
				}
			}
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			//Only handle clicks if game is not over
			if (game.getWinner() == 0)
			{
				int index = game.GetCellNumberFromPoint(hWnd, xPos, yPos);

				HDC hdc = GetDC(hWnd);
				if (hdc != NULL)
				{
					WCHAR temp[100];
					wsprintf(temp, L"Index = %d", index);


					//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

					//Get cell dimension from index
					if (index != -1)
					{
						RECT rcCell;
						if (game.GetCellRect(hWnd, index, &rcCell) && game.getBoardIndex(index) == 0)
						{
							game.setBoardIndex(index, game.getTurn());
							//FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);

							game.DrawIconCentered(hdc, &rcCell, game.getTurn() == 2 ? game.getIcon(2) : game.getIcon(1));

							//Check for winner
							game.CheckWinner();
							if (game.getWinner() == 1 || game.getWinner() == 2)
							{
								game.HighlightWinner(hWnd, hdc);

								//Winner!
								MessageBox(hWnd, game.getWinner() == 1 ? L"Player 1 is the winner!" : L"Player 2 is the winner!", L"Winner!",
									MB_OK | MB_ICONINFORMATION);
								game.setTurn(0);
							}
							else if (game.getWinner() == 3)
							{
								//Draw
								MessageBox(hWnd, L"Cat's game!", L"Draw!", MB_OK | MB_ICONINFORMATION);
							}
							else if (game.getWinner() == 0)
							{
								game.setTurn(game.getTurn() == 2 ? 1 : 2);
							}
							game.ShowTurn(hWnd, hdc);
						}

					}
					ReleaseDC(hWnd, hdc);
				}
			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
		MINMAXINFO* pMINMAX = (MINMAXINFO*)lParam;

		pMINMAX->ptMinTrackSize.x = CELL_SIZE * 4;
		pMINMAX->ptMinTrackSize.y = CELL_SIZE * 4;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			RECT rc;

			if (game.GetGameBoardRect(hWnd, &rc))
			{
				RECT rcClient;

				if (GetClientRect(hWnd, &rcClient))
				{
					SetBkMode(hdc, TRANSPARENT);
					//Draw Player1 and Player2 text
					SetTextColor(hdc, RGB(255, 255, 0));
					TextOut(hdc, 16, 16, L"Player 1", 8);
					DrawIcon(hdc, 24, 48, game.getIcon(1));
					SetTextColor(hdc, RGB(0, 0, 255));
					TextOut(hdc, rcClient.right - 72, 16, L"Player 2", 8);
					DrawIcon(hdc, rcClient.right - 64, 48, game.getIcon(2));

					game.ShowTurn(hWnd, hdc);
				}

				FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}
			
			for (int i = 0; i < 4; i++)
			{
				//Draw vertical lines
				game.DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
				//Draw horizontal lines
				game.DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}

			//Redraw occupied cells
			RECT rcCell;
			for (int i = 0; i < game.getBoardSize(); i++)
			{
				if (game.GetCellRect(hWnd, i, &rcCell) && 0 != game.getBoardIndex(i))
				{
					//FillRect(hdc, &rcCell, gameBoard[i] == 2 ? hbr2 : hbr1);
					game.DrawIconCentered(hdc, &rcCell, game.getBoardIndex(i) == 2 ? game.getIcon(2) : game.getIcon(1));
				}
			}

			if (game.getWinner() == 1 || game.getWinner() == 2)
			{
				game.HighlightWinner(hWnd, hdc);
			}

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		game.BrushDestroy();
		game.IconDestroy();

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
