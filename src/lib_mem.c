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
    free(*lib_ptr);
    *lib_ptr = NULL;
}


lib_mem* lib_mem_new(void) {
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
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to load artists %s\n", sqlite3_errmsg(database));
        return -1;
    }


    artist* atst;
    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        id = sqlite3_column_int(pstmt, 0);
        name = (char*) sqlite3_column_text(pstmt, 1);

        atst = calloc(1, sizeof(*atst));
        if (!atst) {
            perror("load_artists(): failed to alloc artist struct");
            goto uh_oh;
        }

        // create an album vector for this artist
        JVEC* albums = JVEC_new(NULL, album_compare);
        if (!albums) {
            fprintf(stderr, "Failed to create album vector for %s\n", name);
            goto uh_oh;
        }
        atst->albums = albums;

        // space for artist's name from sql ret
        char* name_alloc = malloc(strlen(name) + 1);
        if (!name_alloc) {
            perror("load_artists(): failed to allocate space for artist name");
            goto uh_oh;
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
    // free current artist that caused error
    free_artist(atst);
    return 1;
}

void print_album(album* abm);
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
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to load albums\n");
        return -1;
    }

    album* abm;
    while(sqlite3_step(pstmt) == SQLITE_ROW) {
        // get name now for error msgs
        text = (char*) sqlite3_column_text(pstmt, 2);
        if (!text) {
            fprintf(stderr, "load_albums(): Failed to get album title\n");
            goto uh_oh;
        }
   
        abm = calloc(1, sizeof(*abm));
        if (!abm) {
            perror("load_albums(): failed to alloc album struct");
            return 1;
        }

        // alloc song vector
        JVEC* songs = JVEC_new(NULL, song_compare);
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
        strcpy(abm->title, text);

        // original release date
        text = (char*) sqlite3_column_text(pstmt, 4);
        abm->orig_date = malloc(strlen(text)+1);
        if (!abm->orig_date) {
            perror("load_artists(): failed to alloc space for orig_date");
            goto uh_oh;
        }
        strcpy(abm->orig_date, text);


        // issue release date
        text = (char*) sqlite3_column_text(pstmt, 3);
        abm->date = malloc(strlen(text)+1);
        if (!abm->date) {
            perror("load_artists(): failed to alloc space for date");
            goto uh_oh;
        }
        strcpy(abm->date, text);

        // album id
        int album_id = sqlite3_column_int(pstmt, 0);
        abm->album_id = album_id;

        // artist id 
        int artist_id = sqlite3_column_int(pstmt, 1);
        abm->artist_id = artist_id;


        // also need extra info from artist
        artist* atst = JHASHMAP_get(artist_cache, CAST_INT(artist_id));
        assert(atst);
        abm->artist_name = atst->name;

        // add to general albums vector
        JVEC_append(vec, abm);
        // add to artist's album vector
        JVEC_append(atst->albums, abm);
        // also need to add album to temp album cache. Allows songs to efficiently be associated with album
        JHASHMAP_add(album_cache, CAST_INT(album_id), abm);
        
    }
   
    
    // sort full album vector
    printf("%ld vec len\n", vec->len);
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
    return 1;
}

void print_song(song* sng);

