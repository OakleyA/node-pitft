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

class FrameBuffer
{
public:
    FrameBuffer(const char *path, bool drawToBuffer);
    ~FrameBuffer();
    void Clear();
    void Blit();
    void Color(double r, double g, double b);
    void Fill();
    void Line(double arg0, double arg1, double arg2, double arg3, double arg4);
    void Rect(double arg0, double arg1, double arg2, double arg3, bool arg4, double arg5);
    void Circle(double arg0, double arg1, double arg2, bool arg3, double arg4);
    void Font(std::string arg0, double arg1, bool arg2);
    void Text(double arg0, double arg1, std::string arg2, bool arg3, double arg4, bool arg5);
    void Image(double arg0, double arg1, std::string arg2);
    size_t PatternCreateLinear(double arg0, double arg1, double arg2, double arg3, double arg4);
    size_t PatternCreateRGB(double arg0, double arg1, double arg2, double arg3, double arg4);
    void PatternAddColorStop(double arg0, double arg1, double arg2, double arg3, double arg4, double arg5);
    void PatternDestroy(double arg0);

	cairo_t *getDrawingContext(FrameBuffer *obj);

    long int screenSize;
    char *fbp;
    struct fb_var_screeninfo vinfo;

private:
    int fbfd;
    struct fb_var_screeninfo orig_vinfo;
    struct fb_fix_screeninfo finfo;

    char *bbp;

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
