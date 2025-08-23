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
void do_load_groups( CHAR_DATA *ch, char *argument );
void load_groups( void );
void load_group(FILE *fp, int id);
void save_group(FILE *fp, int id);
void do_save_groups( CHAR_DATA *ch, char *argument );
void save_group_list();

void load_classes( void );
void load_class(FILE *fp, int i );
void load_class_skills(FILE *fp, int id );
void save_class(FILE *fp, int id);
void do_save_class( CHAR_DATA *ch, char *argument );
void load_group_list();

void do_skill_info( CHAR_DATA *ch, char *argument);
void do_group_info( CHAR_DATA *ch, char *argument);
void do_skedit(CHAR_DATA *ch ,char * argument);

// macro definitions
#define CLASS_TEMP "../data/class/t"
#define CLASS_DIR "../data/class/"
#define GROUP_TEMP "../data/group/t"
#define GROUP_DIR "../data/group/"
#define GROUP_LIST "../data/group/group.lst"

int max_groups;



struct group_type *group_table;

/**/
void do_load_groups (CHAR_DATA *ch, char *argument) {
	load_groups();
}

void load_groups( void )
{
    FILE *fp;
    char buf[MSL];
    int i;

	load_group_list();
    /* Initialize Groups */
    {
	int gn, j, k;
	for ( gn = 0; gn < max_groups; gn++ ) {
		// no name? stop.
	    if ( group_table[gn].name == NULL )
			break;
	    
		// loop through classes
	    for( j = 0;j < MAX_CLASS; j++ ) {
			// set class rating
			group_table[gn].rating[j] = -1;
	    }

		// initialize with no spells
		for (k = 0; k < MAX_IN_GROUP; k++) {
			group_table[gn].spells[k] = NULL;
		}

	}
    }

	for (i = 0; i < max_groups; i++) {
		sprintf( buf, "%s%s.group", GROUP_DIR, group_table[i].name );
		logf( "Loading group: %s",  group_table[i].name );

		if ( (fp = fopen( buf, "r" )) == NULL )	{
			log_string( "Error opening group file." );
			exit(1);
		}
		load_group(fp, i);

	}
} // load_groups
/**/

/**/
void load_group(FILE *fp, int id ) {
	int i;		
	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    
	    word = fread_word( fp );

	    if ( !str_cmp( word, "GROUP" ) )
	    {
		char *field;

	    for ( ;; )
		{
			// get next word
		    field = fread_word( fp );

			// is name
		    if ( !str_cmp( field, "Name" ) ) {
				group_table[id].name = fread_string( fp );
			} if ( !str_cmp( field, "Rate" ) ) {
				for (i = 0; i < MAX_CLASS; i++) {
					group_table[id].rating[i] = fread_number( fp );
				}
			}
			
			// if is end, stop
		    else if ( !str_cmp( field, "End~" ) )
			break;
		}
		} else if ( !str_cmp( word, "SPELLS" ) )  {
		int j;
		char *spell;

		for ( ;; )	{
			// loop through lines
		    for ( j = 0; j < MAX_IN_GROUP; j++ )  {		    
				// get next spell
				spell = fread_string( fp );

				// if is end, stop
				if ( !str_cmp( spell, "End" ) )
					break;
				if (NULL == spell)
					exit(1);
				
				group_table[id].spells[j] = spell;
		    }
			break;
		}
		} else if ( !str_cmp( word, "END" ) )  {
		fclose( fp );
		break;
	    }
	}
} // load_group
/**/
/**
		 = one_argument (argument, command);
		
		if command == "add"
		// loop through group
		for (k = 0; k < MAX_IN_GROUP; k++) {
			// count number of spells in group
			if (NULL == group_table[id].spells[k])
				break;
		}

		group_table[id].spells[k] = argument;

 **/
