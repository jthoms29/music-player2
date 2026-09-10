#ifndef SCROLLING_MENU_H
#define SCROLLING_MENU_H

#include <ncurses.h>
#include <stdint.h>
#include <../../JLib/src/JVEC.h>

typedef struct scrolling_menu {

    bool focused;
    WINDOW* win;
    JVEC* vec;  // elements in menu
    size_t idx; // index of currently selected element
    size_t top; // topmost currently viewable element
    char* (*str_func) (void*); // function which prints string rep of element

    size_t height;
    size_t width;
} scrolling_menu;


scrolling_menu* menu_new(JVEC* vec, char* (*str_func)(void*));

void menu_scroll(scrolling_menu* m, int dir);

void menu_draw(scrolling_menu* m);


void menu_resize(scrolling_menu* m, size_t hgt, size_t wdt, size_t y_pos, size_t x_pos);
void menu_focus(scrolling_menu* m);

void menu_unfocus(scrolling_menu* m);
void* menu_get_element(scrolling_menu* m);

#endif