
#include <stdio.h>
#include <stdlib.h>


#include "theclient.h"
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        log_this(10, "printlog: Not a shader or a program");
        return;
    }

    char* log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    log_this(10, "Log: %s", log);
    free(log);
}



/**
 * Compile the shader from file 'filename', with error handling
 */

GLuint create_shader(const char* source, GLenum type)
{

    if (source == NULL) {
        fprintf(stderr, "Error opening %s, %s, error:", source, SDL_GetError());
        return 0;
    }
    GLuint res = glCreateShader(type);

    // GLSL version
    const char* version;
    int profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
    if (profile == SDL_GL_CONTEXT_PROFILE_ES)
        version = "#version 100\n";  // OpenGL ES 2.0
    else
        version = "#version 120\n";  // OpenGL 2.1

    // GLES2 precision specifiers
    const char* precision;
    precision =
        "#ifdef GL_ES                        \n"
        "#  ifdef GL_FRAGMENT_PRECISION_HIGH \n"
        "     precision highp float;         \n"
        "#  else                             \n"
        "     precision mediump float;       \n"
        "#  endif                            \n"
        "#else                               \n"
        // Ignore unsupported precision specifiers
        "#  define lowp                      \n"
        "#  define mediump                   \n"
        "#  define highp                     \n"
        "#endif                              \n";

    const GLchar* sources[] = {
        version,
        precision,
        source
    };
    glShaderSource(res, 3, sources, NULL);
    //free((void*)source);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "glLinkProgram\n");
        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}


/*

GLuint create_program(const char *vertexfile, const char *fragmentfile) {
	GLuint text_program = glCreateProgram();
	GLuint shader;

	if(vertexfile) {
		shader = text_create_shader(vertexfile, GL_VERTEX_SHADER);
		if(!shader)
			return 0;
		glAttachShader(text_program, shader);
	}

	if(fragmentfile) {
		shader = text_create_shader(fragmentfile, GL_FRAGMENT_SHADER);
		if(!shader)
			return 0;
		glAttachShader(text_program, shader);
	}

	glLinkProgram(text_program);
	GLint link_ok = GL_FALSE;
	glGetProgramiv(text_program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "glLinkProgram:");
		print_log(text_program);
		glDeleteProgram(text_program);
		return 0;
	}

	return text_program;
}


*/




/**
 * Compile the shader from file 'filename', with error handling

GLuint create_shader(const char* source, GLenum type)
{
    if (source == NULL) {
        fprintf(stderr, "Error opening %s, %s, error:", source, SDL_GetError());
        return 0;
    }
    GLuint res = glCreateShader(type);
    const GLchar* sources[] = {
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"  // OpenGL ES 2.0
#else
        "#version 120\n"  // OpenGL 2.1
#endif
        ,
        source
    };

    glShaderSource(res, 2, sources, NULL);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {

	    log_this(10, "Error %s, error:", source);
        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}
 */

GLuint create_program(const unsigned char *vs_source,const unsigned char *fs_source, GLuint *vs, GLuint *fs)
{
    GLint link_ok = GL_FALSE;
    GLuint program;
    if ((*vs = create_shader((const char*) vs_source, GL_VERTEX_SHADER))   == 0) return 0;
    if ((*fs = create_shader((const char*) fs_source, GL_FRAGMENT_SHADER)) == 0) return 0;

    program = glCreateProgram();
    glAttachShader(program, *vs);
    glAttachShader(program, *fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        fprintf(stderr, "glLinkProgram");
        print_log(program);
        return 0;
    }

    return program;

}

void reset_shaders(GLuint vs,GLuint fs, GLuint program)
{
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}
