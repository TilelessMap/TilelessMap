#include "theclient.h"
#include "interface/interface.h"
#include "info.h"
#include "matrix_handling.h"
#include "log.h"
#include "cleanup.h"
#include "event_loop.h"
#include "tilelessmap.h"

static SDL_Window* window;
static SDL_GLContext context;

extern int TLM_init(char *f, char *dir)
{
    log_this(10, "Entering function %s \n", __func__);
    
    text_scale=2;
   char projectfile[256];


    if (!(f))
    {
        log_this(110, "Too few arguments \n");
        return 1;
    }
    snprintf(projectfile, 500, "%s",f);
    
    
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    
    if(compiled.major != linked.major || compiled.minor!=linked.minor || compiled.patch!= linked.patch)
    {
        log_this(100,"We compiled against SDL version %d.%d.%d But we are linking against SDL version %d.%d.%d\n",
           compiled.major, compiled.minor, compiled.patch, linked.major, linked.minor, linked.patch);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    glDisable (GL_DEPTH_TEST);


    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

    SDL_Rect r;
    if (SDL_GetDisplayBounds(0, &r) != 0) {
        SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
        return 1;
    }

    CURR_WIDTH = r.w*0.9;
    CURR_HEIGHT = r.h*0.9;

#ifndef __ANDROID__
    window = SDL_CreateWindow("TilelessMap",
                                          0, 0, CURR_WIDTH, CURR_HEIGHT,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
#else

    window = SDL_CreateWindow("TilelessMap",
                                          0, 0, r.w, r.h,
                                          SDL_WINDOW_SHOWN  | SDL_WINDOW_OPENGL);
#endif


    if (window == NULL) {
        log_this(100, "Error: can't create window:  : %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        log_this(100, "Error: SDL_GL_CreateContext : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_GL_SetSwapInterval(1);
    
#ifndef __ANDROID__
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        fprintf(stderr, "Error: glewInit : %s", glewGetErrorString(glew_status) );
        return EXIT_FAILURE;
    }
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0");
        return EXIT_FAILURE;
    }
#endif


    /* Open db-connection*/
    sqlite3_initialize();

    int rc = sqlite3_open_v2(projectfile, &projectDB,SQLITE_OPEN_READWRITE, NULL );

    if (rc != SQLITE_OK) {
        log_this(100, "Cannot open database: %s\n", sqlite3_errmsg(projectDB));
        sqlite3_close(projectDB);
        return 1;
    }


    if (init_text_resources())
    {
        log_this(100,"Problems in init_text_resources");
        return EXIT_FAILURE;
    }

    check_screen_size();

    // destroy_txt_coords();

    if (init_resources(dir))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

extern void TLM_start(struct CTRL *controls)
{
    mainLoop(window, controls);
    
}

extern void TLM_close()
{
    free_resources(window, context);
}


extern CTRL* TLM_init_controls(int approach)
{
    CTRL *controls;
    if(approach == NATIVE_default || approach == NATIVE_custom)
    {
        controls = init_controls(); 
        
        if(approach == NATIVE_default)
            add_default_controls();
    }    
    return controls;
    
}
