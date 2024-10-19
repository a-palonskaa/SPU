#include <assert.h>
#include "drow.h"
#include "logger.h"

//====================================================================================================

static void fill_colors(SDL_Color* colors, double* ram);
static SDL_Color get_color_from_double(double color_bytes);

//====================================================================================================

bool SDL_Ctor(SDL_Window** window, SDL_Renderer** renderer) {
    assert(window != nullptr);
    assert(renderer != nullptr);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG(ERROR, "Initialization error SDL: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Operation Memory",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        LOG(ERROR, "Failed to open a window %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

   *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        LOG(ERROR, "Failed to create a renderer %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }
    return 0;
}

//====================================================================================================

bool drow(double* ram) {
    assert(ram != nullptr);

    for (size_t i = 0; i < 10; i++) {
        for (size_t j = 0; j < 10; j++) {
            printf("%.0f ", *(ram + 10 * i + j));
        }
        printf("\n");
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Ctor(&window, &renderer)) {
        LOG(ERROR, "Failer to create visualization\n");
        return 1;
    }

    SDL_Color colors[100] = {};
    fill_colors(colors, ram);

    SDL_Event e;
    int quit_flag = 0;

    while (!quit_flag) {

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit_flag = 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (size_t i = 0; i < 10; i++) {
            for (size_t j = 0; j < 10; j++) {
                SDL_Rect square = {};
                square.x = (int) (250 + j * 30);
                square.y = (int) (150 + i * 30);
                square.w = 20;
                square.h = 20;

                SDL_SetRenderDrawColor(renderer, colors[i * 10 + j].r, colors[i * 10 + j].g,
                                                 colors[i * 10 + j].b, colors[i * 10 + j].a);
                SDL_RenderFillRect(renderer, &square);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

//====================================================================================================

static void fill_colors(SDL_Color* colors, double* ram) {
    assert(colors != nullptr);
    assert(ram != nullptr);

    for (size_t i = 0; i < 100; i++) {
        colors[i] = get_color_from_double(ram[i]);
    }
}

//можно первый байт - alpfa, second - red, third- gree, 4th- blue
//r — красный компонент (от 0 до 255)
// g — зеленый компонент (от 0 до 255)
// b — синий компонент (от 0 до 255)
// a — альфа-канал (прозрачность, от 0 до 255, где 0 — полностью прозрачный, а 255 — полностью непрозрачный)

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
