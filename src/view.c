#include <music_player2.h>
#include <ncurses.h>

#define S_HEIGHT 30
#define S_WIDTH 10

#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))

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

char* album_string(void* abm) {
    if (!abm) {
        return NULL;
    }
    
    album* _abm = (album*) abm
}

void menu_listW(WINDOW* w, JVEC* vec, size_t top, size_t selected, size_t wdt, size_t hgt) {
    werase(w);
    box(w, 0, 0);
    for (size_t i = 0; i < hgt; i++) {

        // if currently selected, invert colors of string
        if (i == selected) {
            for (size_t j = 0; j < wdt; j++) {
                wattron(w, COLOR_PAIR(1));
                mvwaddnstr(w, i, 0, ((song*)JVEC_get(vec, i))->title, wdt);
                wattroff(w, COLOR_PAIR(1));
            }
        }
        mvwaddnstr(w,i, 0, ((song*)JVEC_get(vec, i))->title, wdt);
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

    // make it so black squares can be drawn to screen
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

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
}

void view_loop() {
    int rows, cols;
    int menu_wdt, menu_hgt;
    
    WINDOW *atst_menu, *abm_menu, *sng_menu;
    
    atst_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);
    abm_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);
    sng_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);

    for(;;) {
        // dynamically resize all windows accounting for current screen size
        getmaxyx(stdscr, rows, cols);
        menu_wdt = cols/3;
        menu_hgt = rows; //placeholder

    }
}