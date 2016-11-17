#include <iostream>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"

#define MUS_PATH "red_laser_0.wav"

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
    SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);
    SDL_RenderClear(gren);
    SDL_RenderPresent(gren);

    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0)
    {
        std::cout << "Mix_OpenAudio error : " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    Mix_Chunk * snd = Mix_LoadWAV(MUS_PATH);

    if(!snd)
    {
        std::cerr << "Could not open " << MUS_PATH << " : " << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    int len = snd->alen / 44100.0f;

    std::cout << "Buffer analysis...\nLen : " << snd->alen << " || " << len << "\nSecs : " << (snd->alen / (2 * 44100.0f)) << "\nContinue?\n";

    std::cin.get();

    std::valarray<Complex> arr;
    arr.resize( len, {0.0, 0.0} );
    for(int i = 0; i < arr.size(); ++i)
    {
        Complex insert = {(double)snd->abuf[i], 0.0};
        arr[i] = insert;
    }

    std::cout << "Done.\n";

    fft(arr);
    std::cout << "FFT complete.";

    for(int i = 0; i < arr.size(); ++i)
    {
        std::cout << "fft[" << i << "] " << arr[i].real() << ", " << arr[i].imag() << '\n';
    }

    for(int i = 0; i < arr.size() / 2 - 1; ++i)
    {
        std::cout << "Amplitude at " << i * 44100.0f / 1024.0f << "hz is " << sqrt(arr[i].real() * arr[i].real() + arr[i].imag() * arr[i].imag()) << '\n';
    }

    Mix_PlayChannel(-1, snd, 0);

    std::cout << "Press enter to quit.\n";
    std::cin.get();

    Mix_FreeChunk(snd);
    Mix_CloseAudio();

    SDL_DestroyRenderer(gren);
    SDL_DestroyWindow(gwin);

    SDL_Quit();

    std::cout << "Complete!\n";

    return 0;
}
