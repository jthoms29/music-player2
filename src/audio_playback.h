#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include <miniaudio.h>
#include <stdbool.h>
#include <music_defs.h>

typedef struct audio_player {
    ma_device device;
    ma_decoder decoder;

    bool decoder_initialized;
    bool playing;

    pthread_mutex_t mutex;
} audio_player;

audio_player* audio_new();


int audio_load_song(audio_player* player, song* song);

double audio_current_pos(audio_player *player);

#endif