/**/
void save_group(FILE *fp, int id) {
	int i;
	int sn;

	// declare a group
	fprintf( fp, "#GROUP\n" );

	// get group name
	fprintf( fp, "Name %s~\n", group_table[id].name );
	
	// get group rates
	fprintf( fp, "Rate" );
	// loop through classes
	for (i = 0; i < MAX_CLASS; i++) {
		// each class gets a rating
		fprintf( fp, " %d", group_table[id].rating[i] );
	}
	// end of line
	fprintf( fp, "~\n", class_table[i].name, group_table[id].rating[i] );
	
	// end of group stats
	fprintf( fp, "End~\n" );
	
	// begin spells section
	fprintf( fp, "#SPELLS\n" );
	for( sn = 0; sn < MAX_IN_GROUP; sn++ )	{
		if ( group_table[id].spells[sn] == NULL)
			break;

		fprintf( fp, "%s~\n", group_table[id].spells[sn]);
	}
} // save_group
/**/

/**/
void do_save_groups( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char strsave[MAX_INPUT_LENGTH];
    int i;
	
	logf("Saving all groups.");
	
	save_group_list();

    for( i = 0; i < max_groups; i++ )
    {
		// close stream
		fclose( fpReserve );

		// get filename
		sprintf( buf, "%s.group", group_table[i].name );
		// get file location
		sprintf( strsave, "%s%s", GROUP_DIR, buf );

	    
		if ( ( fp = fopen( GROUP_TEMP, "w" ) ) == NULL )
		{
			bugf( "do_save_groups: fopen" );
			perror( strsave );
		} else {
			logf("Saving group: %s", group_table[i].name);

			save_group(fp, i);

			fprintf( fp, "End~\n" );
			fprintf( fp, "#END\n" );
			fclose( fp );

			// clean old file
			remove(strsave);

			// save new file
			rename(GROUP_TEMP,strsave);
			
			// clean tempfile
			remove(GROUP_TEMP);

			// reopen stream
			fpReserve = fopen( NULL_FILE, "r" );
		}

		
    }

	if (ch != NULL)
	    send_to_char( "All groups saved.\n\r", ch );
} // do_save_groups
/**/


void save_group_list() {
	FILE *fp;
	int i;

	fp = fopen (GROUP_TEMP, "w");
	
	if (!fp)
	{
		bug ("Could not open " GROUP_TEMP " for writing.",0);
		return;
	}
	
	fprintf (fp, "%d\n", max_groups);

	for ( i = 0 ; i < max_groups ; i++) {
		fprintf (fp, "'%s'\n", group_table[i].name);
	}
	
	fprintf( fp, "End\n" );
	fclose( fp );

	// clean old file
	remove(GROUP_LIST);

	// save new file
	rename(GROUP_TEMP,GROUP_LIST);
			
	// clean tempfile
	remove(GROUP_TEMP);
	

	// reopen stream
	fpReserve = fopen( NULL_FILE, "r" );

}

void load_group_list() {
	FILE *fp;
	int i;
		
	if ( (fp = fopen( GROUP_LIST, "r" )) == NULL )
	{
	    log_string( "Error opening group list." );
	    exit(1);
	}
	
	max_groups = fread_number( fp );

	// initialize table
	group_table = malloc (sizeof(struct group_type) * (max_groups+1));

	// continue to loop through file
	for (i = 0;i < max_groups;i++) {
		char* group;

		// get next line
		group = fread_word( fp );

		// line is end?
		if ( !str_cmp( group, "End" ) )
			break;

		// save group name
		//group_table[i].name = group;
		group_table[i].name = str_dup(group);
	}
	
}

