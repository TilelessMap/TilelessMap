

#ifndef _cleanup_H
#define _cleanup_H

void free_resources(SDL_Window* window,SDL_GLContext context);



void destroy_layer_runtime(LAYER_RUNTIME *lr, int n);
void destroy_layers(LAYERS *l);


#endif
