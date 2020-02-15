#include <napi.h>
#include <uv.h>
#include "framebufferWrapper.h"

using namespace Napi;

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  return FrameBufferWrapper::Init(env, exports);
}

NODE_API_MODULE(pitft, InitAll)
