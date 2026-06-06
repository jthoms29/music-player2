#include <music_player2.h>

int artist_compare(const void* a1, const void* a2);
int album_compare(const void* a1, const void* a2);
int song_compare(const void* s1, const void* s2);


lib_mem* lib_mem_init(void) {
    lib_mem* lib = calloc(1, sizeof(*lib));
    if (!lib) {
        perror("Could not allocate in memory library");
        return NULL;
    }

    JVEC* artists = JVEC_new(NULL, artist_compare);
    if (!artists) {
        fprintf(stderr, "Failed to create artists vector\n");
        return NULL;
    }

    JVEC* albums = JVEC_new(NULL, album_compare);
    if (!albums) {
        fprintf(stderr, "Failed to create albums vector\n");
        JVEC_free(&artists);
        return NULL;
    }

    JVEC* songs = JVEC_new(NULL, song_compare);
    if (!songs) {
        fprintf(stderr, "Failed to create songs vector\n");
        JVEC_free(&artists);
        JVEC_free(&albums);
        return NULL;
    }

    lib->artists = artists;
    lib->albums = albums;
    lib->songs = songs;

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
        
        JVEC* albums = JVEC_new(NULL, album_compare);
        atst->albums = albums;

        char* name_alloc = malloc(strlen(name) + 1);

        strcpy(name_alloc, name);
        atst->name = name_alloc;
        printf("%s\n", name_alloc);

        JVEC_append(vec, atst);
    }
    JVEC_sort(vec);

    return 0;
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


    return _a2->orig_date - _a1->orig_date;
}

int song_compare(const void* s1, const void* s2) {
    song* _s1 = (song*) s1;
    song* _s2 = (song*) s2;

    return _s2->track_num - _s1->track_num;
}

int free_artist(void* atst) {
    artist* _atst = (artist*) atst;
    free(_atst->name);
}