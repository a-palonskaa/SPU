#ifndef DROW_H
#define DROW_H

#include <SDL2/SDL.h>
#include <stdio.h>

bool SDL_Ctor(SDL_Window** window, SDL_Renderer** renderer);
bool drow(double* ram);

#endif /* DROW_H */
