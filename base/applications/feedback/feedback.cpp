/*
 * PROJECT:     BlastmasterOS Feedback
 * PURPOSE:     Native feedback submission interface for BlastmasterOS
 * LICENSE:     GPL-2.0-or-later
 *
 * The UI is intentionally independent from the feedback delivery service.
 * Do not put mail passwords, API keys, or other service credentials here.
 */

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <string>
#include <vector>

#define IDC_CATEGORY       1001
#define IDC_SUMMARY        1002
#define IDC_ENVIRONMENT    1003
#define IDC_STEPS          1004
#define IDC_EXPECTED       1005
#define IDC_ACTUAL         1006
#define IDC_ATTACHMENTS    1007
#define IDC_ADD_ATTACHMENT 1008
#define IDC_REMOVE_FILE    1009
#define IDC_SUBMIT         1010
#define IDC_CLOSE          1011

static const WCHAR g_szClassName[] = L"BlastmasterOSFeedbackWindow";
static HFONT g_hFont = NULL;
static std::vector<std::wstring> g_Attachments;

static HWND AddControl(HWND hParent, DWORD exStyle, LPCWSTR type,
                       LPCWSTR text, DWORD style, int x, int y,
                       int width, int height, int id)
{
    HWND hControl = CreateWindowExW(
        exStyle, type, text, WS_CHILD | WS_VISIBLE | style,
        x, y, width, height, hParent,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
        GetModuleHandleW(NULL), NULL);

    if (hControl && g_hFont)
        SendMessageW(hControl, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFont), TRUE);

    return hControl;
}

static void AddLabel(HWND hWnd, LPCWSTR text, int x, int y)
{
    AddControl(hWnd, 0, L"STATIC", text, SS_LEFT, x, y, 780, 20, 0);
}

static HWND AddTextField(HWND hWnd, int id, int x, int y, int width,
                         int height, bool multiline)
{
    DWORD style = WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL;
    if (multiline)
        style = WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;

    return AddControl(hWnd, WS_EX_CLIENTEDGE, L"EDIT", L"",
                      style, x, y, width, height, id);
}

static std::wstring ReadText(HWND hWnd, int id)
{
    HWND hControl = GetDlgItem(hWnd, id);
    int length = GetWindowTextLengthW(hControl);
    std::wstring value(static_cast<size_t>(length) + 1, L'\0');
    if (length)
        GetWindowTextW(hControl, &value[0], length + 1);
    value.resize(static_cast<size_t>(length));
    return value;
}

static void RefreshAttachmentList(HWND hWnd)
{
    HWND hList = GetDlgItem(hWnd, IDC_ATTACHMENTS);
    SendMessageW(hList, LB_RESETCONTENT, 0, 0);

    for (size_t i = 0; i < g_Attachments.size(); ++i)
        SendMessageW(hList, LB_ADDSTRING, 0,
                     reinterpret_cast<LPARAM>(g_Attachments[i].c_str()));
}

static void AddAttachments(HWND hWnd)
{
    WCHAR files[32768] = {};
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"All files (*.*)\0*.*\0";
    ofn.lpstrFile = files;
    ofn.nMaxFile = static_cast<DWORD>(_countof(files));
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

    if (!GetOpenFileNameW(&ofn))
        return;

    WCHAR *p = files;
    std::wstring directory = p;
    p += directory.length() + 1;

    if (*p == L'\0')
    {
        g_Attachments.push_back(directory);
    }
    else
    {
        while (*p)
        {
            std::wstring fullPath = directory;
            fullPath += L"\\";
            fullPath += p;
            g_Attachments.push_back(fullPath);
            p += wcslen(p) + 1;
        }
    }

    RefreshAttachmentList(hWnd);
}

static void RemoveSelectedAttachment(HWND hWnd)
{
    HWND hList = GetDlgItem(hWnd, IDC_ATTACHMENTS);
    LRESULT selected = SendMessageW(hList, LB_GETCURSEL, 0, 0);
    if (selected == LB_ERR || static_cast<size_t>(selected) >= g_Attachments.size())
        return;

    g_Attachments.erase(g_Attachments.begin() + selected);
    RefreshAttachmentList(hWnd);
}

