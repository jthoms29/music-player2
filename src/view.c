#include <music_player2.h>
#include <scrolling_menu.h>
#include <ncurses.h>
#include <assert.h>


#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))






void draw_playback_menu(WINDOW* w, size_t hgt, size_t wdt) {
    werase(w);
    box(w, 0, 0);
}


view* view_new() {
    view* v = calloc(1, sizeof(*v));
    if (!v) {
        perror("ougghhh");
    }

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

    // initialize windows
    // artist menu
    v->selection_menu[0] = newwin(0, 0, 0, 0);
    // album menu
    v->selection_menu[1] = newwin(0, 0, 0, 0);
    // songs menu
    v->selection_menu[2] = newwin(0, 0, 0, 0);
    v->playback_win = newwin(0, 0, 0, 0);
    return v;
}

void resize_elements(view* v) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // update size vals
    v->selection_wdt = cols/3;
    v->selection_hgt = rows - 6;

    int sw = v->selection_wdt, sh = v->selection_hgt;
    wresize(v->selection_menu[0], sh, sw);
    mvwin(v->selection_menu[0], 0, 0);
    wresize(v->selection_menu[1], sh, sw);
    mvwin(v->selection_menu[1], 0, sw);
    wresize(v->selection_menu[2], sh, sw);
    mvwin(v->selection_menu[2], 0, sw*2);
    wresize(v->playback_win, 6, sw*3);
    mvwin(v->playback_win, sh, 0);
}

int draw_screen(view* v, model* m) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    // screen is currently too small
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        werase(stdscr);
        mvwprintw(stdscr, 0, 0, "Terminal too small. Please resize");
        wrefresh(stdscr);
        return -1;
    }


    WINDOW* atst_menu = v->selection_menu[0];
    WINDOW* abm_menu = v->selection_menu[1];
    WINDOW* sng_menu = v->selection_menu[2];
    WINDOW* playback_win = v->playback_win;
    int sh = v->selection_hgt, sw = v->selection_wdt;
    // TODO!!! change
    draw_selection_menu(atst_menu, m, v, 0, sh, sw, artist_string);
    draw_selection_menu(abm_menu, m, v, 1, sh, sw, album_string);
    draw_selection_menu(sng_menu, m, v, 2, sh, sw, song_string);
    draw_playback_menu(playback_win, 6, sw*3);
    wnoutrefresh(stdscr);
    wnoutrefresh(atst_menu);
    wnoutrefresh(abm_menu);
    wnoutrefresh(sng_menu);
    wnoutrefresh(playback_win);
    doupdate();
}

