/*
 * findrep: A tool to easily find and replace from files
 *
 * Copyright (c) 2015 Arjun Sreedharan <arjun024@gmail.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "grep-rl.h"
#include "sed.h"

#define VERSION "0.0.2"
#define MAX_COMMAND_LENGTH 2048
#define MAX_FILE_PATH_LENGTH 1024

static char *search_folder = ".";
static char command[MAX_COMMAND_LENGTH];
static char match[MAX_FILE_PATH_LENGTH];

static void usage(char **argv, int es)
{
	fprintf(es == EXIT_SUCCESS ? stdout : stderr,
		"usage: %s [-h] <FIND-PATTERN> <REPLACE-WITH> [LOCATION]\n",
		argv[0]);
	_exit(es);
}

static void no_match(char *argv1)
{
	printf("Found no instances of '%s'\n", argv1);
	_exit(EXIT_SUCCESS);
}

/* incomplete */
static char *escape_spl_chars(const char* arg)
{
	int i, j, len, len_temp;
	char *op, *op_temp;
	len = strlen(arg) + 1;
	op = malloc(len);
	len_temp = len;
	op_temp = op;
	for(i = 0, j = 0; i < len; ++i, ++j) {
		if(arg[i] == ' ') {
			op[j++] = '\\';
			op[j] = ' ';
			op_temp = realloc(op, ++len_temp);
		} else {
			op[j] = arg[i];
		}
	}
	return op;
}

int main(int argc, char **argv)
{
	int rc, rc_each;
	char *grep_argv[] = {"grep-rl", " ", " ", NULL};
	int grep_argc = sizeof grep_argv / sizeof (char*) - 1;
	string_item *temp_str_item;
	char *sed_argv[] = {"sed", " ", " ", NULL};
	int sed_argc = sizeof sed_argv / sizeof (char*) - 1;
	char *e_argv1, *e_argv2;
	char option;

	while ((option = getopt(argc, argv, "h")) != -1) {
		switch (option) {
		case 'h':
			fprintf(stdout, "A tool to easily find and replace from"
				" files.\n");
			usage(argv, EXIT_SUCCESS);
		}
	}

	if (argc < 3) {
		usage(argv, EXIT_FAILURE);
	}

	if (argc > 3 && *argv[3]) {
		search_folder = argv[3];
	}

	grep_argv[1] = argv[1];
	grep_argv[2] = search_folder;

	/* grep-rl_main() fills matched_list */
	rc = grep_rl_main(grep_argc, grep_argv);

	if(rc)
		return rc;

	temp_str_item = matched_list;

	if (temp_str_item == NULL) {
		no_match(argv[1]);
	}

	rc = 1;
	while(temp_str_item && temp_str_item->str) {
		strcpy(match, temp_str_item->str);
		if (match[strlen(match) - 1] == '\n')
			match[strlen(match) - 1] = ' ';

		e_argv1 = escape_spl_chars(argv[1]);
		e_argv2 = escape_spl_chars(argv[2]);
		snprintf(command, sizeof command, "s %s %s g",
			e_argv1, e_argv2);
		sed_argv[1] = command;
		sed_argv[2] = match;
		rc_each = sed_main(sed_argc, sed_argv);
		free(e_argv1);
		free(e_argv2);
		if (rc_each) {
			/* sed also prints its  error into stderr */
			fprintf(stderr, "ERROR when performing action on file '%s':\n",
				match);
		} else {
			rc = 0;
			printf("Replaced '%s' in: %s\n", argv[1], match);
		}
		temp_str_item = temp_str_item ->next;
	}

	return rc;

}