/**/
void load_class(FILE *fp, int class) {
	char* field;
	int i;

	for ( ;; ) {
		field = fread_word( fp );
		if ( !str_cmp( field, "Name" ) )
			class_table[class].name = fread_string( fp );
		else if ( !str_cmp( field, "WhoN" ) ) {
			for( i = 0; i < 3; i++ )
			    class_table[class].who_name[i] = fread_letter( fp );
		}
		else if( !str_cmp( field, "Prime" ) )
			class_table[class].attr_prime = fread_number( fp );
		else if ( !str_cmp( field, "Weapon" ) )
			class_table[class].weapon = fread_number( fp );
		else if ( !str_cmp( field, "Guild1" ) )
			class_table[class].guild[0] = fread_number( fp );
		else if ( !str_cmp( field, "Guild2" ) )
			class_table[class].guild[1] = fread_number( fp );
		else if ( !str_cmp( field, "Guild3" ) )
			class_table[class].guild[2] = fread_number( fp );
		else if ( !str_cmp( field, "Skill" ) )
			class_table[class].skill_adept = fread_number( fp );
		else if ( !str_cmp( field, "Thac00" ) )
			class_table[class].thac0_00 = fread_number( fp );
		else if ( !str_cmp( field, "Thac32" ) )
			class_table[class].thac0_32 = fread_number( fp );
		else if ( !str_cmp( field, "HPMin" ) )
			class_table[class].hp_min = fread_number( fp );
		else if ( !str_cmp( field, "HPMax" ) )
			class_table[class].hp_max = fread_number( fp );
		else if ( !str_cmp( field, "FMana" ) )
			class_table[class].fMana = fread_number( fp );
		else if ( !str_cmp( field, "Base" ) )
			class_table[class].base_group = fread_string( fp );
		else if ( !str_cmp( field, "Default" ) )
			class_table[class].default_group = fread_string( fp );
		else if ( !str_cmp( field, "Tier" ) )
			class_table[class].tier = fread_number( fp );
		else if ( !str_cmp( field, "Ctier" ) )
			class_table[class].tier_next = fread_number( fp );
		else if ( !str_cmp( field, "End" ) )
			break;
	}
} // load_class
/**/

/**/
// save a class, given filestream and class id
void save_class(FILE *fp, int id) {
	int sn;

	// log
	logf("Saving class: %s ", class_table[id].name);

	// stats
	fprintf( fp, "#CLASS\n" );
	fprintf( fp, "Name %s~\n", class_table[id].name );
	fprintf( fp, "WhoN %s\n", class_table[id].who_name );
	fprintf( fp, "Prime %d\n", class_table[id].attr_prime );
	fprintf( fp, "Weapon %d\n", class_table[id].weapon );
	fprintf( fp, "Guild1 %d\n", class_table[id].guild[0] );
	fprintf( fp, "Guild2 %d\n", class_table[id].guild[1] );
	fprintf( fp, "Guild3 %d\n", class_table[id].guild[2] );
	fprintf( fp, "Skill %d\n", class_table[id].skill_adept );
	fprintf( fp, "Thac00 %d\n", class_table[id].thac0_00 );
	fprintf( fp, "Thac32 %d\n", class_table[id].thac0_32 );
	fprintf( fp, "HPMin %d\n", class_table[id].hp_min );
	fprintf( fp, "HPMax %d\n", class_table[id].hp_max );
	fprintf( fp, "FMana %d\n", class_table[id].fMana );
	fprintf( fp, "Base %s~\n", class_table[id].base_group );
	fprintf( fp, "Default %s~\n", class_table[id].default_group );
	fprintf( fp, "Tier %d~\n", class_table[id].tier );
	fprintf( fp, "Ctier %d~\n", class_table[id].tier_next );
	fprintf( fp, "End\n" );

	// skills
	fprintf( fp, "#SKILLS\n" );		
	for( sn = 0; sn < MAX_SKILL; sn++ )	{
		if ( skill_table[sn].name == NULL )
			break;

		if ( skill_table[sn].skill_level[id] < LEVEL_IMMORTAL ) {
			fprintf( fp, "%s~\n%d %d\n", skill_table[sn].name, skill_table[sn].skill_level[id],
					 skill_table[sn].rating[id] );
		}
	}	
	fprintf( fp, "End~\n" );

	// eof
	fprintf( fp, "#END\n" );
} // save_class
/**/

/**/
void load_class_skills(FILE *fp, int i)	    {
	int sn;
	char *spell;

	for ( ;; ) {
		spell = fread_string( fp );

		if ( !str_cmp( spell, "End" ) )
			break;

		for ( sn = 0; sn < MAX_SKILL; sn++ ) {
			if ( skill_table[sn].name == NULL )
			    break;

			if ( !str_cmp( spell, skill_table[sn].name )) {
				skill_table[sn].skill_level[i] = fread_number( fp );
				skill_table[sn].rating[i] = fread_number( fp );
				break;
			}
		}
	}
} // load_class_skills
/**/

