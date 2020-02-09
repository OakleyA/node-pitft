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
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(const Napi::CallbackInfo &info);
    static Napi::Value New(const Napi::CallbackInfo &info);
    static Napi::Value Size(const char *whatisthis, const Napi::CallbackInfo &info);
    static Napi::Value Data(const Napi::CallbackInfo &info);
    static Napi::Value Clear(const Napi::CallbackInfo &info);
    static Napi::Value Blit(const Napi::CallbackInfo &info);
    static Napi::Value Color(const Napi::CallbackInfo &info);
    static Napi::Value Fill(const Napi::CallbackInfo &info);
    static Napi::Value Line(const Napi::CallbackInfo &info);
    static Napi::Value Rect(const Napi::CallbackInfo &info);
    static Napi::Value Circle(const Napi::CallbackInfo &info);
    static Napi::Value Font(const Napi::CallbackInfo &info);
    static Napi::Value Text(const Napi::CallbackInfo &info);
    static Napi::Value Image(const Napi::CallbackInfo &info);
    static Napi::Value PatternCreateLinear(const Napi::CallbackInfo &info);
    static Napi::Value PatternCreateRGB(const Napi::CallbackInfo &info);
    static Napi::Value PatternAddColorStop(const Napi::CallbackInfo &info);
    static Napi::Value PatternDestroy(const Napi::CallbackInfo &info);
    static cairo_t *getDrawingContext(FrameBuffer *obj);
    ~FrameBuffer();

private:
    // FrameBuffer(const char *path);
    FrameBuffer(const Napi::CallbackInfo &info);

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
