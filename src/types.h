#ifndef TYPES_H
#define TYPES_H

#include "merc.h"

/* Flag struct */
struct flag_type {
    char *name;
    int bit;
    bool settable;
};

/* Clan struct */
struct clan_type {
    char *name;
    char *who_name;
    sh_int hall;
    sh_int entrance;
    sh_int pit;
    bool independent; /* loners */
    bool pkill;       /* pkilling clans */
    char *exname;
};

/* Position struct */
struct position_type {
    char *name;
    char *short_name;
};

/* Sex struct */
struct sex_type {
    char *name;
};

/* Size struct */
struct size_type {
    char *name;
};

/* Bit type */
struct bit_type {
    const struct flag_type *table;
    char *help;
};

/* Channel struct */
struct chan_type {
    char *chan_pre;
    char *chan_name;
    char *chan_say;
    int bit;
};

#endif /* TYPES_H */
