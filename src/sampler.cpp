#include <algorithm>
#include <iostream>
#include <fstream>

#include "sampler.hpp"

#include "fft.hpp"
#include "kiss/kiss_fft.h"

#include "util.hpp"

int sampler::s_sampleRate = 0;
int sampler::s_channels = 2;

void sampler::initialiseAudio(const int _rate, const int _channels)
{
    if( Mix_OpenAudio( _rate, MIX_DEFAULT_FORMAT, 2, 1024 ) < 0)
    {
        std::cout << "Mix_OpenAudio error : " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    s_sampleRate = _rate;
    s_channels = _channels;
}

sampler::sampler(const std::string _path)
{
    m_snd = Mix_LoadWAV(_path.c_str());
    if(!m_snd)
    {
        std::cerr << "Could not load wav file! " << Mix_GetError() << '\n';
        exit(EXIT_FAILURE);
    }

    m_len = m_snd->alen / (float)(s_sampleRate * s_channels * 2.0f);

    std::cout << "Audio file has len " << m_snd->alen << " bytes.\nWith "
              << s_channels << " channels and a sample rate of "
              << s_sampleRate << " this means that it is " << m_len << " seconds long.\n";
}

sampler::~sampler()
{
    Mix_FreeChunk(m_snd);
}

std::vector<float> sampler::sampleAudio(const float _start, const int _width)
{
    int startSample = _start * s_sampleRate * s_channels;
    //startSample += 30000;
    //startSample -= _width / 2;
    startSample = std::max(startSample, 0);
    int endSample = startSample + _width;
    int lenSample = endSample - startSample;

    if(lenSample == 0)
        return {};

    /*
    //Create 'state buffer'. I have no idea why.
    kiss_fft_cfg cfg = kiss_fft_alloc(lenSample, 0, NULL, NULL);
    //Create an input array, kiss_fft_cpx has i and r members.
    std::vector<kiss_fft_cpx> input;
    input.reserve( lenSample );
    //Add values to input.
    for(size_t i = 0; i < input.size(); ++i)
        input.push_back( {static_cast<float>(m_snd->abuf[startSample + i]), 0.0f} );
    //Create and size output.
    std::vector<kiss_fft_cpx> output;
    output.reserve(input.size());
    //Perform fft.
    std::cout << "pre fft\n";
    kiss_fft(cfg, &input[0], &output[0]);
    std::cout << "post fft\n";
    delete [] cfg;
    */

    std::valarray<Complex> arr;

    //std::cout << "p0\n";
    arr.resize( lenSample, {0.0f, 0.0f} );

    //Concatenates each pair of uint8s into a single uint16.
    //I don't know if this makes sense, but the buffer is all uint8s and the wav is 16-bit.
    for(size_t i = 0; i < arr.size(); i += 2)
    {
        int16_t t = toi16( m_snd->abuf[startSample + i], m_snd->abuf[startSample + i + 1] );
        //std::cout << "sample " << startSample + i << ", " << t << '\n';
        Complex insert = {static_cast<float>(t), 0.0f};
        arr[i] = insert;
    }

    hanning(arr);
    fft(arr);

    std::vector<float> ret;
    ret.reserve( arr.size() / 2 );

    for(size_t i = 0; i < arr.size() / 2; ++i)
    {
        //std::cout << i * ((float)s_sampleRate / arr.size()) << " Hz -> " << mag( arr[i] ) << '\n';
        float val = magns( arr[i] );
        val = 20 * log10(val);
        val /= lenSample;
        ret.push_back( val );
    }

    /*auto m = std::max_element( ret.begin() + 1, ret.end() );
    int ind = std::distance( ret.begin(), m );*/
    //std::cout << ind * ((float)s_sampleRate / arr.size()) << " Hz -> " << *m << '\n';

    //std::cout << "p1\n";

    return ret;
}

