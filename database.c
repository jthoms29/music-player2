#include <music_player2.h>

int lib_db_init(lib_db* lib_db) {
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
        "SELECT album_id FROM albums WHERE title = ?;",
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
    
}