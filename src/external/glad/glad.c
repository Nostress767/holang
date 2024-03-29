/*

    OpenGL ES loader generated by glad 0.1.36 on Sun Mar 24 16:49:09 2024.

    Language/Generator: C/C++
    Specification: gl
    APIs: gles2=2.0
    Profile: core
    Extensions:
        
    Loader: True
    Local files: False
    Omit khrplatform: True
    Reproducible: False

    Commandline:
        --profile="core" --api="gles2=2.0" --generator="c" --spec="gl" --omit-khrplatform --extensions=""
    Online:
        https://glad.dav1d.de/#profile=core&language=c&specification=gl&loader=on&api=gles2%3D2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "external/glad/glad.h"

struct gladGLversionStruct GLVersion = { 0, 0 };

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int max_loaded_major;
static int max_loaded_minor;

static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        unsigned int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **)malloc((size_t)num_exts_i * (sizeof *exts_i));
        }

        if (exts_i == NULL) {
            return 0;
        }

        for(index = 0; index < (unsigned)num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            }
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}

static void free_exts(void) {
    if (exts_i != NULL) {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            free((char *)exts_i[index]);
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;
        if(exts_i == NULL) return 0;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if(exts_i[index] != NULL && strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}
int GLAD_GL_ES_VERSION_2_0 = 0;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLATTACHSHADERPROC glad_glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC glad_glBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBINDFRAMEBUFFERPROC glad_glBindFramebuffer = NULL;
PFNGLBINDRENDERBUFFERPROC glad_glBindRenderbuffer = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLBLENDCOLORPROC glad_glBlendColor = NULL;
PFNGLBLENDEQUATIONPROC glad_glBlendEquation = NULL;
PFNGLBLENDEQUATIONSEPARATEPROC glad_glBlendEquationSeparate = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLBLENDFUNCSEPARATEPROC glad_glBlendFuncSeparate = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
PFNGLCOMPILESHADERPROC glad_glCompileShader = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram = NULL;
PFNGLCREATESHADERPROC glad_glCreateShader = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram = NULL;
PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
PFNGLDELETESHADERPROC glad_glDeleteShader = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
PFNGLDEPTHRANGEFPROC glad_glDepthRangef = NULL;
PFNGLDETACHSHADERPROC glad_glDetachShader = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLGENFRAMEBUFFERSPROC glad_glGenFramebuffers = NULL;
PFNGLGENRENDERBUFFERSPROC glad_glGenRenderbuffers = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLGENERATEMIPMAPPROC glad_glGenerateMipmap = NULL;
PFNGLGETACTIVEATTRIBPROC glad_glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC glad_glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC glad_glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC glad_glGetAttribLocation = NULL;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glad_glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glad_glGetRenderbufferParameteriv = NULL;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog = NULL;
PFNGLGETSHADERPRECISIONFORMATPROC glad_glGetShaderPrecisionFormat = NULL;
PFNGLGETSHADERSOURCEPROC glad_glGetShaderSource = NULL;
PFNGLGETSHADERIVPROC glad_glGetShaderiv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC glad_glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC glad_glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC glad_glGetVertexAttribPointerv = NULL;
PFNGLGETVERTEXATTRIBFVPROC glad_glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC glad_glGetVertexAttribiv = NULL;
PFNGLHINTPROC glad_glHint = NULL;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
PFNGLISFRAMEBUFFERPROC glad_glIsFramebuffer = NULL;
PFNGLISPROGRAMPROC glad_glIsProgram = NULL;
PFNGLISRENDERBUFFERPROC glad_glIsRenderbuffer = NULL;
PFNGLISSHADERPROC glad_glIsShader = NULL;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLLINKPROGRAMPROC glad_glLinkProgram = NULL;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLRELEASESHADERCOMPILERPROC glad_glReleaseShaderCompiler = NULL;
PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
PFNGLSCISSORPROC glad_glScissor = NULL;
PFNGLSHADERBINARYPROC glad_glShaderBinary = NULL;
PFNGLSHADERSOURCEPROC glad_glShaderSource = NULL;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
PFNGLSTENCILFUNCSEPARATEPROC glad_glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
PFNGLSTENCILMASKSEPARATEPROC glad_glStencilMaskSeparate = NULL;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
PFNGLSTENCILOPSEPARATEPROC glad_glStencilOpSeparate = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLUNIFORM1FPROC glad_glUniform1f = NULL;
PFNGLUNIFORM1FVPROC glad_glUniform1fv = NULL;
PFNGLUNIFORM1IPROC glad_glUniform1i = NULL;
PFNGLUNIFORM1IVPROC glad_glUniform1iv = NULL;
PFNGLUNIFORM2FPROC glad_glUniform2f = NULL;
PFNGLUNIFORM2FVPROC glad_glUniform2fv = NULL;
PFNGLUNIFORM2IPROC glad_glUniform2i = NULL;
PFNGLUNIFORM2IVPROC glad_glUniform2iv = NULL;
PFNGLUNIFORM3FPROC glad_glUniform3f = NULL;
PFNGLUNIFORM3FVPROC glad_glUniform3fv = NULL;
PFNGLUNIFORM3IPROC glad_glUniform3i = NULL;
PFNGLUNIFORM3IVPROC glad_glUniform3iv = NULL;
PFNGLUNIFORM4FPROC glad_glUniform4f = NULL;
PFNGLUNIFORM4FVPROC glad_glUniform4fv = NULL;
PFNGLUNIFORM4IPROC glad_glUniform4i = NULL;
PFNGLUNIFORM4IVPROC glad_glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC glad_glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC glad_glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glad_glUniformMatrix4fv = NULL;
PFNGLUSEPROGRAMPROC glad_glUseProgram = NULL;
PFNGLVALIDATEPROGRAMPROC glad_glValidateProgram = NULL;
PFNGLVERTEXATTRIB1FPROC glad_glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC glad_glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB2FPROC glad_glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC glad_glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB3FPROC glad_glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC glad_glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB4FPROC glad_glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC glad_glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
static void load_GL_ES_VERSION_2_0(GLADloadproc load) {
	if(!GLAD_GL_ES_VERSION_2_0) return;
	{
		struct{ union{ PFNGLACTIVETEXTUREPROC ptr; void *tmp; }; } tmp = { .tmp = load("glActiveTexture") };
		glad_glActiveTexture = tmp.ptr;
	}
	{
		struct{ union{ PFNGLATTACHSHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glAttachShader") };
		glad_glAttachShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBINDATTRIBLOCATIONPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBindAttribLocation") };
		glad_glBindAttribLocation = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBINDBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBindBuffer") };
		glad_glBindBuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBINDFRAMEBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBindFramebuffer") };
		glad_glBindFramebuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBINDRENDERBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBindRenderbuffer") };
		glad_glBindRenderbuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBINDTEXTUREPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBindTexture") };
		glad_glBindTexture = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBLENDCOLORPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBlendColor") };
		glad_glBlendColor = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBLENDEQUATIONPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBlendEquation") };
		glad_glBlendEquation = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBLENDEQUATIONSEPARATEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBlendEquationSeparate") };
		glad_glBlendEquationSeparate = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBLENDFUNCPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBlendFunc") };
		glad_glBlendFunc = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBLENDFUNCSEPARATEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBlendFuncSeparate") };
		glad_glBlendFuncSeparate = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBUFFERDATAPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBufferData") };
		glad_glBufferData = tmp.ptr;
	}
	{
		struct{ union{ PFNGLBUFFERSUBDATAPROC ptr; void *tmp; }; } tmp = { .tmp = load("glBufferSubData") };
		glad_glBufferSubData = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCHECKFRAMEBUFFERSTATUSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCheckFramebufferStatus") };
		glad_glCheckFramebufferStatus = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCLEARPROC ptr; void *tmp; }; } tmp = { .tmp = load("glClear") };
		glad_glClear = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCLEARCOLORPROC ptr; void *tmp; }; } tmp = { .tmp = load("glClearColor") };
		glad_glClearColor = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCLEARDEPTHFPROC ptr; void *tmp; }; } tmp = { .tmp = load("glClearDepthf") };
		glad_glClearDepthf = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCLEARSTENCILPROC ptr; void *tmp; }; } tmp = { .tmp = load("glClearStencil") };
		glad_glClearStencil = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOLORMASKPROC ptr; void *tmp; }; } tmp = { .tmp = load("glColorMask") };
		glad_glColorMask = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOMPILESHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCompileShader") };
		glad_glCompileShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOMPRESSEDTEXIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCompressedTexImage2D") };
		glad_glCompressedTexImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCompressedTexSubImage2D") };
		glad_glCompressedTexSubImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOPYTEXIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCopyTexImage2D") };
		glad_glCopyTexImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCOPYTEXSUBIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCopyTexSubImage2D") };
		glad_glCopyTexSubImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCREATEPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCreateProgram") };
		glad_glCreateProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCREATESHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCreateShader") };
		glad_glCreateShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLCULLFACEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glCullFace") };
		glad_glCullFace = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETEBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteBuffers") };
		glad_glDeleteBuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETEFRAMEBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteFramebuffers") };
		glad_glDeleteFramebuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETEPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteProgram") };
		glad_glDeleteProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETERENDERBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteRenderbuffers") };
		glad_glDeleteRenderbuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETESHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteShader") };
		glad_glDeleteShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDELETETEXTURESPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDeleteTextures") };
		glad_glDeleteTextures = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDEPTHFUNCPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDepthFunc") };
		glad_glDepthFunc = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDEPTHMASKPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDepthMask") };
		glad_glDepthMask = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDEPTHRANGEFPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDepthRangef") };
		glad_glDepthRangef = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDETACHSHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDetachShader") };
		glad_glDetachShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDISABLEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDisable") };
		glad_glDisable = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDISABLEVERTEXATTRIBARRAYPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDisableVertexAttribArray") };
		glad_glDisableVertexAttribArray = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDRAWARRAYSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDrawArrays") };
		glad_glDrawArrays = tmp.ptr;
	}
	{
		struct{ union{ PFNGLDRAWELEMENTSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glDrawElements") };
		glad_glDrawElements = tmp.ptr;
	}
	{
		struct{ union{ PFNGLENABLEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glEnable") };
		glad_glEnable = tmp.ptr;
	}
	{
		struct{ union{ PFNGLENABLEVERTEXATTRIBARRAYPROC ptr; void *tmp; }; } tmp = { .tmp = load("glEnableVertexAttribArray") };
		glad_glEnableVertexAttribArray = tmp.ptr;
	}
	{
		struct{ union{ PFNGLFINISHPROC ptr; void *tmp; }; } tmp = { .tmp = load("glFinish") };
		glad_glFinish = tmp.ptr;
	}
	{
		struct{ union{ PFNGLFLUSHPROC ptr; void *tmp; }; } tmp = { .tmp = load("glFlush") };
		glad_glFlush = tmp.ptr;
	}
	{
		struct{ union{ PFNGLFRAMEBUFFERRENDERBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glFramebufferRenderbuffer") };
		glad_glFramebufferRenderbuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLFRAMEBUFFERTEXTURE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glFramebufferTexture2D") };
		glad_glFramebufferTexture2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLFRONTFACEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glFrontFace") };
		glad_glFrontFace = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGENBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGenBuffers") };
		glad_glGenBuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGENERATEMIPMAPPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGenerateMipmap") };
		glad_glGenerateMipmap = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGENFRAMEBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGenFramebuffers") };
		glad_glGenFramebuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGENRENDERBUFFERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGenRenderbuffers") };
		glad_glGenRenderbuffers = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGENTEXTURESPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGenTextures") };
		glad_glGenTextures = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETACTIVEATTRIBPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetActiveAttrib") };
		glad_glGetActiveAttrib = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETACTIVEUNIFORMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetActiveUniform") };
		glad_glGetActiveUniform = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETATTACHEDSHADERSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetAttachedShaders") };
		glad_glGetAttachedShaders = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETATTRIBLOCATIONPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetAttribLocation") };
		glad_glGetAttribLocation = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETBOOLEANVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetBooleanv") };
		glad_glGetBooleanv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETBUFFERPARAMETERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetBufferParameteriv") };
		glad_glGetBufferParameteriv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETERRORPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetError") };
		glad_glGetError = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETFLOATVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetFloatv") };
		glad_glGetFloatv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetFramebufferAttachmentParameteriv") };
		glad_glGetFramebufferAttachmentParameteriv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETINTEGERVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetIntegerv") };
		glad_glGetIntegerv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETPROGRAMIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetProgramiv") };
		glad_glGetProgramiv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETPROGRAMINFOLOGPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetProgramInfoLog") };
		glad_glGetProgramInfoLog = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETRENDERBUFFERPARAMETERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetRenderbufferParameteriv") };
		glad_glGetRenderbufferParameteriv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETSHADERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetShaderiv") };
		glad_glGetShaderiv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETSHADERINFOLOGPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetShaderInfoLog") };
		glad_glGetShaderInfoLog = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETSHADERPRECISIONFORMATPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetShaderPrecisionFormat") };
		glad_glGetShaderPrecisionFormat = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETSHADERSOURCEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetShaderSource") };
		glad_glGetShaderSource = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETSTRINGPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetString") };
		glad_glGetString = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETTEXPARAMETERFVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetTexParameterfv") };
		glad_glGetTexParameterfv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETTEXPARAMETERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetTexParameteriv") };
		glad_glGetTexParameteriv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETUNIFORMFVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetUniformfv") };
		glad_glGetUniformfv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETUNIFORMIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetUniformiv") };
		glad_glGetUniformiv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETUNIFORMLOCATIONPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetUniformLocation") };
		glad_glGetUniformLocation = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETVERTEXATTRIBFVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetVertexAttribfv") };
		glad_glGetVertexAttribfv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETVERTEXATTRIBIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetVertexAttribiv") };
		glad_glGetVertexAttribiv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLGETVERTEXATTRIBPOINTERVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetVertexAttribPointerv") };
		glad_glGetVertexAttribPointerv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLHINTPROC ptr; void *tmp; }; } tmp = { .tmp = load("glHint") };
		glad_glHint = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsBuffer") };
		glad_glIsBuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISENABLEDPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsEnabled") };
		glad_glIsEnabled = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISFRAMEBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsFramebuffer") };
		glad_glIsFramebuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsProgram") };
		glad_glIsProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISRENDERBUFFERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsRenderbuffer") };
		glad_glIsRenderbuffer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISSHADERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsShader") };
		glad_glIsShader = tmp.ptr;
	}
	{
		struct{ union{ PFNGLISTEXTUREPROC ptr; void *tmp; }; } tmp = { .tmp = load("glIsTexture") };
		glad_glIsTexture = tmp.ptr;
	}
	{
		struct{ union{ PFNGLLINEWIDTHPROC ptr; void *tmp; }; } tmp = { .tmp = load("glLineWidth") };
		glad_glLineWidth = tmp.ptr;
	}
	{
		struct{ union{ PFNGLLINKPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glLinkProgram") };
		glad_glLinkProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLPIXELSTOREIPROC ptr; void *tmp; }; } tmp = { .tmp = load("glPixelStorei") };
		glad_glPixelStorei = tmp.ptr;
	}
	{
		struct{ union{ PFNGLPOLYGONOFFSETPROC ptr; void *tmp; }; } tmp = { .tmp = load("glPolygonOffset") };
		glad_glPolygonOffset = tmp.ptr;
	}
	{
		struct{ union{ PFNGLREADPIXELSPROC ptr; void *tmp; }; } tmp = { .tmp = load("glReadPixels") };
		glad_glReadPixels = tmp.ptr;
	}
	{
		struct{ union{ PFNGLRELEASESHADERCOMPILERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glReleaseShaderCompiler") };
		glad_glReleaseShaderCompiler = tmp.ptr;
	}
	{
		struct{ union{ PFNGLRENDERBUFFERSTORAGEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glRenderbufferStorage") };
		glad_glRenderbufferStorage = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSAMPLECOVERAGEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glSampleCoverage") };
		glad_glSampleCoverage = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSCISSORPROC ptr; void *tmp; }; } tmp = { .tmp = load("glScissor") };
		glad_glScissor = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSHADERBINARYPROC ptr; void *tmp; }; } tmp = { .tmp = load("glShaderBinary") };
		glad_glShaderBinary = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSHADERSOURCEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glShaderSource") };
		glad_glShaderSource = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILFUNCPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilFunc") };
		glad_glStencilFunc = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILFUNCSEPARATEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilFuncSeparate") };
		glad_glStencilFuncSeparate = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILMASKPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilMask") };
		glad_glStencilMask = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILMASKSEPARATEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilMaskSeparate") };
		glad_glStencilMaskSeparate = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILOPPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilOp") };
		glad_glStencilOp = tmp.ptr;
	}
	{
		struct{ union{ PFNGLSTENCILOPSEPARATEPROC ptr; void *tmp; }; } tmp = { .tmp = load("glStencilOpSeparate") };
		glad_glStencilOpSeparate = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexImage2D") };
		glad_glTexImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXPARAMETERFPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexParameterf") };
		glad_glTexParameterf = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXPARAMETERFVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexParameterfv") };
		glad_glTexParameterfv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXPARAMETERIPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexParameteri") };
		glad_glTexParameteri = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXPARAMETERIVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexParameteriv") };
		glad_glTexParameteriv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLTEXSUBIMAGE2DPROC ptr; void *tmp; }; } tmp = { .tmp = load("glTexSubImage2D") };
		glad_glTexSubImage2D = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM1FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform1f") };
		glad_glUniform1f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM1FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform1fv") };
		glad_glUniform1fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM1IPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform1i") };
		glad_glUniform1i = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM1IVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform1iv") };
		glad_glUniform1iv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM2FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform2f") };
		glad_glUniform2f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM2FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform2fv") };
		glad_glUniform2fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM2IPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform2i") };
		glad_glUniform2i = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM2IVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform2iv") };
		glad_glUniform2iv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM3FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform3f") };
		glad_glUniform3f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM3FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform3fv") };
		glad_glUniform3fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM3IPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform3i") };
		glad_glUniform3i = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM3IVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform3iv") };
		glad_glUniform3iv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM4FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform4f") };
		glad_glUniform4f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM4FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform4fv") };
		glad_glUniform4fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM4IPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform4i") };
		glad_glUniform4i = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORM4IVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniform4iv") };
		glad_glUniform4iv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORMMATRIX2FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniformMatrix2fv") };
		glad_glUniformMatrix2fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORMMATRIX3FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniformMatrix3fv") };
		glad_glUniformMatrix3fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUNIFORMMATRIX4FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUniformMatrix4fv") };
		glad_glUniformMatrix4fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLUSEPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glUseProgram") };
		glad_glUseProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVALIDATEPROGRAMPROC ptr; void *tmp; }; } tmp = { .tmp = load("glValidateProgram") };
		glad_glValidateProgram = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB1FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib1f") };
		glad_glVertexAttrib1f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB1FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib1fv") };
		glad_glVertexAttrib1fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB2FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib2f") };
		glad_glVertexAttrib2f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB2FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib2fv") };
		glad_glVertexAttrib2fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB3FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib3f") };
		glad_glVertexAttrib3f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB3FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib3fv") };
		glad_glVertexAttrib3fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB4FPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib4f") };
		glad_glVertexAttrib4f = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIB4FVPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttrib4fv") };
		glad_glVertexAttrib4fv = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVERTEXATTRIBPOINTERPROC ptr; void *tmp; }; } tmp = { .tmp = load("glVertexAttribPointer") };
		glad_glVertexAttribPointer = tmp.ptr;
	}
	{
		struct{ union{ PFNGLVIEWPORTPROC ptr; void *tmp; }; } tmp = { .tmp = load("glViewport") };
		glad_glViewport = tmp.ptr;
	}
}
static int find_extensionsGLES2(void) {
	if (!get_exts()) return 0;
	(void)&has_ext;
	free_exts();
	return 1;
}

static void find_coreGLES2(void) {

    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;

    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version) return;

    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLVersion.major = major; GLVersion.minor = minor;
    max_loaded_major = major; max_loaded_minor = minor;
	GLAD_GL_ES_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	if (GLVersion.major > 2 || (GLVersion.major >= 2 && GLVersion.minor >= 0)) {
		max_loaded_major = 2;
		max_loaded_minor = 0;
	}
}

int gladLoadGLES2Loader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	{
		struct{ union{ PFNGLGETSTRINGPROC ptr; void *tmp; }; } tmp = { .tmp = load("glGetString") };
		glGetString = tmp.ptr;
	}
	if(glGetString == NULL) return 0;
	if(glGetString(GL_VERSION) == NULL) return 0;
	find_coreGLES2();
	load_GL_ES_VERSION_2_0(load);

	if (!find_extensionsGLES2()) return 0;
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

