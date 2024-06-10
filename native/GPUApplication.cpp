#include "GPUApplication.hpp"
#include <iostream>

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

struct MainWindowContext {
    HWND child_window = nullptr;
    WNDPROC original_proc = nullptr;
    vb::VBCore* core = nullptr;
};

struct GPUViewportWindowContext {
    vb::VBCore* core = nullptr;
};

LRESULT CALLBACK GPUViewportWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    auto context = reinterpret_cast<GPUViewportWindowContext*>(GetProp(hwnd, L"GPUViewportWindowContext"));

    switch (uMsg) {
    case WM_SIZE: {
        if (context && context->core) context->core->Refresh();
        return 0;
    }
    case WM_DESTROY: {
        if (context && context->core) delete context->core;
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK SubclassProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* context = reinterpret_cast<MainWindowContext*>(GetProp(hwnd, L"Context"));

    switch (message) {
        // case WM_SIZE:
        //     if (context && context->child_window) {
        //         RECT rect;
        //         GetClientRect(hwnd, &rect);
        //         MoveWindow(context->child_window, 0, 0, rect.right / 2, rect.bottom / 2, false);
        //     }
        //     return 0;
    }
    return CallWindowProc(context->original_proc, hwnd, message, wParam, lParam);
}

void RegisterWindowClass(std::wstring& window_class_name)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC | CS_NOCLOSE;
    wc.lpfnWndProc = GPUViewportWindowProc;
    wc.hInstance = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = window_class_name.c_str();

    RegisterClassEx(&wc);
}

Napi::Object GPUApplication::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "GPUApplication", {
        GPUApplication::InstanceMethod("InitDetector", &GPUApplication::InitDetector),
        GPUApplication::InstanceMethod("InitRenderer", &GPUApplication::InitRenderer),
        GPUApplication::InstanceMethod("StartStream", &GPUApplication::StartStream),
        GPUApplication::InstanceMethod("GetROI", &GPUApplication::GetROI),
        GPUApplication::InstanceMethod("SetROI", &GPUApplication::SetROI),
        GPUApplication::InstanceMethod("GetImageSize", &GPUApplication::GetImageSize),
        GPUApplication::InstanceMethod("SetWindowPosition", &GPUApplication::SetWindowPosition)
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("GPUApplication", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);
    return exports;
}

GPUApplication::GPUApplication(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GPUApplication>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    RegisterWindowClass(gpu_window_class_name);
    main_window = reinterpret_cast<HWND>(info[0].ToNumber().Uint32Value());

    main_window_context = std::make_unique<MainWindowContext>();
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

    core = new vb::VBCore();

    main_window_context->child_window = gpu_viewport_window;
    main_window_context->original_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(main_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubclassProc)));

    SetLayeredWindowAttributes(gpu_viewport_window, 0, 255, LWA_ALPHA);
    SetWindowPos(gpu_viewport_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    gpu_viewport_window_context = std::make_unique<GPUViewportWindowContext>();
    gpu_viewport_window_context->core = core;

    SetProp(main_window, L"Context", reinterpret_cast<HANDLE>(main_window_context.get()));
    SetProp(gpu_viewport_window, L"GPUViewportWindowContext", reinterpret_cast<HANDLE>(gpu_viewport_window_context.get()));
}

Napi::Value GPUApplication::SetWindowPosition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    vb::ROI roi = JSObjectToROI(info[0].As<Napi::Object>());
    MoveWindow(gpu_viewport_window, roi.x, roi.y, roi.width, roi.height, false);
}

Napi::Value GPUApplication::InitDetector(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, core->InitDetector());
}

Napi::Value GPUApplication::InitRenderer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, core->InitRender(static_cast<void*>(gpu_viewport_window)));
}

Napi::Value GPUApplication::ConfigureStream(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected a single number parameter").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, core->ConfigureStream(info[0].ToNumber()));
}

Napi::Value GPUApplication::StartStream(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    core->ConfigureStream(1670);
    return Napi::Number::New(env, core->StartStream());
};

Napi::Value GPUApplication::GetROI(const Napi::CallbackInfo& info) {
    return ROIToJSObject(info.Env(), core->GetROI());
}

Napi::Value GPUApplication::SetROI(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected an object").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object obj = info[0].As<Napi::Object>();
    vb::ROI roi = JSObjectToROI(obj);

    return Napi::Number::New(env, core->SetROI(roi));
}

Napi::Value GPUApplication::GetImageSize(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return ROIToJSObject(info.Env(), core->GetImageSize());
}

Napi::Value GPUApplication::SetHistogramEqualisation(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Exactly one argument expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Argument should be a boolean").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool enable = info[0].As<Napi::Boolean>().Value();

    return env.Undefined();
}

Napi::Value GPUApplication::SetDarkCorrection(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Exactly two arguments expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument should be string").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "First argument should be a number").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string path = info[0].As<Napi::String>().ToString();
    uint32_t offset = info[1].As<Napi::Number>().Uint32Value();
}

Napi::Value GPUApplication::SetGainCorrection(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Exactly two arguments expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument should be string").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string path = info[0].As<Napi::String>().ToString();
}

Napi::Value GPUApplication::SetDefectCorrection(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Exactly two arguments expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "First argument should be string").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string path = info[0].As<Napi::String>().ToString();
}