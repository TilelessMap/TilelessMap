

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif



#include <ft2build.h>


#include FT_FREETYPE_H

typedef struct {
    float ax;	// advance.x
    float ay;	// advance.y

    float bw;	// bitmap.width;
    float bh;	// bitmap.height;

    float bl;	// bitmap_left;
    float bt;	// bitmap_top;

    float tx;	// x offset of glyph in texture coordinates
    float ty;	// y offset of glyph in texture coordinates
} C;		// character information

typedef struct   {
    GLuint tex;		// texture object

    unsigned int w;			// width of texture in pixels
    unsigned int h;			// height of texture in pixels
    unsigned int ch;			// max_character_height
    C metrics[256];
} ATLAS;





FT_Library ft;


typedef struct
{
 ATLAS *normal;
 ATLAS *bold; 
} FONTSIZE;

typedef struct
{
    FONTSIZE *fs;
    int max_size;
} FONTSIZES;

typedef struct
{
 char *fontname;
 FONTSIZES *fss;
} FONT;

FONT* init_font(const char* fontname);
int destroy_font(FONT *f);
FONT **fonts;

ATLAS *font_normal[3];

ATLAS *font_bold[3];
