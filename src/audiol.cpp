#include <cassert>
#include <cstring>
#include <cmath>

#include <SDL.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "audiol.hpp"

extern double precision_time_now();

void audiol_init()
{
    ALCint parameters[] = 
    {
        ALC_FREQUENCY, 44100,
        0, 0
    };
    ALCdevice *device = alcOpenDevice(NULL);
    assert(device);
    ALCcontext *context = alcCreateContext(device, parameters);
    alcMakeContextCurrent(context);
}

void audiol_pause()
{
}

void audiol_unpause()
{
}

audiol_wave_t audiol_upload_wave(int sample_count, int channel_count, int format, void *data)
{
    // some limitations...
    //assert(channel_count == 2);
    //assert(format == AL_FORMAT_STEREO16);

    unsigned int b;

    alGenBuffers(1, &b);
    alBufferData(b, format, data, sample_count * channel_count * sizeof(signed short), 44100);

    audiol_wave_t res;
    res.handle = b;
    res.channel_count = channel_count;

    return res;
}

void audiol_delete_wave(const audiol_wave_t &wave)
{
    alDeleteBuffers(1, &wave.handle);
}

audiol_voice_t audiol_create_voice()
{
    unsigned int s;

    alGenSources(1, &s);

    audiol_voice_t res;
    res.handle = s;
    res.currently_playing = NULL;

    return res;
}

void audiol_delete_voice(const audiol_voice_t &voice)
{
    alDeleteSources(1, &voice.handle);
}

void audiol_place_voice(const audiol_voice_t &voice, const vec3_t<> &position)
{
    alSource3f(voice.handle, AL_POSITION, position.x, position.y, position.z);
}

void audiol_place_listener(const vec3_t<> &position, const quat_t<> &orientation)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);

    vec3_t<> forward = orientation.forward();
    vec3_t<> up = orientation.up();

    float direction_vector[6];
    direction_vector[0] = forward.x;
    direction_vector[1] = forward.y;
    direction_vector[2] = forward.z;
    direction_vector[3] = up.x;
    direction_vector[4] = up.y;
    direction_vector[5] = up.z;

    alListenerfv(AL_ORIENTATION, direction_vector);
}

void audiol_attach_wave(audiol_voice_t *voice, const audiol_wave_t &wave)
{
    alSourcei(voice->handle, AL_BUFFER, wave.handle);
    voice->currently_playing = &wave;
    alSourcei(voice->handle, AL_LOOPING, AL_TRUE);
    //alSourcef(voice->handle, AL_MAX_DISTANCE, 20.0f);

    alSourcePlay(voice->handle);
}

