#include <miniaudio.h>
#include <music_defs.h>
#include <playback_menu.h>

#include <audio_playback.h>

void data_callback(ma_device *pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    audio_player *player = pDevice->pUserData;

    if (!player->decoder_initialized) {
        memset(
            pOutput,
            0,
            frameCount *
            pDevice->playback.channels *
            ma_get_bytes_per_sample(pDevice->playback.format)
        );
        return;
    }

    ma_decoder_read_pcm_frames(
        &player->decoder,
        pOutput,
        frameCount,
        NULL
    );

    (void)pInput;

}

audio_player* audio_new() {
    audio_player* player = calloc(1, sizeof(*player));
    if (!player) {
        perror("audio_new(): failed to alloc new player");
        return NULL;
    }
    if (pthread_mutex_init(&player->mutex, NULL)) {
        fprintf(stderr, "audio_new(): Failed to init player mutex\n");
        free(player);
        return NULL;
    }

    // init playback device
    ma_device_config config = ma_device_config_init(ma_device_type_playback);

    config.playback.format   = ma_format_f32;   // Set to ma_format_unknown to use the device's native format.
    config.playback.channels = 2;               // Set to 0 to use the device's native channel count.
    config.sampleRate        = 44100;               // Set to 0 to use the device's native sample rate.
    config.dataCallback      = data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = player;   // Can be accessed from the device object (device.pUserData).

    if (ma_device_init(NULL, &config, &player->device) != MA_SUCCESS) {
        fprintf(stderr, "audio_new(): failed to init device\n");
        goto uh_oh;
    }

    if (ma_device_start(&player->device) != MA_SUCCESS) {
        ma_device_uninit(&player->device);
        fprintf(stderr, "audio_new(): failed to start device\n");
        goto uh_oh;
    }


    return player;
    uh_oh:
        pthread_mutex_destroy(&player->mutex);
        free(player);
        return NULL;
}

int audio_load_song(audio_player* player, song* song) {

    ma_result result;

    // song already playing, reset decoder
    if (player->decoder_initialized) {
        ma_decoder_uninit(&player->decoder);
        player->decoder_initialized = false;

    }
    result = ma_decoder_init_file(song->path, NULL, &player->decoder);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "audio_load_song(): could not load file %s\n", song->path);
        return -1;
    }
    player->decoder_initialized = true;
    return 0;

}

double audio_current_pos(audio_player *player) {
    if (!player->decoder_initialized) {
        return 0.0;
    }

    ma_uint64 frame = 0;
    ma_result res = ma_decoder_get_cursor_in_pcm_frames(&player->decoder, &frame);

    ma_uint64 sample_rate = player->decoder.outputSampleRate;

    return (double) frame/sample_rate;
}