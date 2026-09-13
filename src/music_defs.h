#ifndef MUSIC_DEFS_H
#define MUSIC_DEFS_H


#include <../../JLib/src/JVEC.h>
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

#endif