
#include "theclient.h"
extern char* file_read(const char* filename, int* size);

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



	// Create the vertex buffer object
	glGenBuffers(1, &text_vbo);

	return 0;
}

