#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "interp.h"

// function prototypes
void do_race_info( CHAR_DATA *ch, char *argument);
void do_rcedit(CHAR_DATA *ch ,char * argument);
void load_race_file(const char *filename, int race_no);
void save_race_file(const char *filename, int race_no);
void load_race_files(void);

// macro definitions
#ifdef WIN32
#define RACE_TEMP "../data/race/t"
#define RACE_DIR "../data/race/"
#else
#define RACE_TEMP "data/race/t"
#define RACE_DIR "data/race/"
#endif

/**/
void do_race_info( CHAR_DATA *ch, char *argument ) {
	char race_name[MIL];
	int race_no, i;
	char *attr_names[] = { "strength", "intelligence", "wisdom", "dexterity", "constitution" };
	char *size_names[] = { "tiny", "small", "medium", "large", "huge", "giant" };

	if (!argument[0]) {
		send_to_char ("Syntax is: raceinfo <race>.\n\r",ch);
		return;
	}

	argument = one_argument (argument, race_name);
	
	for (race_no = 0; race_table[race_no].name != NULL; race_no++)
		if (!str_cmp(race_name, race_table[race_no].name))
			break;
	
	if (race_table[race_no].name == NULL) {
		printf_to_char (ch, "No race named '%s' exists.\n\r", race_name);
		return;
	}

	printf_to_char(ch, "Race Info for '%s'\n\r", race_table[race_no].name);
	printf_to_char(ch, "  PC Race: %s\n\r", race_table[race_no].pc_race ? "yes" : "no");
	
	/* Show race flags */
	printf_to_char(ch, "  Act Flags: %s\n\r", act_bit_name(race_table[race_no].act));
	printf_to_char(ch, "  Affected By: %s\n\r", affect_bit_name(race_table[race_no].aff));
	printf_to_char(ch, "  Offensive: %s\n\r", off_bit_name(race_table[race_no].off));
	printf_to_char(ch, "  Immune: %s\n\r", imm_bit_name(race_table[race_no].imm));
	printf_to_char(ch, "  Resistant: %s\n\r", imm_bit_name(race_table[race_no].res));
	printf_to_char(ch, "  Vulnerable: %s\n\r", imm_bit_name(race_table[race_no].vuln));
	printf_to_char(ch, "  Shielded: %s\n\r", shield_bit_name(race_table[race_no].shd));
	printf_to_char(ch, "  Form: %s\n\r", form_bit_name(race_table[race_no].form));
	printf_to_char(ch, "  Parts: %s\n\r", part_bit_name(race_table[race_no].parts));
	
	/* Show PC race specific info if it's a PC race */
	if (race_table[race_no].pc_race && race_no < MAX_PC_RACE) {
		printf_to_char(ch, "\nPC Race Details:\n\r");
		printf_to_char(ch, "  Who Name: %s\n\r", pc_race_table[race_no].who_name);
		printf_to_char(ch, "  Points Cost: %d\n\r", pc_race_table[race_no].points);
		printf_to_char(ch, "  Size: %s\n\r", size_names[pc_race_table[race_no].size]);
		printf_to_char(ch, "  Tier: %d\n\r", pc_race_table[race_no].tier);
		
		printf_to_char(ch, "  Starting Stats:\n\r");
		for (i = 0; i < MAX_STATS; i++) {
			printf_to_char(ch, "    %s: %d\n\r", attr_names[i], pc_race_table[race_no].stats[i]);
		}
		
		printf_to_char(ch, "  Maximum Stats:\n\r");
		for (i = 0; i < MAX_STATS; i++) {
			printf_to_char(ch, "    %s: %d\n\r", attr_names[i], pc_race_table[race_no].max_stats[i]);
		}
		
		printf_to_char(ch, "  Class Experience Multipliers:\n\r");
		for (i = 0; i < MAX_CLASS; i++) {
			printf_to_char(ch, "    %s: %d%%\n\r", class_table[i].name, pc_race_table[race_no].class_mult[i]);
		}
		
		printf_to_char(ch, "  Bonus Skills:\n\r");
		for (i = 0; i < 5; i++) {
			if (pc_race_table[race_no].skills[i] != NULL && pc_race_table[race_no].skills[i][0] != '\0') {
				printf_to_char(ch, "    %s\n\r", pc_race_table[race_no].skills[i]);
			}
		}
	}
	
} // do_race_info()

