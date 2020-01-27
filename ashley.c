#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct default_rule {
	char *result;
	struct default_rule *next;
} DefaultRule;

typedef struct pattern_rule {
	char *pattern;
	char *result;
	struct pattern_rule *next;
} PatternRule;

DefaultRule *default_rule_list;
PatternRule *pattern_rule_list;

enum ParseState {
	PARSE_PATTERN,
	PARSE_DEFAULT,
};

void do_ashley(void);
void load_script(char *path);
void parse_response(char *response);

void load_script(char *path)
{
	FILE *f = fopen(path, "r");
	char pattern[256], result[256];
	int c, i = 0, line_num = 1, line_state = 0, array_index = 0;
	bool ignore = false;
	enum ParseState parse_state = PARSE_PATTERN;

	DefaultRule *current_default_rule = malloc(sizeof(DefaultRule));
	default_rule_list = current_default_rule;

	PatternRule *current_pattern_rule = malloc(sizeof(PatternRule));
	pattern_rule_list = current_pattern_rule;

	while((c = fgetc(f)) != EOF) {
		if (ignore == true) {
			if (parse_state == PARSE_PATTERN) {
				if (line_state == 0)
					pattern[array_index] = c;
				else
					result[array_index] = c;
			} else {
				pattern[array_index] = c;
			}
			array_index++;
			ignore = false;
			continue;
		}

		switch (c) {
			case '\n':
				result[array_index] = '\0';
				if (parse_state == PARSE_PATTERN && strcmp(pattern, "DEFAULT")) {
					current_pattern_rule->pattern = malloc(strlen(pattern) + 1);
					strcpy(current_pattern_rule->pattern, pattern);
					current_pattern_rule->pattern[strlen(pattern)] = '\0';

					current_pattern_rule->result = malloc(strlen(result) + 1);
					strcpy(current_pattern_rule->result, result);
					current_pattern_rule->result[strlen(result)] = '\0';

					current_pattern_rule->next = malloc(sizeof(PatternRule));
					current_pattern_rule = current_pattern_rule->next;
				} else if (parse_state == PARSE_PATTERN && !strcmp(pattern, "DEFAULT")) {
					parse_state = PARSE_DEFAULT;
					current_pattern_rule->next = NULL;
					current_pattern_rule->pattern = NULL;
					current_pattern_rule->result = NULL;
				} else if (parse_state == PARSE_DEFAULT) {
					pattern[array_index] = '\0';
					current_default_rule->result = malloc(strlen(pattern) + 1);
					strcpy(current_default_rule->result, pattern);
					current_default_rule->result[strlen(pattern)] = '\0';

					current_default_rule->next = malloc(sizeof(DefaultRule));
					current_default_rule = current_default_rule->next;
				} else {
					fprintf(stderr, "syntax error at line %d\n", line_num);
				}
				line_num++;
				line_state = 0;
				array_index = 0;
				break;
			case '\\':
				ignore = true;
				break;
			case ':':
				if (parse_state == PARSE_PATTERN) {
					pattern[array_index] = '\0';
					line_state = 1;
					array_index = 0;
				}
				break;
			default:
				if (parse_state == PARSE_PATTERN) {
					if (line_state == 0) {
						pattern[array_index] = c;
						array_index++;
					} else {
						result[array_index] = c;
						array_index++;
					}
				} else {
					pattern[array_index] = c;
					array_index++;
				}
		}
	}

	current_default_rule->next = NULL;
	current_pattern_rule->next = NULL;
}

void do_ashley(void)
{
	PatternRule *patrule = pattern_rule_list;
	DefaultRule *defrule = default_rule_list;
	int c, index = 0;
	char response[512];

	printf("YOU:\t");
	while ((c = fgetc(stdin)) != EOF) {
		if (c == '\n') {
			parse_response(response);
			index = 0;
			memset(response, 0, 512);
			printf("YOU:\t");
		} else {
			response[index] = c;
			index++;
		}
	}
}

void parse_response(char *response)
{
	regex_t regex;
	int regex_ret;
	PatternRule *pattern_rule = pattern_rule_list;

	while (pattern_rule->pattern != NULL) {
		regex_ret = regcomp(&regex, pattern_rule->pattern, REG_EXTENDED);
		if (regex_ret) {
			fprintf(stderr, "Could not compile regex.\n");
			exit(1);
		}

		regex_ret = regexec(&regex, response, 0, NULL, 0);
		if (!regex_ret) {
			printf("ASHLEY:\t%s\n", pattern_rule->result);
			regfree(&regex);
			return;
		}

		regfree(&regex);
		pattern_rule = pattern_rule->next;
	}

	printf("ASHLEY:\t%s\n", default_rule_list->result);
}

int main(int argc, char *argv[])
{
	//char *basedir = "/usr/share/ashley/";
	char *basedir = "./";
	char *script = "DOCTOR.ash";
	char *fullpath;
	char *argv0 = *argv;

	argc--, argv++;
	for (; argc > 0; argv++, argc--) {
		if (!strcmp(*argv, "-f"))
			script = ++(*argv);
		else if (!strcmp(*argv, "-d"))
			basedir = ++(*argv);
		else
			printf("usage: %s [-d scriptdir] [-f script]\n", argv0);
	}

	fullpath = malloc(strlen(basedir) + strlen(script) + 1);
	strcpy(fullpath, basedir);
	strcpy(fullpath + strlen(basedir), script);

	load_script(fullpath);
	do_ashley();
	return 0;
}
