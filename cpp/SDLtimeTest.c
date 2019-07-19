#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <time.h>
#include <unistd.h>

#define WIDTH 740
#define HEIGHT 280
#define IMG_PATH "../xkcd1908.png"


// adapted from https://gist.github.com/armornick/3434362

// to compile : gcc SDLtimeTest.c -lSDL2 -lSDL2_image

// This example desmontrates that taking measures of SDF rendering is quite tedious:
// SDL execution time seems to be adapted to the frame input rate.

// image to put at root: https://xkcd.com/1908/


int main (int argc, char *argv[]) {

  // variable declarations
  SDL_Window *win = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture *img = NULL;
  int w, h; // texture width & height
	
  // Initialize SDL.
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    return 1;
	
  // create the window and renderer
  // note that the renderer is accelerated
  win = SDL_CreateWindow("Image Loading", 0, 0, WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); //|SDL_RENDERER_PRESENTVSYNC
	
  // load our image
  img = IMG_LoadTexture(renderer, IMG_PATH);
  SDL_QueryTexture(img, NULL, NULL, &w, &h); // get the width and height of the texture
  SDL_Rect texr; texr.x = 0; texr.y = 0; texr.w = w; texr.h = h; 
	
  // main loop
  while (1) {

    long start = clock();
	  
    // event handling
    SDL_Event e;
    if ( SDL_PollEvent(&e) ) {
      if (e.type == SDL_QUIT)
	break;
      else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
	break;
    } 
		
    // clear the screen
    SDL_RenderClear(renderer);
    // copy the texture to the rendering context
    SDL_RenderCopy(renderer, img, NULL, &texr);
    // flip the backbuffer
    // this means that everything that we prepared behind the screens is actually shown
    SDL_RenderPresent(renderer);

    long stop = clock();
    fprintf(stderr, "Rendering time (µs): %ld\n", stop-start);
    fflush(stderr);

    /* usleep(500000);		 */
    /* for (int i = 0; i < 1e9; i++) { */
    /*   long a = 2*i; */
    /* } */
  }
	
  SDL_DestroyTexture(img);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
	
  return 0;
}
