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

    //Fill the buffer with the raw data now, converted to uint16s
    m_buf.reserve( m_snd->alen / sizeof(uint16_t) );
    for(size_t i = 0; i < m_snd->alen; i += sizeof(uint16_t))
    {
        m_buf.push_back( toi16( m_snd->abuf[i], m_snd->abuf[i + 1] ) );
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
    //Find start index from time. Uint16 = 2 bytes.
    int startSample = _start * s_sampleRate * s_channels;
    //Clamp it to zero, just in case.
    startSample = std::max(startSample, 0);
    //Get end sample using width.
    int endSample = startSample + _width;
    //Get the difference between these (in current implementation this should be equal to _width).
    int lenSample = endSample - startSample;

    if(lenSample == 0)
        return {};

    //Create our complex buffer.
    std::valarray<Complex> arr;

    //Number of u16s to look at.
    arr.resize( lenSample / 2, {0.0f, 0.0f} );

    //Concatenates each pair of uint8s into a single uint16.
    //I don't know if this makes sense, but the buffer is all uint8s and the wav is 16-bit.
    for(size_t i = 0; i < arr.size(); ++i)
    {
        Complex insert = {static_cast<float>( toi16(m_snd->abuf[startSample + 2 * i], m_snd->abuf[startSample + 2 * i + 1]) ), 0.0f};
        arr[i / 2] = insert;
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

