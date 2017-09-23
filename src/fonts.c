
/**********************************************************************
 *
 * TileLess
 *
 * TileLess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * TileLess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLess.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2017 Nicklas Avén
 *
 **********************************************************************/
#include "theclient.h"
#include "mem.h"


#define MAXWIDTH 1024



static inline uint32_t max_i(int a, int b)
{
    if (b > a)
        return b;
    else
        return a;
}

int destroy_atlas(ATLAS *a)
{
    if(!a)
        return 0;
    glDeleteTextures(1, &(a->tex));
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr,"FONT - opengl error:%d\n", err);
    }
    free(a);
    a=NULL;
    return 0;
}


int destroy_font(FONTS *fonts)
{

    unsigned int i, z;

    for (i=0; i<fonts->nfonts; i++)
    {
        FONT *f = fonts->fonts + i;
        for (z=0; z<f->max_size; z++)
        {
            if(f->a[z])
            {
                destroy_atlas(f->a[z]);
            }
        }
        free(f->a);
        f->a=NULL;
        free(f->fontname);
        f->fontname = NULL;
    }

    st_free(fonts->fonts);
    st_free(fnts);
    return 0;
}


static ATLAS* create_atlas(FT_Face face, int height)
{
    FT_Set_Pixel_Sizes(face, 0, height);
    FT_GlyphSlot g = face->glyph;


    int i;
    unsigned int roww = 0;
    unsigned int rowh = 0;

    ATLAS *a = st_malloc(sizeof(ATLAS));
    a->w = 0;
    a->h = 0;

    memset(a->metrics, 0, sizeof(a->metrics));

    /* Find minimum size for a texture holding all visible ASCII characters */
    for (i = 32; i < 256; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        if (roww + g->bitmap.width + 1 >= MAXWIDTH)
        {
            a->w = max_i(a->w, roww);
            a->h += rowh;
            roww = 0;
            rowh = 0;
        }
        roww += g->bitmap.width + 1;
        rowh = max_i(rowh, g->bitmap.rows);
    }

    a->w = max_i(a->w, roww);
    a->h += rowh;
    a->ch = rowh;
    /* Create a texture that will be used to hold all ASCII glyphs */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &(a->tex));
    glBindTexture(GL_TEXTURE_2D, a->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, a->w, a->h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Paste all glyph bitmaps into the texture, remembering the offset */
    int ox = 0;
    int oy = 0;

    rowh = 0;

    for (i = 32; i < 256; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }

        if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
            oy += rowh;
            rowh = 0;
            ox = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        while ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr,"FONT - opengl error:%d\n", err);
        }
        //     glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        a->metrics[i].ax = (float) (g->advance.x >> 6);
        a->metrics[i].ay = (float) (g->advance.y >> 6);

        a->metrics[i].bw = (float) (1.0 * g->bitmap.width);
        a->metrics[i].bh = (float)(1.0 * g->bitmap.rows);

        a->metrics[i].bl = (float)(g->bitmap_left);
        a->metrics[i].bt = (float)(g->bitmap_top);

        a->metrics[i].tx = ox / (float)a->w;
        a->metrics[i].ty = oy / (float)a->h;

        rowh = max_i(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }


    fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", a->w, a->h, a->w * a->h / 1024);
    return a;
}


int init_text_resources()
{
    log_this(10, "Entering %s\n",__func__);



    fnts = st_malloc(sizeof(FONTS));
    fnts->nfonts = 0;
    fnts->fonts = NULL;




    /* Initialize the FreeType2 library */
    if (FT_Init_FreeType(&ft)) {
        log_this(100,"Could not init freetype library");
        return 1;
    }


    init_txt_coords();



    return 0;
}

