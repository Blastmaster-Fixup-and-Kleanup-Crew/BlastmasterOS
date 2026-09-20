#include "freecell.h"

CardWindow FreeCellWnd;

HWND hwndMain;
HWND hwndStatus;
HMENU hGameMenu;
HINSTANCE hInstance;

TCHAR szAppName[128];
TCHAR MsgAbout[256];
TCHAR MsgQuit[256];
TCHAR MsgWin[256];

static LRESULT CALLBACK WndProcInternal(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

LRESULT CALLBACK WndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    return WndProcInternal(hwnd, message, wParam, lParam);
}

int WINAPI _tWinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInstance,
    LPTSTR lpCmdLine,
    int nCmdShow)
{
    WNDCLASS wndclass;
    MSG msg;
    HWND hwnd;

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    hInstance = hInst;

    LoadString(
        hInst,
        IDS_FREECELL_NAME,
        szAppName,
        sizeof(szAppName) / sizeof(TCHAR));

    LoadString(
        hInst,
        IDS_FREECELL_ABOUT,
        MsgAbout,
        sizeof(MsgAbout) / sizeof(TCHAR));

    LoadString(
        hInst,
        IDS_FREECELL_QUIT,
        MsgQuit,
        sizeof(MsgQuit) / sizeof(TCHAR));

    LoadString(
        hInst,
        IDS_FREECELL_WIN,
        MsgWin,
        sizeof(MsgWin) / sizeof(TCHAR));

    ZeroMemory(&wndclass, sizeof(wndclass));

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInst;
    wndclass.hIcon = LoadIcon(
        hInst,
        MAKEINTRESOURCE(IDI_FREECELL));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground =
        (HBRUSH)(COLOR_BTNFACE + 1);
    wndclass.lpszMenuName =
        MAKEINTRESOURCE(IDR_MENU1);
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
        return 1;

    hwnd = CreateWindow(
        szAppName,
        szAppName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        720,
        NULL,
        NULL,
        hInst,
        NULL);

    if (hwnd == NULL)
        return 1;

    hwndMain = hwnd;
    hGameMenu = GetSubMenu(GetMenu(hwnd), 0);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

static LRESULT CALLBACK WndProcInternal(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    static int statusHeight;

    switch (message)
    {
        case WM_CREATE:
        {
            RECT statusRect;

            hwndStatus = CreateStatusWindow(
                WS_CHILD | WS_VISIBLE | CCS_BOTTOM,
                _T("Ready"),
                hwnd,
                1);

            FreeCellWnd.Create(
                hwnd,
                0,
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                100,
                100);

            CreateFreeCell();

            GetWindowRect(hwndStatus, &statusRect);
            statusHeight =
                statusRect.bottom - statusRect.top;

            NewGame();
            return 0;
        }

        case WM_SIZE:
        {
            int width;
            int height;

            width = LOWORD(lParam);
            height = HIWORD(lParam);

            MoveWindow(
                FreeCellWnd,
                0,
                0,
                width,
                height - statusHeight,
                TRUE);

            MoveWindow(
                hwndStatus,
                0,
                height - statusHeight,
                width,
                statusHeight,
                TRUE);

            return 0;
        }

        case WM_GETMINMAXINFO:
        {
            MINMAXINFO* minmax;
            int minimumWidth;
            int minimumHeight;

            minmax = (MINMAXINFO*)lParam;

            minimumWidth =
                X_BORDER +
                NUM_TABLEAU *
                    (__cardwidth + X_STACK_BORDER) +
                X_BORDER;

            minimumHeight =
                GetSystemMetrics(SM_CYCAPTION) +
                GetSystemMetrics(SM_CYMENU) +
                Y_BORDER +
                __cardheight +
                Y_TABLEAU_BORDER +
                5 * (__cardheight / 5) +
                Y_BORDER +
                24;

            minmax->ptMinTrackSize.x = minimumWidth;
            minmax->ptMinTrackSize.y = minimumHeight;

            return 0;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDM_GAME_NEW:
                    NewGame();
                    return 0;

                case IDM_HELP_ABOUT:
                    MessageBox(
                        hwnd,
                        MsgAbout,
                        szAppName,
                        MB_OK | MB_ICONINFORMATION);
                    return 0;

                case IDM_GAME_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    return 0;
            }

            break;
        }

        case WM_CLOSE:
        {
            if (!fGameStarted)
            {
                DestroyWindow(hwnd);
                return 0;
            }

            if (MessageBox(
                    hwnd,
                    MsgQuit,
                    szAppName,
                    MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                DestroyWindow(hwnd);
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam);
}
