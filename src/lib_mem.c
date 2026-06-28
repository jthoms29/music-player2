#include <music_player2.h>
#include <stdio.h>

int artist_compare(const void* a1, const void* a2);
int album_compare(const void* a1, const void* a2);
int song_compare(const void* s1, const void* s2);

void free_artist(artist* atst) {
    if (atst->name) {
        free(atst->name);
    }
    if (atst->albums) {
        JVEC_free(&(atst->albums));
    }
}

void free_album(album* abm) {
    if (abm->title) {
        free(abm->title);
    }
    if (abm->artist) {
        free(abm->artist);
    }
    if (abm->genre) {
        free(abm->genre);
    }
    if (abm->date) {
        free(abm->date);
    }
    if (abm->orig_date) {
        free(abm->orig_date);
    }
    if (abm->songs) {
        JVEC_free(&(abm->songs));
    }
}

void free_song(song* sng) {
    if (sng->path) {
        free(sng->path);
    }
    if (sng->title) {
        free(sng->title);
    }
    if (sng->artist) {
        free(sng->artist);
    }
    if (sng->album) {
        free(sng->album);
    }
    if (sng->genre) {
        free(sng->genre);
    }
    if (sng->date) {
        free(sng->date);
    }
    if (sng->orig_date) {
        free(sng->orig_date);
    }
}

void lib_mem_free(lib_mem** lib_ptr) {
    if (*lib_ptr == NULL) {
        return;
    }

    if ((*lib_ptr)->artists) {
        JVEC_free(&(*lib_ptr)->artists); // :-)
    }
    if ((*lib_ptr)->albums) {
        JVEC_free(&(*lib_ptr)->albums); // :-)
    }
    if ((*lib_ptr)->songs) {
        JVEC_free(&(*lib_ptr)->songs); // :-)
    }
    if ((*lib_ptr)->artists) {
        JVEC_free(&(*lib_ptr)->artists); // :-)
    }
    if ((*lib_ptr)->artists) {
        JVEC_free(&(*lib_ptr)->artists); // :-)
    }

    free(*lib_ptr);
    *lib_ptr = NULL;
}


lib_mem* lib_mem_init(void) {
    lib_mem* lib = calloc(1, sizeof(*lib));
    if (!lib) {
        perror("Could not allocate in memory library");
        return NULL;
    }

    lib->artists = JVEC_new(NULL, artist_compare);
    if (!lib->artists) {
        fprintf(stderr, "Failed to create artists vector\n");
        goto uh_oh;
    }

    lib->albums = JVEC_new(NULL, album_compare);
    if (!lib->albums) {
        fprintf(stderr, "Failed to create albums vector\n");
        goto uh_oh;
    }

    lib->songs = JVEC_new(NULL, song_compare);
    if (!lib->songs) {
        fprintf(stderr, "Failed to create songs vector\n");
        goto uh_oh;
    }


    lib->artist_cache = JHASHMAP_new(int_hash, int_compare);
    if (!lib->artist_cache) {
        fprintf(stderr, "Failed to create artist cache\n");
        goto uh_oh;
    }

    lib->album_cache = JHASHMAP_new(int_hash, int_compare);
    if (!lib->album_cache) {
        fprintf(stderr, "Failed to create album cache\n");
        goto uh_oh;
    }

    return lib;

    uh_oh:
    lib_mem_free(&lib);
    return NULL;
}

int load_artists(lib_mem* mem, lib_db* db) {
    JVEC* vec = mem->artists;
    JHASHMAP* cache = mem->artist_cache;

    sqlite3_stmt* pstmt;
    char* sql = "SELECT artist_id, name from artists;";

    char* name;
    int id;

    sqlite3* database = db->db;
    int rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to load artists\n");
        return -1;
    }

    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        id = sqlite3_column_int(pstmt, 0);
        name = (char*) sqlite3_column_text(pstmt, 1);

        artist* atst = calloc(1, sizeof(*atst));
        if (!atst) {
            perror("load_artists(): failed to alloc artist struct");
            return 1;
        }

        // create an album vector for this artist
        JVEC* albums = JVEC_new(NULL, album_compare);
        if (!albums) {
            fprintf(stderr, "Failed to create album vector for %s\n", name);
            free(atst);
            return 1;
        }
        atst->albums = albums;

        // space for artist's name from sql ret
        char* name_alloc = malloc(strlen(name) + 1);
        if (!name_alloc) {
            perror("load_artists(): failed to allocate space for artist name");
            free(atst);
            return 1;
        }
        strcpy(name_alloc, name);
        atst->name = name_alloc;

        JVEC_append(vec, atst);

        // add to temporary cache indexed by sql key. Allows albums to be associated with artist efficiently
        JHASHMAP_add(cache, CAST_INT(id), atst);
    }

    // put all artists in alphabetical order
    JVEC_sort(vec);
    return 0;
}