/**/
void do_rcedit(CHAR_DATA *ch ,char * argument) {
	char race_name[MIL], field_name[MIL];
	int race_no, value, i;
	
	argument = one_argument (argument, race_name);
	argument = one_argument (argument, field_name);
	
	if (!argument[0])
	{
		send_to_char ("Syntax is: rcedit <race> <field> <value>.\n\r",ch);
		send_to_char ("PC Race Fields: pcrace, points, size, tier, stat, maxstat, mult, skill\n\r",ch);
		send_to_char ("Race Flag Fields: act, aff, off, imm, res, vuln, shd, form, parts\n\r",ch);
		send_to_char ("Flag Removal: remove_act, remove_aff, remove_off, remove_imm, remove_res, remove_vuln, remove_shd, remove_form, remove_parts\n\r",ch);
		return;
	}
	
	/* Find the race */
	for (race_no = 0; race_table[race_no].name != NULL; race_no++)
		if (!str_cmp(race_name, race_table[race_no].name))
			break;
	
	if (race_table[race_no].name == NULL)
	{
		printf_to_char (ch, "No race named '%s' exists.\n\r", race_name);
		return;
	}
	
	/* Check if it's a PC race for PC-specific editing */
	bool is_pc_race = (race_table[race_no].pc_race && race_no < MAX_PC_RACE);
	
	/* Parse the field and value */
	if (!str_cmp(field_name, "pcrace")) {
		if (!str_cmp(argument, "yes") || !str_cmp(argument, "true") || !str_cmp(argument, "1")) {
			if (race_no >= MAX_PC_RACE) {
				printf_to_char(ch, "Cannot make %s a PC race - race number %d exceeds MAX_PC_RACE (%d).\n\r", 
					race_table[race_no].name, race_no, MAX_PC_RACE);
				return;
			}
			race_table[race_no].pc_race = TRUE;
			printf_to_char(ch, "OK, %s is now a PC race.\n\r", race_table[race_no].name);
		}
		else if (!str_cmp(argument, "no") || !str_cmp(argument, "false") || !str_cmp(argument, "0")) {
			race_table[race_no].pc_race = FALSE;
			printf_to_char(ch, "OK, %s is now an NPC race.\n\r", race_table[race_no].name);
		}
		else {
			send_to_char("Use: yes/no, true/false, or 1/0\n\r", ch);
		}
	}
	else if (!str_cmp(field_name, "points")) {
		if (!is_pc_race) {
			send_to_char("Points can only be set for PC races.\n\r", ch);
			return;
		}
		value = atoi(argument);
		if (!is_number(argument) || value < 0 || value > 50) {
			send_to_char("Points must be between 0 and 50.\n\r", ch);
			return;
		}
		pc_race_table[race_no].points = value;
		printf_to_char(ch, "OK, %s now costs %d points.\n\r", race_table[race_no].name, value);
	}
	else if (!str_cmp(field_name, "size")) {
		if (!is_pc_race) {
			send_to_char("Size can only be set for PC races.\n\r", ch);
			return;
		}
		value = atoi(argument);
		if (!is_number(argument) || value < 0 || value > 5) {
			send_to_char("Size must be between 0 (tiny) and 5 (giant).\n\r", ch);
			return;
		}
		pc_race_table[race_no].size = value;
		printf_to_char(ch, "OK, %s size is now %d.\n\r", race_table[race_no].name, value);
	}
	else if (!str_cmp(field_name, "tier")) {
		if (!is_pc_race) {
			send_to_char("Tier can only be set for PC races.\n\r", ch);
			return;
		}
		value = atoi(argument);
		if (!is_number(argument) || value < 1 || value > 5) {
			send_to_char("Tier must be between 1 and 5.\n\r", ch);
			return;
		}
		pc_race_table[race_no].tier = value;
		printf_to_char(ch, "OK, %s tier is now %d.\n\r", race_table[race_no].name, value);
	}
	else if (!str_cmp(field_name, "stat")) {
		if (!is_pc_race) {
			send_to_char("Stats can only be set for PC races.\n\r", ch);
			return;
		}
		/* Format: rcedit race stat <stat_name> <value> */
		char stat_name[MIL];
		argument = one_argument(argument, stat_name);
		value = atoi(argument);
		
		if (!is_number(argument) || value < 3 || value > 25) {
			send_to_char("Stat value must be between 3 and 25.\n\r", ch);
			return;
		}
		
		/* Find the stat */
		for (i = 0; i < MAX_STATS; i++) {
			char *attr_names[] = { "strength", "intelligence", "wisdom", "dexterity", "constitution" };
			if (!str_cmp(stat_name, attr_names[i])) {
				pc_race_table[race_no].stats[i] = value;
				printf_to_char(ch, "OK, %s starting %s is now %d.\n\r", 
					race_table[race_no].name, stat_name, value);
				return;
			}
		}
		send_to_char("Invalid stat name. Use: strength, intelligence, wisdom, dexterity, constitution\n\r", ch);
	}
	else if (!str_cmp(field_name, "maxstat")) {
		if (!is_pc_race) {
			send_to_char("Max stats can only be set for PC races.\n\r", ch);
			return;
		}
		/* Format: rcedit race maxstat <stat_name> <value> */
		char stat_name[MIL];
		argument = one_argument(argument, stat_name);
		value = atoi(argument);
		
		if (!is_number(argument) || value < 3 || value > 25) {
			send_to_char("Max stat value must be between 3 and 25.\n\r", ch);
			return;
		}
		
		/* Find the stat */
		for (i = 0; i < MAX_STATS; i++) {
			char *attr_names[] = { "strength", "intelligence", "wisdom", "dexterity", "constitution" };
			if (!str_cmp(stat_name, attr_names[i])) {
				pc_race_table[race_no].max_stats[i] = value;
				printf_to_char(ch, "OK, %s maximum %s is now %d.\n\r", 
					race_table[race_no].name, stat_name, value);
				return;
			}
		}
		send_to_char("Invalid stat name. Use: strength, intelligence, wisdom, dexterity, constitution\n\r", ch);
	}
	else if (!str_cmp(field_name, "mult")) {
		if (!is_pc_race) {
			send_to_char("Class multipliers can only be set for PC races.\n\r", ch);
			return;
		}
		/* Format: rcedit race mult <class_name> <value> */
		char class_name[MIL];
		argument = one_argument(argument, class_name);
		value = atoi(argument);
		
		if (!is_number(argument) || value < 50 || value > 300) {
			send_to_char("Multiplier must be between 50 and 300 (percent).\n\r", ch);
			return;
		}
		
		/* Find the class */
		for (i = 0; i < MAX_CLASS; i++) {
			if (!str_cmp(class_name, class_table[i].name)) {
				pc_race_table[race_no].class_mult[i] = value;
				printf_to_char(ch, "OK, %s %s experience multiplier is now %d%%.\n\r", 
					race_table[race_no].name, class_name, value);
				return;
			}
		}
		send_to_char("Invalid class name.\n\r", ch);
	}
	else if (!str_cmp(field_name, "skill")) {
		if (!is_pc_race) {
			send_to_char("Bonus skills can only be set for PC races.\n\r", ch);
			return;
		}
		/* Format: rcedit race skill <skill_name> */
		/* For now, just add to the first available slot */
		for (i = 0; i < 5; i++) {
			if (pc_race_table[race_no].skills[i] == NULL || pc_race_table[race_no].skills[i][0] == '\0') {
				pc_race_table[race_no].skills[i] = str_dup(argument);
				printf_to_char(ch, "OK, %s now has bonus skill '%s'.\n\r", 
					race_table[race_no].name, argument);
				return;
			}
		}
		send_to_char("No more skill slots available for this race.\n\r", ch);
	}
	else if (!str_cmp(field_name, "act")) {
		value = flag_value(act_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid act flag. Use: sentinel, aggressive, etc.\n\r", ch);
			return;
		}
		race_table[race_no].act = value;
		printf_to_char(ch, "OK, %s act flags are now %s.\n\r", 
			race_table[race_no].name, act_bit_name(value));
	}
	else if (!str_cmp(field_name, "aff")) {
		value = flag_value(affect_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid affect flag. Use: charm, sleep, etc.\n\r", ch);
			return;
		}
		race_table[race_no].aff = value;
		printf_to_char(ch, "OK, %s affected by flags are now %s.\n\r", 
			race_table[race_no].name, affect_bit_name(value));
	}
	else if (!str_cmp(field_name, "off")) {
		value = flag_value(off_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid offensive flag. Use: area_attack, etc.\n\r", ch);
			return;
		}
		race_table[race_no].off = value;
		printf_to_char(ch, "OK, %s offensive flags are now %s.\n\r", 
			race_table[race_no].name, off_bit_name(value));
	}
	else if (!str_cmp(field_name, "imm")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid immunity flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race_table[race_no].imm = value;
		printf_to_char(ch, "OK, %s immunity flags are now %s.\n\r", 
			race_table[race_no].name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "res")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid resistance flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race_table[race_no].res = value;
		printf_to_char(ch, "OK, %s resistance flags are now %s.\n\r", 
			race_table[race_no].name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "vuln")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid vulnerability flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race_table[race_no].vuln = value;
		printf_to_char(ch, "OK, %s vulnerability flags are now %s.\n\r", 
			race_table[race_no].name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "shd")) {
		value = flag_value(shield_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid shield flag. Use: sanctuary, fire, etc.\n\r", ch);
			return;
		}
		race_table[race_no].shd = value;
		printf_to_char(ch, "OK, %s shield flags are now %s.\n\r", 
			race_table[race_no].name, shield_bit_name(value));
	}
	else if (!str_cmp(field_name, "form")) {
		value = flag_value(form_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid form flag. Use: animal, sentient, etc.\n\r", ch);
			return;
		}
		race_table[race_no].form = value;
		printf_to_char(ch, "OK, %s form flags are now %s.\n\r", 
			race_table[race_no].name, form_bit_name(value));
	}
	else if (!str_cmp(field_name, "parts")) {
		value = flag_value(part_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid parts flag. Use: head, arms, legs, etc.\n\r", ch);
			return;
		}
		race_table[race_no].parts = value;
		printf_to_char(ch, "OK, %s parts flags are now %s.\n\r", 
			race_table[race_no].name, part_bit_name(value));
	}
	else if (!str_cmp(field_name, "remove_act")) {
		value = flag_value(act_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid act flag. Use: sentinel, aggressive, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].act, value);
		printf_to_char(ch, "OK, removed %s from %s act flags. Now: %s\n\r", 
			act_bit_name(value), race_table[race_no].name, act_bit_name(race_table[race_no].act));
	}
	else if (!str_cmp(field_name, "remove_aff")) {
		value = flag_value(affect_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid affect flag. Use: charm, sleep, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].aff, value);
		printf_to_char(ch, "OK, removed %s from %s affected by flags. Now: %s\n\r", 
			affect_bit_name(value), race_table[race_no].name, affect_bit_name(race_table[race_no].aff));
	}
	else if (!str_cmp(field_name, "remove_off")) {
		value = flag_value(off_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid offensive flag. Use: area_attack, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].off, value);
		printf_to_char(ch, "OK, removed %s from %s offensive flags. Now: %s\n\r", 
			off_bit_name(value), race_table[race_no].name, off_bit_name(race_table[race_no].off));
	}
	else if (!str_cmp(field_name, "remove_imm")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid immunity flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].imm, value);
		printf_to_char(ch, "OK, removed %s from %s immunity flags. Now: %s\n\r", 
			imm_bit_name(value), race_table[race_no].name, imm_bit_name(race_table[race_no].imm));
	}
	else if (!str_cmp(field_name, "remove_res")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid resistance flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].res, value);
		printf_to_char(ch, "OK, removed %s from %s resistance flags. Now: %s\n\r", 
			imm_bit_name(value), race_table[race_no].name, imm_bit_name(race_table[race_no].res));
	}
	else if (!str_cmp(field_name, "remove_vuln")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid vulnerability flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].vuln, value);
		printf_to_char(ch, "OK, removed %s from %s vulnerability flags. Now: %s\n\r", 
			imm_bit_name(value), race_table[race_no].name, imm_bit_name(race_table[race_no].vuln));
	}
	else if (!str_cmp(field_name, "remove_shd")) {
		value = flag_value(shield_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid shield flag. Use: sanctuary, fire, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].shd, value);
		printf_to_char(ch, "OK, removed %s from %s shield flags. Now: %s\n\r", 
			shield_bit_name(value), race_table[race_no].name, shield_bit_name(race_table[race_no].shd));
	}
	else if (!str_cmp(field_name, "remove_form")) {
		value = flag_value(form_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid form flag. Use: animal, sentient, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].form, value);
		printf_to_char(ch, "OK, removed %s from %s form flags. Now: %s\n\r", 
			form_bit_name(value), race_table[race_no].name, form_bit_name(race_table[race_no].form));
	}
	else if (!str_cmp(field_name, "remove_parts")) {
		value = flag_value(part_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid parts flag. Use: head, arms, legs, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race_table[race_no].parts, value);
		printf_to_char(ch, "OK, removed %s from %s parts flags. Now: %s\n\r", 
			part_bit_name(value), race_table[race_no].name, part_bit_name(race_table[race_no].parts));
	}
	else {
		send_to_char("Invalid field. Use: pcrace, points, size, tier, stat, maxstat, mult, skill, act, aff, off, imm, res, vuln, shd, form, parts, remove_act, remove_aff, remove_off, remove_imm, remove_res, remove_vuln, remove_shd, remove_form, remove_parts\n\r", ch);
	}
	
} // do_rcedit()





