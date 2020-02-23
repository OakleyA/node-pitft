#include "framebuffer.h"

FrameBuffer::FrameBuffer(std::string wd, const char *path, bool drawToBuff) {
    cwd = wd;
    drawToBuffer = drawToBuff;

    fbfd = open(path, O_RDWR);
    if (fbfd == -1) {
        throw std::runtime_error("Error opening framebuffer device");
        return;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        throw std::runtime_error("Error retrieving data from framebuffer");
        return;
    }

    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

    vinfo.bits_per_pixel = 8;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        throw std::runtime_error("Error sending data to framebuffer");
        return;
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        throw std::runtime_error("Error retrieving data from framebuffer");
        return;
    }

    screenSize = finfo.smem_len;
    fbp = (char *)mmap(0, screenSize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    bbp = (char *)malloc(screenSize);

    if ((int)fbp == -1) {
        throw std::runtime_error("Error during memory mapping");
        return;
    }

    bufferSurface =
        cairo_image_surface_create_for_data((unsigned char *)bbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres,
                                            cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error("Error creating buffer surface");
        return;
    }

    screenSurface =
        cairo_image_surface_create_for_data((unsigned char *)fbp, CAIRO_FORMAT_RGB16_565, vinfo.xres, vinfo.yres,
                                            cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, vinfo.xres));

    if (cairo_surface_status(bufferSurface) != CAIRO_STATUS_SUCCESS)
        throw std::runtime_error("Error creating screeh surface");
}

