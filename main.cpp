//
// Created by drake on 24/8/22.
//

//#include "Console/Console.hpp"
//#include "Base/Parser.hpp"
#include "SDL.h"
#include<iostream>
#include<vector>

void init(SDL_Window *&window) {
    int flags = SDL_INIT_VIDEO, screen_width = 160, screen_height = 144;
    if (SDL_Init(flags) < 0) { std::cout << "Init\n" << SDL_GetError() << "\n"; }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width,
                              screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr) { std::cout << "Window" << SDL_GetError() << "\n"; }

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);
}

void run(SDL_Window *&window) {
    SDL_Event e;
    bool open = true;
    int channels = 3;
    int native_width = 160, native_height = 144;

    char *pixels = new char[160 * 144 * channels];
    for (int i = 0; i < 160 * 144 * 3; i++)
        pixels[i] = 0;

    for (int i = 0; i < 160 * 3 * 10; i++) {
        pixels[i] = (char) (0xFF * ((i % 3) != 2));
    }
    for (int i = 160 * 3 * 10; i < 160 * 3 * 40; i++) {
        pixels[i] = (char) (0xFF * ((i % 3) != 1));
    }
    for (int i = 160 * 3 * 40; i < 160 * 3 * 100; i++) {
        pixels[i] = (char) (0xFF * ((i % 3) != 0));
    }
    SDL_Surface *native_surface = SDL_CreateRGBSurfaceFrom((void *) pixels,
                                                           native_width,
                                                           native_height,
                                                           channels * 8,
                                                           native_width * channels,
                                                           0x0000FF,
                                                           0x00FF00,
                                                           0xFF0000,
                                                           0);

    SDL_BlitSurface(native_surface, nullptr, SDL_GetWindowSurface(window), nullptr);

    while (open) {
        SDL_Surface *screen = SDL_GetWindowSurface(window);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) open = false;
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {

                    SDL_Log("Window %d resized to %dx%d",
                            e.window.windowID, e.window.data1,
                            e.window.data2);

                    SDL_BlitScaled(native_surface, nullptr, SDL_GetWindowSurface(window), nullptr);
                    SDL_UpdateWindowSurface(window);

                }
            }
        }

        SDL_UpdateWindowSurface(window);
    }
}

void free(SDL_Window *&window) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main([[maybe_unused]] int argv, char **argc) {
    /*Console x;
    std::string name = "main.cpp";
    std::string path(argc[1]);
    auto data = read_file(path);*/

    SDL_Window *window = nullptr;

    init(window);
    run(window);
    free(window);

    return 0;
}

