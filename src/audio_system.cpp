#ifdef _OSX
#include <OpenAL/al.h> // for alGetError
#else
#include <AL/al.h> // for alGetError
#endif

#include "audiol.hpp"
#include "audio_system.hpp"
#include "entity_system.hpp"
#include "components.hpp"
#include "renderh.hpp"

audio_system_t::audio_system_t()
{
}

void audio_system_t::init()
{
}

extern double precision_time_now();

static const mat4_t<> *HAX_view;

void audio_system_t::update(float dt)
{
    entity_list_t players = entity_manager_t::default_manager->entities_possessing_component_type(typeid(player_component_t));
    assert(players.size() == 1);
    int player_entity = *players.begin();
    position_component_t *position = entity_manager_t::default_manager->get_component<position_component_t>(player_entity);
    orientation_component_t *orientation = entity_manager_t::default_manager->get_component<orientation_component_t>(player_entity);
    assert(position);
    assert(orientation);

    audiol_place_listener(position->xyz, orientation->rotation);

    entity_manager_t::default_manager->iterate_nodes<sound_source_component_t, position_component_t>(1, [](sound_source_component_t *sound_source, position_component_t *pos)
    {
        audiol_place_voice(sound_source->voice, pos->xyz);

        if (sound_source->voice.currently_playing == NULL)
        {
            audiol_attach_wave(&sound_source->voice, *sound_source->wave);
        }
    });
    if (alGetError() != AL_NO_ERROR)
    {
        std::cout << "omg problem!" << std::endl;
    }
}

void audio_system_t::pause()
{
    audiol_pause();
}

void audio_system_t::unpause()
{
    audiol_unpause();
}

