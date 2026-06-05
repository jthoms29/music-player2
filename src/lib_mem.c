#include <music_player2.h>

lib_mem* lib_mem_init(void) {
    lib_mem* lib = malloc(sizeof(*lib));
    if (!lib) {
        perror("Could not allocate in memory library");
        return NULL;
    }

    JVEC* artists = JVEC_new(free, string_compare);
    JVEC* albums = JVEC_new(free, string_compare);
    JVEC* songs = JVEC_new(free, string_compare);

    return lib;
}

int load_artists(lib_mem* mem, lib_db* db) {

    JVEC* vec = mem->artists;
    int rc;
    sqlite3_stmt* pstmt;
    char* sql = "SELECT artist_id, name from artists;";

    char* name;
    int id;

    sqlite3* database = db->db;
    rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        id = sqlite3_column_int(pstmt, 0);
        name = (char*) sqlite3_column_text(pstmt, 1);
        artist* atst = calloc(1, sizeof(*atst));
        char* name_alloc = malloc(strlen(name) + 1);

        strcpy(name_alloc, name);
        atst->name = name_alloc;

        JVEC_append(vec, atst);

    }

    JVEC_sort(vec);
}

// load persistent library stored in sql database into memory
int load_library(lib_mem* mem, lib_db* db) {

}