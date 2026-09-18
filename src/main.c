#include <complex.h>
#include <music_player2.h>
#include <playback_menu.h>
#include <scrolling_menu.h>
#include <sqlite3.h>
#include <ncurses.h>
#include <poll.h>
#include <errno.h>
#include <audio_playback.h>
#include <unistd.h>

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



    // setup audio
    audio_player* player = audio_new();

    struct pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;

    //?????
    fds[1].fd = player->notify_read_fd;
    fds[1].events = POLLIN;

    int draw_ret;
    int scrolled = 0;
    e->playback_menu->time_s = 0;




    while (!exit_flag) {
        draw_ret = draw_screen(e);
        // get input

        int pollret = poll(fds, 2, 1000000000);

        // poll doesn't like resize, ignore EINTR
        if (pollret < 0 && errno != EINTR) {
            perror("poll");
            break;
        }

        //timeout, update progress bar if applicable
        if (pollret == 0) {
        }
        if (fds[1].revents & POLLIN) {
            uint8_t signal;
            read(player->notify_read_fd, &signal, 1);
            playback_next_song(e->playback_menu);
            audio_load_song(player, e->playback_menu->cur_song);
            continue;
        }

        if (fds[0].revents & POLLIN) {
            ch = getch();
            switch(ch) {
                case KEY_RESIZE:
                    // Screen has been resized, redraw windows
                    resize_elements(e);
                    clear();
                    refresh();
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

                case 'p':
                    if (e->col_idx == 2) {
                        album* abm = menu_get_selected(e->menus[1]);
                        size_t idx = e->menus[2]->idx;
                        playback_change_song(e->playback_menu, abm->songs, idx);
                        audio_load_song(player, e->playback_menu->cur_song);
                    }

                    break;
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