#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include <miniaudio.h>
#include <stdbool.h>
#include <music_defs.h>
#include <music_player2.h>

typedef struct audio_player {
    ma_engine engine;
    ma_sound sound;

    bool playing;

    bool ready;

} audio_player;

audio_player* audio_new();



double audio_current_pos(audio_player *player);
int audio_load_song(audio_player* ap, song* song);
#endif