void FrameBuffer::Clear() {
    cairo_t *cr = getDrawingContext(this);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Blit() {
    if (this->drawToBuffer) {
        cairo_t *cr = cairo_create(this->screenSurface);
        cairo_set_source_surface(cr, this->bufferSurface, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
    }
}

void FrameBuffer::Color(double r, double g = -1, double b = -1) {
    if (g == -1) {
        this->usedPattern = (r);
        this->usePattern = true;
    } else {
        this->r = r;
        this->g = g;
        this->b = b;
        this->usePattern = false;
    }
}

size_t FrameBuffer::PatternCreateLinear(double arg0, double arg1, double arg2, double arg3, double arg4 = -1) {
    double x0, y0, x1, y1;
    size_t pos;

    if (arg4 == -1) {
        x0 = arg0;
        y0 = arg1;
        x1 = arg2;
        y1 = arg3;
        this->pattern.push_back(cairo_pattern_create_linear(x0, y0, x1, y1));
        pos = this->pattern.size() - 1;
    } else {
        pos = arg0;
        x0 = arg1;
        y0 = arg2;
        x1 = arg3;
        y1 = arg4;

        while (this->pattern.size() <= pos)
            this->pattern.push_back(nullptr);

        if (this->pattern[pos] != nullptr)
            cairo_pattern_destroy(this->pattern[pos]);

        this->pattern[pos] = cairo_pattern_create_linear(x0, y0, x1, y1);
    }

    return pos;
}

size_t FrameBuffer::PatternCreateRGB(double arg0, double arg1, double arg2, double arg3 = 1, double arg4 = 1) {
    double r, g, b, a;
    size_t pos;

    if (arg4 == -1) {
        r = arg0;
        g = arg1;
        b = arg2;
        a = arg3;
        this->pattern.push_back(cairo_pattern_create_rgba(r, g, b, a));
        pos = this->pattern.size() - 1;
    } else {
        pos = arg0;
        r = arg1;
        g = arg2;
        b = arg3;
        a = arg4;

        while (this->pattern.size() <= pos)
            this->pattern.push_back(nullptr);

        if (this->pattern[pos] != nullptr)
            cairo_pattern_destroy(this->pattern[pos]);

        this->pattern[pos] = cairo_pattern_create_rgba(r, g, b, a);
    }

    return pos;
}

void FrameBuffer::PatternAddColorStop(size_t patternIndex, double offset, double r, double g, double b,
                                      double alpha = -1) {

    if (alpha != -1)
        cairo_pattern_add_color_stop_rgba(this->pattern[patternIndex], offset, r, g, b, alpha);
    else
        cairo_pattern_add_color_stop_rgb(this->pattern[patternIndex], offset, r, g, b);
}

void FrameBuffer::PatternDestroy(size_t patternIndex) {
    cairo_pattern_destroy(this->pattern[patternIndex]);

    this->pattern[patternIndex] = nullptr;
}

#define cairoSetSourceMacro(cr, obj)                                                                                   \
    if (obj->usePattern) {                                                                                             \
        if (obj->pattern.size() <= obj->usedPattern) {                                                                 \
            throw std::runtime_error("Error using pattern, index not exists");                                         \
            return;                                                                                                    \
        }                                                                                                              \
        if (obj->pattern[obj->usedPattern] == nullptr) {                                                               \
            throw std::runtime_error("Error using pattern, pattern is destroyed");                                     \
            return;                                                                                                    \
        }                                                                                                              \
        if (cairo_pattern_status(obj->pattern[obj->usedPattern]) != CAIRO_STATUS_SUCCESS) {                            \
            throw std::runtime_error("Error using pattern, pattern status invalid");                                   \
            return;                                                                                                    \
        }                                                                                                              \
        cairo_set_source(cr, obj->pattern[obj->usedPattern]);                                                          \
    } else                                                                                                             \
        cairo_set_source_rgb(cr, obj->r, obj->g, obj->b);

void FrameBuffer::Fill() {
    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this);
    cairo_paint(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Line(double x0, double y0, double x1, double y1, double w = 1) {
    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this);
    cairo_move_to(cr, x0, y0);
    cairo_line_to(cr, x1, y1);
    cairo_set_line_width(cr, w);
    cairo_stroke(cr);
    cairo_destroy(cr);
}

void FrameBuffer::Rect(double x, double y, double w, double h, bool arg4 = true, double arg5 = 1) {
    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this);
    cairo_rectangle(cr, x, y, w, h);

    if (arg4 == false) {
        cairo_set_line_width(cr, arg5);
        cairo_stroke(cr);
    } else
        cairo_fill(cr);

    cairo_destroy(cr);
}

void FrameBuffer::Circle(double x, double y, double radius, bool arg3 = true, double arg4 = 1) {
    cairo_t *cr = getDrawingContext(this);

    cairoSetSourceMacro(cr, this);
    cairo_arc(cr, x, y, radius, 0, 2 * 3.141592654);

    if (arg3 == false) {
        cairo_set_line_width(cr, arg4);
        cairo_stroke(cr);
    } else
        cairo_fill(cr);

    cairo_destroy(cr);
}

void FrameBuffer::Font(std::string fontName, double fontSize = 12, bool fontBold = false) {
    this->fontName = fontName.c_str();
    this->fontSize = fontSize;
    this->fontBold = fontBold;
}

void FrameBuffer::Text(double x, double y, std::string text, bool textCentered, double textRotation, bool textRight) {
    cairo_t *cr = getDrawingContext(this);
    cairoSetSourceMacro(cr, this);

    if (this->fontBold)
        cairo_select_font_face(cr, this->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    else
        cairo_select_font_face(cr, this->fontName, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(cr, this->fontSize);
    cairo_translate(cr, x, y);

    if (textRotation != 0)
        cairo_rotate(cr, textRotation / (180.0 / 3.141592654));

    if (textCentered) {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, text.c_str(), &extents);
        cairo_move_to(cr, -extents.width / 2, extents.height / 2);
    } else if (textRight) {
        cairo_text_extents_t extents;
        cairo_text_extents(cr, text.c_str(), &extents);
        cairo_move_to(cr, -extents.width, 0);
    }

    cairo_show_text(cr, text.c_str());
    cairo_destroy(cr);
}

void FrameBuffer::Image(double x, double y, std::string path) {
    path = cwd + "/" + path;
    cairo_t *cr = getDrawingContext(this);
    cairo_surface_t *image = cairo_image_surface_create_from_png(path.c_str());

    cairo_set_source_surface(cr, image, x, y);
    cairo_paint(cr);

    cairo_status_t status = cairo_status(cr);

    if (status != CAIRO_STATUS_SUCCESS)
        throw std::runtime_error("Error reading image: " + path + " : " + cairo_status_to_string(status));

    cairo_surface_destroy(image);
    cairo_destroy(cr);
}

cairo_t *FrameBuffer::getDrawingContext(FrameBuffer *obj) {
    if (obj->drawToBuffer)
        return cairo_create(obj->bufferSurface);
    else
        return cairo_create(obj->screenSurface);
}

FrameBuffer::~FrameBuffer() {
    size_t patternSize = pattern.size();
    for (size_t i = 0; i < patternSize; i++)
        if (pattern[i] != nullptr)
            cairo_pattern_destroy(pattern[i]);

    if ((int)fbp != -1) {
        free(bbp);
        munmap(fbp, screenSize);

        // if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo))
        // throw std::runtime_error("Error restoring framebuffer state");
    }

    if (fbfd != -1)
        close(fbfd);

    if (cairo_surface_status(bufferSurface) == CAIRO_STATUS_SUCCESS)
        cairo_surface_destroy(bufferSurface);

    if (cairo_surface_status(screenSurface) == CAIRO_STATUS_SUCCESS)
        cairo_surface_destroy(screenSurface);
}
