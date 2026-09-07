/*
 *  @file
 *  @brief
 *   need: opengl >= 3.0
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.11.20
 *
 *  Copyright (C) 2025 miragekiller
 */

// GLSL 1.10	OpenGL 2.0	2004
// GLSL 1.20	OpenGL 2.1	2006	#version 120
// GLSL 1.30	OpenGL 3.0	2008    vao; fbo; rbo; ubo;
// GLSL 1.40	OpenGL 3.1	2009    ubo+block;
// GLSL 1.50	OpenGL 3.2	2009    core; compatibility;
// GLSL 3.30	OpenGL 3.3	2010    layout(location=n);
// GLSL 4.00	OpenGL 4.0	2010
// GLSL 4.10	OpenGL 4.1	2010
// GLSL 4.20	OpenGL 4.2	2011
// GLSL 4.30	OpenGL 4.3	2012
// GLSL 4.40	OpenGL 4.4	2013
// GLSL 4.50	OpenGL 4.5	2014
// GLSL 4.60	OpenGL 4.6	2017	SPIR-V

#pragma once

#ifndef GXGL_H
#define GXGL_H

// glsl < 140: not support ubo;

#include <gl/gl.h>
#include <glext/glext.h>
#include <glext/wglext.h>
#include "tmgdi.h"


#ifdef TM_WIN
#   pragma comment (lib, "opengl32.lib")
#endif

#define gl_trace                DbgTraceNull
#define gl_verify(x)            if (0) { x; } else { x; _glVerifyLastError(#x, FILE_AND_LINE); }
#define gl_check_fb()           _glVerifyFrameBuffer()
// #define gl_trace            DbgTraceNull
//#define gl_verify(x)      x;

//glNamedBufferData

#define GX_GL_NAMEDBUFFER
#define GX_GL_TEXTUREUNIT
#define GX_GL_TEXTUREIMAGE

template<class T> static void* _glInitF(T& f, string_constant<char> name)
{
    f = (T)wglGetProcAddress(name.data());
    if (f)
        return f;
    char c[256];
    name.copy(c, 256);
    int n = name.length();
    c[n] = 'E';
    c[n + 1] = 'X';
    c[n + 2] = 'T';
    c[n + 3] = '\0';
    f = (T)wglGetProcAddress(c);
    if (f)
        return f;
    c[n] = 'A';
    c[n + 1] = 'R';
    c[n + 2] = 'B';
    f = (T)wglGetProcAddress(c);
    return f;
}
#   define WGL_GET_FUN(f)     b = b && _glInitF(f, TM_CS(#f))
#   define WGL_TRY_FUN(f)     _glInitF(f, TM_CS(#f))


static void _glVerifyFrameBuffer()
{
    PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
    if (!WGL_TRY_FUN(glCheckFramebufferStatus))
        return;
    GLenum s = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    LPCSTR error;
    switch (s)
    {
    case GL_FRAMEBUFFER_UNDEFINED: error = "GL_FRAMEBUFFER_UNDEFINED"; return;
    case GL_FRAMEBUFFER_COMPLETE: error = "FRAMEBUFFER_COMPLETE"; return;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: error = "FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: error = "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: error = "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: error = "FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
    case GL_FRAMEBUFFER_UNSUPPORTED: error = "FRAMEBUFFER_UNSUPPORTED"; break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
    default: error = "FRAMEBUFFER - unknown"; break;
    }
    DbgTrace("frame buffer statues: %s", error);
}

static void _glVerifyLastError(const char* func, const char* fileline)
{
    gl_trace(func);
    GLenum errorCode = glGetError();
    if (errorCode == GL_NO_ERROR)
        return;
    
    LPCSTR error;
    switch (errorCode)
    {
    case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        {
            _glVerifyFrameBuffer();
            error = "INVALID_FRAMEBUFFER_OPERATION";
        }
        break;
    default: error = "unknown";
    }
    DbgTrace("%s %s; hdc:%p; hrc:%p; error:%x(%s)", fileline, func, wglGetCurrentDC(), wglGetCurrentContext(), errorCode, error);
}

typedef void(*PFNVAOINITVAO)(struct CGLDevice* dev);
typedef void(*PFNVAOINITLOC)(struct CGLDevice* dev, GLuint prog);

