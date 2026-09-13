#include <miniaudio.h>
#include <music_defs.h>
#include <playback_menu.h>

#include <audio_playback.h>

void end_callback(void* pUserData, ma_sound* pSound) {
    // JUST NOTIFY
    playback_menu* pm = pUserData;
    pm->ready = true;
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

    return ap;
}

int audio_load_album(audio_player* ap, album* abm, size_t idx, playback_menu* pm) {
    ap->abm = abm;
    ap->idx = idx;

    song* sng = JVEC_get(abm->songs, idx);
    audio_load_song(ap, sng, pm);
}


int audio_load_song(audio_player* ap, song* sng, playback_menu* pm) {

    ma_result res;
    if (!ma_sound_is_playing(&ap->sound)) {
        ma_sound_uninit(&ap->sound);
        ap->playing = false;
    }

    res = ma_sound_init_from_file(&ap->engine, sng->path, 0, NULL, NULL, &ap->sound);
    if (res != MA_SUCCESS) {
        ma_sound_uninit(&ap->sound);
        return -1;
    }
    ma_sound_set_end_callback(&ap->sound, end_callback, ap);
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