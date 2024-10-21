#include <assert.h>
#include "drow.h"
#include "logger.h"

//====================================================================================================

const size_t X_SIZE = 100;
const size_t Y_SIZE = 100;

//====================================================================================================

static void fill_colors(SDL_Color* colors, double* ram);
static SDL_Color get_color_from_double(double color_bytes);

//====================================================================================================

bool SDL_Ctor(SDL_Window** window, SDL_Renderer** renderer) {
    assert(window != nullptr);
    assert(renderer != nullptr);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG(ERROR, "Initialization error SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    *window = SDL_CreateWindow("Operation Memory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                                   800, 800, SDL_WINDOW_SHOWN);

    if (*window == nullptr) {
        LOG(ERROR, "Failed to open a window %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

   *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        LOG(ERROR, "Failed to create a renderer %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//====================================================================================================

bool drow(double* ram) {
    assert(ram != nullptr);

#ifdef DEBUG
    for (size_t i = 0; i < Y_SIZE; i++) {
        for (size_t j = 0; j < X_SIZE; j++) {
            printf("%.0f ", *(ram + X_SIZE * i + j));
        }
        printf("\n");
    }
#endif /* DEBUG */

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Ctor(&window, &renderer)) {
        LOG(ERROR, "Failer to create visualization\n");
        return EXIT_FAILURE;
    }

    SDL_Color* colors = (SDL_Color*) calloc(X_SIZE * Y_SIZE, sizeof(SDL_Color));
    fill_colors(colors, ram);

    SDL_Event e = {};
    int quit_flag = 0;
    time_t start_time = time(NULL);
    time_t end_time = start_time + 5;

    while (!quit_flag && (time(NULL) < end_time)) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit_flag = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (size_t i = 0; i < Y_SIZE; i++) {
            for (size_t j = 0; j < X_SIZE; j++) {
                SDL_Rect square = {};
                square.x = (int) (150 + j * 4);
                square.y = (int) (150 + i * 4);
                square.w = 4;
                square.h = 4;

                SDL_SetRenderDrawColor(renderer, colors[i * X_SIZE + j].r, colors[i * X_SIZE + j].g,
                                                 colors[i * X_SIZE + j].b, colors[i * X_SIZE + j].a);
                SDL_RenderFillRect(renderer, &square);
            }
        }

        SDL_RenderPresent(renderer);
    }

    free(colors);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

//====================================================================================================

static void fill_colors(SDL_Color* colors, double* ram) {
    assert(colors != nullptr);
    assert(ram != nullptr);

    for (size_t i = 0; i < X_SIZE * Y_SIZE; i++) {
        colors[i] = get_color_from_double(ram[i]);
    }
}

//first byte - alpfa, second - red, third- gree, 4th- blue (от 0 до 255)
static SDL_Color get_color_from_double(double color_bytes) {
    uint32_t color = (uint32_t) color_bytes;

    SDL_Color c = {};
    c.a = (color >> 24) & 0xFF;
    c.r = (color >> 16) & 0xFF;
    c.g = (color >> 8) & 0xFF;
    c.b = color & 0xFF;

    return c;
}

//====================================================================================================

// TODO: Bad Apple
