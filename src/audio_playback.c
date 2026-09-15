#include <miniaudio.h>
#include <music_defs.h>
#include <playback_menu.h>

#include <audio_playback.h>

void end_callback(void* pUserData, ma_sound* pSound) {
    // JUST NOTIFY
    audio_player* ap = pUserData;
    ap->playing = false;
    ap->ready = true;
}

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

    ap->ready = false;

    return ap;
}



int audio_load_song(audio_player* ap, song* sng) {

    ma_result res;
    if (!ma_sound_is_playing(&ap->sound)) {
        ma_sound_uninit(&ap->sound);
    }

    res = ma_sound_init_from_file(&ap->engine, sng->path, 0, NULL, NULL, &ap->sound);
    if (res != MA_SUCCESS) {
        ma_sound_uninit(&ap->sound);
        return -1;
    }
    ma_sound_set_end_callback(&ap->sound, end_callback, ap);
    ma_sound_start(&ap->sound);
    ap->playing = true;
    ap->ready = false;
    return 0;
}

double audio_current_pos(audio_player* ap) {
    if (!ap->playing) {
        return 0;
    }

    float pos;
    ma_sound_get_cursor_in_seconds(&ap->sound, &pos);
    return (double) pos;
}