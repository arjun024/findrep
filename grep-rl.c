/*
 * grep-rl.c
 * This file is a modification done by Arjun Sreedharan (2015) for findrep
 * on the original file written by Jiri Slaby.
 *
 * Copyright (C) 2015 Arjun Sreedharan <arjun024@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

/*
 *  simple regexp demonstration
 *  Copyright (C) 2003-2006 Jiri Slaby <jirislaby@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "grep-rl.h"

unsigned int optcnt = 0, optlns = 0, optfnam = 0, optcasi = 0, optrec = 1;
string_item *matched_list = NULL;

static int add_to_matched_list(char *s) {
	string_item *temp = matched_list;
	int i = 0;
	while(temp != NULL) {
		if(strcmp(temp->str, s) == 0) /* already in list */
			return -1;
		temp = temp->next;
	}
	/* add new string to front */
	temp = malloc(sizeof (string_item));
	temp->str = malloc(strlen(s) + 1);
	strcpy(temp->str, s);
	temp->next = matched_list;
	matched_list = temp;
}

static void print_matched_list() {
	string_item *temp = matched_list;
	while(temp != NULL) {
		printf("%s\n", temp->str);
		temp = temp->next;
	}
}

static int findinfile(char *name, regex_t *RE, char flagstdin, char *string,
		int files)
{
	int oklines = 0, lines = 1, nfound;
	FILE *in = stdin;

	if (!flagstdin) {
		in = fopen(name, "r");
		if (in == NULL) {
			perror("fopen failed\n");
			return -1;
		}
	}

	while (!feof(in)) {
		fgets(string, 255, in);

		nfound = regexec(RE, string, 0, NULL, 0);

		if (!nfound) {
			if (optcnt)
				oklines++;
			else {
				add_to_matched_list(name);
			}
		}

		if (!feof(in)) {
			string[0] = getc(in);
			if (!feof(in))
				ungetc(string[0], in);
		}
		lines++;
	}
	if (optcnt) {
		add_to_matched_list(name);
	}

	fclose(in);

	return 0;
}

static char rec(char *name, regex_t * RE, char flagstdin, char *string,
		int files)
{
	struct dirent *entry;
	struct stat sstat;
	DIR *dir;
	char *str;

	if (flagstdin || !(dir = opendir(name)))
		return 1;
	else if (!optrec)
		return 2;

	while ((entry = readdir(dir))) {
		if (name[strlen(name) - 1] == '/')
			name[strlen(name) - 1] = 0;

		str = malloc(strlen(name) + strlen(entry->d_name) + 2);
		if (str == NULL) {
			perror("cannot alloc memory");
			return -1;
		}
		sprintf(str, "%s/%s", name, entry->d_name);
		if (stat(str, &sstat)) {
			perror("stat failed");
			return -1;
		}

		if (S_ISREG(sstat.st_mode) &&
				findinfile(str, RE, flagstdin, string, files))
			return -1;

#ifdef debug
		printf("%s - %s (%d)\n", str, S_ISREG(sstat.st_mode) ?
				"file" : "dir", sstat.st_mode);
#endif

		if (strcmp(entry->d_name, ".") &&
				strcmp(entry->d_name, ".."))
			rec(str, RE, flagstdin, string, files);

		free(str);
	}

	closedir(dir);

	return 0;
}

int grep_rl_main(int argc, char **argv)
{
	regex_t RE;
	char flagstdin = 0, string[256];
	int retval, files;
	extern int optind;

#ifdef debug
	printf("Regular expression: %s\n\n", argv[optind]);
#endif

	retval = regcomp(&RE, argv[optind], optcasi ? REG_ICASE : 0);
	if (retval) {
		regerror(retval, &RE, string, 255);
		printf("Error in regular expression: %s\n\n", string);

		return 2;
	}

	optind++;
	if (argc == optind)
		flagstdin = 1;

	optind -= flagstdin;
	files = argc - optind;

#ifdef debug
	printf("souboru:%d, parametru:%d, optind: %d, stdin:%d\n", files, argc,
	       optind, flagstdin);
#endif

	for (; optind < argc; optind++) {
		retval = rec(argv[optind], &RE, flagstdin, string, files);
		if (retval) {
			if (retval < 0)
				break;
			if (retval == 2) {
				if (optcnt)
					puts("0");
				break;
			}
			if (findinfile(argv[optind], &RE, flagstdin, string,
						files))
				break;
		}
	}

	regfree(&RE);
	/* print_matched_list(); */
	return 0;
}
