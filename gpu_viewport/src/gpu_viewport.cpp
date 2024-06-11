#include "gpu_viewport.hpp"
#include <iostream>

#define WM_GPU_VIEWPORT_RESIZE (WM_USER + 1)

vb::ROI JSObjectToROI(const Napi::Object& obj) {
    vb::ROI roi;
    roi.x = obj.Get("x").As<Napi::Number>().Int32Value();
    roi.y = obj.Get("y").As<Napi::Number>().Int32Value();
    roi.width = obj.Get("width").As<Napi::Number>().Int32Value();
    roi.height = obj.Get("height").As<Napi::Number>().Int32Value();
    return roi;
}

Napi::Object ROIToJSObject(const Napi::Env& env, const vb::ROI& roi) {
    Napi::Object obj = Napi::Object::New(env);
    obj.Set("x", Napi::Number::New(env, roi.x));
    obj.Set("y", Napi::Number::New(env, roi.y));
    obj.Set("width", Napi::Number::New(env, roi.width));
    obj.Set("height", Napi::Number::New(env, roi.height));
    return obj;
}

struct gpu_viewport_window_context {
    vb::VBCore* core;
};

LRESULT CALLBACK gpu_viewport_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto context = reinterpret_cast<gpu_viewport_window_context*>(GetProp(hwnd, L"gpu_viewport_window_ctx"));

    switch (uMsg) {
    case WM_DESTROY: {
        if (context && context->core) delete context->core;
        PostQuitMessage(0);
        return 0;
    }
    case WM_PAINT: {
        return 0;
    }
    case WM_GPU_VIEWPORT_RESIZE: {
        vb::ROI* roi = (vb::ROI*)(lParam);
        SetWindowPos(hwnd, NULL, roi->x, roi->y, roi->width, roi->height, SWP_NOZORDER);
        if (context && context->core) context->core->Refresh();
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RegisterWindowClass(std::wstring& window_class_name)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC | CS_NOCLOSE;
    wc.lpfnWndProc = gpu_viewport_window_proc;
    wc.hInstance = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = window_class_name.c_str();

    RegisterClassEx(&wc);
}

Napi::Object gpu_viewport::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "GPUViewport", {
        gpu_viewport::InstanceMethod("initDetector", &gpu_viewport::InitDetector),
        gpu_viewport::InstanceMethod("initRenderer", &gpu_viewport::InitRenderer),
        gpu_viewport::InstanceMethod("startStream", &gpu_viewport::StartStream),
        gpu_viewport::InstanceMethod("stopStream", &gpu_viewport::StopStream),
        gpu_viewport::InstanceMethod("getROI", &gpu_viewport::GetROI),
        gpu_viewport::InstanceMethod("setROI", &gpu_viewport::SetROI),
        gpu_viewport::InstanceMethod("getImageSize", &gpu_viewport::GetImageSize),
        gpu_viewport::InstanceMethod("setWindowPosition", &gpu_viewport::SetWindowPosition),
        gpu_viewport::InstanceMethod("cleanup", &gpu_viewport::Cleanup)
    });

    exports.Set("GPUViewport", func);
    return exports;
}

gpu_viewport::gpu_viewport(const Napi::CallbackInfo& info) : Napi::ObjectWrap<gpu_viewport>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    RegisterWindowClass(gpu_window_class_name);
    main_window = reinterpret_cast<HWND>(info[0].ToNumber().Uint32Value());

    RECT rect;
    GetWindowRect(main_window, &rect);
    int parent_width = rect.right - rect.left;
    int parent_height = rect.bottom - rect.top;

    gpu_viewport_window = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_COMPOSITED,
        gpu_window_class_name.c_str(),
        nullptr,
        WS_VISIBLE | WS_POPUP,
        0,
        0,
        parent_width / 2,
        parent_height / 2,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    SetParent(gpu_viewport_window, main_window);

    // vb::CoreCreateInfo core_create_info {
    //     .file = vb::LogLevel::Trace,
    //     .console = vb::LogLevel::Trace,
    //     .filepath = "log.txt"
    // };

    core = new vb::VBCore();

    SetLayeredWindowAttributes(gpu_viewport_window, 0, 255, LWA_ALPHA);
    SetWindowPos(gpu_viewport_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    gpu_viewport_window_ctx = std::make_unique<gpu_viewport_window_context>();
    gpu_viewport_window_ctx->core = core;

    SetProp(gpu_viewport_window, L"gpu_viewport_window_ctx", static_cast<HANDLE>(gpu_viewport_window_ctx.get()));
}

Napi::Value gpu_viewport::Cleanup(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    core->StopStream();
    return env.Null();
}

Napi::Value gpu_viewport::SetWindowPosition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    vb::ROI roi = JSObjectToROI(info[0].As<Napi::Object>());
    SendMessage(gpu_viewport_window, WM_GPU_VIEWPORT_RESIZE, 0, (LPARAM)&roi);
    return env.Null();
}

Napi::Value gpu_viewport::InitDetector(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, core->InitDetector());
}

Napi::Value gpu_viewport::InitRenderer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // vb::RenderCreateInfo render_create_info {
    //     .window_handle = static_cast<void*>(gpu_viewport_window),
    //     .shader_path = nullptr,
    //     .dark_map_path = nullptr,
    //     .gain_map_path = nullptr,
    //     .defect_map_path = nullptr
    // };

    return Napi::Number::New(env, core->InitRender(static_cast<void*>(gpu_viewport_window)));
}

Napi::Value gpu_viewport::ConfigureStream(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected a single number parameter").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, core->ConfigureStream(info[0].ToNumber()));
}

Napi::Value gpu_viewport::StartStream(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    core->ConfigureStream(1670);
    return Napi::Number::New(env, core->StartStream());
}
Napi::Value gpu_viewport::StopStream(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    return Napi::Number::New(env, core->StopStream());
};

Napi::Value gpu_viewport::GetROI(const Napi::CallbackInfo& info) {
    return ROIToJSObject(info.Env(), core->GetROI());
}

Napi::Value gpu_viewport::SetROI(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object obj = info[0].As<Napi::Object>();
    vb::ROI roi = JSObjectToROI(obj);

    return Napi::Number::New(env, core->SetROI(roi));
}

Napi::Value gpu_viewport::GetImageSize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return ROIToJSObject(info.Env(), core->GetImageSize());
}