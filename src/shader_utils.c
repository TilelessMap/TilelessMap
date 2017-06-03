

#include "SDL.h"
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

    log_this(100, "Log: %s", log);
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
    log_this(100,"problem with shader src : %s", source);
        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}


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



int build_program()
{
    GLuint vs, fs;



    /*Build standard program*/



    const unsigned char gen_vstd[1024] =  "attribute vec2 coord2d; \
uniform mat4 theMatrix;\
void main(void) { \
  gl_Position =  theMatrix * vec4(coord2d,  1.0, 1.0);  \
}";

    const unsigned char gen_fstd[1024] = "uniform vec4 color; \
void main(void) { \
  gl_FragColor = color; \
}";


    std_program = create_program(gen_vstd, gen_fstd, &vs, &fs);


    std_coord2d = glGetAttribLocation(std_program, "coord2d");
    if (std_coord2d == -1) {
        log_this(1, "Could not bind attribute : %s\n", "coord2d");
        return 0;
    }

    std_matrix = glGetUniformLocation(std_program, "theMatrix");
    if (std_matrix == -1) {
        log_this(1, "Could not bind uniform : %s\n", "theMatrix");
        return 0;
    }

    std_color = glGetUniformLocation(std_program, "color");
    if (std_color == -1) {
        log_this(1, "Could not bind uniform : %s\n", "color");
        return 0;
    }


    reset_shaders(vs, fs, std_program);





    /*Build standard text program*/



    const unsigned char gen_vtxt[1024] =  "attribute vec4 box;\
uniform vec2 coord2d; \
uniform mat4 theMatrix; \
uniform vec4 color; \
varying vec2 texpos;\
void main(void) {\
  gl_Position = theMatrix * vec4(coord2d, 1.0, 1.0) + vec4(box.xy, 0.0, 0.0);\
  texpos = box.zw;\
    }";

    const unsigned char gen_ftxt[1024] = "varying vec2 texpos;\
uniform sampler2D tex;\
uniform vec4 color;\
void main(void) {\
  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;\
}\
";

    txt_program = create_program(gen_vtxt, gen_ftxt, &vs, &fs);


    txt_box = glGetAttribLocation(txt_program, "box");
    if (txt_box == -1) {
        log_this(1, "Could not bind attribute : %s\n", "box");
        return 0;
    }

    txt_matrix = glGetUniformLocation(txt_program, "theMatrix");
    if (txt_matrix == -1) {
        log_this(1, "Could not bind uniform : %s\n", "theMatrix");
        return 0;
    }

    txt_color = glGetUniformLocation(txt_program, "color");
    if (txt_color == -1) {
        log_this(1, "Could not bind uniform : %s\n", "color");
        return 0;
    }

    txt_coord2d = glGetUniformLocation(txt_program, "coord2d");
    if (txt_coord2d == -1) {
        log_this(1, "Could not bind uniform : %s\n", "coord2d");
        return 0;
    }
    txt_tex = glGetUniformLocation(txt_program, "tex");

    if (txt_tex == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "tex");
        return 0;
    }

    reset_shaders(vs, fs, txt_program);






    /*create a shader program lines with width*/

    const unsigned char gen_vlw[1024] =  "attribute vec2 coord2d; \
attribute vec2 norm;\
uniform float linewidth;\
uniform float z;\
uniform mat4 px_Matrix;\
uniform mat4 theMatrix;\
void main(void) { \
vec4 delta = vec4(norm * linewidth,0,0); \
vec4 npos = px_Matrix * delta; \
vec4 pos = theMatrix * vec4(coord2d, z, 1.0);  \
  gl_Position = (pos + npos);\
}";

    const unsigned char gen_flw[1024] = "uniform vec4 color; \
void main(void) { \
  gl_FragColor = color; \
}";


    lw_program = create_program((unsigned char *) gen_vlw,(unsigned char *)  gen_flw, &vs, &fs);

    if(lw_program == 0)
        return 0;


    lw_coord2d = glGetAttribLocation(lw_program, "coord2d");
    if (lw_coord2d == -1)
    {
        fprintf(stderr, "test: Could not bind attribute : %s\n", "coord2d");
        return 0;
    }

    lw_norm = glGetAttribLocation(lw_program, "norm");
    if (lw_norm == -1)
    {
        fprintf(stderr, "test: Could not bind attribute : %s\n", "norm");
        return 0;
    }


    lw_linewidth = glGetUniformLocation(lw_program, "linewidth");
    if (lw_linewidth == -1)
    {
        fprintf(stderr, "test: Could not bind uniform : %s\n", "linewidth");
        return 0;
    }

    lw_z = glGetUniformLocation(lw_program, "z");
    if (lw_z == -1)
    {
        fprintf(stderr, "test: Could not bind uniform : %s\n", "z");
        return 0;
    }

    lw_color = glGetUniformLocation(lw_program, "color");
    if (lw_color == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "color");
        return 0;
    }
    lw_matrix = glGetUniformLocation(lw_program, "theMatrix");
    if (lw_matrix == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "theMatrix");
        return 0;
    }
    lw_px_matrix = glGetUniformLocation(lw_program, "px_Matrix");
    if (lw_px_matrix == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "lw_px_matrix");
        return 0;
    }



    reset_shaders(vs, fs, lw_program);

    
    


    /*create a shader program gps-point*/

    const unsigned char gen_vgps[1024] =  "attribute vec2 norm; \
uniform float radius; \
uniform vec2 coord2d;  \
uniform mat4 px_Matrix; \
uniform mat4 theMatrix; \
void main(void) {  \
vec4 delta = vec4(norm * radius,0,0); \
vec4 npos = px_Matrix * delta; \
vec4 pos = theMatrix * vec4(coord2d, 0, 1.0);  \
  gl_Position = (pos + npos); \
}";

    const unsigned char gen_fgps[1024] = "uniform vec4 color; \
void main(void) { \
  gl_FragColor = color; \
}";


    gps_program = create_program((unsigned char *) gen_vgps,(unsigned char *)  gen_fgps, &vs, &fs);

    if(gps_program == 0)
    {
     log_this(100,"problem compiling gps-program");
        return 0;
    }

    gps_norm = glGetAttribLocation(gps_program, "norm");
    if (gps_norm == -1)
    {
        fprintf(stderr, "test: Could not bind attribute : %s\n", "norm");
        return 0;
    }


    gps_coord2d = glGetUniformLocation(gps_program, "coord2d");
    if (gps_coord2d == -1)
    {
        fprintf(stderr, "test: Could not bind uniform : %s\n", "coord2d");
        return 0;
    }

     gps_radius = glGetUniformLocation(gps_program, "radius");
    if (gps_radius == -1)
    {
        fprintf(stderr, "test: Could not bind uniform : %s\n", "radius");
        return 0;
    }

 
    gps_color = glGetUniformLocation(gps_program, "color");
    if (gps_color == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "color");
        return 0;
    }
    gps_matrix = glGetUniformLocation(gps_program, "theMatrix");
    if (gps_matrix == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "theMatrix");
        return 0;
    }
    gps_px_matrix = glGetUniformLocation(gps_program, "px_Matrix");
    if (gps_px_matrix == -1)
    {
        fprintf(stderr, "Could not bind uniform : %s\n", "gps_px_matrix");
        return 0;
    }



    reset_shaders(vs, fs, gps_program);   
    
    return 0;
}







