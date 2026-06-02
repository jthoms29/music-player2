#ifndef MUSIC_PLAYER2_H
#define MUSIC_PLAYER2_H

#include <stdint.h>
#include <sqlite3.h>
#include <../JLib/src/JVEC.h>
#include <../JLib/src/JHASHMAP.h>


typedef struct lib_db {
    sqlite3* db;

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
    uint8_t year;
    uint8_t date;
    uint8_t track_num;
} song;

typedef struct album {
    char* title;
    char* artist;
    char* genre;
    uint8_t year;
    uint8_t date;
    uint8_t tracks;
    song* songs;
} album;

typedef struct artist {
    char* name;
    album* albums;
} artist;

typedef struct lib {
    // 
    JVEC* letters[256];

} lib;

#endif