#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cairo/cairo.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <napi.h>

using namespace Napi;

class FrameBuffer {
  public:
    FrameBuffer(std::string cwd, const char *path, bool drawToBuffer);
    ~FrameBuffer();
    void Clear();
    void Blit();
    void Color(double r, double g, double b);
    void Fill();
    void Line(double x0, double y0, double x1, double y1, double w);
    void Rect(double x, double y, double w, double h, bool filled, double lineWidth);
    void Circle(double x, double y, double radius, bool filled, double lineWidth);
    void Font(std::string fontName, double fontSize, bool fontBold);
    void Text(double x, double y, std::string text, bool textCentered, double textRotation, bool textRight);
    void Image(double x, double y, std::string path);
    size_t PatternCreateLinear(double arg0, double arg1, double arg2, double arg3, double arg4);
    size_t PatternCreateRGB(double arg0, double arg1, double arg2, double arg3, double arg4);
    void PatternAddColorStop(size_t patternIndex, double offset, double r, double g, double b, double alpha);
    void PatternDestroy(size_t patternIndex);

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

    std::string cwd;
};

#endif
