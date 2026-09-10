#include <music_player2.h>
#include <scrolling_menu.h>

char* artist_string(void* atst) {
    if (!atst) {
        return NULL;
    }
    artist* _atst = (artist*) atst;
    return _atst->name;
}

char* album_string(void* abm) {
    if (!abm) {
        return NULL;
    }
    album* _abm = (album*) abm;
    return _abm->title;
}

char* song_string(void* sng) {
    if (!sng) {
        return NULL;
    }
    song* _sng = (song*) sng;
    return _sng->title;
}

elements* elements_new(lib_mem* lib) {
    elements* e = calloc(1, sizeof(*e));
    if (!e) {
        perror("elements_new(): could not allocate new elements struct");
        return NULL;
    }

    e->menu[0] = menu_new(lib->artists, artist_string);
    e->menu[1] = menu_new(lib->albums, album_string);
    e->menu[2] = menu_new(lib->songs, song_string);
    return e;
}


void change_column(model* m, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (m->col_idx == 0 && dir == -1) || (m->col_idx == 2 && dir==1)) {
        return;
    }
    m->col_idx += dir;
}
