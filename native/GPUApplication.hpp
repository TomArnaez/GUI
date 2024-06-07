#include <napi.h>
#include <Windows.h>
#include "VBCore.h"
#include "VBErrorCodes.h"

struct MainWindowContext {
    HWND child_window = nullptr;
    WNDPROC original_proc = nullptr;
    vb::VBCore* core = nullptr;
};

struct GPUViewportWindowContext {
    vb::VBCore* core = nullptr;
};

class GPUApplication : public Napi::ObjectWrap<GPUApplication> {
    HWND main_window;
    std::unique_ptr<MainWindowContext> main_window_context;
    
    HWND gpu_viewport_window;
    std::unique_ptr<GPUViewportWindowContext> gpu_viewport_window_context;
    std::wstring gpu_window_class_name = L"GPUViewportWindow";

    vb::VBCore* core;
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    GPUApplication(const Napi::CallbackInfo& info);

    Napi::Value InitDetector(const Napi::CallbackInfo& info);
    Napi::Value InitRenderer(const Napi::CallbackInfo& info);
    Napi::Value StartStream(const Napi::CallbackInfo& info);
    Napi::Value StopStream(const Napi::CallbackInfo& info);

    Napi::Value SetHistogramEqualisation(const Napi::CallbackInfo& info);
    Napi::Value SetDarkCorrection(const Napi::CallbackInfo& info);
    Napi::Value SetGainCorrection(const Napi::CallbackInfo& info);
    Napi::Value SetDefectCorrection(const Napi::CallbackInfo& info);
};