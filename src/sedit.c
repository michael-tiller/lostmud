/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
 
/* Online Social Editting Module, 
 * (c) 1996 Erwin S. Andreasen <erwin@pip.dknet.dk>
 * See the file "License" for important licensing information
 */ 

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
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include "db.h"


#define SOCIAL_FILE "socials.txt" /* or whatever fits you */
/*stuff to set up key*/
#if defined(KEY)
#undef KEY
#endif

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
                                if ( !str_cmp( word, literal ) )        \
                                {                                       \
                                    field  = value;                     \
                                    fMatch = TRUE;                      \
                                    break;                              \
                                }



int maxSocial; /* max number of socials */

#ifndef CONST_SOCIAL

void load_social (FILE *fp, struct social_type *social)
{
    char *word;
	bool fMatch;
	for (;;)
    {
        word = feof (fp) ? "END" : fread_word (fp);
        fMatch = FALSE;
        switch (UPPER (word[0]))
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol (fp);
                break;
			case 'C':
				KEY("Chauto", social->char_auto, fread_string(fp) );
				KEY("Chnemot", social->chan_emote, fread_string(fp) );
				KEY("Chfound", social->char_found, fread_string(fp) );
				break;
			case 'E':
                if (!str_cmp (word, "END"))
                return;
			case 'N':
				KEY("Name", social->name, fread_string(fp) );
				KEY("Noargch", social->char_no_arg, fread_string(fp ) );
				KEY("Noargotr", social->others_no_arg, fread_string(fp) );
				break;
			case 'O':
				KEY("Otrfound", social->others_found, fread_string(fp) );
				KEY("Otrauto", social->others_auto, fread_string(fp) );
				break;
			case 'V':
				KEY("Vctfound", social->vict_found, fread_string(fp) );
				break;
		}
	}
	return;
}

void load_social_table()
{
	FILE *fp;
	int i;
	
	
	fp = fopen (SOCIAL_FILE, "r");
	
	if (!fp)
	{
		bug ("Could not open " SOCIAL_FILE " for reading.",0);
		exit(1);
	}
	
	fscanf (fp, "%d\n", &maxSocial);

	/* IMPORTANT to use malloc so we can realloc later on */
		
	social_table = malloc (sizeof(struct social_type) * (maxSocial+1));
	
	for (i = 0; i < maxSocial; i++)
		load_social (fp,&social_table[i]);

	/* For backwards compatibility */
			
	social_table[maxSocial].name = str_dup(""); /* empty! */			
		
	fclose (fp);
	
}

#endif /* CONST_SOCIAL */

void save_social (const struct social_type *s, FILE *fp)
{

	fprintf(fp, "Name %s~\n", s->name);
	fprintf(fp, "Noargch %s~\n", s->char_no_arg );
	fprintf(fp, "Noargotr %s~\n", s->others_no_arg );
	fprintf(fp, "Chfound %s~\n", s->char_found );
	fprintf(fp, "Otrfound %s~\n", s->others_found );
	fprintf(fp, "Vctfound %s~\n", s->vict_found );
	fprintf(fp, "Chauto %s~\n", s->char_auto );
	fprintf(fp, "Otrauto %s~\n", s->others_auto );
	fprintf(fp, "Chnemot %s~\n", s->chan_emote );
    fprintf(fp, "END\n\n" );
	return;

}


void save_social_table()
{
	FILE *fp;
	int i;
	
	fp = fopen ("social.temp", "w");
	
	if (!fp)
	{
		bug ("Could not open " SOCIAL_FILE " for writing.",0);
		return;
	}

#ifdef CONST_SOCIAL /* If old table still in use, count socials first */
	
	for (maxSocial = 0 ; social_table[maxSocial].name[0] ; maxSocial++)
		; /* empty */
#endif	
	
	
	fprintf (fp, "%d\n", maxSocial);
	
	for ( i = 0 ; i < maxSocial ; i++)
		save_social (&social_table[i], fp);
		
	fclose (fp);

	// clean old file
	remove(SOCIAL_FILE);

	// rename new file
	rename("social.temp",SOCIAL_FILE);
	
	// clean tempfile
	remove("social.temp");
}


/* Find a social based on name */ 
int social_lookup (const char *name)
{
    int i;

	for (i = 0; i < maxSocial ; i++)
		if (!str_cmp(name, social_table[i].name))
			return i;

    return -1;

}

/*
 * Social editting command
 */

#ifndef CONST_SOCIAL
void do_savesocial(CHAR_DATA *ch, char *argument)
{
	save_social_table();
	return;
}