struct CGLDevice : public TM::CNoCopyable
{
public:
    enum EUBOIndex { eUBOIndexVS = 0, eUBOIndexPS = 1 };

    typedef GLuint      T_VertexLayout;
    typedef GLuint      T_VertexShader;
    typedef GLuint      T_PixelShader;

    struct PS_DESC
    {
        const char*     chCode;
        const char*     szBlock;
    };
    struct VS_DESC
    {
        const char*     chCode;
        const char*     szBlock;
        PFNVAOINITVAO   pfnInitVAO;
        PFNVAOINITLOC   pfnInitLoc;
    };

    struct _BufferImpl
    {
        struct Base : public TM::CNoCopyable
        {
            GLuint obj;

            void Release(CGLDevice* device) { if(obj) { gl_verify(device->glDeleteBuffers(1, &obj)); obj = 0; } }
            void SetCurrent(CGLDevice* device) {}

            Base() : obj(0) {}

            ~Base() { DbgAssert(!obj); }

            void _Create(CGLDevice* device)
            {
                DbgAssert(!obj);
                gl_verify(device->glGenBuffers(1, &obj));
            }
            
            HRESULT _Create(CGLDevice* device, size_t sz, GLenum target, const void* pInit = 0)
            {
                DbgAssert(!obj);
                _Create(device);
                _ResetData(device, sz, target, pInit);
                return S_OK;
            }

            void _ResetData(CGLDevice* device, size_t sz, GLenum target, const void* pInit)
            {
                DbgAssert(obj);
#ifdef GX_GL_NAMEDBUFFER
                if (device->glNamedBufferData)
                {
                    gl_verify(device->glNamedBufferData(obj, sz, pInit, GL_DYNAMIC_DRAW));
                    return;
                }
#endif
                _SetCurrent(device, target);
                gl_verify(device->glBufferData(target, sz, pInit, GL_DYNAMIC_DRAW));
                _ResetCurrent(device, target);
            }
            
            void _SetCurrent(CGLDevice* device, GLenum target) { DbgAssert(obj); gl_verify(device->glBindBuffer(target, obj)); }
            static void _ResetCurrent(CGLDevice* device, GLenum target) { gl_verify(device->glBindBuffer(target, 0)); }
        };

        template <GLenum target> struct _BaseT : public Base
        {
            void _SetCurrent(CGLDevice* device) { Base::_SetCurrent(device, target); }
            static void _ResetCurrent(CGLDevice* device) { Base::_ResetCurrent(device, target); }
        };

        template <GLenum target> struct BaseT : public _BaseT<target>
        {
            HRESULT Create(CGLDevice* device, size_t sz) { return this->_Create(device, sz, target); }
            HRESULT SetDataVec4(CGLDevice* device, const void* pData, UINT Vector4fCount) { this->_ResetData(device, Vector4fCount * sizeof(float) * 4, target, pData); return S_OK; }

        protected:
            HRESULT _CreateUBO(CGLDevice* device, EUBOIndex index, size_t sz)
            {
                Create(device, sz);
                gl_verify(device->glBindBufferBase(GL_UNIFORM_BUFFER, index, this->obj));
                return S_OK;
            }
        };

        template <GLenum target> struct BaseRT : public _BaseT<target>
        {
//         protected:
//             int nSize;
//             BaseRT() : nSize(0) {}

        public:
            void Recreate(CGLDevice* device, const void* pData, int ns) { this->_ResetData(device, ns, target, pData); }
        };
    };

    struct T_VSCBuffer : public _BufferImpl::BaseT<GL_UNIFORM_BUFFER>
    {
        HRESULT Create(CGLDevice* device, size_t sz)
        {
            return this->_CreateUBO(device, eUBOIndexVS, sz);
        }
    };
    
    struct T_PSCBuffer : public _BufferImpl::BaseT<GL_UNIFORM_BUFFER>
    {
        HRESULT Create(CGLDevice* device, size_t sz)
        {
            return this->_CreateUBO(device, eUBOIndexPS, sz);
        }
    };

    struct T_IBO : public _BufferImpl::BaseRT<GL_ELEMENT_ARRAY_BUFFER>  // Index Buffer Objects
    {
        void DrawUINT(CGLDevice* device, GLenum mode, GLsizei count, const GLuint *indices)
        {
//             this->Recreate(device, indices, count * sizeof(GLuint));
            gl_verify(glDrawElements(mode, count, GL_UNSIGNED_INT, 0));
        }
    };

