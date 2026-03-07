/*
 * Copyright (C) 2026 chunquedong
 *
 * Licensed under the Mozilla Public License Version 2.0
 */
#ifndef OGL_BASE_H_
#define OGL_BASE_H_

// Graphics (OpenGL)
#ifdef __ANDROID__
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray;
    extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays;
    extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays;
    extern PFNGLISVERTEXARRAYOESPROC glIsVertexArray;
    extern PFNGLMAPBUFFEROESPROC glMapBuffer;
    extern PFNGLUNMAPBUFFEROESPROC glUnmapBuffer;
    #define GL_WRITE_ONLY GL_WRITE_ONLY_OES
    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
    #define OPENGL_ES
#elif TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    #include "TargetConditionals.h"
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
    #define glBindVertexArray glBindVertexArrayOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES
    #define glGenVertexArrays glGenVertexArraysOES
    #define glIsVertexArray glIsVertexArrayOES
    #define glMapBuffer glMapBufferOES
    #define glUnmapBuffer glUnmapBufferOES
    #define GL_WRITE_ONLY GL_WRITE_ONLY_OES
    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
    #define OPENGL_ES
#else
    #ifdef GLAD
        #include <glad/glad.h> 
    #else
        #define GLEW_STATIC
        #include <GL/glew.h>
    #endif
#endif

#define glClearDepth glClearDepthf
#define glDepthRange glDepthRangef

/**
 * GL assertion that can be used for any OpenGL function call.
 *
 * This macro will assert if an error is detected when executing
 * the specified GL code. This macro will do nothing in release
 * mode and is therefore safe to use for realtime/per-frame GL
 * function calls.
 */
#if defined(NDEBUG) || !defined(_DEBUG) || defined(__EMSCRIPTEN__)
    #define GL_ASSERT( gl_code ) gl_code
#else
    #define GL_ASSERT( gl_code ) do \
    { \
        gl_code; \
        auto __gl_error_code = glGetError(); \
        if (__gl_error_code != GL_NO_ERROR) printf("glGetError:%d\n", __gl_error_code); \
        assert(__gl_error_code == GL_NO_ERROR); \
    } while(0)
#endif

 /** Global variable to hold GL errors
  * @script{ignore} */
extern GLenum __gl_error_code;

#endif
