#include <ncurses.h>
#include <scrolling_menu.h>
#include <assert.h>

scrolling_menu* menu_new(JVEC* vec, char* (*str_func)(void*)) {
    scrolling_menu* m = calloc(1, sizeof(*m));
    if (!m) {
        perror("menu_new(): failed to allocate new scrolling menu");
        return NULL;
    }

    WINDOW* w = newwin(0, 0, 0, 0);
    if (!w) {
        fprintf(stderr, "menu_new(): Failed to create new ncurses window");
        free(m);
        return NULL;
    }

    m->win = w;
    m->vec = vec;
    m->str_func = str_func;

    return m;
}


void menu_free(scrolling_menu** menu_ptr) {
    if (!(*menu_ptr)) { return; }

    // uninit ncurses window
    if ((*menu_ptr)->win) {
        delwin((*menu_ptr)->win);
    }

    // free struct itself
    free(*menu_ptr);
    *menu_ptr = NULL;
}


void menu_scroll(scrolling_menu* m, int dir) {
    JVEC* vec = m->vec;
    size_t rows = m->height;
    
    // don't allow user to scroll out of bounds 
    if (dir == -1 && m->idx == 0) {
        return;
    }
    if (dir == 1 && m->idx == JVEC_len(vec) - 1) {
        return;
    }

    // update row index to reflect user input
    m->idx += dir;

    // if idx less than top, move top up
    if (m->idx < m->top) {
        m->top = m->idx;
    }
    // if difference between top and idx greater than height, move top down
    else if (m->idx - m->top >= rows-2) {
        m->top = m->idx - (rows-2) + 1;
    }

}

int menu_draw(scrolling_menu* m) {
    WINDOW* w = m->win;
    werase(w);

    // if this menu is currently selected, draw with coloured border
    if (m->focused) {
        wattron(w, COLOR_PAIR(2));
        box(w, 0, 0);
        wattroff(w, COLOR_PAIR(2));
    }
    else {
        box(w, 0, 0);
    }

    // vector for current window
    JVEC* vec = m->vec;
    int hgt = m->height;
    int idx = m->idx, top = m->top, wdt = m->width;
    char* str;

    // window currently not large enough to visualize menu
    if (hgt <= 2 || wdt <= 2) {
        return 1;
    }

    // print vector elements currently visible within window
    for (int i = 0; i < hgt-2; i++) {
        str = m->str_func(JVEC_get(vec, top+i));
        // if both element and window currently selected
        if (top+i == idx && m->focused) {
            wattron(w, COLOR_PAIR(3));
            mvwaddnstr(w, i+1, 1, str, wdt-2);
            wattroff(w, COLOR_PAIR(3));
        }
        // element selected but not window
        else if (top+i == idx) {
            wattron(w, COLOR_PAIR(1));
            mvwaddnstr(w, i+1, 1, str, wdt-2);
            wattroff(w, COLOR_PAIR(1));
        }
        // element not selected
        else {
            mvwaddnstr(w, i+1, 1, str, wdt-2);
        }
    }
    // refresh window
    wnoutrefresh(w);
}

void menu_resize(scrolling_menu* m, int hgt, int wdt, int y_pos, int x_pos) {
    m->height = hgt;
    m->width = wdt;
    wresize(m->win, hgt, wdt);
    mvwin(m->win, y_pos, x_pos);
}

void menu_focus(scrolling_menu* m) {
    m->focused = true;
}

void menu_unfocus(scrolling_menu* m) {
    m->focused = false;
}

void* menu_get_selected(scrolling_menu* m) {
    assert(m->idx <= JVEC_len(m->vec));
    return JVEC_get(m->vec, m->idx);
}

int menu_change_vec(scrolling_menu* m, JVEC* vec) {

    m->vec = vec;
    m->idx = 0;
    m->top = 0;
    return 0;
}