    struct T_VBO : public _BufferImpl::BaseRT<GL_ARRAY_BUFFER> // Vertex Buffer Objects
    {
        GLuint vao;

        T_VBO() : vao(0) {}

        void Init(CGLDevice* device, T_IBO* ibo, CONST VS_DESC* desc)
        {
            DbgAssert(!vao);
            DbgAssert(!obj);
            gl_verify(device->glGenVertexArrays(1, &vao));
            SetCurrent(device);                 // VAO bind
            _Create(device);                    // VBO create
            _SetCurrent(device);                // VBO bind
            if (ibo)
            {
                ibo->_Create(device);           // IBO create
                ibo->_SetCurrent(device);       // IBO bind
            }
            desc->pfnInitVAO(device);              // VAO setting
            ResetCurrent(device);               // VAO unbind
            _ResetCurrent(device);              // VBO unbind
            T_IBO::_ResetCurrent(device);       // IBO unbind
        }

        void Release(CGLDevice* device)
        {
            if (vao) { gl_verify(device->glDeleteVertexArrays(1, &vao)); vao = 0; }
            Base::Release(device);
        }

        void SetCurrent(CGLDevice* device)
        {
            gl_verify(device->glBindVertexArray(vao));
//            this->_SetCurrent(device, GL_ARRAY_BUFFER);
        }

        static void ResetCurrent(CGLDevice* device)
        {
            gl_verify(device->glBindVertexArray(0));
        }

        static void Draw(CGLDevice* device, GLenum mode, GLsizei count)
        {
            gl_verify(glDrawArrays(mode, 0, count));
        }

        bool IsCurrent()
        {
            return GetCurrent() == this->vao;
        }

        static GLuint GetCurrent()
        {
            GLint tmp = 0;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &tmp);
            return tmp;
        }
    };
    
//     T_VSCBuffer* GetVSCBufffer() { return 0; }
//     T_PSCBuffer* GetPSCBufffer() { return 0; }

    struct T_VS : public TM::CNoCopyable // vertex shader
    {
        GLuint vert;

        T_VS() : vert(0) {}
        
        void Release(CGLDevice* device)
        {
            if (vert)
            {
                gl_verify(device->glDeleteShader(vert));
                vert = 0;
            }
        }

        HRESULT Create(CGLDevice* device, VS_DESC desc)
        {
            vert = device->_CompileShader(GL_VERTEX_SHADER, desc.chCode);
            return vert ? S_OK : E_FAIL;
        }

        void SetCurrent(CGLDevice* device) {}
    };
    
    struct T_PS : public TM::CNoCopyable  // pixel shader
    {
        GLuint prog;

        T_PS() : prog(0) {}

        void Release(CGLDevice* device) { if (prog) device->glDeleteProgram(prog); }

        operator bool () const { return 0 != prog; }

        HRESULT Create(CGLDevice* device, PS_DESC desc, const T_VS& vs, const VS_DESC& descVS)
        {
            GLuint frag = device->_CompileShader(GL_FRAGMENT_SHADER, desc.chCode);
            if (!frag)
                return E_FAIL;
            prog = device->CreateShaderProgram(vs.vert, frag, descVS.pfnInitLoc);
            gl_verify(device->glDeleteShader(frag));
            if (!prog)
                return E_FAIL;
            _BindBlock(device, eUBOIndexVS, descVS.szBlock);
            _BindBlock(device, eUBOIndexPS, desc.szBlock);
            return S_OK;
        }

        void _BindBlock(CGLDevice* device, EUBOIndex n, const char* szBlock)
        {
            if (szBlock)
            {
                GLuint blockIndex = device->glGetUniformBlockIndex(prog, szBlock);
                gl_verify(device->glUniformBlockBinding(prog, blockIndex, n));
//                 gl_verify(device->glBindBufferBase(GL_UNIFORM_BUFFER, n, ubo->obj));
            }
        }

        void SetCurrent(CGLDevice* device)
        {
            gl_verify(device->glUseProgram(prog));
        }

        static void ResetCurrent(CGLDevice* device)
        {
            gl_verify(device->glUseProgram(0));
        }

        bool IsCurrent()
        {
            return GetCurrent() == this->prog;
        }

        static GLuint GetCurrent()
        {
            GLint tmp = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &tmp);
            return tmp;
        }
    };


