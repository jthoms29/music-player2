#include <music_player2.h>
#include <scrolling_menu.h>
#include <ncurses.h>
#include <assert.h>

#define MIN_ROWS 15
#define MIN_COLS 9

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


    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);

    curs_set(0);
    // block on getch();
    timeout(0);

    // make it so black squares can be drawn to screen
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

}

void resize_elements(elements* e) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (rows < 7 || cols < 3) {
        return;
    }
    // update size vals
    size_t sw = cols/3;
    size_t sh = rows - 6;

    menu_resize(e->menus[0], sh, sw, 0, 0);
    menu_resize(e->menus[1], sh, sw, 0, sw);
    menu_resize(e->menus[2], sh, sw, 0, sw*2);
    playback_resize(e->playback_menu, 6, sw*3, sh);
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

    wnoutrefresh(stdscr);
    menu_draw(e->menus[0]);
    menu_draw(e->menus[1]);
    menu_draw(e->menus[2]);
    playback_draw(e->playback_menu);
    doupdate();
    return 0;
}

