#include <music_player2.h>
#include <sqlite3.h>
#include <ncurses.h>


void main_loop(lib_mem* lib) {
    int rows, cols;
    int menu_wdt = 0, menu_hgt = 0;

    view_init();

    WINDOW *atst_menu, *abm_menu, *sng_menu, *playback_win;
    
    atst_menu = newwin(0, 0, 0, 0);
    abm_menu = newwin(0, 0, 0, 0);
    sng_menu = newwin(0, 0, 0, 0);
    playback_win = newwin(0, 0, 0, 0);

    // init mel
    model* m = model_new(lib);

    int ch;
    uint8_t exit_flag = 0;

    // flag to resize windows
    uint8_t resize = 1;

    int draw_ret;
    while (!exit_flag) {

        draw_ret = draw_screen(m, resize, atst_menu, abm_menu, sng_menu, playback_win);
        // terminal window is currently too small
        if (draw_ret == -1) {
            // Still read input so KEY_RESIZE can be received.
            ch = getch();

            if (ch == 'q' || ch == 'Q') {
                exit_flag = 1;
            }
            continue;
        }
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
                scroll_menu(m, lib, 1, menu_hgt);
                break;
            //scroll current win up
            case 'k':
            case 'K':
                scroll_menu(m, lib, -1, menu_hgt);
                break;
            //move to prev column
            case 'h':
            case 'H':
                change_column(m, -1);
                break;
            // move to next column
            case 'l':
            case 'L':
                change_column(m, 1);
                break;

            // exit program
            case 'q':
            case 'Q':
                exit_flag = 1;
                break;
            
        }
        resize = 0;

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