#include "gpu_viewport.hpp"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    gpu_viewport::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init);