int load_albums(lib_mem* mem, lib_db* db) {
    JVEC* vec = mem->albums;

    JHASHMAP* artist_cache = mem->artist_cache;
    JHASHMAP* album_cache = mem->album_cache;


    sqlite3_stmt* pstmt;
    char* sql = "SELECT album_id, artist_id, title, date, orig_date from albums;";

    // from sql ret
    char* text;

    sqlite3* database = db->db;
    int rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to load albums\n");
        return -1;
    }

    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        // get name now for error msgs
        text = (char*) sqlite3_column_text(pstmt, 3);
    

        album* abm = calloc(1, sizeof(*abm));
        if (!abm) {
            perror("load_albums(): failed to alloc album struct");
            return 1;
        }

        // alloc song vector
        JVEC* songs = JVEC_new(NULL, album_compare);
        if (!songs) {
            fprintf(stderr, "Failed to allocate song vector for %s\n", text);
            free(abm);
            return 1;
        }
        abm->songs = songs;

        // aloc space for name string
        char* title = malloc(strlen(text) + 1);
        if (!title) {
            perror("load_artists(): failed to alloc space for album title");
            free(songs);
            free(abm);
            return 1;
        }
        strcpy(title, text);;
        abm->title = title;

        // original release date
        text = (char*) sqlite3_column_text(pstmt, 5);
        char* orig_date = malloc(strlen(text)+1);
        if (!orig_date) {
            perror("load_artists(): failed to alloc space for orig_date");
            free(title);
            free(songs);
            free(abm);
            return 1;
        }
        strcpy(orig_date, text);
        abm->orig_date = orig_date;

        // issue release date
        text = (char*) sqlite3_column_text(pstmt, 4);
        char* date = malloc(strlen(text)+1);
        if (!date) {
            perror("load_artists(): failed to alloc space for date");
            free(title);
            free(orig_date);
            free(songs);
            free(abm);
            return 1;
        }
        strcpy(date, text);
        abm->date = text;

        // album id
        uint8_t album_id = sqlite3_column_int(pstmt, 1);
        abm->album_id = album_id;

        // artist id 
        uint8_t artist_id = sqlite3_column_int(pstmt, 2);
        abm->artist_id = artist_id;

        // add to general albums vector
        JVEC_append(vec, abm);

        // also need to add to albums vector within artist
        artist* atst = JHASHMAP_get(artist_cache, CAST_INT(artist_id));
        JVEC_append(atst->albums, abm);

        // also need to add album to temp album cache. Allows songs to efficiently be associated with album
        JHASHMAP_add(album_cache, CAST_INT(album_id), abm);
        
    }
    // sort full album vector
    JVEC_sort(vec);

    // now within each artist, the albums must be sorted
    JVEC* artists = mem->artists;
    size_t len = JVEC_len(artists);
    for (size_t i = 0; i < len; i++) {
        artist* atst = JVEC_get(artists, i);
        JVEC_sort(atst->albums);
    }

    return 0;
}

int load_songs(lib_mem* mem, lib_db* db) {
    JVEC* vec = mem->songs;

    JHASHMAP* album_cache = mem->album_cache;

    sqlite3_stmt* pstmt;
    char* sql = 
    "SELECT song_id, album_id, title, track_num, dur_s, bitrate, sample_rate, channels, " 
    "comment, path from songs;";
    
    // for textual sql returns
    char* text;

    sqlite3* database = db->db;
    int rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to load songs");
        return -1;
    }

    while(sqlite3_step(pstmt) == SQLITE_ROW ) {

        song* sng = calloc(1, sizeof(*sng));
        if (!sng) {
            perror("load_songs(): failed to alloc song struct");
            return 1;
        }
        
        text = (char*) sqlite3_column_text(pstmt, 3);
        char* title = malloc(strlen(text) + 1);
        if (!title)
    }





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