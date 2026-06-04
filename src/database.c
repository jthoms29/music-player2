#include <music_player2.h>

int lib_db_init(lib_db* lib_db) {
    // set everything to null
    memset(lib_db, 0, sizeof(*lib_db));

    char* err_msg = NULL;
    int rc = sqlite3_open("lib.db", &lib_db->db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }

    const char* sql_str =
        "CREATE TABLE IF NOT EXISTS artists ("
        "artist_id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL UNIQUE"
        ");"

        "CREATE TABLE IF NOT EXISTS albums ("
        "album_id INTEGER PRIMARY KEY,"
        "artist_id INTEGER NOT NULL,"
        "title TEXT NOT NULL,"
        "year INTEGER,"
        "FOREIGN KEY (artist_id) REFERENCES artists(artist_id),"
        "UNIQUE(artist_id, title, year)"
        ");"

        "CREATE TABLE IF NOT EXISTS songs ("
        "song_id INTEGER PRIMARY KEY,"
        "album_id INTEGER NOT NULL,"
        "title TEXT NOT NULL,"
        "track_num INTEGER,"
        "dur_ms INTEGER,"
        "path TEXT NOT NULL UNIQUE,"
        "mtime INTEGER,"
        "size INTEGER,"
        "FOREIGN KEY (album_id) REFERENCES albums(album_id)"
        ");";
    rc = sqlite3_exec(lib_db->db, sql_str, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        goto uh_oh;
    } 

    //prepare all query strings ///////////////////////

    //INSERT ARTIST
    rc = sqlite3_prepare_v2(
        lib_db->db, 
        "INSERT OR IGNORE INTO artists(name) VALUES(?);", 
        -1, 
        &lib_db->insert_artist, 
        NULL
    );
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }

    // RETRIEVE ARTIST
    rc = sqlite3_prepare_v2(
        lib_db->db, 
        "SELECT artist_id FROM artists WHERE name = ?;",
        -1, 
        &lib_db->select_artist, 
        NULL
    );
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }

    //INSERT ALBUM
    rc = sqlite3_prepare_v2(
        lib_db->db, 
        "INSERT OR IGNORE INTO albums(artist_id, title, year) VALUES(?, ?, ?);", 
        -1, 
        &lib_db->insert_album, 
        NULL
    );
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }

    // RETRIEVE ALBUM
    rc = sqlite3_prepare_v2(
        lib_db->db, 
        "SELECT album_id FROM albums WHERE artist_id = ? and title = ? and year = ?;",
        -1, 
        &lib_db->select_album, 
        NULL
    );
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }

    // INSERT SONG
    rc = sqlite3_prepare_v2(
        lib_db->db, 
        "INSERT OR IGNORE INTO songs(album_id, title, track_num, dur_ms, path, mtime, size) VALUES(?, ?, ?, ?, ?, ?, ?);", 
        -1, 
        &lib_db->insert_song, 
        NULL
    );
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(lib_db->db));
        goto uh_oh;
    }


    //set up primary key caches
    JHASHMAP* artist_cache = JHASHMAP_new(string_hash, string_compare);
    lib_db->artist_cache = artist_cache;
    JHASHMAP* album_cache = JHASHMAP_new(string_hash, string_compare);
    lib_db->album_cache = album_cache;
    return 0;

    uh_oh:
        sqlite3_finalize(lib_db->insert_artist);
        sqlite3_finalize(lib_db->select_artist);
        sqlite3_finalize(lib_db->insert_album);
        sqlite3_finalize(lib_db->select_album);
        sqlite3_finalize(lib_db->insert_song);
        sqlite3_free(err_msg);        
        sqlite3_close(lib_db->db);
        return -1;
}

int insert_artist(lib_db* lib_db, char* artist_name) {
    sqlite3_stmt* stmt = lib_db->insert_artist;
    sqlite3_bind_text(stmt, 1, artist_name, -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int retrieve_artist(lib_db* lib_db, char* artist_name) {
    JHASHMAP* cache = lib_db->artist_cache;

    int artist_id = (int)(intptr_t)JHASHMAP_get(cache, artist_name);
    if (artist_id > 0) {
        return artist_id;
    }
    //not in cache, need to query db for key
    sqlite3_stmt* stmt = lib_db->select_artist;
    sqlite3_bind_text(stmt, 1, artist_name, -1, SQLITE_TRANSIENT);
    
    artist_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        artist_id = sqlite3_column_int(stmt, 0);
        char* name = malloc(strlen(artist_name)+1);
        JHASHMAP_add(cache, name, (void*)(intptr_t) artist_id);
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return artist_id;
}

int insert_album(lib_db* lib_db, int artist_id, char* title, int year) {

    sqlite3_stmt* stmt = lib_db->insert_album;
    sqlite3_bind_int(stmt, 1, artist_id);
    sqlite3_bind_text(stmt, 2, title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, year);
    int rc = sqlite3_step(stmt);

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}

int retrieve_album(lib_db* lib_db, int artist_id, char* album_name, int year) {
    // need to build cache key with more info than just album title
    char local_key[512];
    JHASHMAP* cache = lib_db->album_cache;
    snprintf(local_key, 512, "%d|%s|%d", artist_id, album_name, year);
    int album_id = (int)(intptr_t)JHASHMAP_get(cache, local_key);
    if (album_id > 0) {
        return album_id;
    }
    //not in cache, need to query db for key
    sqlite3_stmt* stmt = lib_db->select_album;
    sqlite3_bind_int(stmt, 1, artist_id);
    sqlite3_bind_text(stmt, 2, album_name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, year);
    
    album_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        album_id = sqlite3_column_int(stmt, 0);
        char* cache_key = malloc(strlen(local_key)+1);
        strcpy(cache_key, local_key);
        JHASHMAP_add(cache, cache_key, (void*)(intptr_t) album_id);
    }

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return album_id;
}

int insert_song(lib_db* lib_db, int album_id, int tracknum, char* song_title, char* path) {
    sqlite3_stmt* stmt = lib_db->insert_song;
    sqlite3_bind_int(stmt, 1, album_id);
    sqlite3_bind_text(stmt, 2, song_title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, tracknum);
    sqlite3_bind_text(stmt, 5, path, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);

    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);

    return (rc == SQLITE_DONE) ? 0 : -1;
}