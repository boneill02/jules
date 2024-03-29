/* jules.c: a chat bot engine
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef MAX_DEFAULT_RULES
#define MAX_DEFAULT_RULES 256
#endif

#ifndef MAX_RESULTS
#define MAX_RESULTS 8
#endif

typedef struct pattern_rule {
	char *pattern;
	char results[MAX_RESULTS][64];
	int result_count;
	struct pattern_rule *next;
} PatternRule;

PatternRule *pattern_rule_list;
char *default_rule_list[MAX_DEFAULT_RULES];
int num_default_rules = 0, num_pattern_rules = 0;

char initial_str[256], user_name[8] = "YOU", script_name[8];

enum ParseState {
	PARSE_META,
	PARSE_PATTERN,
	PARSE_DEFAULT,
};

void do_jules(void);
void load_script(char *path);
void parse_response(char *response);

void do_jules(void)
{
	int c, index = 0;
	char response[512];

	printf("%s:\t%s\n", script_name, initial_str);
	printf("%s:\t", user_name);
	while ((c = fgetc(stdin)) != EOF) {
		if (c == '\n') {
			parse_response(response);
			index = 0;
			memset(response, 0, 512);
			printf("%s:\t", user_name);
		} else {
			response[index] = c;
			index++;
		}
	}
}

void load_script(char *path)
{
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "couldn't open the script.\n");
		exit(1);
	}
	char pattern[256], result[256];
	int c, i = 0, j = 0, k = 0, line_num = 1, line_state = 0, array_index = 0;
	bool ignore = false, comment = false;
	enum ParseState parse_state = PARSE_META;

	PatternRule *current_pattern_rule = malloc(sizeof(PatternRule));
	pattern_rule_list = current_pattern_rule;

	while((c = fgetc(f)) != EOF) {
		if (comment == true) {
			if (c == '\n') {
				comment = false;
				goto handle_newline;
			} else {
				continue;
			}
		}
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
			case '#':
				if (array_index == 0)
					pattern[0] = '\0';
				comment = true;
				break;
			case '\n':
handle_newline:
				result[array_index] = '\0';
				if (parse_state == PARSE_META && strcmp(pattern, "PATTERN") && strlen(pattern) != 0) {
					if (!strcmp(pattern, "START")) {
						strcpy(initial_str, result);
					} else if (!strcmp(pattern, "NAME")) {
						strcpy(script_name, result);
					} else {
						fprintf(stderr, "syntax error at line %d\n", line_num);
						exit(1);
					}
				} else if (parse_state == PARSE_META && !strcmp(pattern, "PATTERN")) {
					parse_state = PARSE_PATTERN;
				} else if (parse_state == PARSE_PATTERN && strcmp(pattern, "DEFAULT")) {
					current_pattern_rule->pattern = malloc(strlen(pattern) + 1);
					strcpy(current_pattern_rule->pattern, pattern);
					current_pattern_rule->pattern[strlen(pattern)] = '\0';
					j = 0, k = 0;
					for (i = 0; i < strlen(result); i++) {
						if (result[i] == ',' && j < MAX_RESULTS) {
							current_pattern_rule->results[j][k] = '\0';
							j++;
							k = 0;
						} else {
							current_pattern_rule->results[j][k] = result[i];
							k++;
						}
					}
					current_pattern_rule->result_count = j + 1;
					current_pattern_rule->results[j][k] = '\0';
					current_pattern_rule->next = malloc(sizeof(PatternRule));
					current_pattern_rule = current_pattern_rule->next;
				} else if (parse_state == PARSE_PATTERN && !strcmp(pattern, "DEFAULT")) {
					parse_state = PARSE_DEFAULT;
					current_pattern_rule->next = NULL;
					current_pattern_rule->pattern = NULL;
				} else if (parse_state == PARSE_DEFAULT) {
					if (num_default_rules >= MAX_DEFAULT_RULES)
						continue; /* ignore it */
					pattern[array_index] = '\0';
					default_rule_list[num_default_rules] = malloc(strlen(pattern) + 1);
					strcpy(default_rule_list[num_default_rules], pattern);
					num_default_rules++;
				} else if (strlen(pattern) == 0) {
					/* do nothing */
				} else {
					fprintf(stderr, "syntax error at line %d\n", line_num);
				}
				line_num++;
				line_state = 0;
				for (int i = 0; i < 256; i++)
					pattern[i] = '\0';
				for (int i = 0; i < 256; i++)
					result[i] = '\0';
				array_index = 0;
				break;
			case '\\':
				ignore = true;
				break;
			case ':':
				pattern[array_index] = '\0';
				line_state = 1;
				array_index = 0;
				break;
			default:
				if (line_state == 0) {
					pattern[array_index] = c;
					array_index++;
				} else {
					result[array_index] = c;
					array_index++;
				}
		}
	}

	current_pattern_rule->next = NULL;
}

void parse_response(char *response)
{
	regex_t regex;
	int regex_ret;
	PatternRule *pattern_rule = pattern_rule_list;

	if (!strcmp(response, "exit") || !strcmp(response, "quit"))
		exit(0);

	while (pattern_rule->pattern != NULL) {
		regex_ret = regcomp(&regex, pattern_rule->pattern, REG_EXTENDED);
		if (regex_ret) {
			fprintf(stderr, "Could not compile regex: %s\n", pattern_rule->pattern);
			exit(1);
		}

		regex_ret = regexec(&regex, response, 0, NULL, 0);

		if (!regex_ret) {
			printf("%s:\t%s\n", script_name, pattern_rule->results[random() % pattern_rule->result_count]);
			regfree(&regex);
			return;
		}

		regfree(&regex);
		pattern_rule = pattern_rule->next;
	}

	printf("%s:\t%s\n", script_name, default_rule_list[random() % num_default_rules]);
}

int main(int argc, char *argv[])
{
	char *basedir = "/usr/share/jules/";
	char *script = "texting.jul";
	char *fullpath;

	if (argc == 2) {
		basedir = "";
		script = argv[1];
	} else if (argc == 1) {
		/* do nothing */
	} else {
		printf("usage: %s [script]\n", argv[0]);
	}

	fullpath = malloc(strlen(basedir) + strlen(script) + 1);
	strcpy(fullpath, basedir);
	strcpy(fullpath + strlen(basedir), script);

	srandom(time(NULL));
	load_script(fullpath);
	do_jules();
	return 0;
}