static FONT* check_font(const char *fontname, int fonttype, int size)
{

    if(size > MAX_FONT_SIZE)
    {
        log_this(100,"TilelessMap doesn't support larger fonts than %d. Font size %d will be used\n",MAX_FONT_SIZE,MAX_FONT_SIZE);
        size = MAX_FONT_SIZE;
    }
    unsigned int i;
    FONT *font = NULL;
    for (i=0; i<fnts->nfonts; i++)
    {
        if(!strcmp(fontname, fnts->fonts[i].fontname) && fonttype == fnts->fonts[i].fonttype)
        {
            font = fnts->fonts+i;
            break;
        }
    }
    if(!font)
    {
        int new_nfonts = fnts->nfonts + 1;
        if(fnts->nfonts>0)
            fnts->fonts = st_realloc(fnts->fonts,sizeof(FONT) * new_nfonts );
        else
        {
            fnts->fonts = st_malloc(sizeof(FONT) * new_nfonts );
        }
        font = fnts->fonts + fnts->nfonts;
        fnts->nfonts=new_nfonts;
        font->max_size = 0;
        font->fontname = st_malloc(strlen(fontname)+1);
        strcpy(font->fontname, fontname);
        font->fonttype = fonttype;
        font->a = st_calloc((MAX_FONT_SIZE + 1),sizeof(ATLAS*));
        font->max_size = MAX_FONT_SIZE;
    }




    return font;
}


ATLAS* loadatlas(const char* fontname,int fonttype, int size)
{
    if(size > MAX_FONT_SIZE)
    {
        log_this(100,"TilelessMap doesn't support larger fonts than %d. Font size %d will be used\n",MAX_FONT_SIZE,MAX_FONT_SIZE);
        size = MAX_FONT_SIZE;
    }
    log_this(10, "Entering %s\n",__func__);
    char *sql_txt;
    int rc;
    sqlite3_stmt *preparedFonts;
    FT_Face face;

    FONT *f;


    int len;
    char *font_data;
    FT_Error fterr;

    if(!size)
        size = 12;
    if(!fonttype)
        fonttype = 1;


    /*By doing this ordering the first result will be right font name if exists,
     * right type if exists for that font name or the one with highest priority
     * (default style with right type) This means right type gets prioritized before font type
     * This could be discussed if it is right*/
    sql_txt = "select name,type, font from main.fonts order by \"name\" = ? desc ,type = ? desc, prio;";
    rc = sqlite3_prepare_v2(projectDB, sql_txt, -1, &preparedFonts, 0);

    if (rc != SQLITE_OK ) {
        log_this(100,"SQL error in %s\n",sql_txt );
        sqlite3_close(projectDB);
        return NULL;
    }
    if(fontname)
        sqlite3_bind_text(preparedFonts,1,fontname,-1,NULL);
    else
        sqlite3_bind_text(preparedFonts,1,"default",-1,NULL);//just a dummy name that will result in default set in db

    sqlite3_bind_int(preparedFonts,2,fonttype);
    if(sqlite3_step(preparedFonts)!= SQLITE_ROW)
    {
        log_this(100, "Problem loading font!");

    }
    // int nStyles = sqlite3_column_int(preparedCountStyle, 0);

    log_this(10,"namn = %s\n",sqlite3_column_text(preparedFonts, 0));
    const unsigned char *res_fontname = sqlite3_column_text(preparedFonts, 0);
    int res_type=sqlite3_column_int(preparedFonts, 1);

    len = sqlite3_column_bytes(preparedFonts, 2);


    font_data = malloc(len);
    memcpy(font_data, sqlite3_column_blob(preparedFonts, 2), len);


    if (font_data == NULL) {
        log_this(100,"Could not load font data ");
        return NULL;
    }
    fterr = FT_New_Memory_Face(ft, (FT_Byte*)font_data, len, 0, &face);
    if (fterr != FT_Err_Ok) {
        log_this(100,"Could not init font: error ");
        return NULL;
    }

    log_this(10,"font name = %s\n", face->style_name);
    f = check_font((const char*) res_fontname, res_type, size);
    if(!f->a[size])
        f->a[size] = create_atlas(face, size);

    FT_Done_Face(face);
    log_this(10,"-------------------returning font %s,typ %d, size %d, pointer %p\n", res_fontname, res_type,size, f->a[size]);
    sqlite3_clear_bindings(preparedFonts);
    sqlite3_reset(preparedFonts);

    sqlite3_finalize(preparedFonts);
    free(font_data);
    return f->a[size];

}
