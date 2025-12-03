#include "wordle.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void to_upper_string(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = toupper((unsigned char)str[i]);
  }
}

bool load_word_list(const char *filename, char ***words, int *count) {
  FILE *f = fopen(filename, "r");
  if (!f)
    return false;

  char buffer[100];
  int capacity = 100;
  *count = 0;
  *words = malloc(capacity * sizeof(char *));

  while (fgets(buffer, sizeof(buffer), f)) {
    // Trim newline
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;

    if (strlen(buffer) != WORD_LENGTH)
      continue;

    if (*count >= capacity) {
      capacity *= 2;
      *words = realloc(*words, capacity * sizeof(char *));
    }

    (*words)[*count] = strdup(buffer);
    to_upper_string((*words)[*count]);
    (*count)++;
  }

  fclose(f);
  return true;
}

void free_word_list(char **words, int count) {
  for (int i = 0; i < count; i++) {
    free(words[i]);
  }
  free(words);
}

bool is_valid_word(const char *word, char **word_list, int word_count) {
  if (strlen(word) != WORD_LENGTH)
    return false;

  // Simple linear search for now, could be binary search if sorted
  for (int i = 0; i < word_count; i++) {
    if (strcmp(word, word_list[i]) == 0) {
      return true;
    }
  }
  return false;
}

char *get_random_word(char **word_list, int word_count) {
  if (word_count == 0)
    return NULL;
  return word_list[rand() % word_count];
}
