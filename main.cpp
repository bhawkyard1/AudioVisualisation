#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"
#include "sampler.hpp"
#include "sim_time.hpp"

#define MUS_PATH "soviet_national_anthem_0.wav"

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
	timer.setCur();
	bool done = false;
	float start = 0.0f;
	float end = 0.0f;

	Mix_PlayChannel(0, smpl.get(), 0);

	SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);

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
			std::cout << "doot\n";
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

		std::vector<float> nums = smpl.sampleAudio(start, end);

		std::vector<SDL_Rect> pts;
		pts.reserve( nums.size() );

		for(int i = 0; i < nums.size(); ++i)
		{
			SDL_Rect pt;
			pt.x = ((float)i / nums.size()) * 1024;
			pt.y = 512;
			pt.h = -nums[i] / 2000000.0f;
			pt.w = 2;
			pts.push_back(pt);

			pts.push_back({pt.x, 512.0f});
		}

		SDL_Delay(10);

		SDL_RenderFillRects(gren, &pts[0], pts.size());
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
