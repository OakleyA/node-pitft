#include <napi.h>
#include "framebufferWrapper.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  return FrameBufferWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll);
