#include <music_player2.h>
#include <assert.h>
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
        JVEC_free(&(*lib_ptr)->albums); 
    }
    if ((*lib_ptr)->songs) {
        JVEC_free(&(*lib_ptr)->songs); 
    }
    if ((*lib_ptr)->artists) {
        JVEC_free(&(*lib_ptr)->artists);
    }
    if ((*lib_ptr)->artists) {
        JVEC_free(&(*lib_ptr)->artists);
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

    uh_oh:

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

    album* abm;
    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        // get name now for error msgs
        text = (char*) sqlite3_column_text(pstmt, 3);
   
        abm = calloc(1, sizeof(*abm));
        if (!abm) {
            perror("load_albums(): failed to alloc album struct");
            return 1;
        }

        // alloc song vector
        JVEC* songs = JVEC_new(NULL, album_compare);
        if (!songs) {
            fprintf(stderr, "Failed to allocate song vector for %s\n", text);
            goto uh_oh;
        }
        abm->songs = songs;

        // album title
        abm->title = malloc(strlen(text) + 1);
        if (!abm->title) {
            perror("load_artists(): failed to alloc space for album title");
            goto uh_oh;
        }
        strcpy(abm->title, text);;

        // original release date
        text = (char*) sqlite3_column_text(pstmt, 5);
        abm->orig_date = malloc(strlen(text)+1);
        if (!abm->orig_date) {
            perror("load_artists(): failed to alloc space for orig_date");
            goto uh_oh;
        }
        strcpy(abm->orig_date, text);


        // issue release date
        text = (char*) sqlite3_column_text(pstmt, 4);
        abm->date = malloc(strlen(text)+1);
        if (!abm->date) {
            perror("load_artists(): failed to alloc space for date");
            goto uh_oh;
        }
        strcpy(abm->date, text);

        // album id
        int album_id = sqlite3_column_int(pstmt, 1);
        abm->album_id = album_id;

        // artist id 
        int artist_id = sqlite3_column_int(pstmt, 2);
        abm->artist_id = artist_id;

        // add to general albums vector
        JVEC_append(vec, abm);

        // also need to add to albums vector within artist
        artist* atst = JHASHMAP_get(artist_cache, CAST_INT(artist_id));
        assert(atst);
        JVEC_append(atst->albums, abm);

        // also need to add album to temp album cache. Allows songs to efficiently be associated with album
        JHASHMAP_add(album_cache, CAST_INT(album_id), abm);
        
    }
    
    // sort full album vector
    JVEC_sort(vec);

    // now within each artist the albums must be sorted
    JVEC* artists = mem->artists;
    size_t len = JVEC_len(artists);
    for (size_t i = 0; i < len; i++) {
        artist* atst = JVEC_get(artists, i);
        JVEC_sort(atst->albums);
    }

    return 0;

    uh_oh:
    // free album which caused failure
    free_album(abm);
    // then free the rest
    JVEC_free(&(mem->albums));
    return 1;
}

int load_songs(lib_mem* mem, lib_db* db) {
    JVEC* vec = mem->songs;

    JHASHMAP* album_cache = mem->album_cache;

    sqlite3_stmt* pstmt;
    char* sql = 
    "SELECT song_id, album_id, track_num, dur_s, bitrate, sample_rate, channels, "
    "title, path, date, orig_date, commen from songs;";
    
    // for textual sql returns
    char* text;

    sqlite3* database = db->db;
    int rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to load songs");
        return -1;
    }

    song* sng;
    while(sqlite3_step(pstmt) == SQLITE_ROW ) {

        sng = calloc(1, sizeof(*sng));
        if (!sng) {
            perror("load_songs(): failed to alloc song struct");
            return 1;
        }
       
        // title
        text = (char*) sqlite3_column_text(pstmt, 8);
        sng->title = malloc(strlen(text) + 1);
        if (!sng->title) {
            perror("load_songs(): failed to alloc space for song title");
            goto uh_oh;
        }
        strcpy(sng->title, text);

        // path
        text = (char*) sqlite3_column_text(pstmt, 9);
        sng->path = malloc(strlen(text) + 1);
        if (!sng->path) {
            perror("load_songs(): failed to alloc space for song path");
            goto uh_oh;
        }
        strcpy(sng->path, text);

        // date
        text = (char*) sqlite3_column_text(pstmt, 10);
        sng->date = malloc(strlen(text) + 1);
        if (!sng->date) {
            perror("load_songs(): failed to alloc space for song date");
            goto uh_oh;
        }
        strcpy(sng->date, text);

        // orig_date
        text = (char*) sqlite3_column_text(pstmt, 11);
        sng->orig_date = malloc(strlen(text) + 1);
        if (!sng->orig_date) {
            perror("load_songs(): failed to alloc space for song orig_date");
            goto uh_oh;
        }
        strcpy(sng->orig_date, text);

        // comment
        text = (char*) sqlite3_column_text(pstmt, 12);
        sng->comment = malloc(strlen(text) + 1);
        if (!sng->comment) {
            perror("load_songs(): failed to alloc space for song comment");
            goto uh_oh;
        }
        strcpy(sng->comment, text);

        // int fields
        sng->song_id = sqlite3_column_int(pstmt, 1);
        sng->album_id = sqlite3_column_int(pstmt, 2);
        sng->track_num = sqlite3_column_int(pstmt, 3);
        sng->dur_s = sqlite3_column_int(pstmt, 4);
        sng->bitrate = sqlite3_column_int(pstmt, 5);
        sng->sample_rate = sqlite3_column_int(pstmt, 6);
        sng->channels = sqlite3_column_int(pstmt, 7);

        // add to general songs vector
        JVEC_append(vec, sng);

        // also need to add to vector associated album
        album* abm = JHASHMAP_get(album_cache, CAST_INT(sng->album_id));
        assert(abm);

        // extra info associated with song, strings not allocated here so DON'T FREE WHEN FREEING SONG
        sng->artist_name = abm->artist;
        sng->album_title = abm->title;

        JVEC_append(abm->songs, sng);
    }

    // sort full song vector
    JVEC_sort(vec);

    // sort the songs within each album
    JVEC* albums = mem->albums;
    size_t len = JVEC_len(albums);
    for (size_t i = 0; i < len; i++) {
        album* abm = JVEC_get(albums, i);
        JVEC_sort(abm->songs);
    }

    return 0;

    uh_oh:
    // free song which caused failure
    free_song(sng);
    // then free the rest
    JVEC_free(&(mem->songs));
    return 1;
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
