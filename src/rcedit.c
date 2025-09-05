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
#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#endif
#include "merc.h"
#include "tables.h"
#include "interp.h"
#include "olc.h"

// function prototypes
void do_race_info( CHAR_DATA *ch, char *argument);
void do_rcedit(CHAR_DATA *ch ,char * argument);
void save_race_file(const char *filename, int race_no);
/* Old race loading function declarations removed - now using file-based system */
/* Old race management function declarations removed - now using file-based system */

// File-based race system prototypes
struct race_data *load_race_from_file(const char *race_name);
struct race_data *find_race_by_name(const char *race_name);
void free_race_data(struct race_data *race);
struct race_data *create_race_data(void);
void list_available_races(CHAR_DATA *ch);
struct race_data *get_race_by_index(int race_num);

// macro definitions
#define RACE_TEMP "../data/race/t"
#define RACE_DIR "../data/race/"

// Global race list
struct race_data *race_list = NULL;

/**/	
void do_race_info( CHAR_DATA *ch, char *argument ) {
	char race_name[MIL];
	struct race_data *race;
	int i;
	char *attr_names[] = { "strength", "intelligence", "wisdom", "dexterity", "constitution" };
	char *size_names[] = { "tiny", "small", "medium", "large", "huge", "giant" };

	if (!argument[0]) {
		send_to_char ("Syntax is: raceinfo <race>.\n\r",ch);
		return;
	}

	argument = one_argument (argument, race_name);
	
	/* Load all races if not already loaded */
	if (race_list == NULL) {
		load_all_races();
	}
	
	/* Find the race */
	race = find_race_by_name(race_name);
	if (race == NULL) {
		printf_to_char (ch, "No race named '%s' exists.\n\r", race_name);
		return;
	}

	printf_to_char(ch, "Race Info for '%s'\n\r", race->name);
	printf_to_char(ch, "  PC Race: %s\n\r", race->pc_race ? "yes" : "no");
	
	/* Show race flags */
	printf_to_char(ch, "  Act Flags: %s\n\r", act_bit_name(race->act));
	printf_to_char(ch, "  Affected By: %s\n\r", affect_bit_name(race->aff));
	printf_to_char(ch, "  Offensive: %s\n\r", off_bit_name(race->off));
	printf_to_char(ch, "  Immune: %s\n\r", imm_bit_name(race->imm));
	printf_to_char(ch, "  Resistant: %s\n\r", imm_bit_name(race->res));
	printf_to_char(ch, "  Vulnerable: %s\n\r", imm_bit_name(race->vuln));
	printf_to_char(ch, "  Shielded: %s\n\r", shield_bit_name(race->shd));
	printf_to_char(ch, "  Form: %s\n\r", form_bit_name(race->form));
	printf_to_char(ch, "  Parts: %s\n\r", part_bit_name(race->parts));
	
	/* Show PC race specific info if it's a PC race */
	if (race->pc_race) {
		printf_to_char(ch, "\nPC Race Details:\n\r");
		printf_to_char(ch, "  Who Name: %s\n\r", race->who_name);
		printf_to_char(ch, "  Points Cost: %d\n\r", race->points);
		printf_to_char(ch, "  Size: %s\n\r", size_names[race->size]);
		printf_to_char(ch, "  Tier: %d\n\r", race->tier);
		
		printf_to_char(ch, "  Starting Stats:\n\r");
		for (i = 0; i < MAX_STATS; i++) {
			printf_to_char(ch, "    %s: %d\n\r", attr_names[i], race->stats[i]);
		}
		
		printf_to_char(ch, "  Maximum Stats:\n\r");
		for (i = 0; i < MAX_STATS; i++) {
			printf_to_char(ch, "    %s: %d\n\r", attr_names[i], race->max_stats[i]);
		}
		
		printf_to_char(ch, "  Class Experience Multipliers:\n\r");
		for (i = 0; i < MAX_CLASS; i++) {
			printf_to_char(ch, "    %s: %d%%\n\r", class_table[i].name, race->class_mult[i]);
		}
		
		printf_to_char(ch, "  Bonus Skills:\n\r");
		for (i = 0; i < 5; i++) {
			if (race->skills[i] != NULL && race->skills[i][0] != '\0') {
				printf_to_char(ch, "    %s\n\r", race->skills[i]);
			}
		}
	}
	
} // do_race_info()

