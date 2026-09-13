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

    pb->win = pb_win;

    return pb;
}

int playback_change_song(playback_menu *pb, song *s) {
    pb->selected = s;
    pb->elapsed_s = 0;
    pb->time_s = s->dur_s;
    return 0;
}

int playback_update_elapsed(playback_menu *pb, size_t new_s) {
    assert(new_s <= pb->time_s);
    pb->elapsed_s = new_s;
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