#include <music_player2.h>
#include <scrolling_menu.h>
#include <playback_menu.h>

char* artist_string(void* atst) {
    if (!atst) {
        return NULL;
    }
    abm_artist* _abm_atst = (abm_artist*) atst;
    return _abm_atst->name;
}

char* album_string(void* abm) {
    if (!abm) {
        return NULL;
    }
    album* _abm = (album*) abm;
    char* od = _abm->orig_date;
    char* d = _abm->date;
    char* title = _abm->date;


    return _abm->title;
}

char* song_string(void* sng) {
    if (!sng) {
        return NULL;
    }

    song* _sng = (song*) sng;
    if (!_sng->str_rep) {
        char buf[256];
        size_t len = snprintf(buf, sizeof(buf), "%02d-%02d - %s", _sng->disc_num, _sng->track_num, _sng->title);
        _sng->str_rep = calloc(len+1, 1);
        strcpy(_sng->str_rep, buf);
    }
    return _sng->str_rep;
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
    e->menus[0] = menu_new(lib->abm_artists, artist_string);
    e->menus[1] = menu_new(((abm_artist*)JVEC_get(e->menus[0]->vec, 0))->albums, album_string);
    e->menus[2] = menu_new(((album*)JVEC_get(e->menus[1]->vec, 0))->songs, song_string);
    menu_focus(e->menus[0]);

    e->playback_menu = playback_new();
    return e;
}


void change_column(elements* e, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (e->col_idx == 0 && dir == -1) || (e->col_idx == 2 && dir==1)) {
        return;
    }
    menu_unfocus(e->menus[e->col_idx]);
    e->col_idx += dir;
    menu_focus(e->menus[e->col_idx]);
}
