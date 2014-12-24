#include "framebuffer.h"

using namespace v8;

Persistent<Function> FrameBuffer::constructor;

FrameBuffer::FrameBuffer(const char *path) {
    fbfd = open(path, O_RDWR);

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
    }
    
    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

    vinfo.bits_per_pixel = 8;
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
        printf("Error setting variable information.\n");
    }

    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
    }

    screensize = finfo.smem_len;
    fbp = (char*)mmap(0, 
                      screensize, 
                      PROT_READ | PROT_WRITE, 
                      MAP_SHARED, 
                      fbfd, 
                      0);
}

FrameBuffer::~FrameBuffer() {
    munmap(fbp, screensize);
    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &orig_vinfo)) {
        printf("Error re-setting variable information.\n");
    }
    close(fbfd);
}

void FrameBuffer::Init(Handle<Object> exports) {
    NanScope();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
    tpl->SetClassName(NanNew("FrameBuffer"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "clear", Clear);
    NODE_SET_PROTOTYPE_METHOD(tpl, "fill", Fill);
    NODE_SET_PROTOTYPE_METHOD(tpl, "rect", Rect);
    NODE_SET_PROTOTYPE_METHOD(tpl, "circle", Circle);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dimension", Dimension);

    NanAssignPersistent(constructor, tpl->GetFunction());
    exports->Set(NanNew("FrameBuffer"), tpl->GetFunction());
}

NAN_METHOD(FrameBuffer::New) {
    NanScope();

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new FrameBuffer()`
        v8::String::Utf8Value path(args[0]->ToString());
        std::string _path = std::string(*path);
        FrameBuffer* obj = new FrameBuffer(_path.c_str());
        obj->Wrap(args.This());
        NanReturnValue(args.This());
    } else {
        // Invoked as plain function `FrameBuffer()`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = NanNew<Function>(constructor);
        NanReturnValue(cons->NewInstance(argc, argv));
    }
}

NAN_METHOD(FrameBuffer::Dimension) {
    NanScope();

    FrameBuffer* obj = ObjectWrap::Unwrap<FrameBuffer>(args.Holder());

    Local<Object> dimensionObject = NanNew<Object>();

    dimensionObject->Set(NanNew<String>("width"), NanNew<Number>(obj->vinfo.xres));
    dimensionObject->Set(NanNew<String>("height"), NanNew<Number>(obj->vinfo.yres));

    NanReturnValue(dimensionObject);
}

NAN_METHOD(FrameBuffer::Clear) {
    NanScope();

    FrameBuffer* obj = ObjectWrap::Unwrap<FrameBuffer>(args.Holder());

    memset(obj->fbp, 0x00, obj->screensize);
    
    NanReturnUndefined();
}

NAN_METHOD(FrameBuffer::Fill) {
    NanScope();

    FrameBuffer* obj = ObjectWrap::Unwrap<FrameBuffer>(args.Holder());

    int r = (int)(args[0]->NumberValue());
    int g = (int)(args[1]->NumberValue());
    int b = (int)(args[2]->NumberValue());

    unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

    for (unsigned int x=0; x<obj->vinfo.xres; x++) {
        for (unsigned int y=0; y<obj->vinfo.yres; y++) {
            unsigned int pix_offset = x * 2 + y * obj->finfo.line_length;
            *((unsigned short*)(obj->fbp + pix_offset)) = c;
        }
    }

    NanReturnUndefined();
}

NAN_METHOD(FrameBuffer::Rect) {
    NanScope();

    FrameBuffer* obj = ObjectWrap::Unwrap<FrameBuffer>(args.Holder());

    int x = (int)(args[0]->NumberValue());
    int y = (int)(args[1]->NumberValue());
    int w = (int)(args[2]->NumberValue());
    int h = (int)(args[3]->NumberValue());

    int r = (int)(args[4]->NumberValue());
    int g = (int)(args[5]->NumberValue());
    int b = (int)(args[6]->NumberValue());

    unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

    for (int _x=x; _x<x+w; _x++) {
        for (int _y=y; _y<y+h; _y++) {
            unsigned int pix_offset = _x * 2 + _y * obj->finfo.line_length;
            *((unsigned short*)(obj->fbp + pix_offset)) = c;
        }
    }

    NanReturnUndefined();
}

NAN_METHOD(FrameBuffer::Circle) {
    NanScope();

    FrameBuffer* obj = ObjectWrap::Unwrap<FrameBuffer>(args.Holder());

    int x0 = (int)(args[0]->NumberValue());
    int y0 = (int)(args[1]->NumberValue());
    int radius = (int)(args[2]->NumberValue());

    int r = (int)(args[3]->NumberValue());
    int g = (int)(args[4]->NumberValue());
    int b = (int)(args[5]->NumberValue());

    unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y) {
        for (int i = x0 - x; i <= x0 + x; i++) {
            if (i>=0 && i<320) {
                if (((y0 + y)>=0) && ((y0 + y)<320)) {
                    unsigned int pix_offset = i * 2 + (y0 + y) * obj->finfo.line_length;
                    *((unsigned short*)(obj->fbp + pix_offset)) = c;
                }

                if (((y0 - y)>=0) && ((y0 - y)<320)) {
                    unsigned int pix_offset = i * 2 + (y0 - y) * obj->finfo.line_length;
                    *((unsigned short*)(obj->fbp + pix_offset)) = c;
                }
            }
        }

        for (int i = x0 - y; i <= x0 + y; i++) {
            if (i>=0 && i<320) {
                if (((y0 + x)>=0) && ((y0 + x)<320)) {
                    unsigned int pix_offset = i * 2 + (y0 + x) * obj->finfo.line_length;
                    *((unsigned short*)(obj->fbp + pix_offset)) = c;
                }

                if (((y0 - x)>=0) && ((y0 - x)<320)) {
                    unsigned int pix_offset = i * 2 + (y0 - x) * obj->finfo.line_length;
                    *((unsigned short*)(obj->fbp + pix_offset)) = c;
                }
            }
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0) {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }

    NanReturnUndefined();
}