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
extern char* file_read(const char* filename, int* size);

extern GLint get_attrib(GLuint text_program, const char *name);
extern GLint get_uniform(GLuint text_program, const char *name);
extern void print_opengl_info();


#define MAXWIDTH 1024




#define max(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a > _b ? _a : _b; })

/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL.  Using SDL_RWops for Android asset support.
 */
char* file_read(const char* filename, int* size) {
	SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) return NULL;
	
	Sint64 res_size = SDL_RWsize(rw);
	char* res = (char*)malloc(res_size + 1);

	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return NULL;
	}
	
	res[nb_read_total] = '\0';
	if (size != NULL)
		*size = nb_read_total;
	return res;
}


GLint get_attrib(GLuint text_program, const char *name) {
	GLint attribute = glGetAttribLocation(text_program, name);
	if(attribute == -1)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Could not bind attribute %s", name);
	return attribute;
}

GLint get_uniform(GLuint text_program, const char *name) {
	GLint uniform = glGetUniformLocation(text_program, name);
	if(uniform == -1)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Could not bind uniform %s", name);
	return uniform;
}

void print_opengl_info() {
	int major, minor, profile;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
	const char* profile_str = "";
	if (profile & SDL_GL_CONTEXT_PROFILE_CORE)
		profile_str = "CORE";
	if (profile & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
		profile_str = "COMPATIBILITY";
	if (profile & SDL_GL_CONTEXT_PROFILE_ES)
		profile_str = "ES";

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
				   "OpenGL %d.%d %s", major, minor, profile_str);
}






int init_text_resources(char *dir) 
{
    char *sql_txt;
    int rc;
    sqlite3_stmt *preparedFonts;
    fontfilename = "FreeSans.ttf";
    //char *fontfilename = "LiberationSerif-Regular.ttf";
    FT_Face face;
    int len;
    char *font_data;
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library");
		return 1;
	}

	/* Load a font */
	int fontsize;
	
   // font_data = file_read(fontfilename, &len);
	
    sql_txt = "select name, font from main.fonts where \"name\" = 'libserif';";
        rc = sqlite3_prepare_v2(projectDB, sql_txt, -1, &preparedFonts, 0);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error in %s\n",sql_txt );
        sqlite3_close(projectDB);
        return 1;
    }
    sqlite3_step(preparedFonts);
   // int nStyles = sqlite3_column_int(preparedCountStyle, 0);
    
    printf("namn = %s\n",sqlite3_column_text(preparedFonts, 0)); 
    len = sqlite3_column_bytes(preparedFonts, 1);
    font_data = malloc(len);
    memcpy(font_data, sqlite3_column_blob(preparedFonts, 1), len);
    
    
    if (font_data == NULL) {
		
		fprintf(stderr, "Could not load font file ");
		return 1;
	}
	FT_Error fterr = FT_New_Memory_Face(ft, (FT_Byte*)font_data, len, 0, &face);
	if (fterr != FT_Err_Ok) {
		fprintf(stderr, "Could not init font: error ");
		return 1;
	}



	// Create the vertex buffer object
	glGenBuffers(1, &text_vbo);
    
    	
	atlases[0] = malloc(sizeof(ATLAS));
	atlases[1] = malloc(sizeof(ATLAS));
	atlases[2] = malloc(sizeof(ATLAS));
    
    
    create_atlas(atlases[0], face, 12);
    create_atlas(atlases[1], face, 16);
    create_atlas(atlases[2], face, 24);
    sqlite3_finalize(preparedFonts);
	return 0;
}


uint32_t utf82unicode(const char *text,const char **the_rest)
{
	uint32_t res = 0;
	uint8_t nbytes=0;
	uint8_t c;
	int i =0;
	
	c = text[0];
	if(c==0)
	{
		return 0;
	}
	else if(!(c & 128))
	{
		res = (uint32_t) c;
		*(the_rest) = text+1;
		return res;
	}

	while((c<<++nbytes) & 128)
	{};
		
	*(the_rest) = text+nbytes;	
	
	res = ((c<<nbytes) & 0xff)>>nbytes;
		
	for (i=1;i<nbytes;i++)	
	{
		c = text[i];
		if(c==0)
		{
			printf("Something went wrong, UTF is invalid\n");
			return 0;
		}
		res = (res<<6) | (c & 63);	
	}
	return res;	
}




	ATLAS* create_atlas(ATLAS *a, FT_Face face, int height) 
	{
	FT_Set_Pixel_Sizes(face, 0, height);
	FT_GlyphSlot g = face->glyph;
    int i;
	unsigned int roww = 0;
	unsigned int rowh = 0;
	 a->w = 0;
	 a->h = 0;

	 memset(a->metrics, 0, sizeof(a->metrics));

	/* Find minimum size for a texture holding all visible ASCII characters */
	for (i = 32; i < 256; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			fprintf(stderr, "Loading character %c failed!\n", i);
			continue;
		}
		if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
			a->w = max(a->w, roww);
			a->h += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += g->bitmap.width + 1;
		rowh = max(rowh, g->bitmap.rows);
	}

	a->w = max(a->w, roww);
	a->h += rowh;

	/* Create a texture that will be used to hold all ASCII glyphs */
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &(a->tex));
	glBindTexture(GL_TEXTURE_2D, a->tex);
	glUniform1i(text_uniform_tex, 0);

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

	//     glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		a->metrics[i].ax = g->advance.x >> 6;
		a->metrics[i].ay = g->advance.y >> 6;

		a->metrics[i].bw = g->bitmap.width;
		a->metrics[i].bh = g->bitmap.rows;

		a->metrics[i].bl = g->bitmap_left;
		a->metrics[i].bt = g->bitmap_top;

		a->metrics[i].tx = ox / (float)a->w;
		a->metrics[i].ty = oy / (float)a->h;

		rowh = max(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;
	}

	fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", a->w, a->h, a->w * a->h / 1024);
	return a;
	}
