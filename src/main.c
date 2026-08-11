#include <music_player2.h>
#include <sqlite3.h>



int main(int argc, char** argv) {
    lib_db* lib_db = lib_db_new();
    //scan_dir(&lib_db, argv[1]);

    lib_mem* mem = lib_mem_new();
    scan_dir(lib_db, argv[1]);
    load_library(mem, lib_db);
    debug_print_mem(mem);
}