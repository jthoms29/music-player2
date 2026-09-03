#include <music_player2.h>
#include <ncurses.h>

#define S_HEIGHT 30
#define S_WIDTH 10

#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))

typedef struct imodel {
    uint8_t col_idx;

    size_t row_top[3];
    size_t row_idx[3];



} imodel;

imodel* imodel_new() {
    imodel* im = calloc(1, sizeof(*im));
    if (!im) {
        perror("imodel_new(): could not allocate new imodel");
        return NULL;
    }


    return im;
}

char* album_string(void* abm) {
    if (!abm) {
        return NULL;
    }
    
    album* _abm = (album*) abm;
    return NULL;
}

void menu_list(WINDOW* w, JVEC* vec, imodel* imod, int8_t col_idx, size_t hgt, size_t wdt) {
    werase(w);
    box(w, 0, 0);

    size_t top = imod->row_top[col_idx];
    size_t selected = imod->row_idx[col_idx];
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

static inline void scroll_menu(imodel* im, lib_mem* lib, int8_t dir, size_t rows) {
    int8_t vec_num = im->col_idx;

    JVEC* vec = lib->vecs[vec_num];

    // don't allow user to scroll out of bounds 
    if (dir == -1 && im->row_idx[vec_num] == 0) {
        return;
    }
    if (dir == 1 && im->row_idx[vec_num] == vec->len - 1) {
        return;
    }

    // update current column's row index to reflect user input
    im->row_idx[vec_num] += dir;

    // recompute top of currently visible list in window
    size_t idx = im->row_idx[vec_num];
    size_t top = im->row_top[vec_num];

    if (idx - top > rows) {
        im->row_top[vec_num] = idx - top;
    }

    if (idx < top) {
        im->row_top[vec_num] = idx;
    }

}


static inline void change_column(imodel* im, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (im->col_idx == 0 && dir == -1) || (im->col_idx == 2 && dir==1)) {
        return;
    }
    im->col_idx += dir;
}

void view_loop(lib_mem* lib) {
    int rows, cols;
    int menu_wdt, menu_hgt;

    JVEC* atsts = lib->vecs[0];
    JVEC* abms = lib->vecs[1];
    JVEC* sngs = lib->vecs[2];
    
    WINDOW *atst_menu, *abm_menu, *sng_menu;
    
    atst_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);
    abm_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);
    sng_menu = newwin(S_HEIGHT, S_WIDTH, 0, 0);

    // init imodel
    imodel* imod = imodel_new();

    int ch;
    uint8_t exit_flag = 0;
    while (exit_flag) {
        // dynamically resize all windows accounting for current screen size
        getmaxyx(stdscr, rows, cols);
        menu_wdt = cols/3;
        menu_hgt = rows; //placeholder
        wresize(atst_menu, rows, cols);
        wresize(abm_menu, rows, cols);
        wresize(sng_menu, rows, cols);

        // render all windows

        //display menus
        menu_list(atst_menu, atsts, imod, 0, rows, cols);
        menu_list(abm_menu, abms, imod, 1, rows, cols);
        menu_list(sng_menu, sngs, imod, 2, rows, cols);

        // get input
        ch = getch();

        switch(ch) {
            //scroll current win down
            case 'k':
            case 'K':
                scroll_menu(imod, lib, 1, rows);
                break;
            //scroll current win up
            case 'j':
            case 'J':
                scroll_menu(imod, lib, -1, rows);
                break;
            //move to prev column
            case 'h':
            case 'H':
                change_column(imod, -1);
                break;
            // move to next column
            case 'l':
            case 'L':
                change_column(imod, 1);
                break;

            // exit program
            case 'q':
            case 'Q':
                exit_flag = 1;
                break;
        }

    }
}