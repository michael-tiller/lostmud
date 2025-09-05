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

// function prototypes
void do_race_info( CHAR_DATA *ch, char *argument);
void do_rcedit(CHAR_DATA *ch ,char * argument);
void load_race_file(const char *filename, int race_no);
void save_race_file(const char *filename, int race_no);
void load_race_files(void);
void save_race_to_file(int race_no);
void reload_race_list(void);

// macro definitions
#define RACE_TEMP "../data/race/t"
#define RACE_DIR "../data/race/"

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
	
	for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
		if (!str_cmp(race_name, race_table[race_no].name)) {
			break;
		}
	}
	
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
	
	/* Special handling for create command */
	if (!str_cmp(race_name, "create")) {
		if (!argument[0]) {
			send_to_char("Syntax is: rcedit create <race_name>.\n\r", ch);
			return;
		}
		
		/* Check if race already exists and find first empty slot */
		race_no = 0;
		for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
			if (!str_cmp(argument, race_table[race_no].name)) {
				printf_to_char(ch, "A race named '%s' already exists.\n\r", argument);
				return;
			}
		}
		/* race_no now points to the first empty slot or beyond the table */
		
		/* Check if we have room for a new race */
		if (race_no >= MAX_PC_RACE) {
			printf_to_char(ch, "Cannot create new race - maximum number of races (%d) reached.\n\r", MAX_PC_RACE);
			return;
		}
		
		/* Check if we're within the PC race limit */
		if (race_no >= MAX_PC_RACE) {
			printf_to_char(ch, "Cannot create new race - maximum number of races (%d) reached.\n\r", MAX_PC_RACE);
			return;
		}
		
		/* Validate race name */
		if (strlen(argument) < 2 || strlen(argument) > 20) {
			send_to_char("Race name must be between 2 and 20 characters.\n\r", ch);
			return;
		}
		
		/* Initialize new race with default values */
		race_table[race_no].name = str_dup(argument);
		race_table[race_no].pc_race = FALSE;
		race_table[race_no].act = 0;
		race_table[race_no].aff = 0;
		race_table[race_no].off = 0;
		race_table[race_no].imm = 0;
		race_table[race_no].res = 0;
		race_table[race_no].vuln = 0;
		race_table[race_no].shd = 0;
		race_table[race_no].form = 0;
		race_table[race_no].parts = 0;
		
		/* Initialize PC race data with defaults */
		if (race_no < MAX_PC_RACE) {
			pc_race_table[race_no].name = str_dup(argument);
			strcpy(pc_race_table[race_no].who_name, "     ");
			pc_race_table[race_no].points = 0;
			pc_race_table[race_no].size = 2; /* SIZE_MEDIUM */
			pc_race_table[race_no].tier = 1;
			
			/* Set default stats */
			for (i = 0; i < MAX_STATS; i++) {
				pc_race_table[race_no].stats[i] = 13;
				pc_race_table[race_no].max_stats[i] = 18;
			}
			
			/* Set default class multipliers */
			for (i = 0; i < MAX_CLASS; i++) {
				pc_race_table[race_no].class_mult[i] = 100;
			}
			
			/* Initialize skills */
			for (i = 0; i < 5; i++) {
				pc_race_table[race_no].skills[i] = NULL;
			}
		}
		
		/* Mark end of table if this is a new race at the end */
		if (race_table[race_no + 1].name == NULL || race_no == 0) {
			race_table[race_no + 1].name = NULL;
		}
		
		printf_to_char(ch, "OK, created new race '%s'.\n\r", argument);
		
		/* Save the race file */
		save_race_to_file(race_no);
		
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
		
		printf("DEBUG: Starting delete for race '%s'\n", argument);
		
		/* Find the race to delete */
		for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
			if (!str_cmp(argument, race_table[race_no].name)) {
				break;
			}
		}
		
		printf("DEBUG: Found race at slot %d\n", race_no);
		
		if (race_table[race_no].name == NULL) {
			printf_to_char(ch, "No race named '%s' exists.\n\r", argument);
			return;
		}
		
		/* Safety check - ensure race_no is within bounds */
		if (race_no < 0 || race_no >= MAX_PC_RACE) {
			printf_to_char(ch, "Invalid race number %d.\n\r", race_no);
			return;
		}
		
		printf("DEBUG: Bounds check passed\n");
		
		/* Check if any players are using this race */
		CHAR_DATA *wch;
		for (wch = char_list; wch != NULL; wch = wch->next) {
			if (wch->race == race_no) {
				printf_to_char(ch, "Cannot delete race '%s' - players are currently using it.\n\r", race_table[race_no].name);
				return;
			}
		}
		
		printf("DEBUG: Player check passed\n");
		
		/* Get filename and race name before freeing data */
		char filename[MIL];
		char deleted_race_name[MIL];
		sprintf(filename, "%s%s.race", RACE_DIR, race_table[race_no].name);
		strncpy(deleted_race_name, race_table[race_no].name, MIL - 1);
		deleted_race_name[MIL - 1] = '\0';
		
		printf("DEBUG: About to free race data\n");
		
		/* Store pointers before freeing */
		char *race_name_ptr = race_table[race_no].name;
		bool is_pc_race = (race_no < MAX_PC_RACE && race_table[race_no].pc_race);
		char *pc_race_name_ptr = is_pc_race ? pc_race_table[race_no].name : NULL;
		bool same_pointer = (pc_race_name_ptr == race_name_ptr);
		
		printf("DEBUG: race_name_ptr = %p, pc_race_name_ptr = %p, same = %s, is_pc_race = %s\n", 
			race_name_ptr, pc_race_name_ptr, same_pointer ? "yes" : "no", is_pc_race ? "yes" : "no");
		
		/* Free race data */
		free_string(race_name_ptr);
		
		printf("DEBUG: Freed race data\n");
		
		/* Free PC race data if applicable - only if it's actually a PC race */
		if (is_pc_race) {
			printf("DEBUG: Inside PC race data free section\n");
			/* Only free pc_race_table name if it's different from race_table name */
			if (!same_pointer && pc_race_name_ptr != NULL) {
				printf("DEBUG: Freeing pc_race_table name\n");
				free_string(pc_race_name_ptr);
			} else {
				printf("DEBUG: Skipping pc_race_table name (same as race_table or NULL)\n");
			}
			printf("DEBUG: About to free skills\n");
			int i;
			for (i = 0; i < 5; i++) {
				if (pc_race_table[race_no].skills[i] != NULL) {
					printf("DEBUG: Freeing skill %d\n", i);
					free_string(pc_race_table[race_no].skills[i]);
				}
			}
			printf("DEBUG: Finished freeing PC race data\n");
		} else {
			printf("DEBUG: Skipping PC race data (not a PC race)\n");
		}
		
		printf("DEBUG: About to shift races down\n");
		
		/* Shift remaining races down */
		int i;
		for (i = race_no; i < MAX_PC_RACE - 1 && race_table[i + 1].name != NULL; i++) {
			race_table[i] = race_table[i + 1];
			if (i < MAX_PC_RACE) {
				pc_race_table[i] = pc_race_table[i + 1];
			}
		}
		race_table[i].name = NULL;
		
		printf("DEBUG: Finished shifting races\n");
		
		printf_to_char(ch, "OK, race deleted.\n\r");
		
		/* Delete the race file */
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
					if (str_cmp(line, deleted_race_name)) {
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
		
		return;
	}
	
	/* Special handling for list command */
	if (!str_cmp(race_name, "list")) {
		send_to_char("Available races:\n\r", ch);
		for (int i = 0; race_table[i].name != NULL; i++) {
			printf_to_char(ch, "  %d: %s (%s)\n\r", i, race_table[i].name, 
				race_table[i].pc_race ? "PC" : "NPC");
		}
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
	
	/* Find the race */
	for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
		if (!str_cmp(race_name, race_table[race_no].name)) {
			break;
		}
	}
	
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
				printf_to_char(ch, "Only races 0-%d can be PC races. Use 'rcedit create <name>' to create a new race.\n\r", MAX_PC_RACE - 1);
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
	else if (!str_cmp(field_name, "rename")) {
		/* Format: rcedit race rename <new_name> */
		if (strlen(argument) < 2 || strlen(argument) > 20) {
			send_to_char("Race name must be between 2 and 20 characters.\n\r", ch);
			return;
		}
		
		/* Check if new name already exists */
		int check_race;
		for (check_race = 0; race_table[check_race].name != NULL; check_race++) {
			if (check_race != race_no && !str_cmp(argument, race_table[check_race].name)) {
				printf_to_char(ch, "A race named '%s' already exists.\n\r", argument);
				return;
			}
		}
		
		/* Free old name and set new name */
		free_string(race_table[race_no].name);
		race_table[race_no].name = str_dup(argument);
		
		/* Update PC race table name if it's a PC race */
		if (race_table[race_no].pc_race && race_no < MAX_PC_RACE) {
			free_string(pc_race_table[race_no].name);
			pc_race_table[race_no].name = str_dup(argument);
		}
		
		printf_to_char(ch, "OK, race renamed to '%s'.\n\r", argument);
		
		/* Save the race file */
		save_race_to_file(race_no);
	}
	else {
		send_to_char("Invalid field. Use: pcrace, points, size, tier, stat, maxstat, mult, skill, act, aff, off, imm, res, vuln, shd, form, parts, remove_act, remove_aff, remove_off, remove_imm, remove_res, remove_vuln, remove_shd, remove_form, remove_parts, rename, delete\n\r", ch);
		return;
	}
	
	/* Save the race file after any modification */
	save_race_to_file(race_no);
	
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
	int fields_loaded = 0;
	
	if ((fp = fopen(filename, "r")) == NULL) {
		bug("load_race_file: cannot open file for reading", 0);
		return;
	}
	
	printf("  Parsing race file: %s\n", filename);
	fflush(stdout);  /* Ensure output is flushed immediately */
	
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
		
		/* Debug: Show what word we're processing */
		if (strcmp(word, "End") != 0) {
			printf("    Processing word: '%s'\n", word);
			fflush(stdout);
		}
		
		/* Check for race table corruption during parsing */
		if (race_table[1].name == NULL) {
			printf("    ERROR: Race table corrupted during parsing! race_table[1].name is NULL\n");
			fflush(stdout);
			/* Try to restore the race table */
			race_table[1].name = "human";
			race_table[1].pc_race = TRUE;
		}
		
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
					fields_loaded++;
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Aff")) {
					race_table[race_no].aff = fread_number(fp);
					fields_loaded++;
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
					char *new_name = fread_string(fp);
					if (new_name == NULL) {
						bug("load_race_file: fread_string returned NULL for race %d", race_no);
						/* Keep the original name if fread_string fails */
					} else {
						race_table[race_no].name = new_name;
						printf("    Race name: %s\n", race_table[race_no].name);
						fields_loaded++;
					}
					fMatch = TRUE;
				}
				break;
				
			case 'O':
				if (!str_cmp(word, "Off")) {
					race_table[race_no].off = fread_number(fp);
					fields_loaded++;
					fMatch = TRUE;
				}
				break;
				
			case 'P':
				if (!str_cmp(word, "PC_RACE")) {
					char *pc_race_str = fread_word(fp);
					race_table[race_no].pc_race = (!str_cmp(pc_race_str, "yes"));
					fields_loaded++;
					fMatch = TRUE;
				}
				else if (!str_cmp(word, "Parts")) {
					race_table[race_no].parts = fread_number(fp);
					fields_loaded++;
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
	
	printf("    Loaded %d fields from %s\n", fields_loaded, filename);
	fclose(fp);
	
} // load_race_file()

