#include <playback_menu.h>
#include <assert.h>

#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))
#define DRAW_UNFILLED(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(4))
playback_menu* playback_new() {

    playback_menu* pb = calloc(1, sizeof(*pb));
    if (!pb) {
        perror("playback_new(): failed to alloc new playback_menu");
        return NULL;
    }

    WINDOW* pb_win = newwin(0,0,0,0);
    if (!pb_win) {
        fprintf(stderr, "playback_new(): failed to create playback menu's window\n");
        free(pb);
        return NULL; 
    }

    pb->win = pb_win;

    return pb;
}

int playback_change_song(playback_menu *pb, JVEC* playlist, size_t idx) {
    song* sng = JVEC_get(playlist, idx);
    pb->cur_song = sng;
    pb->playlist = playlist;
    pb->playlist_idx = idx;
    pb->elapsed_s = 0;
    pb->time_s = sng->dur_s;
    return 0;
}

int playback_next_song(playback_menu *pb) {
    size_t new_idx = pb->playlist_idx+1;
    if (new_idx >= JVEC_len(pb->playlist)) {
        return 0;
    }

    pb->playlist_idx = new_idx;
    pb->cur_song = JVEC_get(pb->playlist, new_idx);
    pb->elapsed_s = 0;
    pb->time_s = pb->cur_song->dur_s;
    return 1;
}

int playback_update_elapsed(playback_menu *pb, size_t new_s) {
    assert(new_s <= pb->time_s);
    pb->elapsed_s = new_s;
    return 0;
}
void playback_resize(playback_menu *pb, int hgt, int wdt, int y) {
    pb->hgt = hgt;
    pb->wdt = wdt;
    wresize(pb->win, hgt, wdt);
    mvwin(pb->win, y, 0);
}

void playback_draw(playback_menu *pb) {
    WINDOW* w = pb->win;
    werase(w);
    box(w, 0, 0);
   
    int bar = pb->wdt * 4 / 6;
    int bar_offset = (pb->wdt - bar)/2;
    int filled_ratio = 0;

    mvwprintw(w, 1, 1, "%ld/%ld\n", pb->elapsed_s, pb->time_s);
    if (pb->time_s != 0) {
        filled_ratio = bar * pb->elapsed_s / pb->time_s;
    }
    for (int i = bar_offset; i < bar_offset+filled_ratio; i++) {
        DRAW_PIXEL(w, i, 3);
    }
    for (int i = bar_offset+filled_ratio; i < bar_offset+bar; i++) {
        DRAW_UNFILLED(w, i, 3);
    }


}


song* playback_cur_song(playback_menu* pb) {
    return JVEC_get(pb->playlist, pb->playlist_idx);
}