#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "fft.hpp"

#define MUS_PATH "soviet_national_anthem_0.wav"

Uint8 * audioPos;
Uint32 remainingLen;

void play(void *userdata, Uint8 *stream, int len);

int main(int argc, char * argv[])
{
    std::cout << "Number of playback devices : " << SDL_GetNumAudioDevices( 0 ) << '\n';

    if(SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        std::cout << "SDL Init Error!\n" << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

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

    audioPos = wavBuf;
    remainingLen = wavLen;
    wavSpec.callback = play;
    wavSpec.userdata = NULL;

    if(SDL_OpenAudio(&wavSpec, NULL) != 0)
    {
        std::cerr << "Couldn't open audio! " << SDL_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    int len = wavLen / sizeof(Uint8);

    std::cout << "Buffer analysis...\nLen : " << len << "Secs : " << (len / 44100.0f) << "\nContinue?\n";

    std::cin.get();

    /*for(int i = 0; i < len; ++i)
    {
        std::cout << "buf [" << i << "]     " << std::to_string(wavBuf[i]) << "       " << ((float)i / (float)len * 100.0f) << "%\n";
    }*/

    std::valarray<Complex> arr;
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

    SDL_PauseAudio(0);
    while(remainingLen > 0)
    {
        SDL_Delay(100);
    }

    SDL_CloseAudio();
    SDL_FreeWAV(wavBuf);

    std::cout << "Complete!\n";

    return 0;
}

void play(void *userdata, Uint8 *stream, int len) {

    if (remainingLen ==0)
        return;

    len = ( len > remainingLen ? remainingLen : len );

    SDL_MixAudio(stream, audioPos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

    audioPos += len;
    remainingLen -= len;
}
