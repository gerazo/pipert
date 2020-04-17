#include <napi.h>

Napi::String Method(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "Addon Under Development!");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "info"),
              Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(pipert, Init)
