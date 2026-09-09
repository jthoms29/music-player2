#include <ncurses.h>
#include <../../JLib/src/JVEC.h>

typedef struct scrolling_menu {

    WINDOW* win;
    JVEC* vec;  // elements in menu
    size_t idx; // index of currently selected element
    size_t top; // topmost currently viewable element
    char* (*str_func) (void*); // function which prints string rep of element
} scrolling_menu;