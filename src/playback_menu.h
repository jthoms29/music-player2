#ifndef PLAYBACK_MENU_H
#define PLAYBACK_MENU_H

#include <music_player2.h>
#include <ncurses.h>

typedef struct playback_menu {
    song* selected;
    bool paused;
    size_t hgt;
    size_t wdt;

    size_t time_s;
    size_t elapsed_s;

    WINDOW* win;
} playback_menu;

playback_menu* playback_new();

int playback_change_song(playback_menu* pb, song* s);


#endif