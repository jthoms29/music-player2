#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include <miniaudio.h>
#include <stdbool.h>
#include <music_defs.h>
#include <music_player2.h>

typedef struct audio_player {
    ma_engine engine;
    ma_sound sound;

    album* abm;
    size_t idx;
    bool playing;

} audio_player;

audio_player* audio_new();


int audio_load_song(audio_player* player, song* song, playback_menu* pm);

double audio_current_pos(audio_player *player);

int audio_load_album(audio_player* ap, album* abm, size_t idx, playback_menu* pm);
#endif