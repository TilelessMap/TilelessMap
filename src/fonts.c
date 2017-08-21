
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

#define MAX_FONT_SIZE 60


static inline uint32_t max_i(int a, int b)
{
    if (b > a)
        return b;
    else
        return a;
}




FONT* init_font(const char* fontname)
{
    
    FONT *f = st_malloc(sizeof(FONT));
    f->fontname = st_malloc(strlen(fontname)+1);
    strcpy(f->fontname,fontname);
    
    f->fss = st_malloc(sizeof(FONTSIZES));
    f->fss->fs = st_calloc(MAX_FONT_SIZE, sizeof(FONTSIZE));
    f->fss->max_size = MAX_FONT_SIZE;   
    return f;
}

int destroy_font(FONT *f)
{
 
    int i;
    
    for (i=0;i<f->fss->max_size;i++)
    {
   //     log_this(100, "delete fs nr %d of %d\n", i,f->fss->max_size);
        FONTSIZE fs = f->fss->fs[i];
        
        if(fs.normal)
        {
            free(fs.normal);
            fs.normal = NULL;
        }
        if(fs.bold)
        {
            free(fs.bold);      
            fs.bold = NULL;
        }
    }
    
    free(f->fss->fs);
    f->fss->fs = NULL;
    free(f->fss);
    f->fss = NULL;
    free(f);
    f = NULL;
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
    char *sql_txt;
    int rc;
    sqlite3_stmt *preparedFonts;
    fontfilename = "FreeSans.ttf";
    //char *fontfilename = "LiberationSerif-Regular.ttf";
    FT_Face face;
    int len;
    char *font_data;
    FT_Error fterr;

    
    fonts = st_malloc(sizeof(FONT*));
    
    fonts[0] = init_font(fontfilename);



    /* Initialize the FreeType2 library */
    if (FT_Init_FreeType(&ft)) {
        log_this(100,"Could not init freetype library");
        return 1;
    }

    /* Load a font */

    // font_data = file_read(fontfilename, &len);

    sql_txt = "select name, font from main.fonts where \"name\" = ?;";
    rc = sqlite3_prepare_v2(projectDB, sql_txt, -1, &preparedFonts, 0);

    if (rc != SQLITE_OK ) {
        log_this(100,"SQL error in %s\n",sql_txt );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_bind_text(preparedFonts,1,"freesans",-1,NULL);
    sqlite3_step(preparedFonts);
    // int nStyles = sqlite3_column_int(preparedCountStyle, 0);

    log_this(10,"namn = %s\n",sqlite3_column_text(preparedFonts, 0));
    len = sqlite3_column_bytes(preparedFonts, 1);
    font_data = malloc(len);
    memcpy(font_data, sqlite3_column_blob(preparedFonts, 1), len);


    if (font_data == NULL) {
        log_this(100,"Could not load font file ");
        return 1;
    }
    fterr = FT_New_Memory_Face(ft, (FT_Byte*)font_data, len, 0, &face);
    if (fterr != FT_Err_Ok) {
        log_this(100,"Could not init font: error ");
        return 1;
    }

    log_this(10,"font name = %s\n", face->style_name);

    // Create the vertex buffer object
    //  glGenBuffers(1, &text_vbo);


/*    font_normal[0] = malloc(sizeof(ATLAS));
    font_normal[1] = malloc(sizeof(ATLAS));
    font_normal[2] = malloc(sizeof(ATLAS));
*/

    FONTSIZE *fs = fonts[0]->fss->fs;

    (fs+1)->normal = create_atlas(face, 20);
    (fs+2)->normal = create_atlas(face, 26);
    (fs+3)->normal = create_atlas(face, 32);
    (fs+4)->normal = create_atlas(face, 46);

    sqlite3_clear_bindings(preparedFonts);
    sqlite3_reset(preparedFonts);
    free(font_data);

    sqlite3_bind_text(preparedFonts,1,"freesans_bold",-1,NULL);
    sqlite3_step(preparedFonts);
    // int nStyles = sqlite3_column_int(preparedCountStyle, 0);

    log_this(10,"namn = %s\n",sqlite3_column_text(preparedFonts, 0));
    len = sqlite3_column_bytes(preparedFonts, 1);
    font_data = malloc(len);
    memcpy(font_data, sqlite3_column_blob(preparedFonts, 1), len);


    if (font_data == NULL) {
        log_this(100,"Could not load font file ");
        return 1;
    }
    fterr = FT_New_Memory_Face(ft, (FT_Byte*)font_data, len, 0, &face);
    if (fterr != FT_Err_Ok) {
        log_this(100,"Could not init font: error ");
        return 1;
    }

    log_this(10,"font name = %s\n", face->style_name);

    // Create the vertex buffer object
    //  glGenBuffers(1, &text_vbo);

/*
    font_bold[0] = st_malloc(sizeof(ATLAS));
    font_bold[1] = st_malloc(sizeof(ATLAS));
    font_bold[2] = st_malloc(sizeof(ATLAS));


    create_atlas(font_bold[0], face, 20);
    create_atlas(font_bold[1], face, 26);
    create_atlas(font_bold[2], face, 32);*/

    (fs+1)->bold = create_atlas(face, 20);
    (fs+2)->bold= create_atlas(face, 26);
    (fs+3)->bold = create_atlas(face, 32);
    (fs+4)->bold = create_atlas(face, 46);
    
    init_txt_coords();
    
    
    sqlite3_clear_bindings(preparedFonts);
    sqlite3_reset(preparedFonts);

    sqlite3_finalize(preparedFonts);
    free(font_data);
    
    return 0;
}