private:
    struct CWndGL
    {
        HWND hWnd;
        HDC hDC;
        HGLRC hRC;
        
        CWndGL() : hRC(0) {}
        
        ~CWndGL()
        {
            if (hRC)
            {
                if (hRC == wglGetCurrentContext())
                    DbgVerify(wglMakeCurrent(0, 0));
                DbgVerify(wglDeleteContext(hRC));
            }
            if (hDC)
                DbgVerify(::ReleaseDC(hWnd, hDC));
            if (hWnd)
                DbgVerify(DestroyWindow(hWnd));
        }
    };

    friend T_VS;
    friend T_PS;

public:
    CWndGL m_wndDevice;

//     PFNGLBLENDFUNCSEPARATEPROC                  glBlendFuncSeparate;            // 1.4

    // cbffer;
    PFNGLGENBUFFERSPROC                         glGenBuffers;                   // 1.5
    PFNGLDELETEBUFFERSPROC                      glDeleteBuffers;
    PFNGLBINDBUFFERPROC                         glBindBuffer;
    PFNGLBUFFERDATAPROC                         glBufferData;
//     PFNGLBUFFERSUBDATAPROC                      glBufferSubData;             // don't use glBufferSubData; it will delay present.
    // texture
    PFNGLACTIVETEXTUREPROC                      glActiveTexture;
    // ubo
    PFNGLGETUNIFORMBLOCKINDEXPROC               glGetUniformBlockIndex;         // 3.1
    PFNGLUNIFORMBLOCKBINDINGPROC                glUniformBlockBinding;          // 3.1
    PFNGLBINDBUFFERBASEPROC                     glBindBufferBase;               // 3.0

    // VAO
    PFNGLGENVERTEXARRAYSPROC                    glGenVertexArrays;              // 3.0
    PFNGLBINDVERTEXARRAYPROC                    glBindVertexArray;
//     PFNGLENABLEVERTEXARRAYATTRIBPROC            glEnableVertexArrayAttrib;   // 4.5
    PFNGLVERTEXATTRIBPOINTERPROC                glVertexAttribPointer;
    PFNGLDELETEVERTEXARRAYSPROC                 glDeleteVertexArrays;
    PFNGLENABLEVERTEXATTRIBARRAYPROC            glEnableVertexAttribArray;
    // FBO
    PFNGLGENFRAMEBUFFERSPROC                    glGenFramebuffers;              // 3.0
    PFNGLDELETEFRAMEBUFFERSPROC                 glDeleteFramebuffers;
    PFNGLBINDFRAMEBUFFERPROC                    glBindFramebuffer;
    PFNGLFRAMEBUFFERTEXTURE2DPROC               glFramebufferTexture2D;
    PFNGLBLITFRAMEBUFFERPROC                    glBlitFramebuffer;
//     PFNGLFRAMEBUFFERRENDERBUFFERPROC            glFramebufferRenderbuffer;              // FBO renderbuffer attachement procedure
    // Renderbuffer object
//     PFNGLGENRENDERBUFFERSPROC                   glGenRenderbuffers;                     // RBO generation procedure
//     PFNGLDELETERENDERBUFFERSPROC                glDeleteRenderbuffers;                  // RBO deletion procedure
//     PFNGLBINDRENDERBUFFERPROC                   glBindRenderbuffer;                     // RBO bind procedure
//    PFNGLRENDERBUFFERSTORAGEPROC                glRenderbufferStorage;                  // RBO memory allocation procedure
    // Shader
    PFNGLCREATESHADERPROC                       glCreateShader;                 // 2.0
    PFNGLSHADERSOURCEPROC                       glShaderSource;
    PFNGLCOMPILESHADERPROC                      glCompileShader;
    PFNGLGETSHADERIVPROC                        glGetShaderiv;
    PFNGLGETSHADERINFOLOGPROC                   glGetShaderInfoLog;
    PFNGLDELETESHADERPROC                       glDeleteShader;
    // shaderProgram
    PFNGLCREATEPROGRAMPROC                      glCreateProgram;                // 2.0
    PFNGLDELETEPROGRAMPROC                      glDeleteProgram;
    PFNGLATTACHSHADERPROC                       glAttachShader;
    PFNGLLINKPROGRAMPROC                        glLinkProgram;
    PFNGLGETPROGRAMIVPROC                       glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC                  glGetProgramInfoLog;
    PFNGLBINDATTRIBLOCATIONPROC                 glBindAttribLocation;
    PFNGLUSEPROGRAMPROC                         glUseProgram;

    // named buffer
