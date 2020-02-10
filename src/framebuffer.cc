#include "framebuffer.h"

using namespace Napi;

Napi::FunctionReference FrameBuffer::constructor;

Napi::Object FrameBuffer::Init(Napi::env env, Napi::Object exports)
{
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "FrameBuffer",
                                      {
                                          InstanceMethod("size", &FrameBuffer::Size),
                                          InstanceMethod("data", &FrameBuffer::Data),
                                          InstanceMethod("clear", &FrameBuffer::Clear),
                                          InstanceMethod("blit", &FrameBuffer::Blit),
                                          InstanceMethod("color", &FrameBuffer::Color),
                                          InstanceMethod("fill", &FrameBuffer::Fill),
                                          InstanceMethod("line", &FrameBuffer::Line),
                                          InstanceMethod("rect", &FrameBuffer::Rect),
                                          InstanceMethod("circle", &FrameBuffer::Circle),
                                          InstanceMethod("font", &FrameBuffer::Font),
                                          InstanceMethod("text", &FrameBuffer::Text),
                                          InstanceMethod("image", &FrameBuffer::Image),
                                          InstanceMethod("patternCreateLinear", &FrameBuffer::PatternCreateLinear),
                                          InstanceMethod("patternCreateRGB", &FrameBuffer::PatternCreateRGB),
                                          InstanceMethod("patternAddColorStop", &FrameBuffer::PatternAddColorStop),
                                          InstanceMethod("patternDestroy", &FrameBuffer::PatternDestroy),
                                      });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FrameBuffer", func);

    return exports;
}

Napi::Object FrameBuffer::Size(const Napi::CallbackInfo &info)
{
    Napi::env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object sizeObject = Napi::Object::New(env);

    sizeObject.Set(Napi::String::New(env, "width"), Napi::Number::New(env, this->vinfo.xres));
    sizeObject.Set(Napi::String::New(env, "height"), Napi::Number::New(env, this->vinfo.yres));

    return sizeObject;
}

Napi::Object FrameBuffer::Data(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object bufferObject = Napi::Buffer<char>::New(env, this.->fbp, this->screenSize);

    return bufferObject;
}

