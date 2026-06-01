#include <music_player2.h>
#include <sqlite3.h>

int DB_setup(sqlite3* lib_db) {
    sqlite3_open("lib.db", &lib_db);

    const char* artist_table = "CREATE TABLE ARTIST"
                         "ID "
                         "";
    const char* album_table = "SONG";

    int exit = 0;
    sqlite3_exec(lib_db, artist_table, NULL, 0)
}




int main(void) {
}