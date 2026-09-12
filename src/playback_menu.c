#include <playback_menu.h>
#include <assert.h>

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
void playback_resize(playback_menu *pb, size_t hgt, size_t wdt) {

}

void playback_draw(playback_menu *pb) {
    WINDOW* w = pb->win;
    box(w, 0, 0);
    

}