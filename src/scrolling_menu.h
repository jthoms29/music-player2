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

    int height;
    int width;
} scrolling_menu;


/**
 * @brief Create a new scrolling_menu for Ncurses
 * @details Allocate new scrolling_menu struct, set up all ncurses elements within
 * @note Must be freed using `menu_free()`
 * @param[in] vec A JVEC which the menu will visualize
 * @param[in] str_func a function which returns a string representation of the given JVEC's elements
 * @pre Ncurses must be initialized
 * @post New scrolling_menu struct allocated, returned
 * @return Pointer to scrolling_menu struct on success, NULL on failure
 */
scrolling_menu* menu_new(JVEC* vec, char* (*str_func)(void*));


/**
 * @brief Free scrolling_menu struct
 * @param[in, out] menu_ptr indirect pointer to scrolling_menu struct
 * @pre  menu_ptr must be NULL or been create with `menu_new()`
 * @post Ncurses elements within struct deinitialized, struct freed and set to NULL
 * @return None
 */
void menu_free(scrolling_menu** menu_ptr);


/**
 * @brief Scroll the given menu up or down
 * @details Increase or decrease the current selected index of the menu
 * @param[in, out] m scrolling_menu struct
 * @param[in] dir direction the user wishes to scroll menu. -1 for up, 1 for down
 * @pre m must have been created with `menu_new()`, direction must be -1 or 1
 * @post If m's index isn't currently at bound, index is incremented/decremented,
 * topmost viewable element adjusted accounting for current window size
 * @return None
 */
void menu_scroll(scrolling_menu* m, int dir);

/**
 * @brief Draw current state of scrolling_menu in ncurses
 * @details Primes display of menu to be visualized. Struct window must subsequently be
 * actually displayed with `doupdate()`. 
 * @note stdscr should be refreshed with `wnoutrefresh()` beforehand
 * @param[in] m scrolling_menu to be visualized
 * @pre Ncurses must be initialized, COLOR_PAIRs 1,2, and 3 must be defined,
 * m's current height and width must be greater than 2 characters
 * @post Current menu state is drawn and refreshed, ready to be shown with `doupdate()`
 * @return 0 on success, anything else on failure
 */
int menu_draw(scrolling_menu* m);


/**
 * @brief Resize and reposition scrolling_menu with given values
 * @param[in, out] m scrolling_menu to be resized and moves
 * @param[in] hgt new height for m
 * @param[in] wdt new width for m
 * @param[in] y_pos new y position for m
 * @param[in] x_pos new x position for m
 * @pre hgt, wdt, y_pos, x_pos must all be non-negative
 * @post m's size and position values are updated to reflect arguments,
 * menu's window is resized and repositioned in ncurses
 * @return None
 */
void menu_resize(scrolling_menu* m, int hgt, int wdt, int y_pos, int x_pos);


/**
 * @brief Set scrolling_menu to be focused
 * @param[out] m scrolling_menu struct
 * @pre m must have been created with `menu_new()`
 * @post m's focused member is set to true
 * @return None
 */
void menu_focus(scrolling_menu* m);


/**
 * @brief Set scrolling_menu to be unfocused
 * @param[out] m scrolling_menu struct
 * @pre m must have been created with `menu_new()`
 * @post m's focused member is set to false
 * @return None
 */
void menu_unfocus(scrolling_menu* m);


/**
 * @brief Return element from vector which is currently selected in menu
 * @param[in] m scrolling_menu struct
 * @pre m must have been created with `menu_new()`, internal vector must be non-NULL, 
 * m's current index must be within internal vector's bounds
 * @post current selected element in menu is returned
 * @return selected element on success, NULL on failure
 */
void* menu_get_selected(scrolling_menu* m);


/**
 * @brief Change the JVEC which the scrolling_menu visualizes
 * @param[out] m scrolling_menu to change vector in 
 * @param[in] vec new JVEC to be visualized
 * @pre m must be created with `menu_new()`
 * @post m's vector set to vec, idx and topmost viewable element values reset
 * @return 0 on success, anything else on failure
 */
int menu_change_vec(scrolling_menu* m, JVEC* vec);


#endif