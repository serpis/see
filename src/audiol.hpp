#ifndef _AUDIOL_HPP
#define _AUDIOL_HPP

#include "math.hpp"

struct audiol_wave_t
{
    unsigned int handle;
    int channel_count;
};

struct audiol_voice_t
{
    unsigned int handle;
    const audiol_wave_t *currently_playing;
};

struct audiol_progress_t
{
    int current_sample;

    // convenience
    audiol_progress_t() : current_sample(0) {}
};

void audiol_init();
void audiol_pause();
void audiol_unpause();
audiol_wave_t audiol_upload_wave(int sample_count, int channel_count, int format, void *data);
void audiol_delete_wave(const audiol_wave_t &wave);
audiol_voice_t audiol_create_voice();
void audiol_delete_voice(const audiol_voice_t &voice);
void audiol_place_voice(const audiol_voice_t &voice, const vec3_t<> &position);
void audiol_place_listener(const vec3_t<> &position, const quat_t<> &orientation);
void audiol_attach_wave(audiol_voice_t *voice, const audiol_wave_t &wave);

#endif // AUDIOL_HPP

