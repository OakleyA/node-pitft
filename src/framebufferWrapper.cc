#include "framebufferWrapper.h"

using namespace Napi;

Napi::FunctionReference FrameBufferWrapper::constructor;

Napi::Object FrameBufferWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "FrameBufferWrapper",
	  {
		  InstanceMethod("size", &FrameBufferWrapper::Size),
		  InstanceMethod("data", &FrameBufferWrapper::Data),
		  InstanceMethod("clear", &FrameBufferWrapper::Clear),
		  InstanceMethod("blit", &FrameBufferWrapper::Blit),
		  InstanceMethod("color", &FrameBufferWrapper::Color),
		  InstanceMethod("fill", &FrameBufferWrapper::Fill),
		  InstanceMethod("line", &FrameBufferWrapper::Line),
		  InstanceMethod("rect", &FrameBufferWrapper::Rect),
		  InstanceMethod("circle", &FrameBufferWrapper::Circle),
		  InstanceMethod("font", &FrameBufferWrapper::Font),
		  InstanceMethod("text", &FrameBufferWrapper::Text),
		  InstanceMethod("image", &FrameBufferWrapper::Image),
		  InstanceMethod("patternCreateLinear", &FrameBufferWrapper::PatternCreateLinear),
		  InstanceMethod("patternCreateRGB", &FrameBufferWrapper::PatternCreateRGB),
		  InstanceMethod("patternAddColorStop", &FrameBufferWrapper::PatternAddColorStop),
		  InstanceMethod("patternDestroy", &FrameBufferWrapper::PatternDestroy)
	  });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FrameBufferWrapper", func);

    return exports;
}

FrameBufferWrapper::FrameBufferWrapper(const Napi::CallbackInfo &info) : Napi::ObjectWrap<FrameBufferWrapper>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
        Napi::TypeError::New(env, "expected 1 or 2 parameters").ThrowAsJavaScriptException();

    const char *path = info[0].As<Napi::String>().Utf8Value().c_str();
    bool drawToBuffer = false;

    if (info.Length() == 2)
    {
        if (!info[1].IsBoolean())
            Napi::TypeError::New(env, "expected boolean");
        else
            drawToBuffer = info[1].As<Napi::Boolean>().Value();
    }

    this->frameBufferClass = new FrameBuffer(path, drawToBuffer);
}

Napi::Object FrameBufferWrapper::Size(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object sizeObject = Napi::Object::New(env);

    sizeObject.Set("width", this->frameBufferClass->vinfo.xres);
    sizeObject.Set("height", this->frameBufferClass->vinfo.yres);

    return sizeObject;
}

Napi::Object FrameBufferWrapper::Data(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object bufferObject = Napi::Buffer<char>::New(env, this->frameBufferClass->fbp, this->frameBufferClass->screenSize);

    return bufferObject;
}

void FrameBufferWrapper::Clear(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass->Clear();
}

void FrameBufferWrapper::Blit(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass->Blit();
}

void FrameBufferWrapper::Color(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsNumber())
        this->frameBufferClass->Color(info[0].As<Napi::Number>().DoubleValue(), -1, -1);
    else if (info.Length() == 3 && info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        this->frameBufferClass->Color(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
}

Napi::Number FrameBufferWrapper::PatternCreateLinear(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    size_t pos = -1;

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        pos = this->frameBufferClass->PatternCreateLinear(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? -1 : info[4].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

Napi::Number FrameBufferWrapper::PatternCreateRGB(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    size_t pos = -1;

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        pos = this->frameBufferClass->PatternCreateRGB(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].IsUndefined() ? 1 : info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

void FrameBufferWrapper::PatternAddColorStop(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber() && info[4].IsNumber())
        this->frameBufferClass->PatternAddColorStop(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].IsUndefined() ? 1 : info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue(),
            info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::PatternDestroy(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsNumber())
        this->frameBufferClass->PatternDestroy(info[0].As<Napi::Number>().DoubleValue());
}

void FrameBufferWrapper::Fill(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass->Fill();
}

void FrameBufferWrapper::Line(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber() && info[4].IsNumber() && info[5].IsNumber())
        this->frameBufferClass->Line(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Rect(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        this->frameBufferClass->Rect(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? true : info[4].As<Napi::Boolean>().Value(),
            info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Circle(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        this->frameBufferClass->Circle(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].IsUndefined() ? true : info[3].As<Napi::Boolean>().Value(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Font(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsString())
        this->frameBufferClass->Font(
            info[0].As<Napi::String>(),
            info[1].IsUndefined() ? 12 : info[1].As<Napi::Number>().DoubleValue(),
            info[2].IsUndefined() ? false : info[2].As<Napi::Boolean>().Value());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Text(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString())
        this->frameBufferClass->Text(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::String>(),
            info[3].IsUndefined() ? false : info[3].As<Napi::Boolean>().Value(),
            info[4].IsUndefined() ? 0 : info[4].As<Napi::Number>().DoubleValue(),
            info[5].IsUndefined() ? false : info[5].As<Napi::Boolean>().Value());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Image(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString())
        this->frameBufferClass->Image(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::String>());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}
