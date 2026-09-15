#ifndef PLAYBACK_MENU_H
#define PLAYBACK_MENU_H

#include <music_defs.h>
#include <ncurses.h>

typedef struct playback_menu {
    JVEC* playlist;
    size_t playlist_idx;
    song* cur_song;

    bool paused;
    size_t hgt;
    size_t wdt;

    size_t time_s;
    size_t elapsed_s;

    bool ready;

    WINDOW* win;
} playback_menu;

playback_menu* playback_new();
int playback_change_song(playback_menu *pb, JVEC* playlist, size_t idx);
int playback_next_song(playback_menu *pb);
song* playback_cur_song(playback_menu* pb);
void playback_resize(playback_menu *pb, int hgt, int wdt, int y);

void playback_draw(playback_menu *pb);
#endif