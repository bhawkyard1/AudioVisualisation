#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"
#include "sampler.hpp"
#include "sim_time.hpp"

#define MUS_PATH "soviet_national_anthem_1.wav"

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

    sim_time timer(0.0f);
    bool done = false;
    timer.setCur();
    float start = 0.0f;
    float end = 0.0f;

    Mix_PlayChannel(-1, smpl.get(), 0);

    SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);

    while(!done)
    {
        timer.setCur();
        start = end;
        end = timer.getTime();

        if(timer.getTime() > smpl.getLenSecs())
            done = true;

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            default:
                break;
            }
        }

        SDL_SetRenderDrawColor(gren, 0, 0, 0, 255);
        SDL_RenderClear(gren);
        SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);

        std::vector<float> nums = smpl.sampleAudio(start, end);
        std::vector<SDL_Point> pts;
        pts.reserve( nums.size() );

        for(int i = 0; i < nums.size(); ++i)
        {
            SDL_Point pt;
            pt.x = ((float)i / nums.size()) * 1024;
            pt.y = nums[i] / 512.0f + 512;
            pts.push_back(pt);
        }

        SDL_Delay(10);

        SDL_RenderDrawLines(gren, &pts[0], pts.size());
        SDL_RenderPresent(gren);
    }

    std::cout << "Press enter to quit.\n";
    std::cin.get();

    Mix_CloseAudio();

    SDL_DestroyRenderer(gren);
    SDL_DestroyWindow(gwin);

    SDL_Quit();

    std::cout << "Complete!\n";

    return 0;
}
