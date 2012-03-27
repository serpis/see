#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#include "entity_system.hpp"
#include "render_system.hpp"
#include "input_system.hpp"
#include "audio_system.hpp"
#include "physics_system.hpp"

class engine_t
{
public:
    static engine_t *instance;

    entity_manager_t manager;
    render_system_t render_system;
    input_system_t input_system;
    audio_system_t audio_system;
    physics_system_t physics_system;

    engine_t();

    void init();
    void run();
};

#endif // _ENGINE_HPP

