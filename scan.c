#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <tag_c.h>
#include <music_player2.h>
#include <sqlite3.h>

int read_tags(sqlite3* lib_db, char* path) {
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

    //dynamic str query here

    // insert artist

    sqlite3_stmt* stmt;
    const char* sql_str = "INSERT OR IGNORE INTO artists(name) VALUES(?)";
    sqlite3_prepare_v2(lib_db, sql_str, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, artist, -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // get artist id
    int artist_id = -1;
    sql_str = "SELECT artist_id FROM artists WHERE name = ?;";
    sqlite3_prepare_v2(lib_db, sql_str, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, artist, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        artist_id = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);


    sql_str = "INSERT OR INGNORE INTO albums()";

    return 0;
}


size_t scan_dir(sqlite3* lib_db, char* path) {
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
            scan_dir(lib_db, subpath);
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