/**/
void do_rcedit(CHAR_DATA *ch ,char * argument) {
	char race_name[MIL], field_name[MIL];
	int value, i;
	
	argument = one_argument (argument, race_name);
	
	/* Special handling for create command */
	if (!str_cmp(race_name, "create")) {
		if (!argument[0]) {
			send_to_char("Syntax is: rcedit create <race_name>.\n\r", ch);
			return;
		}
		
		/* Load all races if not already loaded */
		if (race_list == NULL) {
			load_all_races();
		}
		
		/* Check if race already exists */
		if (find_race_by_name(argument) != NULL) {
			printf_to_char(ch, "A race named '%s' already exists.\n\r", argument);
			return;
		}
		
		/* Validate race name */
		if (strlen(argument) < 2 || strlen(argument) > 20) {
			send_to_char("Race name must be between 2 and 20 characters.\n\r", ch);
			return;
		}
		
		/* Create new race using file-based system */
		struct race_data *new_race = create_race_data();
		new_race->name = str_dup(argument);
		new_race->pc_race = FALSE;
		
		/* Add to race list */
		new_race->next = race_list;
		race_list = new_race;
		
		printf_to_char(ch, "OK, created new race '%s'.\n\r", argument);
		
		/* Save the race file */
		char filename[MIL];
		sprintf(filename, "%s%s.race", RACE_DIR, argument);
		save_race_file(filename, 0); /* Use 0 as dummy race_no for file-based system */
		
		/* Add to race list file */
		char list_filename[MIL];
		sprintf(list_filename, "%srace_list.txt", RACE_DIR);
		FILE *list_fp = fopen(list_filename, "a");
		if (list_fp != NULL) {
			fprintf(list_fp, "%s\n", argument);
			fclose(list_fp);
		}
		
		return;
	}
	
	/* Special handling for delete command */
	if (!str_cmp(race_name, "delete")) {
		if (!argument[0]) {
			send_to_char("Syntax is: rcedit delete <race_name>.\n\r", ch);
			return;
		}
		
		/* Load all races if not already loaded */
		if (race_list == NULL) {
			load_all_races();
		}
		
		/* Find the race to delete */
		struct race_data *race_to_delete = find_race_by_name(argument);
		if (race_to_delete == NULL) {
			printf_to_char(ch, "No race named '%s' exists.\n\r", argument);
			return;
		}
		
		/* Check if any players are using this race */
		int race_index = race_lookup(argument);
		CHAR_DATA *wch;
		for (wch = char_list; wch != NULL; wch = wch->next) {
			if (wch->race == race_index) {
				printf_to_char(ch, "Cannot delete race '%s' - players are currently using it.\n\r", argument);
				return;
			}
		}
		
		/* Remove from race list */
		if (race_list == race_to_delete) {
			race_list = race_to_delete->next;
		} else {
			struct race_data *prev = race_list;
			while (prev != NULL && prev->next != race_to_delete) {
				prev = prev->next;
			}
			if (prev != NULL) {
				prev->next = race_to_delete->next;
			}
		}
		
		printf_to_char(ch, "OK, race deleted.\n\r");
		
		/* Delete the race file */
		char filename[MIL];
		sprintf(filename, "%s%s.race", RACE_DIR, argument);
		unlink(filename);
		
		/* Remove from race list file */
		char list_filename[MIL];
		sprintf(list_filename, "%srace_list.txt", RACE_DIR);
		FILE *list_fp = fopen(list_filename, "r");
		if (list_fp != NULL) {
			char temp_filename[MIL];
			sprintf(temp_filename, "%srace_list.tmp", RACE_DIR);
			FILE *temp_fp = fopen(temp_filename, "w");
			if (temp_fp != NULL) {
				char line[MIL];
				while (fgets(line, sizeof(line), list_fp) != NULL) {
					/* Remove newline */
					char *newline = strchr(line, '\n');
					if (newline) *newline = '\0';
					char *carriage = strchr(line, '\r');
					if (carriage) *carriage = '\0';
					
					/* Skip the deleted race */
					if (str_cmp(line, argument)) {
						fprintf(temp_fp, "%s\n", line);
					}
				}
				fclose(temp_fp);
				fclose(list_fp);
				
				/* Replace the original file */
				unlink(list_filename);
				rename(temp_filename, list_filename);
			} else {
				fclose(list_fp);
			}
		}
		
		/* Free the race data */
		free_race_data(race_to_delete);
		
		return;
	}
	
	/* Special handling for list command */
	if (!str_cmp(race_name, "list")) {
		list_available_races(ch);
		return;
	}
	
	argument = one_argument (argument, field_name);
	
	if (!argument[0])
	{
		send_to_char ("Syntax is: rcedit <race> <field> <value>.\n\r",ch);
		send_to_char ("PC Race Fields: pcrace, points, size, tier, stat, maxstat, mult, skill\n\r",ch);
		send_to_char ("Race Flag Fields: act, aff, off, imm, res, vuln, shd, form, parts\n\r",ch);
		send_to_char ("Flag Removal: remove_act, remove_aff, remove_off, remove_imm, remove_res, remove_vuln, remove_shd, remove_form, remove_parts\n\r",ch);
		send_to_char ("Race Management: rename <new_name>\n\r",ch);
		send_to_char ("Special Commands: rcedit create <race_name>, rcedit delete <race_name>, rcedit list\n\r",ch);
		return;
	}
	
	/* Load all races if not already loaded */
	if (race_list == NULL) {
		load_all_races();
	}
	
	/* Find the race */
	struct race_data *race = find_race_by_name(race_name);
	if (race == NULL) {
		printf_to_char (ch, "No race named '%s' exists.\n\r", race_name);
		return;
	}
	
	/* Check if it's a PC race for PC-specific editing */
	bool is_pc_race = race->pc_race;
	
	/* Parse the field and value */
	if (!str_cmp(field_name, "pcrace")) {
		if (!str_cmp(argument, "yes") || !str_cmp(argument, "true") || !str_cmp(argument, "1")) {
			race->pc_race = TRUE;
			printf_to_char(ch, "OK, %s is now a PC race.\n\r", race->name);
		}
		else if (!str_cmp(argument, "no") || !str_cmp(argument, "false") || !str_cmp(argument, "0")) {
			race->pc_race = FALSE;
			printf_to_char(ch, "OK, %s is now an NPC race.\n\r", race->name);
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
		race->points = value;
		printf_to_char(ch, "OK, %s now costs %d points.\n\r", race->name, value);
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
		race->size = value;
		printf_to_char(ch, "OK, %s size is now %d.\n\r", race->name, value);
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
		race->tier = value;
		printf_to_char(ch, "OK, %s tier is now %d.\n\r", race->name, value);
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
				race->stats[i] = value;
				printf_to_char(ch, "OK, %s starting %s is now %d.\n\r", 
					race->name, stat_name, value);
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
				race->max_stats[i] = value;
				printf_to_char(ch, "OK, %s maximum %s is now %d.\n\r", 
					race->name, stat_name, value);
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
				race->class_mult[i] = value;
				printf_to_char(ch, "OK, %s %s experience multiplier is now %d%%.\n\r", 
					race->name, class_name, value);
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
			if (race->skills[i] == NULL || race->skills[i][0] == '\0') {
				race->skills[i] = str_dup(argument);
				printf_to_char(ch, "OK, %s now has bonus skill '%s'.\n\r", 
					race->name, argument);
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
		race->act = value;
		printf_to_char(ch, "OK, %s act flags are now %s.\n\r", 
			race->name, act_bit_name(value));
	}
	else if (!str_cmp(field_name, "aff")) {
		value = flag_value(affect_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid affect flag. Use: charm, sleep, etc.\n\r", ch);
			return;
		}
		race->aff = value;
		printf_to_char(ch, "OK, %s affected by flags are now %s.\n\r", 
			race->name, affect_bit_name(value));
	}
	else if (!str_cmp(field_name, "off")) {
		value = flag_value(off_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid offensive flag. Use: area_attack, etc.\n\r", ch);
			return;
		}
		race->off = value;
		printf_to_char(ch, "OK, %s offensive flags are now %s.\n\r", 
			race->name, off_bit_name(value));
	}
	else if (!str_cmp(field_name, "imm")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid immunity flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race->imm = value;
		printf_to_char(ch, "OK, %s immunity flags are now %s.\n\r", 
			race->name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "res")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid resistance flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race->res = value;
		printf_to_char(ch, "OK, %s resistance flags are now %s.\n\r", 
			race->name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "vuln")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid vulnerability flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		race->vuln = value;
		printf_to_char(ch, "OK, %s vulnerability flags are now %s.\n\r", 
			race->name, imm_bit_name(value));
	}
	else if (!str_cmp(field_name, "shd")) {
		value = flag_value(shield_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid shield flag. Use: sanctuary, fire, etc.\n\r", ch);
			return;
		}
		race->shd = value;
		printf_to_char(ch, "OK, %s shield flags are now %s.\n\r", 
			race->name, shield_bit_name(value));
	}
	else if (!str_cmp(field_name, "form")) {
		value = flag_value(form_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid form flag. Use: animal, sentient, etc.\n\r", ch);
			return;
		}
		race->form = value;
		printf_to_char(ch, "OK, %s form flags are now %s.\n\r", 
			race->name, form_bit_name(value));
	}
	else if (!str_cmp(field_name, "parts")) {
		value = flag_value(part_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid parts flag. Use: head, arms, legs, etc.\n\r", ch);
			return;
		}
		race->parts = value;
		printf_to_char(ch, "OK, %s parts flags are now %s.\n\r", 
			race->name, part_bit_name(value));
	}
	else if (!str_cmp(field_name, "remove_act")) {
		value = flag_value(act_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid act flag. Use: sentinel, aggressive, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->act, value);
		printf_to_char(ch, "OK, removed %s from %s act flags. Now: %s\n\r", 
			act_bit_name(value), race->name, act_bit_name(race->act));
	}
	else if (!str_cmp(field_name, "remove_aff")) {
		value = flag_value(affect_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid affect flag. Use: charm, sleep, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->aff, value);
		printf_to_char(ch, "OK, removed %s from %s affected by flags. Now: %s\n\r", 
			affect_bit_name(value), race->name, affect_bit_name(race->aff));
	}
	else if (!str_cmp(field_name, "remove_off")) {
		value = flag_value(off_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid offensive flag. Use: area_attack, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->off, value);
		printf_to_char(ch, "OK, removed %s from %s offensive flags. Now: %s\n\r", 
			off_bit_name(value), race->name, off_bit_name(race->off));
	}
	else if (!str_cmp(field_name, "remove_imm")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid immunity flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->imm, value);
		printf_to_char(ch, "OK, removed %s from %s immunity flags. Now: %s\n\r", 
			imm_bit_name(value), race->name, imm_bit_name(race->imm));
	}
	else if (!str_cmp(field_name, "remove_res")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid resistance flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->res, value);
		printf_to_char(ch, "OK, removed %s from %s resistance flags. Now: %s\n\r", 
			imm_bit_name(value), race->name, imm_bit_name(race->res));
	}
	else if (!str_cmp(field_name, "remove_vuln")) {
		value = flag_value(imm_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid vulnerability flag. Use: fire, cold, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->vuln, value);
		printf_to_char(ch, "OK, removed %s from %s vulnerability flags. Now: %s\n\r", 
			imm_bit_name(value), race->name, imm_bit_name(race->vuln));
	}
	else if (!str_cmp(field_name, "remove_shd")) {
		value = flag_value(shield_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid shield flag. Use: sanctuary, fire, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->shd, value);
		printf_to_char(ch, "OK, removed %s from %s shield flags. Now: %s\n\r", 
			shield_bit_name(value), race->name, shield_bit_name(race->shd));
	}
	else if (!str_cmp(field_name, "remove_form")) {
		value = flag_value(form_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid form flag. Use: animal, sentient, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->form, value);
		printf_to_char(ch, "OK, removed %s from %s form flags. Now: %s\n\r", 
			form_bit_name(value), race->name, form_bit_name(race->form));
	}
	else if (!str_cmp(field_name, "remove_parts")) {
		value = flag_value(part_flags, argument);
		if (value == NO_FLAG) {
			send_to_char("Invalid parts flag. Use: head, arms, legs, etc.\n\r", ch);
			return;
		}
		REMOVE_BIT(race->parts, value);
		printf_to_char(ch, "OK, removed %s from %s parts flags. Now: %s\n\r", 
			part_bit_name(value), race->name, part_bit_name(race->parts));
	}
	else if (!str_cmp(field_name, "rename")) {
		/* Format: rcedit race rename <new_name> */
		if (strlen(argument) < 2 || strlen(argument) > 20) {
			send_to_char("Race name must be between 2 and 20 characters.\n\r", ch);
			return;
		}
		
		/* Check if new name already exists */
		if (find_race_by_name(argument) != NULL) {
			printf_to_char(ch, "A race named '%s' already exists.\n\r", argument);
			return;
		}
		
		/* Free old name and set new name */
		free_string(race->name);
		race->name = str_dup(argument);
		
		printf_to_char(ch, "OK, race renamed to '%s'.\n\r", argument);
		
		/* Save the race file */
		char filename[MIL];
		sprintf(filename, "%s%s.race", RACE_DIR, argument);
		save_race_file(filename, 0); /* Use 0 as dummy race_no for file-based system */
	}
	else {
		send_to_char("Invalid field. Use: pcrace, points, size, tier, stat, maxstat, mult, skill, act, aff, off, imm, res, vuln, shd, form, parts, remove_act, remove_aff, remove_off, remove_imm, remove_res, remove_vuln, remove_shd, remove_form, remove_parts, rename, delete\n\r", ch);
		return;
	}
	
	/* Save the race file after any modification */
	char filename[MIL];
	sprintf(filename, "%s%s.race", RACE_DIR, race->name);
	save_race_file(filename, 0); /* Use 0 as dummy race_no for file-based system */
	
} // do_rcedit()