#ifdef GX_GL_NAMEDBUFFER
    PFNGLNAMEDBUFFERDATAPROC                    glNamedBufferData;              // 4.5
#endif

#ifdef GX_GL_TEXTUREUNIT
    PFNGLBINDTEXTUREUNITPROC                    glBindTextureUnit;              // 4.5
    PFNGLBINDTEXTURESPROC                       glBindTextures;                 // 4.4
#endif

#ifdef GX_GL_TEXTUREIMAGE
    PFNGLGETTEXTUREIMAGEPROC                    glGetTextureImage;              // GL_EXT_direct_state_access
#endif

    // wgl
    PFNWGLCHOOSEPIXELFORMATARBPROC              wglChoosePixelFormatARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC           wglCreateContextAttribsARB;

public:
    TMBool InitProc()
    {
        TMLParam b = true;

//         WGL_GET_FUN(glBlendFuncSeparate);
        
        WGL_GET_FUN(glGenBuffers);
        WGL_GET_FUN(glDeleteBuffers);
        WGL_GET_FUN(glBindBuffer);
        WGL_GET_FUN(glBufferData);
//         WGL_GET_FUN(glBufferSubData);

        WGL_GET_FUN(glActiveTexture);

        WGL_GET_FUN(glGetUniformBlockIndex);
        WGL_GET_FUN(glUniformBlockBinding);
        WGL_GET_FUN(glBindBufferBase);

        WGL_GET_FUN(glGenVertexArrays);
        WGL_GET_FUN(glBindVertexArray);
//         WGL_GET_FUN(glEnableVertexArrayAttrib);
        WGL_GET_FUN(glVertexAttribPointer);
        WGL_GET_FUN(glDeleteVertexArrays);
        WGL_GET_FUN(glEnableVertexAttribArray);

        WGL_GET_FUN(glGenFramebuffers);
        WGL_GET_FUN(glDeleteFramebuffers);
        WGL_GET_FUN(glBindFramebuffer);
        WGL_GET_FUN(glFramebufferTexture2D);
        WGL_GET_FUN(glBlitFramebuffer);
//         WGL_GET_FUN(glFramebufferRenderbuffer);
        
//         WGL_GET_FUN(glGenRenderbuffers);
//         WGL_GET_FUN(glDeleteRenderbuffers);
//         WGL_GET_FUN(glBindRenderbuffer);
//        WGL_GET_FUN(glRenderbufferStorage);
        
        WGL_GET_FUN(glCreateShader);
        WGL_GET_FUN(glShaderSource);
        WGL_GET_FUN(glCompileShader);
        WGL_GET_FUN(glGetShaderiv);
        WGL_GET_FUN(glGetShaderInfoLog);
        WGL_GET_FUN(glDeleteShader);
        
        WGL_GET_FUN(glCreateProgram);
        WGL_GET_FUN(glDeleteProgram);
        WGL_GET_FUN(glAttachShader);
        WGL_GET_FUN(glLinkProgram);
        WGL_GET_FUN(glGetProgramiv);
        WGL_GET_FUN(glGetProgramInfoLog);
        WGL_GET_FUN(glBindAttribLocation);
        WGL_GET_FUN(glUseProgram);

#ifdef GX_GL_NAMEDBUFFER
        WGL_TRY_FUN(glNamedBufferData);
#endif
        
#ifdef GX_GL_TEXTUREUNIT
        WGL_TRY_FUN(glBindTextureUnit);
        WGL_TRY_FUN(glBindTextures);
#endif

#ifdef GX_GL_TEXTUREIMAGE
        WGL_TRY_FUN(glGetTextureImage);
#endif

        WGL_GET_FUN(wglChoosePixelFormatARB);
        WGL_GET_FUN(wglCreateContextAttribsARB);

#undef WGL_GET_FUN

        return 0 != b;
    }

    TMBool InitDC(HDC hDC)
    {
        const int pixelAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_FALSE,
            // WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, // Hardware accelerated
            WGL_COLOR_BITS_ARB, 32,
            WGL_ALPHA_BITS_ARB, 8,
            // WGL_DEPTH_BITS_ARB, 24,
            // WGL_STENCIL_BITS_ARB, 8,
            // WGL_SAMPLE_BUFFERS_ARB, GL_FALSE,
            // WGL_SAMPLES_ARB, 4, // multisample support
            0
        };
        
        int pixelFormatID; UINT numFormats;
        DbgVerify(wglChoosePixelFormatARB(hDC, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats));
        
        PIXELFORMATDESCRIPTOR PFD;
        DbgVerify(DescribePixelFormat(hDC, pixelFormatID, sizeof(PFD), &PFD));
        DbgVerify(SetPixelFormat(hDC, pixelFormatID, &PFD));

        return numFormats;
    }

