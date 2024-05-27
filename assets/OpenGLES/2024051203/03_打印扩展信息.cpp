#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysymdef.h>

#include <GLES3/gl32.h>
#include <EGL/egl.h>

#include <glib.h>

#include <iostream>

int main() {
    // 初始化 EGL Display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, NULL, NULL);

    const char *eglexts;
    unsigned const char *glexts;

    eglexts = eglQueryString (display, EGL_EXTENSIONS);
    glexts = glGetString (GL_EXTENSIONS);

    g_print ("Available EGL extensions: %s\n\n", eglexts);
    g_print ("Available GLES extensions: %s\n", glexts);

    // 终止 EGL
    eglTerminate(display);
    return 0;
}
