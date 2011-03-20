provider phonon {
    probe node_create(void*);
    probe node_destroy(void*);
    probe media_state_changed(void*,int, int);
    probe media_play(void*);
    probe media_pause(void*);
    probe media_stop(void*);
    probe media_seek(void*, unsigned long long int);
    probe media_new_source(void*, void*);
    probe source_create(void*, const char*);
};
