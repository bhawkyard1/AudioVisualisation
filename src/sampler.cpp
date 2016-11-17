#include <iostream>

#include "sampler.hpp"

#include "fft.hpp"

int sampler::m_sampleRate = 0;

void sampler::initialiseAudio(const int _rate)
{
    if( Mix_OpenAudio( _rate, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0)
    {
        std::cout << "Mix_OpenAudio error : " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    m_sampleRate = _rate;
}

sampler::sampler(const std::string _path)
{
    m_snd = Mix_LoadWAV(_path.c_str());
    if(!m_snd)
    {
        std::cerr << "Could not load wav file! " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }
}


void sampler::sampleAudio(const float _start, const float _end)
{
    int startSample =_start / m_sampleRate;
    int endSample = _end / m_sampleRate;
    int lenSample = endSample - startSample;

    //Set to start of audio.
    Uint8 * start = m_snd->abuf;
    //Move up by n samples. NB 1 = 1 byte (size of Uint8).
    start += startSample;

    std::valarray<Complex> arr;

    arr.resize( lenSample, {0.0, 0.0} );
    for(int i = 0; i < arr.size(); ++i)
    {
        Complex insert = {(double)m_snd->abuf[i], 0.0};
        arr[i] = insert;
    }

    fft(arr);

    std::cout << "Sampling completed.\n";
}

