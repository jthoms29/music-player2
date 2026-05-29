#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <tag_c.h>
#include <music_defs.h>

int read_tags(lib* lib, char* path) {
    TagLib_File *file;
    TagLib_Tag *tag;

    if ( !(file = taglib_file_new(path)) || !(tag = taglib_file_tag(file)) ) {
        return -1; 
    }
    char* title = taglib_tag_title(tag);
    char* artist = taglib_tag_artist(tag);
    char* album = taglib_tag_album(tag);
    char* genre = taglib_tag_genre(tag);
    unsigned int year = taglib_tag_year(tag);

    return 0;
}

size_t scan_dir(lib* lib, char* path) {
    struct dirent* de; // pointer for directory entry
    struct stat statbuf;
    char* subpath;


    DIR* dr = opendir(path);
    if (!dr) {
        printf("ERROR: could not open directory %s\n", path);
    }

    while ((de = readdir(dr)) != NULL) {
        if (lstat(de->d_name, &statbuf)) {continue;}
        // ignore self and parent
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) { continue; }

        // space for parent dir, '/', filename, and '\0'
        subpath = malloc(strlen(path) + strlen(de->d_name) + 2);
        // build pathname
        strcpy(subpath, path);
        strcpy(subpath, "/");
        strcpy(subpath, de->d_name);

        if (statbuf.st_mode & S_IFDIR) {
            //recurse through sub directory
            scan_dir(lib, subpath);
            free(subpath);
            continue;
        }

        //otherwise, if file is audio add it to lib
        int tag_ret = read_tags(lib, subpath);
        if (!tag_ret) {
            free(subpath);
            continue;
        }
    }

    return 0;
}