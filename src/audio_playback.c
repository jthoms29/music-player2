#include <miniaudio.h>
#include <music_defs.h>
#include <playback_menu.h>

#include <audio_playback.h>

audio_player* audio_new() {

    audio_player* ap = calloc(1, sizeof(*ap));
    if (!ap) {
        perror("audio_new(): failed to alloc new audio_player struct");
        return NULL;
    }

    ma_result result;

    result = ma_engine_init(NULL, &ap->engine);
    if (result != MA_SUCCESS) {
        free(ap);
        return NULL;
    }

    return ap;
}


int audio_load_song(audio_player* ap, song* sng) {

    ma_result res;

    res = ma_sound_init_from_file(&ap->engine, sng->path, 0, NULL, NULL, &ap->sound);
    //CHECK !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ma_sound_start(&ap->sound);
    ap->playing = true;
    return 0;
}

double audio_current_pos(audio_player* ap) {
    if (!ap->playing) {
        return 0;
    }

    float pos;
    ma_sound_get_cursor_in_seconds(&ap->sound, &pos);
    return pos;
}