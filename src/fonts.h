
#ifndef _fonts_H
#define _fonts_H
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif



#include <ft2build.h>


#include FT_FREETYPE_H

#define NORMAL_TYPE 1
#define BOLD_TYPE 2
#define ITALIC_TYPE 3

#define MAX_FONT_SIZE 127

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
 int fonttype;
 //FONTSIZES *fss;
 ATLAS **a;
 size_t max_size;
} FONT;

typedef struct
{
        FONT *fonts;
        size_t nfonts;
}FONTS;



FONT* init_font(const char* fontname);
int destroy_font(FONTS *fonts);
FONT **fonts;

FONTS *fnts;

ATLAS *font_normal[3];

ATLAS *font_bold[3];
ATLAS* loadatlas(const char* fontname,int fonttype, int size);

#endif
