#ifndef GREP_RL_HEADER
#define GREP_RL_HEADER
extern int grep_rl_main(int argc, char **argv);
typedef struct string_item {
	char *str;
	struct string_item *next;
} string_item;
extern string_item *matched_list;

#endif