/**/
void save_race_file(const char *filename, int race_no) {
	FILE *fp;
	int i;
	
	if ((fp = fopen(filename, "w")) == NULL) {
		bug("save_race_file: cannot open file for writing", 0);
		return;
	}
	
	/* Write race header */
	fprintf(fp, "#RACE\n");
	fprintf(fp, "Name %s~\n", race_table[race_no].name);
	fprintf(fp, "PC_RACE %s\n", race_table[race_no].pc_race ? "yes" : "no");
	fprintf(fp, "Act %ld\n", race_table[race_no].act);
	fprintf(fp, "Aff %ld\n", race_table[race_no].aff);
	fprintf(fp, "Off %ld\n", race_table[race_no].off);
	fprintf(fp, "Imm %ld\n", race_table[race_no].imm);
	fprintf(fp, "Res %ld\n", race_table[race_no].res);
	fprintf(fp, "Vuln %ld\n", race_table[race_no].vuln);
	fprintf(fp, "Shd %ld\n", race_table[race_no].shd);
	fprintf(fp, "Form %ld\n", race_table[race_no].form);
	fprintf(fp, "Parts %ld\n", race_table[race_no].parts);
	
	/* Write PC race specific data if it's a PC race */
	if (race_table[race_no].pc_race && race_no < MAX_PC_RACE) {
		fprintf(fp, "Who_Name %s~\n", pc_race_table[race_no].who_name);
		fprintf(fp, "Points %d\n", pc_race_table[race_no].points);
		fprintf(fp, "Size %d\n", pc_race_table[race_no].size);
		fprintf(fp, "Tier %d\n", pc_race_table[race_no].tier);
		
		/* Write starting stats */
		fprintf(fp, "Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " %d", pc_race_table[race_no].stats[i]);
		}
		fprintf(fp, "\n");
		
		/* Write max stats */
		fprintf(fp, "Max_Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " %d", pc_race_table[race_no].max_stats[i]);
		}
		fprintf(fp, "\n");
		
		/* Write class multipliers */
		fprintf(fp, "Class_Mult");
		for (i = 0; i < MAX_CLASS; i++) {
			fprintf(fp, " %d", pc_race_table[race_no].class_mult[i]);
		}
		fprintf(fp, "\n");
		
		/* Write bonus skills */
		fprintf(fp, "Skills");
		for (i = 0; i < 5; i++) {
			if (pc_race_table[race_no].skills[i] != NULL && pc_race_table[race_no].skills[i][0] != '\0') {
				fprintf(fp, " %s", pc_race_table[race_no].skills[i]);
			}
		}
		fprintf(fp, "\n");
	} else {
		/* For NPC races, write default PC race data */
		fprintf(fp, "Who_Name %s~\n", race_table[race_no].name);
		fprintf(fp, "Points 0\n");
		fprintf(fp, "Size 2\n");  /* SIZE_MEDIUM */
		fprintf(fp, "Tier 1\n");
		
		/* Write default starting stats */
		fprintf(fp, "Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " 13");  /* Default stat value */
		}
		fprintf(fp, "\n");
		
		/* Write default max stats */
		fprintf(fp, "Max_Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " 18");  /* Default max stat value */
		}
		fprintf(fp, "\n");
		
		/* Write default class multipliers */
		fprintf(fp, "Class_Mult");
		for (i = 0; i < MAX_CLASS; i++) {
			fprintf(fp, " 100");  /* Default 100% multiplier */
		}
		fprintf(fp, "\n");
		
		/* Write empty skills */
		fprintf(fp, "Skills\n");
	}
	
	fprintf(fp, "End\n");
	fclose(fp);
	
} // save_race_file()

