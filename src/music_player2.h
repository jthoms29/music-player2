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
    JVEC* vecs[3];

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
 * @pre lib_db must be initialized with lib_db_init()
 * @post if path refers to a valid audio file, the sqlite3 database within lib_db is updated, adding an entry to the songs table,
 * as well as adding or updating the tables for artist and album associated with song
 * @return 0 on success, anything else on failure
 */
int read_tags(lib_db* lib_db, char* path);

/**
 * @brief Scans directory specified in path, updates sql database with songs found within
 * @details Recursively scans directory specified in path, calls `read_tags()` on each file found
 * @note
 * @param[in, out] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] path string directory path
 * @pre lib_db must be created with `lib_db_new()`
 * @post if path refers to a directory containing valid audio files, the sqlite3 database within lib_db is updated,
 * adding entries to the song table, as well as adding entries to the the artist and album tables associated with the song
 * @return 0 on success, anything else on failure
 */
int scan_dir(lib_db* lib_db, char* path);


/* DATABASE FUNCTIONS (database.c) $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */


/**
 * @brief Allocates and returns a new lib_db struct.
 * @details Allocates and returns a new lib_db struct, which contains:
 * - A reference to an sqlite3 database, containing tables for artists, albums, and songs. This is created in this function if it doesn't exist already
 * - Multiple prepared sqlite3 statements for interacting with said database
 * @note The lib_db struct returned by this function must be freed with `lib_db_free()`
 * @pre None
 * @post A new lib_db struct which can be used in the other database functions is returned
 * @return An initialized lib_db struct
 */
lib_db* lib_db_new();

/**
 * @brief Inserts given artist name into sqlite3 artist table
 * @details Artist specified in artist_name is added to sqlite3 database contained in lib_db. If this artist is
 * already present, nothing happens
 * @param[in, out] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] artist_name string referring to the name of an artist
 * @pre lib_db must be created with `lib_db_new()`
 * @post Artist referred to by artist_name is added to database's 'artists' table if not already present 
 * @return 0 on success, anything else on failure
 */
int insert_artist(lib_db* lib_db, char* artist_name);

/**
 * @brief Retrieves artist_id primary key of artist referred to by 'artist_name'
 * @details Retrieves artist_id primary key of artist referred to by 'artist_name'. Retrieved from 'artists' table contained in
 * lib_db's sqlite3 database
 * @param[in] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] artist_name string referring to the name of an artist
 * @pre lib_db must be created with `lib_db_new()`
 * @post primary key associated with artist returned
 * @return artist primary key if present in database, -1 otherwise
 */
int retrieve_artist(lib_db* lib_db, char* artist_name);

/**
 * @brief Inserts given album into sqlite3 album table
 * @details album specified in `title` is added to sqlite3 database contained in lib_db. 'date' and 'orig_date' tags
 * associated with album also inserted, as well as 'artist_id' foreign key - associated primary key in 'artists' table
 * @param[in] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] artist_id primary key for artist associated with album from database's 'artists' table
 * @param[in] title album title
 * @param[in] date album's 'date' tag
 * @param[in] orig_date album's 'orginal_date' tag
 * @pre lib_db must be created with `lib_db_new()`
 * artist associated with album must already be present in database's 'artist' table
 * artist_id must be the primary key associated with the album artist
 * @post album is added to database's 'albums' table
 * @return 0 on success, anything else on failure
 */
int insert_album(lib_db* lib_db, int artist_id, char* title, char* date, char* orig_date);

/**
 * @brief Retrieves album_id primary key or album referred to by 'album_name'
 * @details Retrieves album_id primary key or album referred to by 'album_name' by artist referred to by 'artist_id'
 * with date tag 'date'. 
 * @param[in] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] artist_id primary key for artist associated with album from database's 'artists' table
 * @param[in] album_name album title
 * @param[in] date album's 'date' tag
 * @pre lib_db must be created with `lib_db_new()`
 * @post if album exists in database, its primary key is returned
 * @return album's primary key on success, -1 on failure
 */
int retrieve_album(lib_db* lib_db, int artist_id, char* album_name, char* date);

/**
 * @brief Inserts given song into sqlite3 songs table
 * @details Song from specified path is added to sqlite3 database contained in lib_db. Associated tags as well as album_id foreign key inserted
 * @param[in, out] lib_db struct containing pointer to sqlite3 db, premade statements for db interaction
 * @param[in] album_id primary key for album associated with song from database's 'songs' table
 * @param[in] song_title song's title
 * @param[in] tracknum song's track number within associated album
 * @param[in] dur_s song's duration in seconds
 * @param[in] bitrate song's bitrate
 * @param[in] sample_rate song's samplerate
 * @param[in] channels song's channels (mono, stereo)
 * @param[in] comment contents of song's comment tag
 * @param[in] path path to song's file
 * @pre lib_db must be created with `lib_db_new()`
 * album associated with song must already be present in database's 'albums' table
 * album id must be associated album's primary key
 * @post song is added to database's 'songs' table
 * @return 0 on success, -1 on failure
 */
int insert_song(lib_db* lib_db, int album_id, char* song_title, int tracknum, int dur_s, int bitrate, int sample_rate, int channels, char* comment, char* path);


/**
 * @brief Frees lib_db struct
 * @details Frees lib_db struct created with `lib_db_new()`
 * @param[in, out] lib_db_ptr indirect pointer to lib_db_struct
 * @pre lib_db_ptr must have been created with `lib_db_new()`
 * @post all allocated fields within struct are freed, as well as the struct itself. The lib_db* that lib_db_ptr points to is set to NULL
 * @return none
 */
void lib_db_free(lib_db** lib_db_ptr);

/* IN-MEM LIBRARY FUNCTIONS (lib_mem.c) $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/**
 * @brief Creates data structure to hold all database info in memory
 * @details Creates a struct containing multiple vectors which hold all database info.
 * Separate vectors for artists, albums, songs. Nested structure, artist also contains references to associated albums,
 * albums contain references to associated songs
 * @note Must be freed with `lib_mem_free()`
 * @pre None
 * @post New lib_mem struct is created and returned
 * @return New lib_mem struct
 */
lib_mem* lib_mem_new(void);

/**
 * @brief Load all info from artists table into memory
 * @details Load all info from artists table from sqlite3 database in `db` into artists vector in `mem`
 * @param[in, out]
 * @param[in]
 * @pre `mem` must be created with `lib_mem_new, `db` must be created with `lib_db_new`
 * @post Library info from sqlite3 database held in `db` loaded into `mem`'s data structures
 * @return 0 on success, anything else on failure
 */
int load_artists(lib_mem* mem, lib_db* db);



void debug_print_mem(lib_mem* mem);

int load_library(lib_mem* mem, lib_db* db);
void lib_mem_free(lib_mem** lib_ptr);


void view_loop(lib_mem* lib);
#endif