void FrameBuffer::Clear(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    cairo_t *cr = getDrawingContext(this);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Blit(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (this->drawToBuffer)
    {
        cairo_t *cr = cairo_create(this->screenSurface);
        cairo_set_source_surface(cr, this->bufferSurface, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
    }
}

void FrameBuffer::Color(const Napi::CallbackInfo &info)
{
    Napi::env env = info.Env();
    Napi::HandleScope scope(env);

    if (info[1].IsUndefined())
    {
        this->usedPattern = (info[0].As<Napi::Number>().DoubleValue());
        this->usePattern = true;
    }
    else
    {
        this->r = (info[0].As<Napi::Number>().DoubleValue());
        this->g = (info[1].As<Napi::Number>().DoubleValue());
        this->b = (info[2].As<Napi::Number>().DoubleValue());
        this->usePattern = false;
    }
}

Napi::Number FrameBuffer::PatternCreateLinear(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double x0, y0, x1, y1;
    size_t pos;

    if (info[4].IsUndefined())
    {
        x0 = (info[0].As<Napi::Number>().DoubleValue());
        y0 = (info[1].As<Napi::Number>().DoubleValue());
        x1 = (info[2].As<Napi::Number>().DoubleValue());
        y1 = (info[3].As<Napi::Number>().DoubleValue());
        this->pattern.push_back(cairo_pattern_create_linear(x0, y0, x1, y1));
        pos = this->pattern.size() - 1;
    }
    else
    {
        pos = (info[0].As<Napi::Number>().DoubleValue());
        x0 = (info[1].As<Napi::Number>().DoubleValue());
        y0 = (info[2].As<Napi::Number>().DoubleValue());
        x1 = (info[3].As<Napi::Number>().DoubleValue());
        y1 = (info[4].As<Napi::Number>().DoubleValue());

        while (this->pattern.size() <= pos)
            this->pattern.push_back(nullptr);

        if (this->pattern[pos] != nullptr)
            cairo_pattern_destroy(this->pattern[pos]);

        this->pattern[pos] = cairo_pattern_create_linear(x0, y0, x1, y1);
    }

    return Napi::Number::New(env, pos);
}

Napi::Number FrameBuffer::PatternCreateRGB(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double r, g, b, a;
    size_t pos;

    if (info[4].IsUndefined())
    {
        r = (info[0].As<Napi::Number>().DoubleValue());
        g = (info[1].As<Napi::Number>().DoubleValue());
        b = (info[2].As<Napi::Number>().DoubleValue());
        a = info[3].IsUndefined() ? 1 : (info[3].As<Napi::Number>().DoubleValue());
        this->pattern.push_back(cairo_pattern_create_rgba(r, g, b, a));
        pos = this->pattern.size() - 1;
    }
    else
    {
        pos = (info[0].As<Napi::Number>().DoubleValue());
        r = (info[1].As<Napi::Number>().DoubleValue());
        g = (info[2].As<Napi::Number>().DoubleValue());
        b = (info[3].As<Napi::Number>().DoubleValue());
        a = info[4].IsUndefined() ? 1 : (info[4].As<Napi::Number>().DoubleValue());

        while (othisbj->pattern.size() <= pos)
            this->pattern.push_back(nullptr);

        if (this->pattern[pos] != nullptr)
            cairo_pattern_destroy(obj->pattern[pos]);

        this->pattern[pos] = cairo_pattern_create_rgba(r, g, b, a);
    }

    return Napi::Number::New(env, pos);
}

void FrameBuffer::PatternAddColorStop(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    size_t patternIndex = (info[0].As<Napi::Number>().DoubleValue());

    double offset = (info[1].As<Napi::Number>().DoubleValue());
    double r = (info[2].As<Napi::Number>().DoubleValue());
    double g = (info[3].As<Napi::Number>().DoubleValue());
    double b = (info[4].As<Napi::Number>().DoubleValue());

    if (!info[5].IsUndefined())
    {
        double alpha = info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue();
        cairo_pattern_add_color_stop_rgba(this->pattern[patternIndex], offset, r, g, b, alpha);
    }
    else
    {
        cairo_pattern_add_color_stop_rgb(this->pattern[patternIndex], offset, r, g, b);
    }
}

void FrameBuffer::PatternDestroy(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    size_t patternIndex = (info[0].As<Napi::Number>().DoubleValue());

    cairo_pattern_destroy(this->pattern[patternIndex]);

    this->pattern[patternIndex] = nullptr;
}

#define cairoSetSourceMacro(cr, obj, env)                                                                      \
    if (obj->usePattern)                                                                                       \
    {                                                                                                          \
        if (obj->pattern.size() <= obj->usedPattern)                                                           \
        {                                                                                                      \
            Napi::Error::New(env, "Error using pattern, index not exists").ThrowAsJavaScriptException();       \
            return;                                                                                            \
        }                                                                                                      \
        if (obj->pattern[obj->usedPattern] == nullptr)                                                         \
        {                                                                                                      \
            Napi::Error::New(env, "Error using pattern, pattern is destroyed").ThrowAsJavaScriptException();   \
            return;                                                                                            \
        }                                                                                                      \
        if (cairo_pattern_status(obj->pattern[obj->usedPattern]) != CAIRO_STATUS_SUCCESS)                      \
        {                                                                                                      \
            Napi::Error::New(env, "Error using pattern, pattern status invalid").ThrowAsJavaScriptException(); \
            return;                                                                                            \
        }                                                                                                      \
        cairo_set_source(cr, obj->pattern[obj->usedPattern]);                                                  \
    }                                                                                                          \
    else                                                                                                       \
        cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

void FrameBuffer::Fill(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this, env);
    cairo_paint(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Line(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double x0 = (info[0].As<Napi::Number>().DoubleValue());
    double y0 = (info[1].As<Napi::Number>().DoubleValue());
    double x1 = (info[2].As<Napi::Number>().DoubleValue());
    double y1 = (info[3].As<Napi::Number>().DoubleValue());
    double w = info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue();

    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this, env);

    cairo_move_to(cr, x0, y0);
    cairo_line_to(cr, x1, y1);
    cairo_set_line_width(cr, w);
    cairo_stroke(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Rect(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double x = (info[0].As<Napi::Number>().DoubleValue());
    double y = (info[1].As<Napi::Number>().DoubleValue());
    double w = (info[2].As<Napi::Number>().DoubleValue());
    double h = (info[3].As<Napi::Number>().DoubleValue());

    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this, env);

    cairo_rectangle(cr, x, y, w, h);

    if (!info[4].IsUndefined() && info[4].As<Napi::Boolean>().Value() == false)
    {
        double w = info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().DoubleValue();
        cairo_set_line_width(cr, w);
        cairo_stroke(cr);
    }
    else if (!info[4].IsUndefined() && info[4].As<Napi::Boolean>().Value() == true)
        cairo_fill(cr);
    else
        cairo_fill(cr);

    cairo_destroy(cr);
}

void FrameBuffer::Circle(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double x = (info[0].As<Napi::Number>().DoubleValue());
    double y = (info[1].As<Napi::Number>().DoubleValue());
    double radius = (info[2].As<Napi::Number>().DoubleValue());

    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this, env);

    cairo_arc(cr, x, y, radius, 0, 2 * 3.141592654);

    if (!info[3].IsUndefined() && info[3].As<Napi::Boolean>().Value() == false)
    {
        double w = info[4].IsUndefined() ? 1 : info[4].As<Napi::Number>().DoubleValue();
        cairo_set_line_width(cr, w);
        cairo_stroke(cr);
    }
    else if (!info[3].IsUndefined() && info[3].As<Napi::Boolean>().Value() == true)
        cairo_fill(cr);
    else
        cairo_fill(cr);

    cairo_destroy(cr);
}

void FrameBuffer::Font(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::string fontName = info[0].As<Napi::String>();
    std::string _fontName = std::string(*fontName);

    this->fontName = _fontName.c_str();
    this->fontSize = info[1].IsUndefined() ? 12 : info[1].As<Napi::Number>().DoubleValue();
    this->fontBold = info[2].IsUndefined() ? false : info[2].As<Napi::Boolean>().Value();
}

void FrameBuffer::Text(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double x = (info[0].As<Napi::Number>().DoubleValue());
    double y = (info[1].As<Napi::Number>().DoubleValue());

    std::string text = info[2].As<Napi::String>();
    std::string _text = std::string(*text);

    bool textCentered = info[3].IsUndefined() ? false : info[3].As<Napi::Boolean>().Value();
    double textRotation = info[4].IsUndefined() ? 0 : info[4].As<Napi::Number>().DoubleValue();
    bool textRight = info[5].IsUndefined() ? false : info[5].As<Napi::Boolean>().Value();

    cairo_t *cr = getDrawingContext(othisbj);

    cairoSetSourceMacro(cr, this);

    if (this->fontBold)
        cairo_select_font_face(cr, this->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    else
        cairo_select_font_face(cr, this->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(cr, this->fontSize);

    cairo_translate(cr, x, y);

    if (textRotation != 0)
        cairo_rotate(cr, textRotation / (180.0 / 3.141592654));

    if (textCentered)
    {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, _text.c_str(), &extents);

        cairo_move_to(cr, -extents.width / 2, extents.height / 2);
    }
    else if (textRight)
    {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, _text.c_str(), &extents);

        cairo_move_to(cr, -extents.width, 0);
    }

    cairo_show_text(cr, _text.c_str());

    cairo_destroy(cr);
}

void FrameBuffer::Image(const Napi::CallbackInfo &info)
{
    Napi::env env = info.Env();
    Napi::HandleScope scope(env);

    double x = (info[0].As<Napi::Number>().DoubleValue());
    double y = (info[1].As<Napi::Number>().DoubleValue());

    std::string path = info[2].As<Napi::String>();
    std::string _path = std::string(*path);

    cairo_t *cr = getDrawingContext(this);

    cairo_surface_t *image = cairo_image_surface_create_from_png(_path.c_str());
    cairo_set_source_surface(cr, image, x, y);
    cairo_paint(cr);

    cairo_status_t status = cairo_status(cr);

    if (status != CAIRO_STATUS_SUCCESS)
        Napi::Error::New(env, "Error reading image").ThrowAsJavaScriptException();

    cairo_surface_destroy(image);
    cairo_destroy(cr);
}

cairo_t *FrameBuffer::getDrawingContext(FrameBuffer *obj)
{
    if (obj->drawToBuffer)
        return cairo_create(obj->bufferSurface);
    else
        return cairo_create(obj->screenSurface);
}

FrameBuffer::FrameBuffer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<FrameBuffer>(info)
{
    Napi::Env env = info.Env();
    char *path = info[0].As<Napi::String>().c_str();

    fbfd = open(path, O_RDWR);
    if (fbfd == -1)
    {
        Napi::Error::New(env, "Error opening framebuffer device").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        Napi::Error::New(env, "Error retrieving data from framebuffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

    vinfo.bits_per_pixel = 8;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo))
    {
        Napi::Error::New(env, "Error sending data to framebuffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        Napi::Error::New(env, "Error retrieving data from framebuffer").ThrowAsJavaScriptException();
        return env.Null();
    }

    screenSize = finfo.smem_len;
    fbp = (char *)mmap(0,
                       screenSize,
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED,
                       fbfd,
                       0);

    bbp = (char *)malloc(screenSize);

    if ((int)fbp == -1)
    {
        Napi::Error::New(env, "Error during memory mapping").ThrowAsJavaScriptException();
        return env.Null();
    }

    bufferSurface = cairo_image_surface_create_for_data((unsigned char *)bbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres, cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS)
    {
        Napi::Error::New(env, "Error creating buffer surface").ThrowAsJavaScriptException();
        return env.Null();
    }

    screenSurface = cairo_image_surface_create_for_data((unsigned char *)fbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres, cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS)
        Napi::Error::New(env, "Error creating screeh surface").ThrowAsJavaScriptException();

    drawToBuffer = info[1].IsUndefined() ? false : info[1].As<Napi::Boolean>().Value();
}

FrameBuffer::~FrameBuffer()
{
    size_t patternSize = pattern.size();
    for (size_t i = 0; i < patternSize; i++)
        if (pattern[i] != nullptr)
            cairo_pattern_destroy(pattern[i]);

    if ((int)fbp != -1)
    {
        free(bbp);
        munmap(fbp, screenSize);

        // if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo))
        // {
        //     Napi::Error::New(env, "Error restoring framebuffer state").ThrowAsJavaScriptException();
        // }
    }

    if (fbfd != -1)
        close(fbfd);

    if (cairo_surface_status(bufferSurface) == CAIRO_STATUS_SUCCESS)
        cairo_surface_destroy(bufferSurface);

    if (cairo_surface_status(screenSurface) == CAIRO_STATUS_SUCCESS)
        cairo_surface_destroy(screenSurface);
}
