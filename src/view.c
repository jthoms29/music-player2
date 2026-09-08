#include <music_player2.h>
#include <ncurses.h>
#include <assert.h>


#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))




char* artist_string(void* atst) {
    if (!atst) {
        return NULL;
    }
    artist* _atst = (artist*) atst;
    return _atst->name;
}

char* album_string(void* abm) {
    if (!abm) {
        return NULL;
    }
    album* _abm = (album*) abm;
    return _abm->title;
}

char* song_string(void* sng) {
    if (!sng) {
        return NULL;
    }
    song* _sng = (song*) sng;
    return _sng->title;
}

void draw_selection_menu(WINDOW* w, model* mod, int8_t col_idx, size_t hgt, size_t wdt, char* (*str_func)(void*)) {
    werase(w);
    if (mod->col_idx == col_idx) {
        wattron(w, COLOR_PAIR(2));
        box(w, 0, 0);
        wattroff(w, COLOR_PAIR(2));
    }
    else {
        box(w, 0, 0);
    }

    // vector for current window
    JVEC* vec = mod->vecs[col_idx];

    size_t top = mod->row_top[col_idx];
    size_t selected = mod->row_idx[col_idx];
    char* str;

    assert(hgt >= 3);
    for (size_t i = 0; i < hgt-2; i++) {
    
        str = str_func(JVEC_get(vec, top+i));
        // if currently selected, invert colors of string
        if (top+i == selected && mod->col_idx == col_idx) {
            wattron(w, COLOR_PAIR(3));
            mvwaddnstr(w, i+1, 1, str, wdt);
            wattroff(w, COLOR_PAIR(3));
        }
        else if (top+i == selected) {
            wattron(w, COLOR_PAIR(1));
            mvwaddnstr(w, i+1, 1, str, wdt);
            wattroff(w, COLOR_PAIR(1));
        }
        else {
            mvwaddnstr(w, i+1, 1, str, wdt);
        }
    }
}

void draw_playback_menu(WINDOW* w, size_t hgt, size_t wdt) {
    werase(w);
    box(w, 0, 0);
}


void view_init() {
    // ncurses init
    initscr();

    // set up mousewheel input
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    keypad(stdscr, TRUE);

    curs_set(0);
    // block on getch();
    timeout(-1);

    // make it so black squares can be drawn to screen
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
}

int draw_screen(model* m, int resize, WINDOW* atst_menu, WINDOW* abm_menu, WINDOW* sng_menu, WINDOW* playback_win) {
    int rows, cols;
    static int menu_wdt, menu_hgt;
    getmaxyx(stdscr, rows, cols);
    
    // screen is currently too small
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        werase(stdscr);
        mvwprintw(stdscr, 0, 0, "Terminal too small. Please resize");
        wrefresh(stdscr);
        return -1;
    }

    if (resize) {
        // dynamically resize all windows accounting for current screen size
        menu_wdt = cols/3;
        menu_hgt = rows - 6;
        wresize(atst_menu, menu_hgt, menu_wdt);
        mvwin(atst_menu, 0, 0);
        wresize(abm_menu, menu_hgt, menu_wdt);
        mvwin(abm_menu, 0, menu_wdt);
        wresize(sng_menu, menu_hgt, menu_wdt);
        mvwin(sng_menu, 0, menu_wdt*2);
        wresize(playback_win, 6, menu_wdt*3);
        mvwin(playback_win, menu_hgt, 0);

        // TODO!!! change
        draw_selection_menu(atst_menu, m, 0, menu_hgt, menu_wdt, artist_string);
        draw_selection_menu(abm_menu, m, 1, menu_hgt, menu_wdt, album_string);
        draw_selection_menu(sng_menu, m, 2, menu_hgt, menu_wdt, song_string);
        draw_playback_menu(playback_win, 6, menu_wdt*3);
        wnoutrefresh(stdscr);
        wnoutrefresh(atst_menu);
        wnoutrefresh(abm_menu);
        wnoutrefresh(sng_menu);
        wnoutrefresh(playback_win);
        doupdate();
    }
}

