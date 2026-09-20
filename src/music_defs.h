#ifndef MUSIC_DEFS_H
#define MUSIC_DEFS_H


#include <../../JLib/src/JVEC.h>
typedef struct song {
    int song_id;
    int album_id;
    int disc_num;
    int track_num;
    int dur_s;
    int bitrate;
    int sample_rate;
    int channels;
    char* title;
    char* path;
    char* comment;
    char* artist_name;

    // not allocated within song init function
    char* abm_artist_name;
    char* album_title;
    char* date;
    char* orig_date;

    char* str_rep;
} song;

typedef struct album {
    int album_id;
    int abm_artist_id;
    int tracks;

    char* title;
    char* genre;
    char* date;
    char* orig_date;
    JVEC* songs;

    // not allocated within album init function
    char* abm_artist_name;

    char* str_rep;
} album;

typedef struct abm_artist {
    char* name;
    JVEC* albums;
} abm_artist;

#endif