#include <algorithm>
#include <iostream>

#include "sampler.hpp"

#include "fft.hpp"

//#include "kiss_fft.h"

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

std::vector<float> sampler::sampleAudio(const float _start, const float _end)
{
	int startSample = _start * s_sampleRate * s_channels;
	int endSample = _end * s_sampleRate * s_channels;
	int lenSample = endSample - startSample;

	//std::cout << "Start is byte " << startSample << ", end is " << endSample << " therefore len is " << lenSample << '\n';
	//std::cout << startSample / (s_sampleRate) << '\n';

	/*for(int i = 0; i < 128; ++i)
		std::cout << +m_snd->abuf[i] << '\n';
	return {0.0f};*/

	std::valarray<Complex> arr;

	arr.resize( lenSample, {0.0f, 0.0f} );
	for(int i = 0; i < arr.size(); ++i)
	{
		Complex insert = {static_cast<float>(m_snd->abuf[startSample + i]), 0.0f};
		arr[i] = insert;
	}

	fft(arr);

	std::vector<float> ret;
	ret.reserve( arr.size() / 2 );

	for(int i = 0; i < arr.size() / 2; ++i)
	{
		//std::cout << i * ((float)s_sampleRate / arr.size()) << " Hz -> " << mag( arr[i] ) << '\n';
		ret.push_back( magns( arr[i] ) );
	}

	auto m = std::max_element( ret.begin() + 1, ret.end() );
	int ind = std::distance( ret.begin(), m );
	std::cout << ind * ((float)s_sampleRate / arr.size()) << " Hz -> " << *m << '\n';

	return ret;
}

