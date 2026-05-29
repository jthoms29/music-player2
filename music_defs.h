#ifndef MUSIC_DEFS_H
#define MUSIC_DEFS_H

#include <stdint.h>
#include <../JLib/src/JVEC.h>

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