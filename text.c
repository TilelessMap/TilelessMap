
#include "theclient.h"
extern char* file_read(const char* filename, int* size);
extern GLuint text_create_shader(const char* filename, GLenum type);
extern GLuint create_program(const char* vertexfile, const char *fragmentfile);
//extern GLuint create_gs_program(const char* vertexfile, const char *geometryfile, const char *fragmentfile, GLint input, GLint output, GLint vertices);
extern GLint get_attrib(GLuint text_program, const char *name);
extern GLint get_uniform(GLuint text_program, const char *name);
extern void print_opengl_info();


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


/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint text_create_shader(const char* filename, GLenum type) {
	const GLchar* source = file_read(filename, NULL);
	if (source == NULL) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Error opening %s: %s", filename, SDL_GetError());
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
	free((void*)source);
	
	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s:\n", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}
	
	return res;
}

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






int init_text_resources() 
{
    fontfilename = "FreeSans.ttf";
    
FT_Face face;
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&ft)) {
		fprintf(stderr, "Could not init freetype library");
		return 1;
	}

	/* Load a font */
	int fontsize;
	char* font = file_read(fontfilename, &fontsize);
	if (font == NULL) {
		
		fprintf(stderr, "Could not load font file ");
		return 1;
	}
	FT_Error fterr = FT_New_Memory_Face(ft, (FT_Byte*)font, fontsize, 0, &face);
	if (fterr != FT_Err_Ok) {
		fprintf(stderr, "Could not init font: error ");
		return 1;
	}

	text_program = create_program("text.v.glsl", "text.f.glsl");
	if(text_program == 0)
		return 1;

	text_attribute_coord = get_attrib(text_program, "coord");
	text_uniform_tex = get_uniform(text_program, "tex");
	text_uniform_color = get_uniform(text_program, "color");

	if(text_attribute_coord == -1 || text_uniform_tex == -1 || text_uniform_color == -1)
		return 1;

	// Create the vertex buffer object
	glGenBuffers(1, &text_vbo);

    
    
    create_atlas(atlases[0], face, 12);
    create_atlas(atlases[1], face, 24);
    create_atlas(atlases[2], face, 48);
    
    
	return 0;
}



void render_txt(SDL_Window* window) 
{
    
	float sx = 2.0 / CURR_WIDTH;
	float sy = 2.0 / CURR_HEIGHT;

	glUseProgram(text_program);

	/* White background */


	GLfloat black[4] = { 0, 0, 0, 1 };
	GLfloat red[4] = { 1, 0, 0, 1 };
	GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

	/* Set font size to 48 pixels, color to black */
	//FT_Set_Pixel_Sizes(face, 0, 48);
	
	glUniform4fv(text_uniform_color, 1, red);
	FT_Set_Pixel_Sizes(face, 0, 7);
	render_text_test("The Tiny Font Sized Fox Jumps Over The Lazy Dog", -1 + 8 * sx, 1 - 250 * sy, sx, sy);

	//SDL_GL_SwapWindow(window);
}



	
	 ATLAS* create_atlas(ATLAS *a, FT_Face face, int height) 
     {
		FT_Set_Pixel_Sizes(face, 0, height);
		FT_GlyphSlot g = face->glyph;

		unsigned int roww = 0;
		unsigned int rowh = 0;
		 a->w = 0;
		 a->h = 0;

		 memset(a->metrics, 0, sizeof(a->metrics));

		/* Find minimum size for a texture holding all visible ASCII characters */
		for (int i = 32; i < 256; i++) {
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
		glUniform1i(uniform_tex, 0);

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

		for (int i = 32; i < 256; i++) {
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
