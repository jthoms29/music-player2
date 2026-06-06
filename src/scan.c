#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <tag_c.h>
#include <music_player2.h>
#include <sqlite3.h>


int read_tags(lib_db* lib_db, char* path) {
    TagLib_File *file;
    TagLib_Tag *tag;
    const TagLib_AudioProperties *properties;

    if ( !(file = taglib_file_new(path)) || !(tag = taglib_file_tag(file)) ) {
        return -1; 
    }

    properties = taglib_file_audioproperties(file);

    char* title = taglib_tag_title(tag);
    char* artist = taglib_tag_artist(tag);
    char* album = taglib_tag_album(tag);
    char* genre = taglib_tag_genre(tag);
    char* comment = taglib_tag_comment(tag);
    int tracknum = taglib_tag_track(tag);

    int bitrate = taglib_audioproperties_bitrate(properties);
    int sample_rate = taglib_audioproperties_samplerate(properties);
    int dur_s = taglib_audioproperties_length(properties);
    int channels = taglib_audioproperties_channels(properties);
   

    char **vals1, **vals2;
    vals1 = taglib_property_get(file, "ORIGINALDATE");
    char* orig_date = (vals1) ? vals1[0] : "unknown";
    vals2 = taglib_property_get(file, "DATE");
    char* date = (vals2) ? vals2[0] : "unknown";


    insert_artist(lib_db, artist);
    int artist_id = retrieve_artist(lib_db, artist);

    insert_album(lib_db, artist_id, album, date, orig_date);
    int album_id = retrieve_album(lib_db, artist_id, album, date);
    printf("album id: %d\n", album_id);

    insert_song(lib_db, album_id, title, tracknum, dur_s, bitrate, sample_rate, channels, comment, path);

    if (vals1) { taglib_property_free(vals1); }
    if (vals2) { taglib_property_free(vals2); }

    return 0;
}


size_t scan_dir(lib_db* lib_db, char* path) {
    struct dirent* de; // pointer for directory entry
    struct stat statbuf;
    char* subpath;


    printf("%s\n", path);
    DIR* dr = opendir(path);
    if (!dr) {
        printf("ERROR: could not open directory %s\n", path);
    }

    while ((de = readdir(dr)) != NULL) {
        // ignore self and parent
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) { continue; }

        // space for parent dir, '/', filename, and '\0'
        subpath = malloc(strlen(path) + strlen(de->d_name) + 2);
        // build pathname
        strcpy(subpath, path);
        strcat(subpath, "/");
        strcat(subpath, de->d_name);
        
        if (lstat(subpath, &statbuf)) { continue;}


        if (S_ISDIR(statbuf.st_mode)) {
            //recurse through sub directory
            scan_dir(lib_db, subpath);
            free(subpath);
            continue;
        }

        //otherwise, if file is audio add it to lib
        int tag_ret = read_tags(lib_db, subpath);
        free(subpath);
        continue;
    }
    closedir(dr);

    return 0;
}