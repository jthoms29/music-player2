#include <miniaudio.h>
#include <music_defs.h>
#include <playback_menu.h>
#include <unistd.h>
#include <audio_playback.h>

void end_callback(void* pUserData, ma_sound* pSound) {
    audio_player *ap = pUserData;

    // signal poll to switch song
    uint8_t signal = 1;
    write(ap->notify_write_fd, &signal, 1);
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
        fprintf(stderr, "audio_new(): failed to init audio engine");
        free(ap);
        return NULL;
    }

    int notify[2];
    if (pipe(notify) == -1) {
        perror("audio_new(): failed to create pipe");
        free(ap);
        ma_engine_uninit(&ap->engine);
    }
    ap->notify_read_fd = notify[0];
    ap->notify_write_fd = notify[1];
    return ap;
}

void audio_free(audio_player** ap_ptr) {
    if (!(*ap_ptr)) { return; }

    // pretty sure it's safe to pass NULL to these, don't need check
    ma_engine_uninit(&(*ap_ptr)->engine); // :)
    ma_sound_uninit(&(*ap_ptr)->sound);
    

    // close the pipe
    close((*ap_ptr)->notify_read_fd);
    close((*ap_ptr)->notify_write_fd);

    // free the ap struct itself
    free(*ap_ptr);
    *ap_ptr = NULL;
}



int audio_load_song(audio_player* ap, song* sng) {
    ma_result res;

    // if song was initialized before this, get rid of it
    if (ma_sound_is_playing(&ap->sound) || ma_sound_at_end(&ap->sound)) {
        ma_sound_uninit(&ap->sound);
    }

    // load new song
    res = ma_sound_init_from_file(&ap->engine, sng->path, 0, NULL, NULL, &ap->sound);
    if (res != MA_SUCCESS) {
        ma_sound_uninit(&ap->sound);
        return -1;
    }
    // set function sound will call when ends
    ma_sound_set_end_callback(&ap->sound, end_callback, ap);

    // start playing song in miniaudio thread
    ma_sound_start(&ap->sound);
    return 0;
}

bool audio_is_playing(audio_player* ap) {
    return ma_sound_is_playing(&ap->sound);
}

int audio_pause(audio_player* ap) {
    if (!ma_sound_is_playing(&ap->sound)) {
        return 0;
    }
    ma_sound_stop(&ap->sound);
    return 0;
}

double audio_current_pos(audio_player* ap) {
    // song is not currently playing
    if (!ma_sound_is_playing(&ap->sound)) {
        return 0;
    }

    float pos = ma_sound_get_cursor_in_seconds(&ap->sound, &pos);
    return (double) pos;
}