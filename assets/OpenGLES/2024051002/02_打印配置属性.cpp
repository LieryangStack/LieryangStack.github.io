#include <EGL/egl.h>
#include <stdio.h>

// 打印配置属性的函数
void printConfigAttributes(EGLDisplay display, EGLConfig config) {
    EGLint value;
    
    // 要查询的属性列表
    EGLint attributes[] = {
        EGL_BUFFER_SIZE,
        EGL_RED_SIZE,
        EGL_GREEN_SIZE,
        EGL_BLUE_SIZE,
        EGL_ALPHA_SIZE,
        EGL_DEPTH_SIZE,
        EGL_STENCIL_SIZE,
        EGL_CONFIG_CAVEAT,
        EGL_CONFIG_ID,
        EGL_LEVEL,
        EGL_MAX_PBUFFER_HEIGHT,
        EGL_MAX_PBUFFER_PIXELS,
        EGL_MAX_PBUFFER_WIDTH,
        EGL_NATIVE_RENDERABLE,
        EGL_NATIVE_VISUAL_ID,
        EGL_NATIVE_VISUAL_TYPE,
        EGL_SAMPLES,
        EGL_SAMPLE_BUFFERS,
        EGL_SURFACE_TYPE,
        EGL_TRANSPARENT_TYPE,
        EGL_TRANSPARENT_RED_VALUE,
        EGL_TRANSPARENT_GREEN_VALUE,
        EGL_TRANSPARENT_BLUE_VALUE,
        EGL_BIND_TO_TEXTURE_RGB,
        EGL_BIND_TO_TEXTURE_RGBA,
        EGL_MIN_SWAP_INTERVAL,
        EGL_MAX_SWAP_INTERVAL,
        EGL_LUMINANCE_SIZE,
        EGL_ALPHA_MASK_SIZE,
        EGL_COLOR_BUFFER_TYPE,
        EGL_RENDERABLE_TYPE,
        EGL_MATCH_NATIVE_PIXMAP,
        EGL_CONFORMANT
    };
    
    // 属性名称字符串
    const char* attributeNames[] = {
        "EGL_BUFFER_SIZE",
        "EGL_RED_SIZE",
        "EGL_GREEN_SIZE",
        "EGL_BLUE_SIZE",
        "EGL_ALPHA_SIZE",
        "EGL_DEPTH_SIZE",
        "EGL_STENCIL_SIZE",
        "EGL_CONFIG_CAVEAT",
        "EGL_CONFIG_ID",
        "EGL_LEVEL",
        "EGL_MAX_PBUFFER_HEIGHT",
        "EGL_MAX_PBUFFER_PIXELS",
        "EGL_MAX_PBUFFER_WIDTH",
        "EGL_NATIVE_RENDERABLE",
        "EGL_NATIVE_VISUAL_ID",
        "EGL_NATIVE_VISUAL_TYPE",
        "EGL_SAMPLES",
        "EGL_SAMPLE_BUFFERS",
        "EGL_SURFACE_TYPE",
        "EGL_TRANSPARENT_TYPE",
        "EGL_TRANSPARENT_RED_VALUE",
        "EGL_TRANSPARENT_GREEN_VALUE",
        "EGL_TRANSPARENT_BLUE_VALUE",
        "EGL_BIND_TO_TEXTURE_RGB",
        "EGL_BIND_TO_TEXTURE_RGBA",
        "EGL_MIN_SWAP_INTERVAL",
        "EGL_MAX_SWAP_INTERVAL",
        "EGL_LUMINANCE_SIZE",
        "EGL_ALPHA_MASK_SIZE",
        "EGL_COLOR_BUFFER_TYPE",
        "EGL_RENDERABLE_TYPE",
        "EGL_MATCH_NATIVE_PIXMAP",
        "EGL_CONFORMANT"
    };
    
    // 查询并打印每个属性的值
    for (int i = 0; i < sizeof(attributes) / sizeof(attributes[0]); i++) {
        if (eglGetConfigAttrib(display, config, attributes[i], &value)) {
            printf("%s: %d\n", attributeNames[i], value);
        } else {
            printf("Failed to get attribute %s\n", attributeNames[i]);
        }
    }
}

int main() {
    // 初始化 EGL Display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    // 配置属性列表
    EGLint configAttribs[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    // 获取配置
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);

    if (numConfigs > 0) {
        // 打印配置的具体信息
        printConfigAttributes(display, config);
    } else {
        printf("No matching EGL configurations found.\n");
    }

    // 终止 EGL
    eglTerminate(display);
    return 0;
}
