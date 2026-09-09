#include <music_player2.h>
#include <sqlite3.h>
#include <ncurses.h>


void main_loop(lib_mem* lib) {
    int rows, cols;
    size_t menu_wdt = 0, menu_hgt = 0;

    view* v = view_new();

    model* m = model_new(lib);
   
    resize_elements(v);

    int ch;
    uint8_t exit_flag = 0;

    // flag to resize windows
    int resize = 1;



    int draw_ret;
    while (!exit_flag) {
        draw_ret = draw_screen(v, m);
        // get input
        ch = getch();
        switch(ch) {
            case KEY_RESIZE:
                // Screen has been resized, redraw windows
                resize_elements(v);
                break;

            //scroll current win down
            case 'j':
            case 'J':
                scroll_menu(m, v, lib, 1);
                break;
            //scroll current win up
            case 'k':
            case 'K':
                scroll_menu(m, v, lib, -1);
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