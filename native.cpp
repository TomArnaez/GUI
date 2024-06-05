#include <napi.h>
#include <iostream>
#include <cassert>
#include <string>
#include <cstdint>
#include <mutex>  // Include mutex header

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <thread>

#define VIEWPORT_WIDTH 300
#define VIEWPORT_HEIGHT 300
#define WINDOW_CLASS_NAME "MyWindowClass"

HWND childWindowHandle = nullptr;
bool resizing = false;
std::mutex resizeMutex;  // Declare a mutex

void DrawContent(HWND hwnd);

// from https://github.com/stream-labs/obs-studio-node/blob/36eeb480f36c9c414fb73223d144f4889e331029/obs-studio-client/source/nodeobs_display.cpp#L32
static BOOL CALLBACK EnumChromeWindowsProc(HWND hwnd, LPARAM lParam)
{
    char buf[256];
    if (GetClassNameA(hwnd, buf, sizeof(buf) / sizeof(*buf))) {
        if (strstr(buf, "Intermediate D3D Window")) {
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            if ((style & WS_CLIPSIBLINGS) == 0) {
                style |= WS_CLIPSIBLINGS;
                SetWindowLongPtr(hwnd, GWL_STYLE, style);
            }
        }
    }
    return TRUE;
}

// from https://github.com/stream-labs/obs-studio-node/blob/36eeb480f36c9c414fb73223d144f4889e331029/obs-studio-client/source/nodeobs_display.cpp#L47
static void FixChromeD3DIssue(HWND chromeWindow)
{
    // auto handle = FindWindowEx(chromeWindow, nullptr, "Intermediate D3D Window", "");
    // assert(handle != nullptr);
    (void)EnumChildWindows(chromeWindow, EnumChromeWindowsProc, 0);

    LONG_PTR style = GetWindowLongPtr(chromeWindow, GWL_STYLE);
    if ((style & WS_CLIPCHILDREN) == 0) {
        style |= WS_CLIPCHILDREN;
        SetWindowLongPtr(chromeWindow, GWL_STYLE, style);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_PAINT:
        DrawContent(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DrawContent(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Drawing code here, for example, filling the client area with red
    RECT rect;
    GetClientRect(hwnd, &rect);
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Red brush
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    EndPaint(hwnd, &ps);
}

static void RegisterWindowClass()
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC | CS_NOCLOSE | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    RegisterClassEx(&wc);
}

static HWND CreateChildWindow(HWND parent)
{
    RECT rect;
    GetWindowRect(parent, &rect);
    int parentWidth = rect.right - rect.left;
    int parentHeight = rect.bottom - rect.top;

    auto child = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_COMPOSITED,
        WINDOW_CLASS_NAME,
        nullptr,
        WS_VISIBLE | WS_POPUP | WS_BORDER,
        0,
        0,
        parentWidth / 2,
        parentHeight / 2,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    SetParent(child, parent);
    SetLayeredWindowAttributes(child, 0, 255, LWA_ALPHA);

    // Force the window to be on top
    SetWindowPos(child, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    return child;
}

void run(HWND hwnd) {
    RegisterWindowClass();
    FixChromeD3DIssue(hwnd);
    childWindowHandle = CreateChildWindow(hwnd);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#include <chrono>


void startThread(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    HWND hwnd = reinterpret_cast<HWND>(info[0].ToNumber().Uint32Value());
    std::thread thread(run, hwnd);
    thread.detach();
}

void resizeChildWindow(const Napi::CallbackInfo& info) {
    using namespace std::chrono;

    napi_value promise;

    Napi::Env env = info.Env();
    if (childWindowHandle == nullptr) {
        Napi::TypeError::New(env, "Child window not created").ThrowAsJavaScriptException();
        return;
    }
    int width = info[0].As<Napi::Number>().Int32Value();
    int height = info[1].As<Napi::Number>().Int32Value();

    // Capture the start time
    auto start = high_resolution_clock::now();

    // Make the SetWindowPos call
    // SetWindowPos(childWindowHandle, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE | SWP_ASYNCWINDOWPOS);
    DWORD dwLastError = GetLastError();
    std::cout << dwLastError << std::endl;

    // Capture the end time
    auto end = high_resolution_clock::now();

    // Calculate the duration
    auto duration = duration_cast<nanoseconds>(end - start).count();

    // Output the duration
    std::cout << "SetWindowPos took " << duration << " nanoseconds" << std::endl;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startThread"), Napi::Function::New(env, startThread));
    exports.Set(Napi::String::New(env, "resizeChildWindow"), Napi::Function::New(env, resizeChildWindow));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
