#include "framebufferWrapper.h"

Napi::FunctionReference FrameBufferWrapper::constructor;

Napi::Object FrameBufferWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(
        env, "FrameBufferWrapper",
        // clang-format off
        {InstanceMethod("size", &FrameBufferWrapper::Size),
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
         InstanceMethod("patternDestroy", &FrameBufferWrapper::PatternDestroy)});
    // clang-format on
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FrameBuffer", func);

    return exports;
}

FrameBufferWrapper::FrameBufferWrapper(const Napi::CallbackInfo &info) : Napi::ObjectWrap<FrameBufferWrapper>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
        Napi::TypeError::New(env, "expected 1 or 2 parameters").ThrowAsJavaScriptException();

    std::string cwd = info[0].As<Napi::String>().Utf8Value();
    const char *path = info[1].As<Napi::String>().Utf8Value().c_str();
    bool drawToBuffer = false;

    if (info.Length() == 2) {
        if (!info[1].IsBoolean())
            Napi::TypeError::New(env, "expected boolean");
        else
            drawToBuffer = info[1].As<Napi::Boolean>().Value();
    }

    this->frameBufferClass_ = new FrameBuffer(cwd, path, drawToBuffer);
}

Napi::Value FrameBufferWrapper::Size(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object sizeObject = Napi::Object::New(env);

    sizeObject.Set("width", this->frameBufferClass_->vinfo.xres);
    sizeObject.Set("height", this->frameBufferClass_->vinfo.yres);

    return sizeObject;
}

Napi::Value FrameBufferWrapper::Data(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object bufferObject =
        Napi::Buffer<char>::New(env, this->frameBufferClass_->fbp, this->frameBufferClass_->screenSize);

    return bufferObject;
}

void FrameBufferWrapper::Clear(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass_->Clear();
}

void FrameBufferWrapper::Blit(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass_->Blit();
}

void FrameBufferWrapper::Color(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsNumber())
        this->frameBufferClass_->Color(info[0].As<Napi::Number>().DoubleValue(), -1, -1);
    else if (info.Length() == 3 && info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        this->frameBufferClass_->Color(info[0].As<Napi::Number>().DoubleValue(),
                                       info[1].As<Napi::Number>().DoubleValue(),
                                       info[2].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
}

Napi::Value FrameBufferWrapper::PatternCreateLinear(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    size_t pos = -1;

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        // clang-format off
        pos = this->frameBufferClass_->PatternCreateLinear(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? -1 : info[4].As<Napi::Number>().DoubleValue());
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

Napi::Value FrameBufferWrapper::PatternCreateRGB(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    size_t pos = -1;

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        // clang-format off
        pos = this->frameBufferClass_->PatternCreateRGB(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].IsUndefined() ? 1 : info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

void FrameBufferWrapper::PatternAddColorStop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber() && info[4].IsNumber())
        // clang-format off
        this->frameBufferClass_->PatternAddColorStop(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].IsUndefined() ? 1 : info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue(),
            info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue());
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::PatternDestroy(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsNumber())
        this->frameBufferClass_->PatternDestroy(info[0].As<Napi::Number>().DoubleValue());
}

void FrameBufferWrapper::Fill(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass_->Fill();
}

void FrameBufferWrapper::Line(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber() && info[4].IsNumber())
        // clang-format off
        this->frameBufferClass_->Line(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Rect(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        // clang-format off
        this->frameBufferClass_->Rect(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            info[4].IsUndefined() ? true : info[4].As<Napi::Boolean>().Value(),
            info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue());
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Circle(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        this->frameBufferClass_->Circle(info[0].As<Napi::Number>().DoubleValue(),
                                        info[1].As<Napi::Number>().DoubleValue(),
                                        info[2].As<Napi::Number>().DoubleValue(),
                                        info[3].IsUndefined() ? true : info[3].As<Napi::Boolean>().Value(),
                                        info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Font(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    double fontSize = 12;

    if (info[0].IsString()) {
        if (!info[1].IsUndefined() && info[1].IsNumber())
            fontSize = info[1].As<Napi::Number>().DoubleValue();
        this->frameBufferClass_->Font(info[0].As<Napi::String>().Utf8Value(), fontSize,
                                      info[2].IsUndefined() ? false : info[2].As<Napi::Boolean>().Value());
    } else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Text(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString()) {
        double x = info[0].As<Napi::Number>().DoubleValue();
        double y = info[1].As<Napi::Number>().DoubleValue();
        std::string text = info[2].As<Napi::String>().Utf8Value();
        bool centered = false;
        double rotation = 0;
        bool alignRight = false;

        if (!info[3].IsUndefined() && info[3].IsBoolean())
            centered = info[3].As<Napi::Boolean>().Value();

        if (!info[4].IsUndefined() && info[4].IsNumber())
            rotation = info[4].As<Napi::Number>().DoubleValue();

        if (!info[5].IsUndefined() && info[5].IsBoolean())
            alignRight = info[5].As<Napi::Boolean>().Value();

        this->frameBufferClass_->Text(x, y, text, centered, rotation, alignRight);
    } else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Image(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString())
        this->frameBufferClass_->Image(info[0].As<Napi::Number>().DoubleValue(),
                                       info[1].As<Napi::Number>().DoubleValue(),
                                       info[2].As<Napi::String>().Utf8Value());
    else
        Napi::TypeError::New(env, "invalid arguments").ThrowAsJavaScriptException();
}