/**/
void load_race_files(void) {
	char filename[MIL];
	int race_no;
	int loaded_count = 0;
	int skipped_count = 0;
	int list_loaded_count = 0;
	
	/* The pc_race_table is already initialized with hardcoded race data in const.c */
	/* We don't need to overwrite it with "null race" - that destroys the existing data */
	
	log_string("Race file loading starting...");
	log_string("Race directory: " RACE_DIR);
	
	/* First, load races from the static table (existing races) */
	log_string("Loading races from static table...");
	for (race_no = 0; race_table[race_no].name != NULL; race_no++) {
		/* Safety check to prevent infinite loops or crashes */
		if (race_no >= MAX_PC_RACE) {
			printf("WARNING: Race table appears to be corrupted, stopping at race %d\n", race_no);
			break;
		}
		
		/* Safety check for NULL race name */
		if (race_table[race_no].name == NULL) {
			printf("WARNING: Found NULL race name at index %d, stopping\n", race_no);
			break;
		}
		
		/* Additional safety check for race table integrity */
		if (race_no > 0 && race_table[race_no-1].name == NULL) {
			printf("WARNING: Previous race %d has NULL name, possible table corruption\n", race_no-1);
			fflush(stdout);
		}
		
		printf("Processing race %d: %s\n", race_no, race_table[race_no].name);
		fflush(stdout);  /* Ensure output is flushed immediately */
		sprintf(filename, "%s%s.race", RACE_DIR, race_table[race_no].name);
		
		/* Try to load the race file - if it doesn't exist, skip it */
		FILE *fp = fopen(filename, "r");
		if (fp != NULL) {
			fclose(fp);
			printf("Loading race file: %s\n", filename);
			fflush(stdout);  /* Ensure output is flushed immediately */
			
			/* Add error handling around race file loading */
			printf("  About to call load_race_file for race %d\n", race_no);
			fflush(stdout);
			
			/* Check if this is the problematic race file */
			if (race_no == 29) {
				printf("  WARNING: This is race 29 (fox) - potential problem race\n");
				fflush(stdout);
			}
			
			load_race_file(filename, race_no);
			
			printf("  Completed load_race_file for race %d\n", race_no);
			fflush(stdout);
			loaded_count++;
			
			/* Additional safety check after each race load */
			printf("  Race %d loaded successfully, loaded_count=%d\n", race_no, loaded_count);
			fflush(stdout);
			
			/* Check for race table corruption after each load */
			if (race_table[1].name == NULL) {
				printf("  ERROR: Race table corrupted after loading race %d! race_table[1].name is NULL\n", race_no);
				fflush(stdout);
				/* Try to restore the race table */
				race_table[1].name = "human";
				race_table[1].pc_race = TRUE;
			}
		} else {
			printf("Race file not found: %s (using hardcoded values)\n", filename);
			skipped_count++;
		}
		/* If file doesn't exist, race keeps its hardcoded values */
	}
	
	printf("DEBUG: Exited race loading loop, race_no=%d\n", race_no);
	fflush(stdout);
	
	printf("Finished processing static races. Processed %d races.\n", race_no);
	fflush(stdout);
	
	/* Verify race table termination */
	printf("DEBUG: Checking race table termination at index %d\n", race_no);
	fflush(stdout);
	
	if (race_table[race_no].name != NULL) {
		printf("ERROR: Race table not properly terminated! Race %d has name: %s\n", 
			race_no, race_table[race_no].name);
		/* Force termination to prevent crashes */
		race_table[race_no].name = NULL;
	} else {
		printf("DEBUG: Race table properly terminated at index %d\n", race_no);
		fflush(stdout);
	}
	
	/* Debug: Check if race table is accessible */
	printf("DEBUG: Race table accessibility check:\n");
	fflush(stdout);
	
	printf("  race_table[0].name = %s\n", race_table[0].name ? race_table[0].name : "NULL");
	fflush(stdout);
	
	printf("  race_table[1].name = %s\n", race_table[1].name ? race_table[1].name : "NULL");
	fflush(stdout);
	
	/* Add safety check before accessing race_table[2] */
	printf("DEBUG: About to access race_table[2]\n");
	fflush(stdout);
	
	/* Check if we can safely access race_table[2] */
	if (race_table[2].name != NULL) {
		printf("  race_table[2].name = %s\n", race_table[2].name);
		fflush(stdout);
	} else {
		printf("  race_table[2].name = NULL\n");
		fflush(stdout);
	}
	
	printf("DEBUG: Successfully accessed race_table[2]\n");
	fflush(stdout);
	
	printf("DEBUG: Completed race table accessibility check\n");
	fflush(stdout);
	
	/* Add a simple test to see if we can continue */
	printf("DEBUG: Testing basic operations\n");
	fflush(stdout);
	
	int test_int = 42;
	printf("DEBUG: Integer test: %d\n", test_int);
	fflush(stdout);
	
	/* Check race table size and memory layout */
	printf("DEBUG: Race table size check\n");
	fflush(stdout);
	printf("  sizeof(race_table[0]) = %ld\n", sizeof(race_table[0]));
	fflush(stdout);
	printf("  sizeof(race_table) = %ld\n", sizeof(race_table));
	fflush(stdout);
	printf("  MAX_PC_RACE = %d\n", MAX_PC_RACE);
	fflush(stdout);
	
	printf("DEBUG: About to start race table structure check\n");
	fflush(stdout);
	
	/* Debug: Check race table structure integrity */
	printf("DEBUG: Checking race table structure integrity\n");
	fflush(stdout);
	
	/* Check if we can safely access the race table */
	printf("DEBUG: Starting race table loop\n");
	fflush(stdout);
	
	for (int i = 0; i < 3; i++) {
		printf("DEBUG: Loop iteration %d\n", i);
		fflush(stdout);
		
		if (race_table[i].name != NULL) {
			printf("  Race %d: name='%s', pc_race=%d\n", i, race_table[i].name, race_table[i].pc_race);
			fflush(stdout);
		} else {
			printf("  Race %d: name is NULL\n", i);
			fflush(stdout);
		}
		
		printf("DEBUG: Completed loop iteration %d\n", i);
		fflush(stdout);
	}
	
	printf("DEBUG: Completed race table loop\n");
	fflush(stdout);
	
	printf("DEBUG: Completed race table structure check\n");
	fflush(stdout);
	
	/* Test a simple operation to see if we can continue */
	printf("DEBUG: Testing simple string operation\n");
	fflush(stdout);
	
	char test_string[100];
	strcpy(test_string, "test");
	printf("DEBUG: String operation successful: %s\n", test_string);
	fflush(stdout);
	
	/* Load additional races from a race list file */
	/* This is a simpler cross-platform approach */
	printf("DEBUG: About to load additional races from race list file\n");
	fflush(stdout);
	
	printf("DEBUG: MIL constant value: %d\n", MIL);
	fflush(stdout);
	
	printf("DEBUG: RACE_DIR constant: '%s'\n", RACE_DIR);
	fflush(stdout);
	
	char list_filename[MIL];
	
	printf("DEBUG: About to call sprintf for list_filename\n");
	fflush(stdout);
	
	sprintf(list_filename, "%srace_list.txt", RACE_DIR);
	
	printf("DEBUG: Race list filename: %s\n", list_filename);
	fflush(stdout);
	
	printf("DEBUG: Completed sprintf for list_filename\n");
	fflush(stdout);
	printf("Checking for race list file: %s\n", list_filename);
	
	FILE *list_fp = fopen(list_filename, "r");
	if (list_fp != NULL) {
		printf("Loading additional races from race list file...\n");
		char line[MIL];
		while (fgets(line, sizeof(line), list_fp) != NULL) {
			/* Remove newline */
			char *newline = strchr(line, '\n');
			if (newline) *newline = '\0';
			char *carriage = strchr(line, '\r');
			if (carriage) *carriage = '\0';
			
			/* Skip empty lines and comments */
			if (line[0] == '\0' || line[0] == '#') continue;
			
			/* Check if this race is already loaded */
			int found = 0;
			for (int i = 0; race_table[i].name != NULL; i++) {
				if (!str_cmp(line, race_table[i].name)) {
					found = 1;
					break;
				}
			}
			
			/* If not found, add it to the race table */
			if (!found) {
				/* Find first empty slot within existing table bounds */
				int empty_slot = 0;
				for (empty_slot = 0; race_table[empty_slot].name != NULL; empty_slot++) {
					/* Keep going until we find the end */
				}
				
				/* Check if we can extend the table */
				if (empty_slot < MAX_PC_RACE) {
					printf("Adding new race '%s' to slot %d\n", line, empty_slot);
					/* Create the race entry */
					race_table[empty_slot].name = str_dup(line);
					race_table[empty_slot].pc_race = FALSE;
					race_table[empty_slot].act = 0;
					race_table[empty_slot].aff = 0;
					race_table[empty_slot].off = 0;
					race_table[empty_slot].imm = 0;
					race_table[empty_slot].res = 0;
					race_table[empty_slot].vuln = 0;
					race_table[empty_slot].shd = 0;
					race_table[empty_slot].form = 0;
					race_table[empty_slot].parts = 0;
					
					/* Load the race file */
					sprintf(filename, "%s%s.race", RACE_DIR, line);
					printf("Loading race file: %s\n", filename);
					load_race_file(filename, empty_slot);
					list_loaded_count++;
					
					/* Mark end of table after loading */
					race_table[empty_slot + 1].name = NULL;
				} else {
					printf("Cannot add race '%s': table full (MAX_PC_RACE=%d)\n", line, MAX_PC_RACE);
				}
			} else {
				printf("Race '%s' already loaded, skipping\n", line);
			}
		}
		fclose(list_fp);
		printf("Race list file processing complete\n");
	} else {
		printf("Race list file not found: %s\n", list_filename);
	}
	
	/* Count total races loaded */
	int total_races = 0;
	for (int i = 0; race_table[i].name != NULL; i++) {
		total_races++;
	}
	
	printf("Race loading complete: %d static races loaded, %d skipped, %d from list, %d total\n", 
		loaded_count, skipped_count, list_loaded_count, total_races);
	
	/* Debug: Test race_lookup function */
	printf("Testing race_lookup('human'): %d\n", race_lookup("human"));
	printf("Testing race_lookup('elf'): %d\n", race_lookup("elf"));
	printf("Testing race_lookup('dwarf'): %d\n", race_lookup("dwarf"));
	
	/* Debug: Check race table state */
	printf("DEBUG: Race table state:\n");
	for (int i = 0; i < 5 && race_table[i].name != NULL; i++) {
		printf("  Race %d: name='%s', pc_race=%d\n", i, race_table[i].name, race_table[i].pc_race);
	}
	
	/* Debug: Test race_lookup with direct string comparison */
	printf("DEBUG: Direct race table search test:\n");
	for (int i = 0; race_table[i].name != NULL; i++) {
		if (!str_cmp("human", race_table[i].name)) {
			printf("  Found 'human' at index %d\n", i);
			break;
		}
	}
	
	/* Final flush to ensure all output is written */
	fflush(stdout);
	fflush(stderr);
	
} // load_race_files()

