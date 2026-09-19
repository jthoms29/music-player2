#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H
#include <miniaudio.h>
#include <stdbool.h>
#include <music_defs.h>
#include <music_player2.h>

typedef struct audio_player {
    ma_engine engine;
    ma_sound sound;

    // for telling main thread to switch to next song
    int notify_read_fd;
    int notify_write_fd;
} audio_player;

/**
 * @brief Create and return a new audio_player struct
 * @details Allocates a new audio_player struct, initializes miniaudio engine
 * @note Must be freed with `audio_free()`
 * @pre None 
 * @post Heap allocated audio_player struct returned, miniaudio engine within it initialized
 * @return Pointer to struct on success, NULL on failure
 */
audio_player* audio_new();

/**
 * @brief Frees audio_player struct
 * @details Frees audio_player struct, unitializes all miniaudio components stored within
 * @param[in, out] ap_ptr Indirect pointer to audio_player struct
 * @pre struct must be null or been allocated with `audio_new()`
 * @post struct freed, all miniaudio components within unitialized, struct set to NULL
 * @return None
 */
void audio_free(audio_player** ap_ptr);

/**
 * @brief Load and start playing a new song
 * @details Halts playback on current song if playing, loads song specified in sng's `path` member
 * @param[in, out] ap pointer to audio_player struct created by `audio_new()`
 * @param[in] sng song struct retrieved from in-memory music library
 * @pre ap must be non-NULL, created with `audio_new()`, sng must be non-NULL
 * @post Current song halted if playing, song specified by sng is loaded and started
 * @return 0 on success, anything else on failure
 */
int audio_load_song(audio_player* ap, song* sng);

/**
 * @brief Retrieves the time of current song's playback in seconds
 * @param[in] player pointer to audio_player created by `audio_new()`
 * @pre player must have been created with `audio_new()`
 * @post Time of currently playing song returned
 * @return Time in seconds of currently playing song, 0 if no song currently playing
 */
long audio_current_pos(audio_player *player);

/**
 * @brief Returns if song is currently playing
 * @param[in] ap pointer to audio_player struct
 * @pre ap must have been created with `audio_new()`
 * @post current state of playing audio returned
 * @return true if song is currently playing, false otherwise
 */
bool audio_is_playing(audio_player* ap);

/**
 * @brief Pause the currently playing song
 * @details Pause playback of currently selected song within ap
 * @param[in, out] ap pointer to ap audio_player struct
 * @pre ap must have been created with `audio_new()`
 * @post if song is currently playing, it is paused
 * @return 0 on success, anything else on failure
 */
int audio_pause(audio_player* ap);

#endif