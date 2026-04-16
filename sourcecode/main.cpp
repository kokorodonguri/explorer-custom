#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <random>

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"User32.lib")
#pragma comment (lib,"Gdi32.lib")

using namespace Gdiplus;
namespace fs = std::filesystem;

HWND g_hWndOverlay = NULL;
Image* g_pImage = nullptr;
ULONG_PTR g_gdiplusToken;
HWND g_hLastExplorer = NULL;
bool g_bImageErrorShown = false;

HWND g_hwndTargetArea = NULL;
int g_maxAreaSize = 0;
int g_lastWidth = 0;
int g_lastHeight = 0;

HWINEVENTHOOK g_hEventHook = NULL;
const WCHAR* CLASS_NAME = L"ExplorerOverlayClass";

fs::path GetExeDirectory() {
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return fs::path(path).parent_path();
}

void LoadRandomImage() {
    if (g_pImage) {
        delete g_pImage;
        g_pImage = nullptr;
    }

    fs::path exeDir = GetExeDirectory();
    fs::path imgDir = exeDir / L"img";

    if (!fs::exists(imgDir)) {
        fs::create_directories(imgDir);
    }

    std::vector<std::wstring> images;
    if (fs::exists(imgDir) && fs::is_directory(imgDir)) {
        for (const auto& entry : fs::directory_iterator(imgDir)) {
            auto ext = entry.path().extension().wstring();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
            if (ext == L".png" || ext == L".jpg" || ext == L".jpeg") {
                images.push_back(entry.path().wstring());
            }
        }
    }

    if (images.size() > 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, (int)images.size() - 1);
        g_pImage = new Image(images[distrib(gen)].c_str());
    }
    else {
        fs::path fallbackImg = exeDir / L"background.png";
        g_pImage = new Image(fallbackImg.c_str());
    }

    if (!g_pImage || g_pImage->GetLastStatus() != Ok) {
        if (!g_bImageErrorShown) {
            MessageBoxW(NULL, L"Image Error: Check img folder.", L"Notice", MB_ICONINFORMATION);
            g_bImageErrorShown = true;
        }
    }
    else {
        g_bImageErrorShown = false;
    }
}

void UpdateOverlay(HWND hwndOverlay, HWND hwndTarget, HWND hwndExplorer) {
    if (!g_pImage || g_pImage->GetLastStatus() != Ok) return;

    RECT rc;
    GetWindowRect(hwndTarget, &rc);
    int winWidth = rc.right - rc.left;
    int winHeight = rc.bottom - rc.top;

    if (winWidth <= 100 || winHeight <= 100) return;

    if (winWidth != g_lastWidth || winHeight != g_lastHeight) {
        HDC hdcScreen = GetDC(NULL);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, winWidth, winHeight);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

        Graphics graphics(hdcMem);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        graphics.Clear(Color(0, 0, 0, 0));

        UINT imgWidth = g_pImage->GetWidth();
        UINT imgHeight = g_pImage->GetHeight();

        int squareSize = (int)(winHeight * 0.45);
        int margin = 15;

        float scale = (float)squareSize / (float)(max(imgWidth, imgHeight));
        int drawWidth = (int)(imgWidth * scale);
        int drawHeight = (int)(imgHeight * scale);

        int drawX = winWidth - (drawWidth * 2) - margin;
        int drawY = winHeight - drawHeight - margin;

        graphics.DrawImage(g_pImage, drawX, drawY, drawWidth, drawHeight);

        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT ptPos = { rc.left, rc.top };
        SIZE sizeWnd = { winWidth, winHeight };
        POINT ptSrc = { 0, 0 };

        UpdateLayeredWindow(hwndOverlay, hdcScreen, &ptPos, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

        SelectObject(hdcMem, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcScreen);

        g_lastWidth = winWidth;
        g_lastHeight = winHeight;
    }

    SetWindowPos(hwndOverlay, HWND_TOPMOST, rc.left, rc.top, winWidth, winHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE);
}

BOOL CALLBACK FindFileListProc(HWND hwnd, LPARAM lParam) {
    char className[256];
    GetClassNameA(hwnd, className, sizeof(className));

    if (strcmp(className, "DirectUIHWND") == 0 || strcmp(className, "SysListView32") == 0) {
        if (IsWindowVisible(hwnd)) {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            int area = (rc.right - rc.left) * (rc.bottom - rc.top);
            if (area > g_maxAreaSize) {
                g_maxAreaSize = area;
                g_hwndTargetArea = hwnd;
            }
        }
    }
    return TRUE;
}

VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_OBJECT_LOCATIONCHANGE && idObject == OBJID_WINDOW) {
        if (g_hLastExplorer && (hwnd == g_hLastExplorer || hwnd == g_hwndTargetArea)) {
            if (g_hWndOverlay && IsWindow(g_hwndTargetArea) && IsWindowVisible(g_hwndTargetArea)) {
                UpdateOverlay(g_hWndOverlay, g_hwndTargetArea, g_hLastExplorer);
            }
        }
    }
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    HWND hwndForeground = GetForegroundWindow();
    if (!hwndForeground) return;

    char className[256];
    GetClassNameA(hwndForeground, className, sizeof(className));

    if (strcmp(className, "CabinetWClass") == 0) {
        if (g_hLastExplorer != hwndForeground) {
            LoadRandomImage();
            g_hLastExplorer = hwndForeground;
        }

        g_hwndTargetArea = NULL;
        g_maxAreaSize = 0;

        HWND hShellView = FindWindowExA(hwndForeground, NULL, "ShellTabWindowClass", NULL);
        if (!hShellView) hShellView = hwndForeground;

        EnumChildWindows(hShellView, FindFileListProc, 0);

        if (g_hwndTargetArea) {
            RECT rc;
            GetWindowRect(g_hwndTargetArea, &rc);

            if ((rc.right - rc.left) < 250) {
                ShowWindow(g_hWndOverlay, SW_HIDE);
            }
            else {
                UpdateOverlay(g_hWndOverlay, g_hwndTargetArea, hwndForeground);
            }
        }
    }
    else if (hwndForeground != g_hWndOverlay) {
        ShowWindow(g_hWndOverlay, SW_HIDE);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hOldWnd = FindWindowW(CLASS_NAME, L"ExplorerOverlay");
    while (hOldWnd) {
        SendMessageW(hOldWnd, WM_CLOSE, 0, 0);
        Sleep(50);
        hOldWnd = FindWindowW(CLASS_NAME, L"ExplorerOverlay");
    }

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wcex);

    g_hWndOverlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CLASS_NAME, L"ExplorerOverlay", WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    if (!g_hWndOverlay) return 0;

    LoadRandomImage();

    g_hEventHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    SetTimer(g_hWndOverlay, 1, 30, TimerProc);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hEventHook) UnhookWinEvent(g_hEventHook);
    if (g_pImage) delete g_pImage;
    GdiplusShutdown(g_gdiplusToken);
    return (int)msg.wParam;
}