void do_sedit (CHAR_DATA *ch, char *argument)
{
	char cmd[MAX_INPUT_LENGTH], social[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int iSocial;
	
	smash_tilde (argument);
	
	argument = one_argument (argument,cmd);
	argument = one_argument (argument,social);
	
	if (!cmd[0])
	{
		send_to_char ("Huh? Type HELP SEDIT to see syntax.\n\r",ch);
		return;
	}
		
	if (!social[0])
	{
		send_to_char ("What social do you want to operate on?\n\r",ch);
		return;
	}
	
	iSocial = social_lookup (social);
	
	if (str_cmp(cmd,"new") && (iSocial == -1))
	{
		send_to_char ("No such social exists.\n\r",ch);
		return;
	}

	if (!str_cmp(cmd, "delete")) /* Remove a social */
	{
		int i,j;
		struct social_type *new_table = malloc (sizeof(struct social_type) * maxSocial);
		
		if (!new_table)
		{
			send_to_char ("Memory allocation failed. Brace for impact...\n\r",ch);
			return;
		}
		
		/* Copy all elements of old table into new table, except the deleted social */
		for (i = 0, j = 0; i < maxSocial+1; i++)
			if (i != iSocial) /* copy, increase only if copied */
			{
				new_table[j] = social_table[i];
				j++;
			}

		free (social_table);
		social_table = new_table;
		
		maxSocial--; /* Important :() */
		
		send_to_char ("That social is history now.\n\r",ch);
				
	}
	
	else if (!str_cmp(cmd, "new")) /* Create a new social */
	{
		struct social_type *new_table;
		
		if (iSocial != -1)
		{
			send_to_char ("A social with that name already exists\n\r",ch);
			return;
		}
		
		/* reallocate the table */
		/* Note that the table contains maxSocial socials PLUS one empty spot! */
		
		maxSocial++;
		new_table = realloc (social_table, sizeof(struct social_type) * maxSocial + 1);
		
		if (!new_table) /* realloc failed */
		{
			send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
			return;
		}
		
		social_table = new_table;
		
		social_table[maxSocial-1].name = str_dup (social);
		social_table[maxSocial-1].chan_emote = str_dup("");
		social_table[maxSocial-1].char_no_arg = str_dup ("");
		social_table[maxSocial-1].others_no_arg = str_dup ("");
		social_table[maxSocial-1].char_found = str_dup ("");
		social_table[maxSocial-1].others_found = str_dup ("");
		social_table[maxSocial-1].vict_found = str_dup ("");
		social_table[maxSocial-1].char_auto = str_dup ("");
		social_table[maxSocial-1].others_auto = str_dup ("");
		
		social_table[maxSocial].name = str_dup (""); /* 'terminating' empty string */
		
		send_to_char ("New social added.\n\r",ch);
			
	}
	
	else if (!str_cmp(cmd, "show")) /* Show a certain social */
	{
		sprintf (buf, "\n\r"
					  "SEdit:     %s\n\r"
					  "-------    -----------------\n\r"
		              "chanmt:    %s\n\r"
					  "cnoarg:    %s\n\r"
		              "onoarg:    %s\n\r"
		              "cfound:    %s\n\r"
					  "ofound:    %s\n\r"
		              "vfound:    %s\n\r"
		              "cself:     %s\n\r"
		              "oself:     %s\n\r",
		              
		              social_table[iSocial].name,
					  social_table[iSocial].chan_emote,
		              social_table[iSocial].char_no_arg,
		              social_table[iSocial].others_no_arg,
		              social_table[iSocial].char_found,
		              social_table[iSocial].others_found,
		              social_table[iSocial].vict_found,
		              social_table[iSocial].char_auto,
		              social_table[iSocial].others_auto);

		send_to_char (buf,ch);		          
		return; /* return right away, do not save the table */
	}
	
	else if (!str_cmp(cmd, "cnoarg")) /* Set that argument */
	{
		free_string (social_table[iSocial].char_no_arg);
		social_table[iSocial].char_no_arg = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Character will now see nothing when this social is used without arguments.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}

	else if (!str_cmp(cmd, "chanmt")) /* Set that argument */
	{
		free_string (social_table[iSocial].chan_emote);
		social_table[iSocial].chan_emote = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Oooo. Bad idea! We want full social-emoting! But its nulled for now!\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else if (!str_cmp(cmd, "onoarg"))
	{
		free_string (social_table[iSocial].others_no_arg);
		social_table[iSocial].others_no_arg = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when this social is used without arguments.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "cfound"))
	{
		free_string (social_table[iSocial].char_found);
		social_table[iSocial].char_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("The character will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "ofound"))
	{
		free_string (social_table[iSocial].others_found);
		social_table[iSocial].others_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
			
	}
	
	else if (!str_cmp(cmd, "vfound"))
	{
		free_string (social_table[iSocial].vict_found);
		social_table[iSocial].vict_found = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Victim will now see nothing when a target is found.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else if (!str_cmp(cmd, "cself"))
	{
		free_string (social_table[iSocial].char_auto);
		social_table[iSocial].char_auto = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Character will now see nothing when targetting self.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);

	}
	
	else if (!str_cmp(cmd, "oself"))
	{
		free_string (social_table[iSocial].others_auto);
		social_table[iSocial].others_auto = str_dup(argument);		

		if (!argument[0])
			send_to_char ("Others will now see nothing when character targets self.\n\r",ch);
		else
			printf_to_char (ch,"New message is now:\n\r%s\n\r", argument);
	}
	
	else
	{
		send_to_char ("Huh? Try HELP SEDIT.\n\r",ch);
		return;
	}
	
	/* We have done something. update social table */
	
	save_social_table();
}
#endif /* CONST_SOCIAL */
