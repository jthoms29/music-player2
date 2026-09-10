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


    scrolling_menu** menus = calloc(3, sizeof(*menus));
    if (!menus) {
        perror("elements_new(): could not allocate scrolling menu array");
        return NULL;
    }
    e->menus = menus;
    e->menus[0] = menu_new(lib->artists, artist_string);
    e->menus[1] = menu_new(lib->albums, album_string);
    e->menus[2] = menu_new(lib->songs, song_string);
    menu_focus(e->menus[0]);
    return e;
}


void change_column(elements* e, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (e->col_idx == 0 && dir == -1) || (e->col_idx == 2 && dir==1)) {
        return;
    }
    menu_unfocus(e->menus[e->col_idx]);

    if (e->col_idx == 0) {
        //DDDDDDDDDDDDDDDDDDDDD
    }


    e->col_idx += dir;
    menu_focus(e->menus[e->col_idx]);
}
