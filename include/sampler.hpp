#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <string>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

class sampler
{
public:
    static void initialiseAudio(const int _rate);
    sampler(const std::string _path);
    void sampleAudio(const float _start, const float _end);
private:
    Mix_Chunk * m_snd;
    static int m_sampleRate;
};

#endif