/**/
void save_race_file(const char *filename, int race_no) {
	FILE *fp;
	int i;
	
	if ((fp = fopen(filename, "w")) == NULL) {
		bug("save_race_file: cannot open file for writing", 0);
		return;
	}
	
	/* Find the race in the race_list */
	struct race_data *race = NULL;
	if (race_list != NULL) {
		/* Extract race name from filename */
		char race_name[MIL];
		strcpy(race_name, filename);
		char *slash = strrchr(race_name, '/');
		if (slash) slash++;
		else slash = race_name;
		char *dot = strrchr(slash, '.');
		if (dot) *dot = '\0';
		
		race = find_race_by_name(slash);
	}
	
	if (race == NULL) {
		bug("save_race_file: race not found in race_list", 0);
		fclose(fp);
		return;
	}
	
	/* Write race header */
	fprintf(fp, "#RACE\n");
	fprintf(fp, "Name %s~\n", race->name);
	fprintf(fp, "PC_RACE %s\n", race->pc_race ? "yes" : "no");
	fprintf(fp, "Act %ld\n", race->act);
	fprintf(fp, "Aff %ld\n", race->aff);
	fprintf(fp, "Off %ld\n", race->off);
	fprintf(fp, "Imm %ld\n", race->imm);
	fprintf(fp, "Res %ld\n", race->res);
	fprintf(fp, "Vuln %ld\n", race->vuln);
	fprintf(fp, "Shd %ld\n", race->shd);
	fprintf(fp, "Form %ld\n", race->form);
	fprintf(fp, "Parts %ld\n", race->parts);
	
	/* Write PC race specific data if it's a PC race */
	if (race->pc_race) {
		fprintf(fp, "Who_Name %s~\n", race->who_name);
		fprintf(fp, "Points %d\n", race->points);
		fprintf(fp, "Size %d\n", race->size);
		fprintf(fp, "Tier %d\n", race->tier);
		
		/* Write starting stats */
		fprintf(fp, "Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " %d", race->stats[i]);
		}
		fprintf(fp, "\n");
		
		/* Write max stats */
		fprintf(fp, "Max_Stats");
		for (i = 0; i < MAX_STATS; i++) {
			fprintf(fp, " %d", race->max_stats[i]);
		}
		fprintf(fp, "\n");
		
		/* Write class multipliers */
		fprintf(fp, "Class_Mult");
		for (i = 0; i < MAX_CLASS; i++) {
			fprintf(fp, " %d", race->class_mult[i]);
		}
		fprintf(fp, "\n");
		
		/* Write bonus skills */
		fprintf(fp, "Skills");
		for (i = 0; i < 5; i++) {
			if (race->skills[i] != NULL && race->skills[i][0] != '\0') {
				fprintf(fp, " %s", race->skills[i]);
			}
		}
		fprintf(fp, "\n");
	} else {
		/* For NPC races, write default PC race data */
		fprintf(fp, "Who_Name %s~\n", race->name);
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

/* File-based race system implementation */

struct race_data *create_race_data(void) {
	struct race_data *race;
	
	race = alloc_mem(sizeof(struct race_data));
	race->next = NULL;
	race->name = NULL;
	race->pc_race = FALSE;
	race->act = 0;
	race->aff = 0;
	race->off = 0;
	race->imm = 0;
	race->res = 0;
	race->vuln = 0;
	race->shd = 0;
	race->form = 0;
	race->parts = 0;
	strcpy(race->who_name, "     ");
	race->points = 0;
	race->size = 2; /* SIZE_MEDIUM */
	race->tier = 1;
	
	/* Initialize arrays */
	int i;
	for (i = 0; i < MAX_CLASS; i++) {
		race->class_mult[i] = 100;
	}
	for (i = 0; i < MAX_STATS; i++) {
		race->stats[i] = 13;
		race->max_stats[i] = 18;
	}
	for (i = 0; i < 5; i++) {
		race->skills[i] = NULL;
	}
	
	return race;
}

void free_race_data(struct race_data *race) {
	if (race == NULL) return;
	
	if (race->name) {
		free_string(race->name);
	}
	
	int i;
	for (i = 0; i < 5; i++) {
		if (race->skills[i]) {
			free_string(race->skills[i]);
		}
	}
	
	free_mem(race, sizeof(struct race_data));
}

struct race_data *find_race_by_name(const char *race_name) {
	struct race_data *race;
	
	for (race = race_list; race != NULL; race = race->next) {
		if (!str_cmp(race_name, race->name)) {
			return race;
		}
	}
	
	return NULL;
}

struct race_data *load_race_from_file(const char *race_name) {
	char filename[MIL];
	FILE *fp;
	char *word;
	bool fMatch;
	int i;
	struct race_data *race;
	
	/* Check if race is already loaded */
	race = find_race_by_name(race_name);
	if (race != NULL) {
		return race;
	}
	
	/* Create new race data */
	race = create_race_data();
	race->name = str_dup(race_name);
	
	/* Try to load from file */
	sprintf(filename, "%s%s.race", RACE_DIR, race_name);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		/* File doesn't exist, use defaults */
		free_race_data(race);
		return NULL;
	}
	
	/* Parse the race file */
	for (;;) {
		if (feof(fp)) {
			word = "End";
		} else {
			word = fread_word(fp);
			if (word == NULL || word[0] == '\0') {
				word = "End";
			}
		}
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
					race->act = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Aff")) {
					race->aff = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'C':
				if (!str_cmp(word, "Class_Mult")) {
					for (i = 0; i < MAX_CLASS; i++) {
						race->class_mult[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				break;
				
			case 'E':
				if (!str_cmp(word, "End")) {
					fclose(fp);
					/* Add to race list */
					race->next = race_list;
					race_list = race;
					return race;
				}
				break;
				
			case 'F':
				if (!str_cmp(word, "Form")) {
					race->form = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'I':
				if (!str_cmp(word, "Imm")) {
					race->imm = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'M':
				if (!str_cmp(word, "Max_Stats")) {
					for (i = 0; i < MAX_STATS; i++) {
						race->max_stats[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				break;
				
			case 'N':
				if (!str_cmp(word, "Name")) {
					char *new_name = fread_string(fp);
					if (new_name != NULL) {
						free_string(race->name);
						race->name = new_name;
					}
					fMatch = TRUE;
				}
				break;
				
			case 'O':
				if (!str_cmp(word, "Off")) {
					race->off = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'P':
				if (!str_cmp(word, "PC_RACE")) {
					char *pc_race_str = fread_word(fp);
					race->pc_race = (!str_cmp(pc_race_str, "yes"));
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Parts")) {
					race->parts = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Points")) {
					race->points = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'R':
				if (!str_cmp(word, "Res")) {
					race->res = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'S':
				if (!str_cmp(word, "Shd")) {
					race->shd = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Size")) {
					race->size = fread_number(fp);
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Stats")) {
					for (i = 0; i < MAX_STATS; i++) {
						race->stats[i] = fread_number(fp);
					}
					fMatch = TRUE;
				}
				break;
				
			case 'T':
				if (!str_cmp(word, "Tier")) {
					race->tier = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'V':
				if (!str_cmp(word, "Vuln")) {
					race->vuln = fread_number(fp);
					fMatch = TRUE;
				}
				break;
				
			case 'W':
				if (!str_cmp(word, "Who_Name")) {
					strcpy(race->who_name, fread_string(fp));
					fMatch = TRUE;
				}
				break;
		}
		
		if (!fMatch) {
			fread_to_eol(fp);
		}
	}
	
	fclose(fp);
	free_race_data(race);
	return NULL;
}

void list_available_races(CHAR_DATA *ch) {
	struct race_data *race;
	int pc_count = 0, npc_count = 0;
	struct race_data *pc_races[100];  /* Assuming max 100 races */
	struct race_data *npc_races[100];  /* Assuming max 100 races */
	int pc_index = 0, npc_index = 0;
	int max_count, i;
	char line[200];
	char pc_name[50];
	char npc_name[50];
	
	send_to_char("Available races:\n\r", ch);
	send_to_char("PC Races:                    NPC Races:\n\r", ch);
	send_to_char("----------                   ----------\n\r", ch);
	
	/* First pass: collect PC races */
	for (race = race_list; race != NULL; race = race->next) {
		if (race->pc_race && pc_index < 100) {
			pc_races[pc_index++] = race;
			pc_count++;
		}
	}
	
	/* Second pass: collect NPC races */
	for (race = race_list; race != NULL; race = race->next) {
		if (!race->pc_race && npc_index < 100) {
			npc_races[npc_index++] = race;
			npc_count++;
		}
	}
	
	/* Display races side by side */
	max_count = (pc_count > npc_count) ? pc_count : npc_count;
	
	for (i = 0; i < max_count; i++) {
		/* Initialize strings */
		strcpy(pc_name, "");
		strcpy(npc_name, "");
		
		/* Get PC race name if available */
		if (i < pc_count) {
			strcpy(pc_name, pc_races[i]->name);
		}
		
		/* Get NPC race name if available */
		if (i < npc_count) {
			strcpy(npc_name, npc_races[i]->name);
		}
		
		/* Format the line with proper spacing */
		sprintf(line, "  %-20s          %s\n\r", pc_name, npc_name);
		send_to_char(line, ch);
	}
	
	if (pc_count == 0 && npc_count == 0) {
		send_to_char("  No races available.\n\r", ch);
	} else {
		printf_to_char(ch, "\nTotal: %d PC races, %d NPC races\n\r", pc_count, npc_count);
	}
}

void list_pc_races_only(CHAR_DATA *ch) {
	struct race_data *race;
	int pc_count = 0;
	struct race_data *pc_races[100];  /* Assuming max 100 races */
	int pc_index = 0;
	int i;
	char line[200];
		
	/* Collect PC races only */
	for (race = race_list; race != NULL; race = race->next) {
		if (race->pc_race && pc_index < 100) {
			pc_races[pc_index++] = race;
			pc_count++;
		}
	}
	
	/* Display PC races */
	for (i = 0; i < pc_count; i++) {
		sprintf(line, "  %s\n\r", pc_races[i]->name);
		send_to_char(line, ch);
	}
	
	if (pc_count == 0) {
		send_to_char("  No PC races available.\n\r", ch);
	} else {
		printf_to_char(ch, "\nTotal: %d PC races available\n\r", pc_count);
	}
}

void load_all_races(void) {
	char filename[MIL];
	char race_name[MIL];
	
	/* Load races from race_list.txt first */
	char list_filename[MIL];
	sprintf(list_filename, "%srace_list.txt", RACE_DIR);
	FILE *list_fp = fopen(list_filename, "r");
	if (list_fp != NULL) {
		char line[MIL];
		while (fgets(line, sizeof(line), list_fp) != NULL) {
			/* Remove newline */
			char *newline = strchr(line, '\n');
			if (newline) *newline = '\0';
			char *carriage = strchr(line, '\r');
			if (carriage) *carriage = '\0';
			
			/* Skip empty lines and comments */
			if (line[0] == '\0' || line[0] == '#') continue;
			
			load_race_from_file(line);
		}
		fclose(list_fp);
	}
	
	/* Load all race files from the race directory */
	/* Use a simple approach that works on both Windows and Unix */
	/* First, try to load all the race files we know exist */
	const char *known_races[] = {
		"human", "elf", "dwarf", "halfling", "gnome", "halfelf", "halforc",
		"gnoll", "goblin", "hobgoblin", "kobold", "orc", "troll", "giant",
		"dragon", "draconian", "minotaur", "centaur", "satyr", "pixie",
		"kenku", "modron", "titan", "wyvern", "bat", "bear", "bovine",
		"cat", "centipede", "deer", "dog", "doll", "fido", "fox",
		"lizard", "pig", "rabbit", "rodent", "snake", "song bird",
		"water fowl", "wolf", "avian", "school monster", "unique", "heucuva"
	};
	
	int num_known_races = sizeof(known_races) / sizeof(known_races[0]);
	
	for (int i = 0; i < num_known_races; i++) {
		/* Load the race if not already loaded */
		if (find_race_by_name(known_races[i]) == NULL) {
			load_race_from_file(known_races[i]);
		}
	}
}

struct race_data *get_race_by_index(int race_num) {
	struct race_data *race;
	int count = 0;
	
	/* Load all races if not already loaded */
	if (race_list == NULL) {
		load_all_races();
	}
	
	for (race = race_list; race != NULL; race = race->next, count++) {
		if (count == race_num) {
			return race;
		}
	}
	
	return NULL;
}

/* Helper function to get race name by index */
const char *get_race_name_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->name : NULL;
}

/* Helper function to check if race is PC race by index */
bool is_pc_race_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->pc_race : FALSE;
}

/* Helper function to get race flags by index */
long get_race_aff_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->aff : 0;
}

long get_race_shd_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->shd : 0;
}

long get_race_imm_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->imm : 0;
}

long get_race_res_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->res : 0;
}

long get_race_vuln_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->vuln : 0;
}

long get_race_form_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->form : 0;
}

long get_race_parts_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->parts : 0;
}

/* Helper function to get PC race data by index */
int get_pc_race_tier_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->tier : 0;
}

int get_pc_race_points_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->points : 0;
}

int get_pc_race_size_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->size : 0;
}

