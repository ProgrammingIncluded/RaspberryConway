/***********************************************
 * Project: RaspberryConway
 * File: opengl.cpp
 * By: ProgrammingIncluded
 * Website: ProgrammingIncluded.github.io
 * License: GNU GPLv3 (see LICENSE file)
 * Inspired by:https://blogs.igalia.com/elima/2016/10/06/example-run-an-opengl-es-compute-shader-on-a-drm-render-node/
***********************************************/

#if (OPENGLES_MODE == 1)
#include "opengl.hpp"
#include "app.hpp"

#include <iostream>
#include <assert.h>
#include <gbm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


// Open GL libraries
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define check() assert(glGetError() == 0)

uint32_t GScreenHeight = SCREEN_Y;
EGLDisplay GDisplay;
EGLSurface GSurface;
EGLContext GContext;

void updateScreen() {
    std::cout << "Render" << std::endl;
    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(GDisplay, GSurface);
}

void closeOpenGLES() {
    std::cout << "Shutdown OpenGLES" << std::endl;
    eglDestroySurface(GDisplay, GSurface);
    eglMakeCurrent(GDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(GDisplay, GContext);
    eglTerminate(GDisplay);
}

static int match_config_to_visual(EGLDisplay egl_display, EGLint visual_id, EGLConfig *configs, int count) {

    EGLint id;
    for (uint i = 0; i < count; ++i) {
        if (!eglGetConfigAttrib(egl_display, configs[i], EGL_NATIVE_VISUAL_ID,&id)) continue;
        if (id == visual_id) return i;
    }
    return -1;
}

void setupOpenGLES() {
    int32_t fd = open ("/dev/dri/renderD128", O_RDWR);
    assert (fd > 0);

    struct gbm_device *gbm = gbm_create_device(fd);
    assert (gbm != NULL);

    struct gbm_surface *gbm_surface = gbm_surface_create(gbm, SCREEN_Y, SCREEN_X, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT|GBM_BO_USE_RENDERING);
    
    /* setup EGL from the GBM device */
    EGLDisplay GDisplay = eglGetPlatformDisplay (EGL_PLATFORM_GBM_MESA, gbm, NULL);
    assert (GDisplay != NULL);

    bool res = eglInitialize (GDisplay, NULL, NULL);
    assert (res);

    const char *egl_extension_st = eglQueryString (GDisplay, EGL_EXTENSIONS);
    assert (strstr (egl_extension_st, "EGL_KHR_create_context") != NULL);
    assert (strstr (egl_extension_st, "EGL_KHR_surfaceless_context") != NULL);

    static const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint count;
    EGLint num_config;
    int config_index;
    
    res = eglChooseConfig (GDisplay, config_attribs, &cfg, 1, &count);
    assert (res);

    config_index = match_config_to_visual(GDisplay, GBM_FORMAT_XRGB8888, &cfg, num_config);

    res = eglBindAPI (EGL_OPENGL_ES_API);
    assert (res);
    
    static const EGLint attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    EGLContext core_ctx = eglCreateContext (GDisplay,
                                            cfg,
                                            EGL_NO_CONTEXT,
                                            attribs);
    assert (core_ctx != EGL_NO_CONTEXT);

    GSurface = eglCreateWindowSurface(GDisplay, cfg, gbm_surface, NULL);
    
    res = eglMakeCurrent (GDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, core_ctx);
    assert (res);
    
    /* setup a compute shader */
    // GLuint compute_shader = glCreateShader (GL_COMPUTE_SHADER);
    // assert (glGetError () == GL_NO_ERROR);
    
    // const char *shader_source = COMPUTE_SHADER_SRC;
    // glShaderSource (compute_shader, 1, &shader_source, NULL);
    // assert (glGetError () == GL_NO_ERROR);
    
    // glCompileShader (compute_shader);
    // assert (glGetError () == GL_NO_ERROR);
    
    // GLuint shader_program = glCreateProgram ();
    
    // glAttachShader (shader_program, compute_shader);
    // assert (glGetError () == GL_NO_ERROR);
    
    // glLinkProgram (shader_program);
    // assert (glGetError () == GL_NO_ERROR);
    
    // glDeleteShader (compute_shader);
    
    // glUseProgram (shader_program);
    // assert (glGetError () == GL_NO_ERROR);
    
    /* dispatch computation */
    // glDispatchCompute (1, 1, 1);
    // assert (glGetError () == GL_NO_ERROR);
    
    printf ("Compute shader dispatched and finished successfully\n");
    
    /* free stuff */
    // glDeleteProgram (shader_program);
    eglDestroyContext (GDisplay, core_ctx);
    eglTerminate (GDisplay);
    gbm_device_destroy (gbm);
    close (fd);
}
#endif