/**/
void load_race_file(const char *filename, int race_no) {
	FILE *fp;
	char *word;
	bool fMatch;
	int i;
	
	if ((fp = fopen(filename, "r")) == NULL) {
		bug("load_race_file: cannot open file for reading", 0);
		return;
	}
	

	
	for (;;) {
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = FALSE;
		
		switch (UPPER(word[0])) {
			case '#':
				fMatch = TRUE;
				fread_to_eol(fp);
				break;
				
			case '*':
				fMatch = TRUE;
				fread_to_eol(fp);
				break;
				
			case 'A':
				if (!str_cmp(word, "Act")) {
					race_table[race_no].act = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Aff")) {
					race_table[race_no].aff = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'B':
				if (!str_cmp(word, "bash") || !str_cmp(word, "berserk")) {
					/* Find first empty skill slot */
					for (i = 0; i < 5; i++) {
						if (pc_race_table[race_no].skills[i] == NULL) {
							pc_race_table[race_no].skills[i] = str_dup(word);
							break;
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'C':
				if (!str_cmp(word, "Class_Mult")) {
					for (i = 0; i < MAX_CLASS; i++) {
						pc_race_table[race_no].class_mult[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				break;
				
			case 'E':
				if (!str_cmp(word, "End")) {
					fclose(fp);
					return;
				}
				else if (!str_cmp(word, "enhanced")) {
					/* Read the next word to get "enhanced damage" */
					char *next_word = fread_word(fp);
					if (next_word && !str_cmp(next_word, "damage")) {
						/* Find first empty skill slot */
						for (i = 0; i < 5; i++) {
							if (pc_race_table[race_no].skills[i] == NULL) {
								pc_race_table[race_no].skills[i] = str_dup("enhanced damage");
								break;
							}
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'F':
				if (!str_cmp(word, "Form")) {
					race_table[race_no].form = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "fast")) {
					/* Read the next word to get "fast healing" */
					char *next_word = fread_word(fp);
					if (next_word && !str_cmp(next_word, "healing")) {
						/* Find first empty skill slot */
						for (i = 0; i < 5; i++) {
							if (pc_race_table[race_no].skills[i] == NULL) {
								pc_race_table[race_no].skills[i] = str_dup("fast healing");
								break;
							}
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'H':
				if (!str_cmp(word, "hide")) {
					/* Find first empty skill slot */
					for (i = 0; i < 5; i++) {
						if (pc_race_table[race_no].skills[i] == NULL) {
							pc_race_table[race_no].skills[i] = str_dup("hide");
							break;
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'I':
				if (!str_cmp(word, "Imm")) {
					race_table[race_no].imm = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'M':
				if (!str_cmp(word, "Max_Stats")) {
					for (i = 0; i < MAX_STATS; i++) {
						pc_race_table[race_no].max_stats[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "meditation")) {
					/* Find first empty skill slot */
					for (i = 0; i < 5; i++) {
						if (pc_race_table[race_no].skills[i] == NULL) {
							pc_race_table[race_no].skills[i] = str_dup("meditation");
							break;
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'N':
				if (!str_cmp(word, "Name")) {
					race_table[race_no].name = fread_string(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'O':
				if (!str_cmp(word, "Off")) {
					race_table[race_no].off = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'P':
				if (!str_cmp(word, "PC_RACE")) {
					char *pc_race_str = fread_word(fp);
					race_table[race_no].pc_race = (!str_cmp(pc_race_str, "yes"));
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Parts")) {
					race_table[race_no].parts = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Points")) {
					pc_race_table[race_no].points = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'R':
				if (!str_cmp(word, "Res")) {
					race_table[race_no].res = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'S':
				if (!str_cmp(word, "Shd")) {
					race_table[race_no].shd = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Size")) {
					pc_race_table[race_no].size = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Skills")) {
					/* Initialize all skills to NULL first */
					for (i = 0; i < 5; i++) {
						pc_race_table[race_no].skills[i] = NULL;
					}
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Stats")) {
					for (i = 0; i < MAX_STATS; i++) {
						pc_race_table[race_no].stats[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "sneak")) {
					/* Find first empty skill slot */
					for (i = 0; i < 5; i++) {
						if (pc_race_table[race_no].skills[i] == NULL) {
							pc_race_table[race_no].skills[i] = str_dup("sneak");
							break;
						}
					}
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "second")) {
					/* Read the next word to get "second attack" */
					char *next_word = fread_word(fp);
					if (next_word && !str_cmp(next_word, "attack")) {
						/* Find first empty skill slot */
						for (i = 0; i < 5; i++) {
							if (pc_race_table[race_no].skills[i] == NULL) {
								pc_race_table[race_no].skills[i] = str_dup("second attack");
								break;
							}
						}
					}
					fMatch = TRUE;
				}
				break;
				
			case 'T':
				if (!str_cmp(word, "Tier")) {
					pc_race_table[race_no].tier = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'V':
				if (!str_cmp(word, "Vuln")) {
					race_table[race_no].vuln = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'W':
				if (!str_cmp(word, "Who_Name")) {
					strcpy(pc_race_table[race_no].who_name, fread_string(fp));
					fMatch = TRUE;
				}
				break;
				

		}
		
		if (!fMatch) {
			bug("load_race_file: no match for word '%s'", word);
			fread_to_eol(fp);
		}
	}
	
} // load_race_file()

/**/
void load_race_files(void) {
	char filename[MIL];
	int race_no;
	
	/* Try to load each race file based on the race table */
	for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
		sprintf(filename, "%s%s.race", RACE_DIR, race_table[race_no].name);
		
		/* Try to load the race file - if it doesn't exist, skip it */
		FILE *fp = fopen(filename, "r");
		if (fp != NULL) {
			fclose(fp);
			load_race_file(filename, race_no);
		}
		/* If file doesn't exist, race keeps its hardcoded values */
	}
	
} // load_race_files()
