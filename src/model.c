#include <music_player2.h>


model* model_new(lib_mem* lib) {
    model* m = calloc(1, sizeof(*m));
    if (!m) {
        perror("imodel_new(): could not allocate new imodel");
        return NULL;
    }

    m->vecs[0] = lib->artists;
    m->vecs[1] = lib->albums;
    m->vecs[2] = lib->songs;
    return m;
}

void scroll_menu(model* m, lib_mem* lib, int8_t dir, size_t rows) {
    int8_t vec_num = m->col_idx;

    JVEC* vec = m->vecs[vec_num];

    // don't allow user to scroll out of bounds 
    if (dir == -1 && m->row_idx[vec_num] == 0) {
        return;
    }
    if (dir == 1 && m->row_idx[vec_num] == vec->len - 1) {
        return;
    }

    // update current column's row index to reflect user input
    m->row_idx[vec_num] += dir;

    // recompute top of currently visible list in window
    size_t idx = m->row_idx[vec_num];
    size_t top = m->row_top[vec_num];

    if (idx - top >= rows-2) {
        m->row_top[vec_num] = idx - (rows-2) + 1;
    }

    if (idx < top) {
        m->row_top[vec_num] = idx;
    }

    if (vec_num == 0) {
        m->vecs[1] =  ((artist*) JVEC_get(lib->artists, idx))->albums;
        m->vecs[2] = ((album*) JVEC_get(m->vecs[1], 0))->songs;
        m->row_idx[1] = 0;
        m->row_idx[2] = 0;
        m->row_top[1] = 0;
        m->row_top[2] = 0;
    }
    if (vec_num == 1) {
        m->vecs[2] = ((album*) JVEC_get(m->vecs[1], idx))->songs;
        m->row_idx[2] = 0;
        m->row_top[2] = 0;
    }

}

void change_column(model* m, int8_t dir) {
    // don't let user scroll out of bounds
    if ( (m->col_idx == 0 && dir == -1) || (m->col_idx == 2 && dir==1)) {
        return;
    }
    m->col_idx += dir;
}
