#include <iostream>
#include <algorithm>
#include <numeric>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cmath>
#include "fft.hpp"
#include "sampler.hpp"
#include "sim_time.hpp"
#include "util.hpp"

//#define MUS_PATH "tone_2.wav"
//#define MUS_PATH "soviet_national_anthem_0.wav"
#define MUS_PATH "je_viens_de_la.wav"

SDL_Window * gwin;
SDL_Renderer * gren;

int g_WIN_WIDTH = 0;
int g_WIN_HEIGHT = 0;

typedef std::vector<SDL_Point> Line;

int main(int argc, char * argv[])
{
    std::cout << "Boo\n";
    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << "SDL Init Error!\n" << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    getLargestWinDim(&g_WIN_WIDTH, &g_WIN_HEIGHT);

		gwin = SDL_CreateWindow("vis", 4, 32, g_WIN_WIDTH, g_WIN_HEIGHT, SDL_WINDOW_RESIZABLE);
    gren = SDL_CreateRenderer(gwin, -1, NULL);
    SDL_ShowWindow(gwin);
    SDL_SetRenderDrawColor(gren, 0, 0, 0, 255);
    SDL_RenderClear(gren);
    SDL_RenderPresent(gren);

    SDL_SetRenderDrawBlendMode( gren, SDL_BLENDMODE_BLEND );

    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    sampler::initialiseAudio( 44100, 2 );

    sampler smpl (MUS_PATH);

    sim_time timer(0.0f);
    timer.setCur();
    bool done = false;
    float start = 0.0f;
    float end = 0.0f;

    Mix_PlayChannel(-1, smpl.get(), 0);

    SDL_Delay(100);

    SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);

    std::vector<Line> averageLines;
    int averageLineMaxSize = 2;

    while(!done)
    {
        timer.setCur();
        start = end;
        end = timer.getTime();

        if(end > smpl.getLenSecs())
            done = true;

        //Ensure that the length of audio to sample is always greater that 0.
        if(start == end)
        {
            end += 0.0001;
        }

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

				std::vector<float> nums = smpl.sampleAudio(start, 16384);

				int averageSample = 16;
				std::vector<float> averaged;
				averaged.assign( nums.size() / averageSample, 0.0f );
				for(size_t i = 0; i < nums.size() - averageSample; i += averageSample)
				{
					for(int j = 0; j < averageSample; ++j)
						averaged[i / averageSample] += pow(nums[i + j], 0.99);
				}

				for(auto &i : averaged)
					i /= (float)averageSample;

        std::vector<SDL_Rect> rects;
				rects.reserve( averaged.size() );

				int rectWidths = ceil(g_WIN_WIDTH / (float)averaged.size());

				for(size_t i = 0; i < averaged.size(); ++i)
        {
						float mag = -averaged[i] * 16384;
            SDL_Rect pt;
						pt.x = ((float)i / averaged.size()) * g_WIN_WIDTH;
            pt.y = 512 - mag / 2;
            pt.h = mag;
            pt.w = rectWidths;

            rects.push_back( pt );
        }

        for(auto &i : rects)
        {
            int col = clamp(static_cast<int>(abs(i.h) / 2), 0, 255);
            SDL_SetRenderDrawColor(gren, col, col, col, 255);
            SDL_RenderFillRect(gren, &i);
        }

        int rectSamples = 16;
        Line averageLine;
        averageLine.reserve( rects.size() );
        for(size_t i = 0; i < rects.size(); ++i)
        {
            //Set min and max bounds for averaging.
            int min = i - rectSamples / 2;
            int max = i + rectSamples / 2;
            //If min is too low (below 0) add to max.
            max -= std::min(min, 0);
            //If max is too high above rects.size(), sub diff from min.
            min -= std::max((int)rects.size() - max, 0);
            //Clip both to container extents.
            min = clamp(min, 0, (int)rects.size());
            max = clamp(max, 0, (int)rects.size());

            SDL_Point pt = {rects[i].x + rectWidths / 2, 0};
            for(size_t j = min; j < max; ++j)
            {
                pt.y += rects[j].h / 2;
            }
            pt.y /= max - min;
            pt.y += 512;
            averageLine.push_back( pt );
        }
        averageLines.insert( averageLines.begin(), averageLine );
        if(averageLines.size() > averageLineMaxSize)
            averageLines.pop_back();

        SDL_SetRenderDrawColor( gren, 255, 0, 0, 255 );
        //SDL_RenderDrawLines( gren, &averageLine[0], averageLine.size() );

        averageLine.clear();
        averageLine.assign( rects.size(), {0, 0} );
        for(auto &line : averageLines)
        {
            for(size_t i = 0; i < line.size(); ++i)
            {
                averageLine[i].x += line[i].x;
                averageLine[i].y += line[i].y;
            }
        }
        for(auto &point : averageLine)
        {
            point.x /= averageLines.size();
            point.y /= averageLines.size();
        }

        for(float i = 1.0; i > -1.0f; i -= 2.0f / g_WIN_HEIGHT)
        {
						/*Line newline = averageLine;
            for(auto &pt : newline)
            {
                pt.y -= 512;
                pt.y *= i;
                pt.y += 512;
            }

            float col = (1.0f - std::abs(i)) * 255.0f;
            SDL_SetRenderDrawColor( gren, 255, 255, 255, col );
						SDL_RenderDrawLines( gren, &newline[0], newline.size() );*/
        }

        SDL_RenderPresent(gren);
    }

    //std::cout << "Press enter to quit.\n";
    //std::cin.get();

    Mix_CloseAudio();

    SDL_DestroyRenderer(gren);
    SDL_DestroyWindow(gwin);

    SDL_Quit();

    std::cout << "Complete!\n";

    return 0;
}
