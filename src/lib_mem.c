#include <music_player2.h>

int artist_compare(const void* a1, const void* a2);
int album_compare(const void* a1, const void* a2);
int song_compare(const void* s1, const void* s2);

lib_mem* lib_mem_init(void) {
    lib_mem* lib = malloc(sizeof(*lib));
    if (!lib) {
        perror("Could not allocate in memory library");
        return NULL;
    }

    JVEC* artists = JVEC_new(NULL, artist_compare);
    JVEC* albums = JVEC_new(NULL, album_compare);
    JVEC* songs = JVEC_new(NULL, song_compare);

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
        JVEC* albums = JVEC_new(NULL, string_compare);
        atst->albums = albums;

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


int artist_compare(const void* a1, const void* a2) {
    artist* _a1 = (artist*) a1;
    artist* _a2 = (artist*) a2;
    return strcmp(_a1->name, _a2->name);
}

int album_compare(const void* a1, const void* a2) {
    album* _a1 = (album*) a1;
    album* _a2 = (album*) a2;

    return _a2->original_year - _a1->original_year;
}

int song_compare(const void* s1, const void* s2) {
    song* _s1 = (song*) s1;
    song* _s2 = (song*) s2;

    return _s2->track_num - _s1->track_num;
}