const char *get_pc_race_who_name_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->who_name : NULL;
}

int get_pc_race_stat_by_index(int race_num, int stat) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL && stat >= 0 && stat < MAX_STATS) ? race->stats[stat] : 0;
}

const char *get_pc_race_skill_by_index(int race_num, int skill) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL && skill >= 0 && skill < MAX_SKILL) ? race->skills[skill] : NULL;
}

/* Helper function to get race act and off flags by index */
long get_race_act_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->act : 0;
}

long get_race_off_by_index(int race_num) {
	struct race_data *race = get_race_by_index(race_num);
	return (race != NULL) ? race->off : 0;
}

/* Test function for file-based race system */
void test_file_based_races(void) {
	printf("Testing file-based race system...\n");
	
	/* Load all races */
	load_all_races();
	
	/* Test finding races by name */
	struct race_data *human = find_race_by_name("human");
	if (human != NULL) {
		printf("Found human race: %s (PC: %s)\n", human->name, human->pc_race ? "yes" : "no");
	} else {
		printf("ERROR: Could not find human race\n");
	}
	
	struct race_data *elf = find_race_by_name("elf");
	if (elf != NULL) {
		printf("Found elf race: %s (PC: %s)\n", elf->name, elf->pc_race ? "yes" : "no");
	} else {
		printf("ERROR: Could not find elf race\n");
	}
	
	/* Test race_lookup function */
	int human_num = race_lookup("human");
	int elf_num = race_lookup("elf");
	printf("race_lookup results: human=%d, elf=%d\n", human_num, elf_num);
	
	/* Test get_race_by_index */
	struct race_data *race0 = get_race_by_index(0);
	struct race_data *race1 = get_race_by_index(1);
	if (race0 != NULL) {
		printf("Race 0: %s\n", race0->name);
	}
	if (race1 != NULL) {
		printf("Race 1: %s\n", race1->name);
	}
	
	printf("File-based race system test completed.\n");
}
