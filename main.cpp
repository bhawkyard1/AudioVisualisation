#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"
#include "sampler.hpp"

#define MUS_PATH "440hz.wav"

SDL_Window * gwin;
SDL_Renderer * gren;

int main(int argc, char * argv[])
{
    std::cout << "Boo\n";
    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL Init Error!\n" << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    gwin = SDL_CreateWindow("vis", 512, 100, 1024, 1024, SDL_WINDOW_RESIZABLE);
    gren = SDL_CreateRenderer(gwin, -1, NULL);
    SDL_ShowWindow(gwin);
    SDL_SetRenderDrawColor(gren, 0, 0, 0, 255);
    SDL_RenderClear(gren);
    SDL_RenderPresent(gren);

    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    sampler::initialiseAudio( 44100, 2 );

    sampler smpl (MUS_PATH);

    std::vector<float> nums;
    for(float i = 0.0f; i < smpl.getLenSecs() - 0.1f; i += 0.1f)
    {
        SDL_SetRenderDrawColor(gren, 0, 0, 0, 255);
        SDL_RenderClear(gren);
        SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);

        int px = 0;
        int py = 0;
        int cx = 0;
        int cy = 0;

        nums = smpl.sampleAudio(i, i + 0.1f);
        for(int j = 0; j < nums.size(); ++j)
        {
            cx = (float)j / nums.size() * 1024.0f;
            cy = nums[j] / 512.0f + 512.0f;
            SDL_RenderDrawLine(gren, px, py, cx, cy);
            px = cx;
            py = cy;
        }
        SDL_RenderPresent(gren);
    }

    Mix_PlayChannel(-1, smpl.get(), 0);

    std::cout << "Press enter to quit.\n";
    std::cin.get();

    Mix_CloseAudio();

    SDL_DestroyRenderer(gren);
    SDL_DestroyWindow(gwin);

    SDL_Quit();

    std::cout << "Complete!\n";

    return 0;
}
