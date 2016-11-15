#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"

#define MUS_PATH "red_laser_0.wav"

Uint8 * audioPos;
Uint32 remainingLen;

void playcallback(void *userdata, Uint8 *stream, int len);

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

    gwin = SDL_CreateWindow("vis", 100, 100, 1024, 1024, SDL_WINDOW_RESIZABLE);
    gren = SDL_CreateRenderer(gwin, -1, NULL);
    SDL_ShowWindow(gwin);
    SDL_SetRenderDrawColor(gren, 255, 0, 0, 255);
    SDL_RenderClear(gren);
    SDL_RenderPresent(gren);

    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    /*if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0)
    {
        std::cout << "Mix_OpenAudio error : " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }*/

    SDL_AudioSpec wavSpec;
    Uint32 wavLen;
    Uint8 * wavBuf;

    if(SDL_LoadWAV(MUS_PATH, &wavSpec, &wavBuf, &wavLen) == 0)
    {
        std::cerr << "Could not open " << MUS_PATH << " : " << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    wavSpec.callback = playcallback;
    wavSpec.userdata = NULL;
    audioPos = wavBuf;
    remainingLen = wavLen;

    if(SDL_OpenAudio(&wavSpec, NULL) != 0)
    {
        std::cerr << "Couldn't open audio! " << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    int len = 1024;//wavLen / sizeof(Uint8);

    std::cout << "Buffer analysis...\nLen : " << len << "Secs : " << (len / 44100.0f) << "\nContinue?\n";

    std::cin.get();

    /*std::valarray<Complex> arr;
    arr.resize( len, {0.0, 0.0} );
    for(int i = 0; i < len; ++i)
    {
        Complex insert = {(double)wavBuf[i], 0.0};
        arr[i] = insert;
    }

    std::cout << "Done.\n";

    fft(arr);
    std::cout << "FFT complete.";

    for(int i = 0; i < len; ++i)
    {
        std::cout << "fft[" << i << "] " << arr[i].real() << ", " << arr[i].imag() << '\n';
    }

    for(int i = 0; i < len / 2 - 1; ++i)
    {
        std::cout << "Amplitude at " << i * 44100.0f / 1024.0f << "hz is " << sqrt(arr[i].real() * arr[i].real() + arr[i].imag() * arr[i].imag()) << '\n';
    }*/

    SDL_PauseAudio(0);
    while(remainingLen > 0)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudio();
    SDL_FreeWAV(wavBuf);
    SDL_DestroyRenderer(gren);
    SDL_DestroyWindow(gwin);

    std::cout << "Complete!\n";

    return 0;
}

void playcallback(void *userdata, Uint8 *stream, int len) {

    if (remainingLen == 0)
        return;

    if(len > remainingLen)
        len = remainingLen;
    else
        len = len;

    //len = ( len > remainingLen ? remainingLen : len );
    std::cout << len << '\n';

    SDL_MixAudio(stream, audioPos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

    audioPos += len;
    remainingLen -= len;
}
