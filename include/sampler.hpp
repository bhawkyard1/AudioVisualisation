#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <string>

#include <vector>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

class sampler
{
public:
    sampler(const std::string _path);
private:
    std::vector<float> m_frequencies;
};

#endif
