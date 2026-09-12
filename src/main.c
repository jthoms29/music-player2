#include <music_player2.h>
#include <sqlite3.h>
#include <ncurses.h>

void main_loop(lib_mem* lib) {
    int rows, cols;
    size_t menu_wdt = 0, menu_hgt = 0;

    ncurses_init();
    elements* e = elements_new(lib);
   
    resize_elements(e);

    int ch;
    uint8_t exit_flag = 0;

    // flag to resize windows
    int resize = 1;



    int draw_ret;
    int scrolled = 0;
    while (!exit_flag) {
        draw_ret = draw_screen(e);
        // get input
        ch = getch();
        switch(ch) {
            case KEY_RESIZE:
                // Screen has been resized, redraw windows
                resize_elements(e);
                break;

            //scroll current win down
            case 'j':
            case 'J':
                menu_scroll(e->menus[e->col_idx], 1);
                scrolled = 1;
                break;
            //scroll current win up
            case 'k':
            case 'K':
                menu_scroll(e->menus[e->col_idx], -1);
                scrolled = 1;
                break;
            //move to prev column
            case 'h':
            case 'H':
                change_column(e, -1);
                break;
            // move to next column
            case 'l':
            case 'L':
                change_column(e, 1);
                break;

            // exit program
            case 'q':
            case 'Q':
                exit_flag = 1;
                break;

            case KEY_ENTER:
                if (e->col_idx == 2)
        }
        if (scrolled) {

            if (e->col_idx == 0) {
                JVEC* albums_vec = ((artist*) menu_get_selected(e->menus[0]))->albums;
                menu_change_vec(e->menus[1], albums_vec);
            }
            if (e->col_idx < 2) {
                JVEC* songs_vec = ((album*)menu_get_selected(e->menus[1]))->songs;
                menu_change_vec(e->menus[2], songs_vec);
            }
            scrolled = 0;

        }
    }
    endwin();
}

int main(int argc, char** argv) {
    lib_db* lib_db = lib_db_new();
    //scan_dir(&lib_db, argv[1]);

    lib_mem* mem = lib_mem_new();
    scan_dir(lib_db, argv[1]);
    if (load_library(mem, lib_db)) {
        lib_mem_free(&mem);
        printf("failed\n");
        return 1;
    }
    lib_db_free(&lib_db);
    //debug_print_mem(mem);
    main_loop(mem);
    lib_mem_free(&mem);
}