static void SubmitReport(HWND hWnd)
{
    std::wstring category = L"Bug Report";
    if (SendMessageW(GetDlgItem(hWnd, IDC_CATEGORY), CB_GETCURSEL, 0, 0) == 1)
        category = L"Feature Request";

    std::wstring summary = ReadText(hWnd, IDC_SUMMARY);
    if (summary.empty())
    {
        MessageBoxW(hWnd, L"Please enter a short summary before submitting.",
                    L"BlastmasterOS Feedback", MB_OK | MB_ICONWARNING);
        SetFocus(GetDlgItem(hWnd, IDC_SUMMARY));
        return;
    }

    /*
     * Submission is deliberately not simulated. The HTTPS endpoint and
     * server-side delivery service must be configured before this button
     * can transmit reports. Attachments remain local until that is done.
     */
    MessageBoxW(hWnd,
        L"The feedback form is ready, but online submission has not been "
        L"configured yet.\n\nNo report or attachment was sent. "
        L"Please try again after the BlastmasterOS Feedback service is enabled.",
        L"Feedback service not configured", MB_OK | MB_ICONINFORMATION);
}

static void CreateFeedbackControls(HWND hWnd)
{
    int x = 18;
    int width = 790;

    AddLabel(hWnd, L"Help improve BlastmasterOS by reporting a problem or suggesting a feature.", x, 14);

    AddLabel(hWnd, L"Feedback type:", x, 45);
    HWND hCategory = AddControl(hWnd, 0, L"COMBOBOX", L"",
        CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL, x, 66, 280, 180, IDC_CATEGORY);
    SendMessageW(hCategory, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Bug Report"));
    SendMessageW(hCategory, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Feature Request"));
    SendMessageW(hCategory, CB_SETCURSEL, 0, 0);

    AddLabel(hWnd, L"Summary:", x, 101);
    AddTextField(hWnd, IDC_SUMMARY, x, 122, width, 26, false);

    AddLabel(hWnd, L"Environment (version, edition, hardware/VM, file system, tested software):", x, 158);
    AddTextField(hWnd, IDC_ENVIRONMENT, x, 179, width, 62, true);

    AddLabel(hWnd, L"Steps to Reproduce:", x, 249);
    AddTextField(hWnd, IDC_STEPS, x, 270, width, 82, true);

    AddLabel(hWnd, L"Expected Behavior:", x, 360);
    AddTextField(hWnd, IDC_EXPECTED, x, 381, width, 62, true);

    AddLabel(hWnd, L"Actual Behavior:", x, 451);
    AddTextField(hWnd, IDC_ACTUAL, x, 472, width, 62, true);

    AddLabel(hWnd, L"Attachments (debug logs, screenshots, or short videos):", x, 542);
    AddControl(hWnd, WS_EX_CLIENTEDGE, L"LISTBOX", L"",
        WS_TABSTOP | LBS_NOTIFY | WS_VSCROLL, x, 563, 600, 94, IDC_ATTACHMENTS);
    AddControl(hWnd, 0, L"BUTTON", L"Add Files...",
        WS_TABSTOP | BS_PUSHBUTTON, 630, 563, 178, 28, IDC_ADD_ATTACHMENT);
    AddControl(hWnd, 0, L"BUTTON", L"Remove Selected",
        WS_TABSTOP | BS_PUSHBUTTON, 630, 597, 178, 28, IDC_REMOVE_FILE);

    AddControl(hWnd, 0, L"BUTTON", L"Submit Report",
        WS_TABSTOP | BS_DEFPUSHBUTTON, 630, 680, 178, 30, IDC_SUBMIT);
    AddControl(hWnd, 0, L"BUTTON", L"Close",
        WS_TABSTOP | BS_PUSHBUTTON, 530, 680, 88, 30, IDC_CLOSE);

    AddLabel(hWnd, L"Required for crash, freeze, or visual bug reports: include relevant logs or screenshots.", x, 665);
}

static LRESULT CALLBACK FeedbackWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            CreateFeedbackControls(hWnd);
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_ADD_ATTACHMENT:
                    AddAttachments(hWnd);
                    return 0;
                case IDC_REMOVE_FILE:
                    RemoveSelectedAttachment(hWnd);
                    return 0;
                case IDC_SUBMIT:
                    SubmitReport(hWnd);
                    return 0;
                case IDC_CLOSE:
                    DestroyWindow(hWnd);
                    return 0;
            }
            break;

        case WM_DESTROY:
            g_Attachments.clear();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    INITCOMMONCONTROLSEX controls = {};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&controls);

    g_hFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = FeedbackWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
    wc.lpszClassName = g_szClassName;

    if (!RegisterClassExW(&wc))
        return 1;

    HWND hWnd = CreateWindowExW(
        WS_EX_APPWINDOW, g_szClassName, L"BlastmasterOS Feedback",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 850, 770,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
        return 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
