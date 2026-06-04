#include <music_player2.h>
#include <sqlite3.h>



int main(int argc, char** argv) {
    lib_db lib_db;
    lib_db_init(&lib_db);
    scan_dir(&lib_db, argv[1]);
}