#ifndef MUSIC_PLAYER2_H
#define MUSIC_PLAYER2_H

#include <stdint.h>
#include <sqlite3.h>
#include <../../JLib/src/JHELPER.h>
#include <../../JLib/src/JHASHMAP.h>
#include <../../JLib/src/JVEC.h>
#include <../../JLib/src/JARENA.h>

/* 
 * Contains a pointer to the sqlite database which contains the user's music library info,
 * as well as premade statements for interacting with the db.
 */
typedef struct lib_db {
    sqlite3* db;

    // hashmaps containing the primary keys for each album and artist. Makes it so when building
    // db, don't need to retrieve these primary keys over and over when setting foreign keys.
    // keys are names/album titles, vals are sql primary keys
    JHASHMAP* artist_cache;
    JHASHMAP* album_cache;

    // sql statements for inserting and retrieving library elements
    sqlite3_stmt* insert_artist;
    sqlite3_stmt* select_artist;

    sqlite3_stmt* insert_album;
    sqlite3_stmt* select_album;

    sqlite3_stmt* insert_song;
} lib_db;



typedef struct song {
    int song_id;
    int album_id;
    int track_num;
    int dur_s;
    int bitrate;
    int sample_rate;
    int channels;
    char* title;
    char* path;
    char* comment;

    // not allocated within song init function
    char* artist_name;
    char* album_title;
    char* date;
    char* orig_date;
} song;

typedef struct album {
    int album_id;
    int artist_id;
    int tracks;

    char* title;
    char* genre;
    char* date;
    char* orig_date;
    JVEC* songs;

    // not allocated within album init function
    char* artist_name;
} album;

typedef struct artist {
    char* name;
    JVEC* albums;
} artist;

typedef struct lib_mem {
    // each letter will hold a vector of artists
    JVEC* artists;
    JVEC* albums;
    JVEC* songs;

    // allows structs to be nested efficiently when loading library. Key is sql primary key, vals are 
    // the wanted parent structs
    JHASHMAP* artist_cache;
    JHASHMAP* album_cache;

} lib_mem;


/* DIRECTORY SCAN FUCNTIONS (scan.c) $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/**
 * @brief Read tags from audio file, update sql database with song
 * @details Reads tags from an audio file using taglib, updates sqlite3 database within lib_db
 * with song, as well as associated album and artist
 * @note Database will only be updated if path refers to an audio file readable by taglib
 * @param[in, out] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] path string filepath
 * @pre lib_db must be non-null
 * @post if path refers to a valid audio file, the sqlite3 database within lib_db is updated, adding an entry to the songs table,
 * as well as adding or updating the tables for artist and album associated with song
 * @return 0 on success, anything else on failure
 */
int read_tags(lib_db* lib_db, char* path);

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
size_t scan_dir(lib_db* lib_db, char* path);


/* DATABASE FUNCTIONS (database.c) $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */


/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int lib_db_init(lib_db* lib_db);

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int insert_artist(lib_db* lib_db, char* artist_name);
/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int retrieve_artist(lib_db* lib_db, char* artist_name);

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int insert_album(lib_db* lib_db, int artist_id, char* title, char* date, char* orig_date);
/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int retrieve_album(lib_db* lib_db, int artist_id, char* album_name, char* date);

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int insert_song(lib_db* lib_db, int album_id, char* song_title, int tracknum, int dur_s, int bitrate, int sample_rate, int channels, char* comment, char* path);


/* IN-MEM LIBRARY FUNCTIONS (lib_mem.c) $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
lib_mem* lib_mem_init(void);

/**
 * @brief
 * @details
 * @note
 * @param[in, out]
 * @param[in]
 * @pre
 * @post
 * @return
 */
int load_artists(lib_mem* mem, lib_db* db);
#endif