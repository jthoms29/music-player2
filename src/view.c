#include <music_player2.h>
#include <ncurses.h>

#define S_HEIGHT 30
#define S_WIDTH 10

typedef struct imodel {
    // in memory library
    lib_mem* lib;

    uint8_t col_idx;

    size_t artist_top;
    size_t artist_idx;

    size_t album_top;
    size_t album_idx;

    size_t song_top;
    size_t song_idx;

} imodel;

imodel* imodel_new(lib_mem* lib) {
    imodel* im = calloc(1, sizeof(*im));
    if (!im) {
        perror("imodel_new(): could not allocate new imodel");
        return NULL;
    }

    im->lib = lib;

    return im;
}


void menu_listW(WINDOW* w, JVEC* vec, size_t top, size_t selected, size_t wdt, size_t hgt) {
    werase(w);
    box(w, 0, 0);
    for (size_t i = 0; i < hgt; i++) {
        mvprintw(i, 0, "%s", ((song*)JVEC_get(vec, i))->title);
    }
}



WINDOW* view_new() {
    // ncurses init
    initscr();

    // set up mousewheel input
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    keypad(stdscr, TRUE);

    // don't block on getch();
    timeout(0);

    return newwin(S_HEIGHT, S_WIDTH, 0, 0);
}

static inline void scroll_artist(imodel* im, int8_t dir) {
    JVEC* ats = im->lib->artists;

    // don't allow user to scroll out of bounds 
    if (dir == -1 && im->artist_idx == 0) {
        return;
    }
    if (dir == 1 && im->artist_idx == ats->len - 1) {
        return;
    }

    im->artist_idx += dir;
}

static inline void scroll_album(imodel* im, int8_t dir) {
    JVEC* abms = im->lib->albums;

    // don't allow user to scroll out of bounds 
    if (dir == -1 && im->album_idx == 0) {
        return;
    }
    if (dir == 1 && im->album_idx == abms->len - 1) {
        return;
    }

    im->album_idx += dir;
}

static inline void scroll_song(imodel* im, int8_t dir) {
    JVEC* sngs = im->lib->songs;

    // don't allow user to scroll out of bounds 
    if (dir == -1 && im->song_idx == 0) {
        return;
    }
    if (dir == 1 && im->song_idx == sngs->len - 1) {
        return;
    }

    im->song_idx += dir;
}

static inline void change_column(imodel* im, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (im->col_idx == 0 && dir == -1) || (im->col_idx == 2 && dir==1)) {
        return;
    }
    im->col_idx += dir;
