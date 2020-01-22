#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int rank;
	char *key_phrase;
} SentenceResult;

char *rankwords[4] = {
	"terrible", "horrible", "sad", "depressed"
};

SentenceResult *parse_sentence(char *sentence, size_t size)
{
	memset(sentence, 0, size);
	fgets(sentence, size, stdin);

	SentenceResult *result = malloc(sizeof(SentenceResult));
	result->rank = 0;
	int parsing = 1, i, catching_keyphrase = 0, keyphrase_index = 0, str_index = 0;
	int started = 0;
	char *cache[8], *word;

	while (parsing) {
		/* get new word */
		if (started == 0) {
			word = strtok(sentence, " \n\0\t");
			if (word == NULL || strlen(word) == 0) {
				parsing = 0;
				continue;
			}
			str_index += strlen(word) + 1;
			started = 1;
		} else {
			word = strtok(sentence + str_index, " ");
			if (word == NULL || strlen(word) == 0) {
				parsing = 0;
				continue;
			}
			str_index += strlen(word) + 1;
		}

		if (catching_keyphrase) {
			strcpy(result->key_phrase + keyphrase_index, " ");
			keyphrase_index++;
			strcpy(result->key_phrase + keyphrase_index, word);
			keyphrase_index += strlen(word);
		}

		/* format string */
		for (i = 0; i < strlen(word); i++)
			word[i] = tolower(word[i]);

		if (ispunct(word[strlen(word) - 1]))
			word[strlen(word) - 1] = '\0';

		/* shift cache */
		for (i = 6; i >= 0; i--) 
			cache[i + 1] = cache[i];

		/* add new word to cache */
		cache[0] = word;

		/* this should be done for any emotional adjectives like this */
		for (i = 0; i < 4; i++)
			if (!strncmp(word, rankwords[i], strlen(rankwords[i])))
				result->rank++;

		/* catch key phrases */
		if (!strcmp(word, "well") || !strcmp(word, "because")) {
			catching_keyphrase = 1;
			result->key_phrase = malloc(512);
		}
	}

	return result;
}

int main(int argc, char *argv[])
{
	char input[512];
	int running = 1;
	SentenceResult *result;

	/* Welcome message */
	printf("Welcome to ASHLEY.\nAshley is a mock psychotherapist.\n\n");
	printf("Copyright (C) 2020 Ben O'Neill <ben@benoneill.xyz>. Source code is "
			"licensed under the GNU GPL v3 or later.\n");
	printf("This program is based off the idea of Joseph Weizenbaum's program "
			"ELIZA written in 1966.\n\n");

	/* Start conversing */
	printf("ASHLEY:\tHi, is something troubling you?\nYOU:\t");
	while (running) {
		result = parse_sentence(input, 512);
		if (!strcmp(input, "yeah")) {
			printf("ASHLEY:\tWhat is it?\nYOU:\t");
			result = parse_sentence(input, 512);
		}
		if (result->rank < 2) {
			/* ask one deeper question */
			printf("ASHLEY:\tWhy do you think that is?\nYOU:\t");
			result = parse_sentence(input, 512);
		} else if (result->rank < 5) {
			/* ask two or three deeper questions */
			printf("ASHLEY:\tWhat could you do to fix this?\nYOU:\t");
			result = parse_sentence(input, 512);
			printf("ASHLEY:\tWill that work?\nYOU:\t");
			result = parse_sentence(input, 512);
		} else {
			/* damn this is really bothering them, ask away ash */
			printf("ASHLEY:\tOh, is that important to you?\nYOU:\t");
			result = parse_sentence(input, 512);
			printf("ASHLEY:\tWhy do you think that is?\nYOU:\t");
			result = parse_sentence(input, 512);
			printf("ASHLEY:\tWhat could you do to repair this?\nYOU:\t");
			result = parse_sentence(input, 512);
		}

		/* back to the surface - ask a question */
		printf("ASHLEY:\tOK. Is there anything else bothering you?\nYOU:\t");
	}
	return 0;
}
