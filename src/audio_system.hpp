#ifndef _AUDIO_SYSTEM_HPP
#define _AUDIO_SYSTEM_HPP

#include "entity_system.hpp"

class audio_system_t : public system_t
{
public:
    audio_system_t();

    void init();
    void update(float dt);

    void pause();
    void unpause();
};

#endif // _AUDIO_SYSTEM_HPP

