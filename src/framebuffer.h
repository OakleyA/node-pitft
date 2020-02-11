#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <cairo/cairo.h>

#include <v8.h>
#include <napi.h>
#include <uv.h>

using namespace Napi;

class FrameBuffer : public Napi::ObjectWrap<FrameBuffer>
{
public:
    FrameBuffer(const char *path);
    ~FrameBuffer();
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static void New(const Napi::CallbackInfo &info);
    static Napi::Object Size(const Napi::CallbackInfo &info);
    static Napi::Object Data(const Napi::CallbackInfo &info);
    static void Clear(const Napi::CallbackInfo &info);
    static void Blit(const Napi::CallbackInfo &info);
    static void Color(const Napi::CallbackInfo &info);
    static void Fill(const Napi::CallbackInfo &info);
    static void Line(const Napi::CallbackInfo &info);
    static void Rect(const Napi::CallbackInfo &info);
    static void Circle(const Napi::CallbackInfo &info);
    static void Font(const Napi::CallbackInfo &info);
    static void Text(const Napi::CallbackInfo &info);
    static void Image(const Napi::CallbackInfo &info);
    static Napi::Number PatternCreateLinear(const Napi::CallbackInfo &info);
    static Napi::Number PatternCreateRGB(const Napi::CallbackInfo &info);
    static void PatternAddColorStop(const Napi::CallbackInfo &info);
    static void PatternDestroy(const Napi::CallbackInfo &info);
    static cairo_t *getDrawingContext(FrameBuffer *obj);

private:
    static Napi::FunctionReference constructor;
    int fbfd;
    struct fb_var_screeninfo orig_vinfo;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screenSize;

    char *bbp;
    char *fbp;

    cairo_surface_t *bufferSurface;
    cairo_surface_t *screenSurface;

    double r, g, b;

    std::vector<cairo_pattern_t *> pattern;
    size_t usedPattern;
    bool usePattern;

    const char *fontName;
    double fontSize;
    bool fontBold;

    bool drawToBuffer;
};

#endif
