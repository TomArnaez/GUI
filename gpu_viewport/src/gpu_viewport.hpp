#include <napi.h>
#include <Windows.h>

#include "VBCore.h"
#include "VBErrorCodes.h"

struct gpu_viewport_window_context;

class gpu_viewport : public Napi::ObjectWrap<gpu_viewport> {
    HWND main_window;
    HWND gpu_viewport_window;
    
    std::unique_ptr<gpu_viewport_window_context> gpu_viewport_window_ctx;
    std::wstring gpu_window_class_name = L"GPUViewportWindow";

    vb::VBCore* core;
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    gpu_viewport(const Napi::CallbackInfo& info);

    Napi::Value Cleanup(const Napi::CallbackInfo& info);

    Napi::Value SetWindowPosition(const Napi::CallbackInfo& info);

    Napi::Value InitDetector(const Napi::CallbackInfo& info);
    Napi::Value InitRenderer(const Napi::CallbackInfo& info);
    Napi::Value ConfigureStream(const Napi::CallbackInfo& info);
    Napi::Value StartStream(const Napi::CallbackInfo& info);
    Napi::Value StopStream(const Napi::CallbackInfo& info);
    Napi::Value GetROI(const Napi::CallbackInfo& info);
    Napi::Value SetROI(const Napi::CallbackInfo& info);
    Napi::Value GetImageSize(const Napi::CallbackInfo& info);
};