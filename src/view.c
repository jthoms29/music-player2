#include <music_player2.h>
#include <scrolling_menu.h>
#include <ncurses.h>
#include <assert.h>


#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))






void draw_playback_menu(WINDOW* w, size_t hgt, size_t wdt) {
    werase(w);
    box(w, 0, 0);
}


void ncurses_init() {

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

void resize_elements(elements* e) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // update size vals
    size_t sw = cols/3;
    size_t sh = rows - 6;

    menu_resize(e->menus[0], sh, sw, 0, 0);
    menu_resize(e->menus[1], sh, sw, 0, sw);
    menu_resize(e->menus[2], sh, sw, 0, sw*2);
    wresize(e->playback_win, 6, sw*3);
    mvwin(e->playback_win, sh, 0);
}

int draw_screen(elements* e) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    
    // screen is currently too small
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        werase(stdscr);
        mvwprintw(stdscr, 0, 0, "Terminal too small. Please resize");
        wrefresh(stdscr);
        return -1;
    }

    menu_draw(e->menus[0]);
    menu_draw(e->menus[1]);
    menu_draw(e->menus[2]);
    // TODO!!! change
    wnoutrefresh(stdscr);
    wnoutrefresh(e->menus[0]->win);
    wnoutrefresh(e->menus[1]->win);
    wnoutrefresh(e->menus[2]->win);
    doupdate();
    return 0;
}

