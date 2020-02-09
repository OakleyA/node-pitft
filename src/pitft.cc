#include <napi.h>
#include <uv.h>
#include "framebuffer.h"

using namespace Napi;

Napi::Value CreateObject(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  return FrameBuffer::NewInstance(info);
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  return FrameBuffer::Init(env, exports);
}

NODE_API_MODULE(pitft, InitAll)
