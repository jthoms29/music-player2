#ifndef PLAYBACK_MENU_H
#define PLAYBACK_MENU_H

#include <music_defs.h>
#include <ncurses.h>

typedef struct playback_menu {
    JVEC* playlist;
    size_t playlist_idx;
    song* cur_song;

    int hgt;
    int wdt;

    long time_s;
    long elapsed_s;

    WINDOW* win;
} playback_menu;

/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
playback_menu* playback_new();

/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
void playback_free(playback_menu** pb_ptr);


/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int playback_change_song(playback_menu *pb, JVEC* playlist, size_t idx);

/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int playback_next_song(playback_menu *pb);

/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
song* playback_cur_song(playback_menu* pb);

/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
void playback_resize(playback_menu *pb, int hgt, int wdt, int y);


/**
 * @brief
 * @details
 * @note
 * @param[in]
 * @param[in]
 * @pre
 * @post
 * @return
 */
void playback_draw(playback_menu *pb);

#endif