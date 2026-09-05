#include <music_player2.h>
#include <sqlite3.h>



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
    //debug_print_mem(mem);
    view_loop(mem);
    lib_mem_free(&mem);
}