int load_songs(lib_mem* mem, lib_db* db) {
    JVEC* vec = mem->songs;

    JHASHMAP* album_cache = mem->album_cache;

    sqlite3_stmt* pstmt;
    char* sql = 
    "SELECT song_id, album_id, track_num, dur_s, bitrate, sample_rate, channels, "
    "title, path, comment from songs;";
    
    // for textual sql returns
    char* text;

    sqlite3* database = db->db;
    int rc = sqlite3_prepare_v2(database, sql, -1, &pstmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to load songs\n");
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
        text = (char*) sqlite3_column_text(pstmt, 7);
        sng->title = malloc(strlen(text) + 1);
        if (!sng->title) {
            perror("load_songs(): failed to alloc space for song title");
            goto uh_oh;
        }
        strcpy(sng->title, text);

        // path
        text = (char*) sqlite3_column_text(pstmt, 8);
        sng->path = malloc(strlen(text) + 1);
        if (!sng->path) {
            perror("load_songs(): failed to alloc space for song path");
            goto uh_oh;
        }
        strcpy(sng->path, text);


        // comment
        text = (char*) sqlite3_column_text(pstmt, 9);
        sng->comment = malloc(strlen(text) + 1);
        if (!sng->comment) {
            perror("load_songs(): failed to alloc space for song comment");
            goto uh_oh;
        }
        strcpy(sng->comment, text);

        // int fields
        sng->song_id = sqlite3_column_int(pstmt, 0);
        sng->album_id = sqlite3_column_int(pstmt, 1);
        sng->track_num = sqlite3_column_int(pstmt, 2);
        sng->dur_s = sqlite3_column_int(pstmt, 3);
        sng->bitrate = sqlite3_column_int(pstmt, 4);
        sng->sample_rate = sqlite3_column_int(pstmt, 5);
        sng->channels = sqlite3_column_int(pstmt, 6);


        // also need to add to vector associated album
        album* abm = JHASHMAP_get(album_cache, CAST_INT(sng->album_id));
        assert(abm);

        // extra info associated with song, strings not allocated here so DON'T FREE WHEN FREEING SONG
        sng->album_title = abm->title;
        sng->artist_name = abm->artist_name;
        sng->date =  abm->date;
        sng->orig_date = abm->date;

        print_song(sng);
        // add to general songs vector
        JVEC_append(vec, sng);
        // add to album's songs vector
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
    return 1;
}

// load persistent library stored in sql database into memory
int load_library(lib_mem* mem, lib_db* db) {
    if (load_artists(mem, db)) {
        fprintf(stderr, "failed to load artists in load_library()\n");
        return 1;
    }
    if (load_albums(mem, db)) {
        fprintf(stderr, "Failed to load albums in load_library()\n");
        return 1;
    }
    if (load_songs(mem, db)) {
        fprintf(stderr, "Failed to load songs in load_library()\n");
        return 1;
    }

    return 0;
}

void debug_print_mem(lib_mem* mem) {
    JVEC* artists = mem->artists;
    //JVEC* albums = mem->albums;
    //JVEC* songs = mem->songs;

    printf("ARTISTS:\n");
    for (size_t i = 0; i < JVEC_len(artists); i++) {
        artist* atst = JVEC_get(artists, i);
        printf("%s\n", atst->name);
        printf("  artist's albums:\n");

        JVEC* a_albums = atst->albums;
        for (size_t j = 0; j < JVEC_len(a_albums); j++) {
            album* a_abm = JVEC_get(a_albums, j);
            printf("    %s\n", a_abm->title);

            JVEC* a_songs = a_abm->songs;
            for (size_t k = 0; k < JVEC_len(a_songs); k++) {
                song* a_sng = JVEC_get(a_songs, k);
                printf("      %d: %s\n", a_sng->track_num ,a_sng->title);
            }
        }
    }
}

void print_song(song* sng) {
    printf("id: %d\nalbum_id: %d\ntrack num: %d\ndur_s: %d\nbitrate: %d\nsample rate: %d\nchannels: %d\ntitle: %s\npath: %s\ncomment: %s\nartist %s\ntitle: %s\ndate: %s\norig_date: %s\n", sng->song_id, sng->album_id, sng->track_num, sng->dur_s, sng->bitrate, sng->sample_rate, sng->channels, sng->title, sng->path, sng->comment, sng->artist_name, sng->album_title, sng->date, sng->orig_date);
}
void print_album(album* abm) {
    printf("id: %d\na_id: %d\ntracks: %d\ntitle: %s\ngenre: %s\ndate: %s\norig_date: %s\nartist: %s\n", abm->album_id, abm->artist_id, abm->tracks, abm->title, abm->genre, abm->date, abm->orig_date, abm->artist_name);
}

int artist_compare(const void* a1, const void* a2) {
    artist* _a1 = *(artist**) a1;
    artist* _a2 = *(artist**) a2;
    return strcmp(_a1->name, _a2->name);
}

int album_compare(const void* a1, const void* a2) {
    album* _a1 = *(album**) a1;
    album* _a2 = *(album**) a2;


    return strcmp(_a1->orig_date, _a2->orig_date);
}

int song_compare(const void* s1, const void* s2) {
    song* _s1 = *(song**) s1;
    song* _s2 = *(song**) s2;

    return _s1->track_num - _s2->track_num;
}
