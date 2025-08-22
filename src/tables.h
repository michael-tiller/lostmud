#ifndef TABLES_H
#define TABLES_H

#include "merc.h"

/* game tables */
extern const struct clan_type       clan_table[MAX_CLAN];
extern const struct position_type   position_table[];
extern const struct sex_type        sex_table[];
extern const struct size_type       size_table[];

/* flag tables */
extern const struct flag_type act_flags[];
extern const struct flag_type plr_flags[];
extern const struct flag_type affect_flags[];
extern const struct flag_type shield_flags[];
extern const struct flag_type off_flags[];
extern const struct flag_type imm_flags[];
extern const struct flag_type form_flags[];
extern const struct flag_type part_flags[];
extern const struct flag_type comm_flags[];
extern const struct flag_type item_extra[];
extern const struct flag_type item_wear[];
extern const struct flag_type item_weapon[];
extern const struct flag_type extra_flags[];
extern const struct flag_type wear_flags[];
extern const struct flag_type weapon_flags[];
extern const struct flag_type container_flags[];
extern const struct flag_type portal_flags[];
extern const struct flag_type room_flags[];
extern const struct flag_type area_room[];
extern const struct flag_type exit_flags[];
extern const struct flag_type mprog_flags[];
extern const struct flag_type area_flags[];
extern const struct flag_type sector_flags[];
extern const struct flag_type door_resets[];
extern const struct flag_type wear_loc_strings[];
extern const struct flag_type wear_loc_flags[];
extern const struct flag_type res_flags[];
extern const struct flag_type vuln_flags[];
extern const struct flag_type type_flags[];
extern const struct flag_type apply_flags[];
extern const struct flag_type sex_flags[];
extern const struct flag_type furniture_flags[];
extern const struct flag_type weapon_class[];
extern const struct flag_type apply_types[];
extern const struct flag_type weapon_type2[];
extern const struct flag_type size_flags[];
extern const struct flag_type position_flags[];
extern const struct flag_type ac_type[];
extern const struct bit_type  bitvector_type[];

extern const struct chan_type chan_table[];

#endif /* TABLES_H */