/**/
void load_classes( void )
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    int class;

    /* Initialize Skill levels */
    {
	int sn, i;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    
	    for( i = 0; i < MAX_CLASS; i++ )
	    {
		skill_table[sn].skill_level[i] = LEVEL_IMMORTAL;
		skill_table[sn].rating[i] = 0;
	    }
	}
    }

	for( class = 0; class < MAX_CLASS; class++ ) {
	    sprintf( buf, "%s%s.class", CLASS_DIR, class_table[class].name );
	    logf( "Loading class: %s",  class_table[class].name );

	if ( (fp = fopen( buf, "r" )) == NULL )
	{
	    log_string( "Error opening class file." );
	    exit(1);
	}

	for ( ;; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    if ( letter != '#' )
	    {
		bug( "Load_classes: # not found.", 0 );
		break;
	    }
	    
	    word = fread_word( fp );

	    if ( !str_cmp( word, "CLASS" ) ) {
			load_class(fp, class);
	    }
	    else if ( !str_cmp( word, "SKILLS" ) ) {
			load_class_skills(fp, class);
		}
	    else if ( !str_cmp( word, "END" ) )  {
			fclose( fp );
			break;
	    }
	}
	}
}
/**/

/**/
//saves all classes
void do_save_class( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    char buf[MAX_STRING_LENGTH];
    char strsave[MAX_INPUT_LENGTH];
    int i;
	
	logf("Saving all classes.");

    for( i = 0; i < MAX_CLASS; i++ )
    {
		// close stream
		fclose( fpReserve );

		// get filename
		sprintf( buf, "%s.class", class_table[i].name );
		// get file location
		sprintf( strsave, "%s%s", CLASS_DIR, buf );

	    
		if ( ( fp = fopen( CLASS_TEMP, "w" ) ) == NULL )
		{
			bugf( "do_save_class: fopen" );
			perror( strsave );
		} else {
			save_class(fp, i);

			fclose( fp );

			// clean old file
			remove(strsave);

			// save new file
			rename(CLASS_TEMP,strsave);
			
			// clean tempfile
			remove(CLASS_TEMP);

			// reopen stream
			fpReserve = fopen( NULL_FILE, "r" );
		}

		
    }

    send_to_char( "All classes saved.\n\r", ch );
} // do_save_classes
/**/

void do_group_info( CHAR_DATA *ch, char *argument )
{
	int gn;
	int cn;
	int sn;
//	char group[MIL];

	if (!argument[0])
	{
		ptc (ch, "Syntax is: groupinfo <name>.\n\r");
		ptc (ch, "           groupinfo list.\n\r");
		return;
	}
		
	if (!str_cmp(argument,"list"))
	{
		do_function(ch, do_groups, "all");
		return;

	} else {

		//argument = one_argument (argument, group);
	
		if ( (gn = group_lookup (argument)) == -1)
		{
			ptc (ch, "No group named '%s' found.\n\r", argument);
			return;
		}

		/*
		for (gn = 0; gn < max_groups; gn++)
			if (!str_cmp(group, group_table[gn].name))
				break;
		*/

		ptc(ch, "Group Info for '%s'\n\r", group_table[gn].name);	
	
		ptc(ch, "Ratings\n\r"); 
		for (cn = 0; cn < MAX_CLASS; cn++)
			ptc(ch, "  %s %d\n\r", class_table[cn].name, group_table[gn].rating[cn]);

		ptc(ch, "Spells\n\r"); 
		for (sn = 0; sn < MAX_IN_GROUP ; sn++) {
			if (NULL == group_table[gn].spells[sn])
				break;

			ptc(ch, "  %s\n\r", group_table[gn].spells[sn]);
		}
	}

} // do_group_info()
/**/


