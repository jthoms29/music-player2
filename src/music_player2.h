#ifndef MUSIC_PLAYER2_H
#define MUSIC_PLAYER2_H

#include <stdint.h>
#include <sqlite3.h>
#include <../../JLib/src/JHELPER.h>
#include <../../JLib/src/JHASHMAP.h>
#include <../../JLib/src/JVEC.h>
#include <../../JLib/src/JARENA.h>

typedef struct lib_db {
    sqlite3* db;
    JHASHMAP* artist_cache;
    JHASHMAP* album_cache;

    sqlite3_stmt* insert_artist;
    sqlite3_stmt* select_artist;

    sqlite3_stmt* insert_album;
    sqlite3_stmt* select_album;

    sqlite3_stmt* insert_song;
} lib_db;

int lib_db_init(lib_db* lib_db);


typedef struct song {
    char* path;
    char* title;
    char* artist;
    char* album;
    char* genre;
    char* date;
    char* orig_date;
    uint8_t track_num;
} song;

typedef struct album {
    char* title;
    char* artist;
    char* genre;
    char* date;
    char* orig_date;
    uint8_t tracks;
    uint8_t artist_id;
    uint8_t album_id;
    JVEC* songs;
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



int insert_artist(lib_db* lib_db, char* artist_name);
int retrieve_artist(lib_db* lib_db, char* artist_name);
int insert_album(lib_db* lib_db, int artist_id, char* title, char* date, char* orig_date);
int retrieve_album(lib_db* lib_db, int artist_id, char* album_name, char* date);

int insert_song(lib_db* lib_db, int album_id, char* song_title, int tracknum, int dur_s, int bitrate, int sample_rate, int channels, char* comment, char* path);
size_t scan_dir(lib_db* lib_db, char* path);



lib_mem* lib_mem_init(void);
int load_artists(lib_mem* mem, lib_db* db);
#endif