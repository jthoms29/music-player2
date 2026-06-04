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

    printf("%s\n", path);
    if ( !(file = taglib_file_new(path)) || !(tag = taglib_file_tag(file)) ) {
        return -1; 
    }
    char* title = taglib_tag_title(tag);
    char* artist = taglib_tag_artist(tag);
    char* album = taglib_tag_album(tag);
    char* genre = taglib_tag_genre(tag);
    int tracknum = taglib_tag_track(tag);
    unsigned int year = taglib_tag_year(tag);

    
    insert_artist(lib_db, artist);
    int artist_id = retrieve_artist(lib_db, artist);

    insert_album(lib_db, artist_id, album, year);
    int album_id = retrieve_album(lib_db, artist_id, album, year);
    printf("album id: %d\n", album_id);

    insert_song(lib_db, album_id, tracknum, title, path);



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