void do_skill_info( CHAR_DATA *ch, char *argument ) {
	char skill_name[MIL];
	int sn, class_no;

	if (!argument[0]) {
		send_to_char ("Syntax is: skillinfo <skill>.\n\r",ch);
		return;
	}

	argument = one_argument (argument, skill_name);
	if ( (sn = skill_lookup (skill_name)) == -1) {
		printf_to_char (ch, "There is no such spell/skill as '%s'.\n\r", skill_name);
		return;
	}
  
  
  printf_to_char(ch, "Skill/Spell Info for '%s'\n\r",  skill_table[sn].name);
	for (class_no = 0; class_no < MAX_CLASS; class_no++) {    
		printf_to_char (ch, "  %s %d\n\r", class_table[class_no].name, skill_table[sn].skill_level[class_no]);
  }
  
} // do_skill_info()

/**/
void do_skedit(CHAR_DATA *ch ,char * argument) {
	char class_name[MIL], skill_name[MIL];
	int sn, level, class_no;
	
	argument = one_argument (argument, class_name);
	argument = one_argument (argument, skill_name);
	
	if (!argument[0])
	{
		send_to_char ("Syntax is: skedit <class> <skill> <level>.\n\r",ch);
		return;
	}
	
	level = atoi (argument);
	
	if (!is_number(argument) || level < 0 || level > MAX_LEVEL)
	{
		printf_to_char (ch, "Level range is from 0 to %d.\n\r", LEVEL_IMMORTAL);
		return;
	}
	
	
	if ( (sn = skill_lookup (skill_name)) == -1)
	{
		printf_to_char (ch, "There is no such spell/skill as '%s'.\n\r", skill_name);
		return;
	}
	
	for (class_no = 0; class_no < MAX_CLASS; class_no++)
		if (!str_cmp(class_name, class_table[class_no].name))
			break;
	
	if (class_no == MAX_CLASS)
	{
		printf_to_char (ch, "No class named '%s' exists.\n\r", class_name);
		return;
	}
	
	skill_table[sn].skill_level[class_no] = level;
	
	printf_to_char (ch, "OK, %ss will now gain %s at level %d%s.\n\r", class_table[class_no].name,
								skill_table[sn].name, level, level >= LEVEL_IMMORTAL ? " (i.e. never)" : "");
	
	
	do_save_class(ch, argument);
}
/**/

void do_gredit_spells( CHAR_DATA *ch, char *argument );
void do_gredit_rate( CHAR_DATA *ch, char *argument );
void do_gredit( CHAR_DATA *ch, char *argument );

void do_gredit( CHAR_DATA *ch, char *argument )
{
    char command[MIL];

    argument = one_argument(argument,command);

    if (command[0] == '\0') {
		stc("Syntax:\n\r",ch);
		stc("  gredit spells <group> add <spell/skill>\n\r",ch);
		stc("  gredit spells <group> rem <spell/skill>\n\r",ch);
		stc("  gredit rate   <group> <class>   <value>\n\r",ch);
		return;
    }
	
    if (!str_prefix(command,"rate")) {
		do_function(ch, &do_gredit_rate, argument);
		return;
    }
	
    if (!str_prefix(command,"spells")) {
		do_function(ch, &do_gredit_spells, argument);
		return;
    }
	
    /* echo syntax */
    do_function(ch, &do_gredit, "");
}


