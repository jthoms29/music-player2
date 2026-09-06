#include <music_player2.h>
#include <ncurses.h>
#include <assert.h>


#define MIN_ROWS 9
#define MIN_COLS 15
#define DRAW_PIXEL(w, x, y) mvwaddch(w, y, x, ' ' | COLOR_PAIR(1))

typedef struct imodel {
    uint8_t col_idx;
    size_t row_top[3];
    size_t row_idx[3];
    JVEC* vecs[3];
} imodel;


imodel* imodel_new(lib_mem* lib) {
    imodel* im = calloc(1, sizeof(*im));
    if (!im) {
        perror("imodel_new(): could not allocate new imodel");
        return NULL;
    }

    im->vecs[0] = lib->vecs[0];
    im->vecs[1] = lib->vecs[1];
    im->vecs[2] = lib->vecs[2];
    return im;
}

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

void menu_list(WINDOW* w, JVEC* vec, imodel* imod, int8_t col_idx, size_t hgt, size_t wdt, char* (*str_func)(void*)) {
    werase(w);
    if (imod->col_idx == col_idx) {
        wattron(w, COLOR_PAIR(2));
        box(w, 0, 0);
        wattroff(w, COLOR_PAIR(2));
    }
    else {
        box(w, 0, 0);
    }


    size_t top = imod->row_top[col_idx];
    size_t selected = imod->row_idx[col_idx];
    char* str;

    assert(hgt >= 3);
    for (size_t i = 0; i < hgt-2; i++) {
    
        str = str_func(JVEC_get(vec, top+i));
        // if currently selected, invert colors of string
        if (top+i == selected && imod->col_idx == col_idx) {
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

void playback_menu(WINDOW* w, size_t hgt, size_t wdt) {
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

static inline void scroll_menu(imodel* im, lib_mem* lib, int8_t dir, size_t rows) {
    int8_t vec_num = im->col_idx;

    JVEC* vec = im->vecs[vec_num];

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

    if (idx - top >= rows-2) {
        im->row_top[vec_num] = idx - (rows-2) + 1;
    }

    if (idx < top) {
        im->row_top[vec_num] = idx;
    }

    if (vec_num == 0) {
        im->vecs[1] =  ((artist*) JVEC_get(lib->vecs[0], idx))->albums;
        im->vecs[2] = ((album*) JVEC_get(im->vecs[1], 0))->songs;
        im->row_idx[1] = 0;
        im->row_idx[2] = 0;
        im->row_top[1] = 0;
        im->row_top[2] = 0;
    }
    if (vec_num == 1) {
        im->vecs[2] = ((album*) JVEC_get(lib->vecs[1], idx))->songs;
        im->row_idx[2] = 0;
        im->row_top[2] = 0;
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
    int menu_wdt = 0, menu_hgt = 0;

    ncurses_init();

    WINDOW *atst_menu, *abm_menu, *sng_menu, *playback_win;
    
    atst_menu = newwin(0, 0, 0, 0);
    abm_menu = newwin(0, 0, 0, 0);
    sng_menu = newwin(0, 0, 0, 0);
    playback_win = newwin(0, 0, 0, 0);

    // init imodel
    imodel* imod = imodel_new(lib);

    int ch;
    uint8_t exit_flag = 0;

    // flag to resize windows
    uint8_t resize = 1;
    
    while (!exit_flag) {
        getmaxyx(stdscr, rows, cols);
        if (rows < MIN_ROWS || cols < MIN_COLS) {
            // Terminal is too small to draw the UI.
            werase(stdscr);
            mvwprintw(stdscr, 0, 0, "Terminal too small. Please resize");
            wrefresh(stdscr);

            // Still read input so KEY_RESIZE can be received.
            ch = getch();

            if (ch == 'q' || ch == 'Q') {
                exit_flag = 1;
            }
            continue;
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


            resize = 0;
        }
        //display menus
        menu_list(atst_menu, imod->vecs[0], imod, 0, menu_hgt, menu_wdt, artist_string);
        menu_list(abm_menu, imod->vecs[1], imod, 1, menu_hgt, menu_wdt, album_string);
        menu_list(sng_menu, imod->vecs[2], imod, 2, menu_hgt, menu_wdt, song_string);
        playback_menu(playback_win, 6, menu_wdt*3);
        wnoutrefresh(stdscr);
        wnoutrefresh(atst_menu);
        wnoutrefresh(abm_menu);
        wnoutrefresh(sng_menu);
        wnoutrefresh(playback_win);
        doupdate();
        // get input
        ch = getch();

        switch(ch) {
            case KEY_RESIZE:
                // restart loop to redraw windows. Screen has been resized
                resize = 1;
                break;

            //scroll current win down
            case 'j':
            case 'J':
                scroll_menu(imod, lib, 1, menu_hgt);
                break;
            //scroll current win up
            case 'k':
            case 'K':
                scroll_menu(imod, lib, -1, menu_hgt);
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
    endwin();
}