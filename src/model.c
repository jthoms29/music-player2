#include <music_player2.h>
#include <scrolling_menu.h>
#include <playback_menu.h>

char* abm_artist_string(void* atst) {
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
    if (!_abm->str_rep) {
        char buf[256];
        size_t len = snprintf(buf, sizeof(buf), "(%s) %s [%s]", _abm->orig_date, _abm->title, _abm->date);
        _abm->str_rep = calloc(len+1, 1);
        strcpy(_abm->str_rep, buf);
    }

    return _abm->str_rep;
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

void elements_update_vectors(elements* e) {
    scrolling_menu** menus = e->menus;

    // if selected abm_artist changed, need to change albums vec
    if (e->col_idx == 0) {
        abm_artist* abm_a = menu_get_hovered(menus[0]);
        JVEC* vec2 = (abm_a)
            ? abm_a->albums
            : NULL;
        menu_change_vec(menus[1], vec2);
    }

    // if either abm_artist or album changed, need to change songs vec
    if (e->col_idx < 2) {
        album* abm = menu_get_hovered(menus[1]);
        JVEC* vec3 = (abm)
            ? abm->songs
            : NULL;
        menu_change_vec(menus[2], vec3);
    }
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
    e->menus[0] = menu_new(lib->abm_artists, abm_artist_string);
    e->menus[1] = menu_new(NULL, album_string);
    e->menus[2] = menu_new(NULL, song_string);
    elements_update_vectors(e);


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