void do_gredit_spells( CHAR_DATA *ch, char *argument ) {
    char arg1 [MIL];
    char arg2 [MIL];
	int group, skill;
	
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	
	if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )  {
		stc( "Syntax:\n\r",ch);
		stc("  gredit spells <group> add <spell/skill>\n\r",ch);
		stc("  gredit spells <group> rem <spell/skill>\n\r",ch);
		return;
    }
	
	if ( (group = group_lookup (arg1)) == -1)	{
		printf_to_char (ch, "There is no such group as '%s'.\n\r", arg1);
		return;
	}
	
	if ( (skill = skill_lookup (argument)) == -1) {
		printf_to_char (ch, "There is no such spell/skill as '%s'.\n\r", argument);
		return;
	}
	
	if ( !str_prefix( arg2, "add" ) ) {
		int j;
		
		for (j = 0; j < MAX_IN_GROUP; j++) {
			// next entry is blank
			if (group_table[group].spells[j] == NULL) {
				// we found the first empty spot
				break;
			}

			// entry matches addendum
			if (group_table[group].spells[j] == skill_table[skill].name) {
				// error : already exists in group
				printf_to_char (ch, "Group '%s' already contains '%s'.\n\r", group_table[group].name, skill_table[skill].name);
				return;
			}
		}

		// first empty spot is last spot
		if (j == MAX_IN_GROUP) {
			// error : group full
			printf_to_char (ch, "Group 's' is full.\n\r", group_table[group].name);
			return;
		}
		
		// we've come this far, time to do it
		group_table[group].spells[j] = skill_table[skill].name;
		printf_to_char (ch, "Added '%s' to group '%s'.\n\r", skill_table[skill].name, group_table[group].name);
				
		// save groups
		do_save_groups(NULL, "");
		
		// fin
	    return;
	}
	
	if ( !str_prefix( arg2, "rem" ) ) {
		int i,j;
		int a, b;

		struct group_type *new_table = malloc (sizeof(struct group_type) * max_groups);
		
		if (!new_table)
		{
			send_to_char ("Memory allocation failed. Brace for impact...\n\r",ch);
			return;
		}
				
		// loop through groups
		for (a = 0, b = 0; a < max_groups; a++) {
			// target group
			if (group_table[a].name == group_table[group].name) {
				// copy name
				new_table[a].name = group_table[a].name;
					
				// loop through classes, copy ratings
				{
					int x;
					for (x = 0; x < MAX_CLASS; x++)
						new_table[a].rating[x] = group_table[a].rating[x];
				}

				// loop through groups spells
				for (i = 0, j = 0; i < MAX_IN_GROUP; i++) {
					// no more spells
					if (NULL == group_table[a].spells[i]) {
						new_table[a].spells[j] = NULL;
					}
					
					// copy all spells except our excluded skill
					if (!str_cmp(group_table[a].spells[i], skill_table[skill].name)) {
						continue;
					} else {
						new_table[a].spells[j] = group_table[a].spells[i];
						j++;
					}
				}
				new_table[a].spells[MAX_IN_GROUP] = NULL;
			// all other groups
			} else {
				// copy entire group
				new_table[a] = group_table[a];
			}
		}
	
		free (group_table);
		group_table = new_table;

		ptc(ch, "Removed '%s' from group '%s'.\n\r", skill_table[skill].name, group_table[group].name);
		do_save_groups(NULL, "");

	    return;
	}
	
    /* echo syntax */
    do_function(ch, &do_gredit_spells, "" );
    return;
}

void do_gredit_rate( CHAR_DATA *ch, char *argument ) {
    char arg1 [MIL];
    char arg2 [MIL];
    char arg3 [MIL];
	int group, class;
    int value;
	
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
	
	if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )  {
		stc( "Syntax:\n\r",ch);
		stc("  gredit rate <group> <class> <value>\n\r",ch);
		return;
    }
	
	if ( (group = group_lookup (arg1)) == -1)	{
		printf_to_char (ch, "There is no such group as '%s'.\n\r", arg1);
		return;
	}
	
	for (class = 0; class < MAX_CLASS; class++) {
		if (!str_cmp(arg2, class_table[class].name))
			break;
	}
		
	if (class == MAX_CLASS) {
		printf_to_char (ch, "No class named '%s' exists.\n\r", arg2);
		return;
	}
		
    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) ) {
		send_to_char( "Value must be numeric.\n\r", ch );
		return;
    }

    value = atoi( arg3 );
    if ( value < -1 || value > 100 ) {
		send_to_char( "Value range is -1 to 100.\n\r", ch );
		return;
    } else  {
		ptc(ch, "The group '%s' is now rated %d for the '%s' class.", group_table[group].name, value, class_table[class].name);
		group_table[group].rating[class] = value;
		
		do_save_groups(NULL, "");
		return;
	}
	
	
	
    /* echo syntax */
    do_function(ch, &do_gredit_rate, "" );
    return;
}