public:
    operator bool () const { return m_wndDevice.hWnd != 0; }

    bool CreateDevice()
    {
        struct CWndGLTemp : CWndGL
        {
            CWndGLTemp()
            {
                // some device will render failed if window client size < 1.
                hWnd = ::CreateWindow(L"static", L"TMDui openGL device", WS_POPUP, 0, 0, 640, 480, 0, 0, 0, 0);
                hDC = ::GetDC(hWnd);
            }
        };
        
        // create fake window and DC, RC
        CWndGLTemp wndFake;
        
        PIXELFORMATDESCRIPTOR  pfd = { sizeof(PIXELFORMATDESCRIPTOR) };
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;// | PFD_DOUBLEBUFFER;// | PFD_SWAP_COPY;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
//         pfd.iLayerType = PFD_MAIN_PLANE;
        // 	pfd.cDepthBits = 24;
        
        int PixelFormat;
        if (!(PixelFormat = ChoosePixelFormat(wndFake.hDC, &pfd)))
            return false;
        
        if(!SetPixelFormat(wndFake.hDC, PixelFormat, &pfd))
            return false;
        
        if (!(wndFake.hRC = wglCreateContext(wndFake.hDC)))
            return false;
        
        DbgVerify(wglMakeCurrent(wndFake.hDC, wndFake.hRC));
        
        // init GL proc
        if (!InitProc())
            return false;
        
        // create device window and DC, RC
        CWndGLTemp wndDev;
        InitDC(wndDev.hDC);
        
        const int major_min = 3, minor_min = 0;
        const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
//            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            //		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0
        };
        
        if (!(wndDev.hRC = wglCreateContextAttribsARB(wndDev.hDC, 0, contextAttribs)))
            return false;

        m_wndDevice = wndDev;
        wndDev.hWnd = 0;
        wndDev.hDC = 0;
        wndDev.hRC = 0;

#ifdef TM_DBG
        const char* c1 = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        const char* c2 = (const char*)glGetString(GL_VENDOR);
        const char* c3 = (const char*)glGetString(GL_RENDERER);
        const char* c4 = (const char*)glGetString(GL_VERSION);
        DbgTrace("GL_SHADING_LANGUAGE_VERSION: %s", c1);
        DbgTrace("GL_VENDOR: %s", c2);
        DbgTrace("GL_RENDERER: %s", c3);
        DbgTrace("GL_VERSION: %s", c4);

        GLint maj = 0, min = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &maj);
        glGetIntegerv(GL_MINOR_VERSION, &min);
        DbgTrace("GL_MAJOR_VERSION: %d", maj);
        DbgTrace("GL_MINOR_VERSION: %d", min);

        PFNGLGETSTRINGIPROC glGetStringi;
        if (WGL_TRY_FUN(glGetStringi))
        {
            for (int i = 0; ; ++i)
            {
                const char* cx = (const char*)glGetStringi(GL_EXTENSIONS, i);
                if (!cx)
                    break;
                DbgTrace("GL_EXTENSIONS_%03d: %s", i, cx);
            }
        }
        else
        {
            TM::CStrViewA c5((const char*)glGetString(GL_EXTENSIONS));
            TM::CStringA strTmp;
            int j = 0;
            for (size_t i = 0; i = c5.Split(i, ' ', strTmp); ++j)
            {
                DbgTrace("GL_EXTENSIONS_%03d: %s", j, strTmp.c_str());
            }
        }
        gl_verify(glEnable(GL_DEBUG_OUTPUT));
