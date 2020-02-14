#ifndef FRAMEBUFFERWRAPPER_H
#define FRAMEBUFFERWRAPPER_H

#include <napi.h>
#include "framebuffer.h"

class FrameBufferWrapper : public Napi::ObjectWrap<FrameBufferWrapper>
{
public:
    FrameBufferWrapper(const Napi::CallbackInfo &info);
    ~FrameBufferWrapper();
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

private:
    static Napi::FunctionReference constructor;
    
	void Clear(const Napi::CallbackInfo &info);
    void Blit(const Napi::CallbackInfo &info);
    void Color(const Napi::CallbackInfo &info);
    void Fill(const Napi::CallbackInfo &info);
    void Line(const Napi::CallbackInfo &info);
    void Rect(const Napi::CallbackInfo &info);
    void Circle(const Napi::CallbackInfo &info);
    void Font(const Napi::CallbackInfo &info);
    void Text(const Napi::CallbackInfo &info);
    void Image(const Napi::CallbackInfo &info);
	Napi::Object Size(const Napi::CallbackInfo &info);
	Napi::Object Data(const Napi::CallbackInfo &info);
    Napi::Number PatternCreateLinear(const Napi::CallbackInfo &info);
    Napi::Number PatternCreateRGB(const Napi::CallbackInfo &info);
    void PatternAddColorStop(const Napi::CallbackInfo &info);
    void PatternDestroy(const Napi::CallbackInfo &info);
	
	FrameBuffer *frameBufferClass;
};

#endif
