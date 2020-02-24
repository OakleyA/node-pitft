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

    // the first parameter is hard coded to the JS execution path
    if (info.Length() < 2)
        Napi::TypeError::New(env, "expected 1 or 2 parameters").ThrowAsJavaScriptException();

    // JS execution path
    std::string cwd = info[0].As<Napi::String>().Utf8Value();
    // framebuffer device path
    const char *path = info[1].As<Napi::String>().Utf8Value().c_str();
    bool drawToBuffer = false;

    if (info.Length() == 3) {
        if (!info[2].IsBoolean())
            Napi::TypeError::New(env, "expected boolean");
        else
            drawToBuffer = info[2].As<Napi::Boolean>().Value();
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
        Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
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
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

Napi::Value FrameBufferWrapper::PatternCreateRGB(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    size_t pos = -1;
    double arg3 = 1;
    double arg4 = 1;

    if (!info[3].IsUndefined()) {
        if (info[3].IsNumber())
            arg3 = info[3].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid argument 3").ThrowAsJavaScriptException();
    }

    if (!info[4].IsUndefined()) {
        if (info[4].IsNumber())
            arg4 = info[4].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid argument 4").ThrowAsJavaScriptException();
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        // clang-format off
        pos = this->frameBufferClass_->PatternCreateRGB(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            arg3, arg4);
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();

    return Napi::Number::New(env, pos);
}

void FrameBufferWrapper::PatternAddColorStop(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    double arg3 = 1;
    double arg4 = 1;
    double arg5 = 1;

    if (!info[3].IsUndefined()) {
        if (info[3].IsNumber())
            arg3 = info[3].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid argument 3").ThrowAsJavaScriptException();
    }

    if (!info[4].IsUndefined()) {
        if (info[4].IsNumber())
            arg4 = info[4].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid argument 4").ThrowAsJavaScriptException();
    }

    if (!info[5].IsUndefined()) {
        if (info[5].IsNumber())
            arg5 = info[5].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid argument 5").ThrowAsJavaScriptException();
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        // clang-format off
        this->frameBufferClass_->PatternAddColorStop(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            arg3, arg4, arg5);
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::PatternDestroy(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 && info[0].IsNumber())
        this->frameBufferClass_->PatternDestroy(info[0].As<Napi::Number>().DoubleValue());
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Fill(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->frameBufferClass_->Fill();
}

void FrameBufferWrapper::Line(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    double width = 1;

    if (!info[4].IsUndefined()) {
        if (info[4].IsNumber())
            width = info[4].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid width argument").ThrowAsJavaScriptException();
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        // clang-format off
        this->frameBufferClass_->Line(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            width);
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Rect(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    bool filled = true;
    double lineWidth = 1;

    if (!info[4].IsUndefined()) {
        if (info[4].IsBoolean())
            filled = info[4].As<Napi::Boolean>().Value();
        else
            Napi::TypeError::New(env, "invalid filled argument").ThrowAsJavaScriptException();
    }

    if (!info[5].IsUndefined()) {
        if (info[5].IsNumber())
            lineWidth = info[5].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid line width").ThrowAsJavaScriptException();
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber() && info[3].IsNumber())
        // clang-format off
        this->frameBufferClass_->Rect(
            info[0].As<Napi::Number>().DoubleValue(),
            info[1].As<Napi::Number>().DoubleValue(),
            info[2].As<Napi::Number>().DoubleValue(),
            info[3].As<Napi::Number>().DoubleValue(),
            filled, lineWidth);
    // clang-format on
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Circle(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    bool filled = true;
    double lineWidth = 1;

    if (!info[3].IsUndefined()) {
        if (info[3].IsBoolean())
            filled = info[3].As<Napi::Boolean>().Value();
        else
            Napi::TypeError::New(env, "invalid filled argument").ThrowAsJavaScriptException();
    }

    if (!info[4].IsUndefined()) {
        if (info[4].IsNumber())
            lineWidth = info[4].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid line width").ThrowAsJavaScriptException();
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsNumber())
        this->frameBufferClass_->Circle(info[0].As<Napi::Number>().DoubleValue(),
                                        info[1].As<Napi::Number>().DoubleValue(),
                                        info[2].As<Napi::Number>().DoubleValue(), filled, lineWidth);
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Font(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    double fontSize = 12;
    bool bold = false;

    if (!info[1].IsUndefined()) {
        if (info[1].IsNumber())
            fontSize = info[1].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid font size").ThrowAsJavaScriptException();
    }

    if (!info[2].IsUndefined()) {
        if (info[2].IsBoolean())
            bold = info[2].As<Napi::Boolean>().Value();
        else
            Napi::TypeError::New(env, "invalid bold argument").ThrowAsJavaScriptException();
    }

    if (info[0].IsString())
        this->frameBufferClass_->Font(info[0].As<Napi::String>().Utf8Value(), fontSize, bold);
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Text(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    bool centered = false;
    double rotation = 0;
    bool alignRight = false;

    if (!info[3].IsUndefined()) {
        if (info[3].IsBoolean())
            centered = info[3].As<Napi::Boolean>().Value();
        else
            Napi::TypeError::New(env, "invalid centered argument");
    }

    if (!info[4].IsUndefined()) {
        if (info[4].IsNumber())
            rotation = info[4].As<Napi::Number>().DoubleValue();
        else
            Napi::TypeError::New(env, "invalid rotation");
    }

    if (!info[5].IsUndefined()) {
        if (info[5].IsBoolean())
            alignRight = info[5].As<Napi::Boolean>().Value();
        else
            Napi::TypeError::New(env, "invalid right align argument");
    }

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString()) {
        double x = info[0].As<Napi::Number>().DoubleValue();
        double y = info[1].As<Napi::Number>().DoubleValue();
        std::string text = info[2].As<Napi::String>().Utf8Value();

        this->frameBufferClass_->Text(x, y, text, centered, rotation, alignRight);
    } else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}

void FrameBufferWrapper::Image(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[0].IsNumber() && info[1].IsNumber() && info[2].IsString())
        this->frameBufferClass_->Image(info[0].As<Napi::Number>().DoubleValue(),
                                       info[1].As<Napi::Number>().DoubleValue(),
                                       info[2].As<Napi::String>().Utf8Value());
    else
        Napi::TypeError::New(env, "invalid argument").ThrowAsJavaScriptException();
}
