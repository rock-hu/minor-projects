/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NATIVE_XCOMPONENT_EGL_CORE_H
#define NATIVE_XCOMPONENT_EGL_CORE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

namespace NativeXComponentSample {
class EGLCore {
public:
    explicit EGLCore(){};
    ~EGLCore() {}
    bool EglContextInit(void *window, int width, int height);
    bool CreateEnvironment();
    void Release();
    void UpdateSize(int width, int height);

private:
    GLuint LoadShader(GLenum type, const char *shaderSrc);
    GLuint CreateProgram(const char *vertexShader, const char *fragShader);

private:
    EGLNativeWindowType eglWindow_;
    EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
    EGLConfig eglConfig_ = EGL_NO_CONFIG_KHR;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    EGLContext eglContext_ = EGL_NO_CONTEXT;
    GLuint program_;
    bool flag_ = false;
    int width_;
    int height_;
    GLfloat widthPercent_;
};
} // namespace NativeXComponentSample
#endif // NATIVE_XCOMPONENT_EGL_CORE_H