/**/
void reload_race_list(void) {
	char filename[MIL];
	char list_filename[MIL];
	
	/* Load additional races from a race list file */
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
			
			/* Check if this race is already loaded */
			int found = 0;
			for (int i = 0; race_table[i].name != NULL; i++) {
				if (!str_cmp(line, race_table[i].name)) {
					found = 1;
					break;
				}
			}
			
			/* If not found, add it to the race table */
			if (!found) {
				/* Find first empty slot within existing table bounds */
				int empty_slot = 0;
				for (empty_slot = 0; race_table[empty_slot].name != NULL; empty_slot++) {
					/* Keep going until we find the end */
				}
				
				/* Check if we can extend the table */
				if (empty_slot < MAX_PC_RACE) {
					/* Create the race entry */
					race_table[empty_slot].name = str_dup(line);
					race_table[empty_slot].pc_race = FALSE;
					race_table[empty_slot].act = 0;
					race_table[empty_slot].aff = 0;
					race_table[empty_slot].off = 0;
					race_table[empty_slot].imm = 0;
					race_table[empty_slot].res = 0;
					race_table[empty_slot].vuln = 0;
					race_table[empty_slot].shd = 0;
					race_table[empty_slot].form = 0;
					race_table[empty_slot].parts = 0;
					
					/* Load the race file */
					sprintf(filename, "%s%s.race", RACE_DIR, line);
					load_race_file(filename, empty_slot);
					
					/* Mark end of table after loading */
					race_table[empty_slot + 1].name = NULL;
				}
			}
		}
		fclose(list_fp);
	}
} // reload_race_list()

/**/
void save_race_to_file(int race_no) {
	char filename[MIL];
	sprintf(filename, "%s%s.race", RACE_DIR, race_table[race_no].name);
	save_race_file(filename, race_no);
} // save_race_to_file()