#endif

        return true;
    }

//     void GetDeviceDesc(GXADAPTERDESC& desc)
//     {
//         CComPtr<IDirect3D9> ptrD3D;
//         DbgVerifyHResult(m_pd3dDevice9->GetDirect3D(&ptrD3D));
//         D3DADAPTER_IDENTIFIER9 id9;
//         DbgVerifyHResult(ptrD3D->GetAdapterIdentifier(m_Adapter, 0, &id9));
//         StrAssign(desc.Description, tm_countof(desc.Description), id9.Description, -1);
//         desc.VendorId = id9.VendorId;
//         desc.DeviceId = id9.DeviceId;
//         desc.SubSysId = id9.SubSysId;
//         desc.Revision = id9.Revision;
//         desc.DedicatedVideoMemory = m_pd3dDevice9->GetAvailableTextureMem();
//         desc.DedicatedSystemMemory = 0;
//         desc.SharedSystemMemory = 0;
//         desc.AdapterLuid = id9.DeviceIdentifier;
//         desc.DriverVersion = id9.DriverVersion.QuadPart;
//     }

    void EnableScissor()
    {
        gl_verify(glEnable(GL_SCISSOR_TEST));
    }

    void DisableScissor()
    {
        gl_verify(glDisable(GL_SCISSOR_TEST));
    }

    void SetScissorRect(const TMRect& rc)
    {
        DbgAssert(*this);
        glScissor(RectLeft(rc), RectTop(rc), RectWidth(rc), RectHeight(rc));
    }

    GLint GetTexture0()
    {
        GLint tex;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex);
        return tex;
    }

    void SetTexture1(GLuint tex0)
    {
        DbgAssert(*this);
#ifdef GX_GL_TEXTUREUNIT
        if (glBindTextureUnit)
        {
            DbgAssert(*this);
            gl_verify(glBindTextureUnit(0, tex0));
            return;
        }
#endif
        gl_verify(glBindTexture(GL_TEXTURE_2D, tex0));
    }

    void SetTexture2(GLuint tex0, GLuint tex1)
    {
#ifdef GX_GL_TEXTUREUNIT
        if (glBindTextures)
        {
            DbgAssert(*this);
            GLuint tmp[] = { tex0, tex1 };
            gl_verify(glBindTextures(0, 2, tmp));
            return;
        }
#endif
        SetTextureAt(1, tex1);
        SetTextureAt(0, tex0);
    }

    void SetTexture3(GLuint tex0, GLuint tex1, GLuint tex2)
    {
#ifdef GX_GL_TEXTUREUNIT
        if (glBindTextures)
        {
            DbgAssert(*this);
            GLuint tmp[] = { tex0, tex1, tex2 };
            gl_verify(glBindTextures(0, 3, tmp));
            return;
        }
#endif
        SetTextureAt(2, tex2);
        SetTexture2(tex0, tex1);
    }

    void SetTextureAt(DWORD Stage, GLuint texture)
    {
        DbgAssert(*this);
#ifdef GX_GL_TEXTUREUNIT
        if (glBindTextureUnit)
        {
            gl_verify(glBindTextureUnit(Stage, texture));
            return;
        }
#endif
        gl_verify(glActiveTexture(GL_TEXTURE0 + Stage));
        gl_verify(glBindTexture(GL_TEXTURE_2D, texture));
    }
    
// 
//     void SetRenderTarget(IDirect3DSurface9* pRenderTarget)
//     {
//         DbgAssert(m_pd3dDevice9);
//         DbgVerifyHResult(m_pd3dDevice9->SetRenderTarget(0, pRenderTarget));
//     }
// 
//     void SetRenderTarget(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pNewZStencil)
//     {
//         DbgAssert(m_pd3dDevice9);
//         DbgVerifyHResult(m_pd3dDevice9->SetRenderTarget(0, pRenderTarget));
//         DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(pNewZStencil));
//     }
// 
//     HRESULT CreateVertexLayout(
//         CONST D3DVERTEXELEMENT9* pVertexElements, UINT NumElements,
//         const void *pShaderBytecode, size_t BytecodeLength,
//         IDirect3DVertexDeclaration9** ppDecl)
//     {
//         DbgAssert(m_pd3dDevice9);
//         HRESULT hr;
//         DbgVerifyHResult(hr = m_pd3dDevice9->CreateVertexDeclaration(pVertexElements, ppDecl));
//         return hr;
//     }
// 
//     void SetVertexLayout(IDirect3DVertexDeclaration9* pDecl)
//     {
//         DbgAssert(m_pd3dDevice9);
//         DbgVerifyHResult(m_pd3dDevice9->SetVertexDeclaration(pDecl));
//     }
// 
//     HRESULT CreateVertexLayoutAndShader(
//         CONST D3DVERTEXELEMENT9* pVertexElements, UINT NumElements,
//         const void *pShaderBytecode, size_t BytecodeLength,
//         IDirect3DVertexDeclaration9** ppDecl, IDirect3DVertexShader9 **ppShader)
//     {
//         DbgAssert(m_pd3dDevice9);
//         HRESULT hr;
//         DbgVerifyHResult(hr = m_pd3dDevice9->CreateVertexDeclaration(pVertexElements, ppDecl));
//         DbgVerifyHResult(hr |= m_pd3dDevice9->CreateVertexShader((const DWORD*)pShaderBytecode, ppShader));
//         return hr;
//     }

private:
    GLuint _CompileShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            DbgTrace(infoLog);
            gl_verify(glDeleteShader(shader));
            shader = 0;
        }
        
        return shader;
    }

public:
    HRESULT CreateVertexShader(const char *pShaderBytecode, size_t BytecodeLength, T_VertexShader* pShader)
    {
        DbgAssert(*this);
        *pShader = _CompileShader(GL_VERTEX_SHADER, pShaderBytecode);
        return *pShader ? S_OK : E_FAIL;
    }

//     void SetVertexShader(IDirect3DVertexShader9* pShader)
//     {
//         DbgAssert(m_pd3dDevice9);
//         DbgVerifyHResult(m_pd3dDevice9->SetVertexShader(pShader));
//     }
    
    HRESULT CreatePixelShader(const char *pShaderBytecode, size_t BytecodeLength, T_PixelShader* pShader)
    {
        DbgAssert(*this);
        *pShader = _CompileShader(GL_FRAGMENT_SHADER, pShaderBytecode);
        return *pShader ? S_OK : E_FAIL;
    }

//     void SetPixelShader(IDirect3DPixelShader9* pShader)
//     {
//         DbgAssert(m_pd3dDevice9);
//         DbgVerifyHResult(m_pd3dDevice9->SetPixelShader(pShader));
//     }

    GLuint CreateShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, PFNVAOINITLOC pfn)
    {
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        if (pfn)
            pfn(this, shaderProgram);
        
//             gl_verify(glBindAttribLocation(shaderProgram, 0, "in_var_POSITION0"));
//             gl_verify(glBindAttribLocation(shaderProgram, 1, "in_var_COLOR0"));
//             gl_verify(glBindAttribLocation(shaderProgram, 2, "in_var_TEXCOORD0"));
        
        glLinkProgram(shaderProgram);
        
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            DbgTrace(infoLog);
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }
        
        return shaderProgram;
    }

//     
//     HRESULT InitVSConstantBuffer(size_t sz) { return S_OK; }
//     HRESULT InitPSConstantBuffer(size_t sz) { return S_OK; }
// 
//     HRESULT SetVSConstantF(const void* pConstantData, UINT Vector4fCount)
//     {
//         DbgAssert(m_pd3dDevice9);
//         HRESULT hr;
//         DbgVerifyHResult(hr = m_pd3dDevice9->SetVertexShaderConstantF(0, (const float*)pConstantData, Vector4fCount));
//         return hr;
//     }
// 
//     HRESULT SetPSConstantF(const void* pConstantData, UINT Vector4fCount)
//     {
//         DbgAssert(m_pd3dDevice9);
//         HRESULT hr;
//         DbgVerifyHResult(hr = m_pd3dDevice9->SetPixelShaderConstantF(0, (const float*)pConstantData, Vector4fCount));
//         return hr;
//     }


//     void DrawUINT(GLenum mode, GLsizei count, const GLuint *indices)
//     {
//         gl_verify(glDrawElements(mode, count, GL_UNSIGNED_INT, indices));
//     }

};



#endif //GXGL_H
