#include "wordle.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Converts a string to uppercase in place.
 * @param str The string to convert.
 */
void to_upper_string(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = toupper((unsigned char)str[i]);
  }
}

/**
 * Loads the word list from a file.
 * @param filename The path to the file containing words.
 * @param words Pointer to the array of strings to store the words.
 * @param count Pointer to an integer to store the number of words loaded.
 * @return true if successful, false otherwise.
 */
bool load_word_list(const char *filename, char ***words, int *count) {
  FILE *f = fopen(filename, "r");
  if (!f)
    return false;

  char buffer[100];
  int capacity = 100;
  *count = 0;
  *words = malloc(capacity * sizeof(char *));

  while (fgets(buffer, sizeof(buffer), f)) {
    // Trim newline characters
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "\r")] = 0;

    // Skip words that are not the correct length
    if (strlen(buffer) != WORD_LENGTH)
      continue;

    // Resize array if needed
    if (*count >= capacity) {
      capacity *= 2;
      *words = realloc(*words, capacity * sizeof(char *));
    }

    // Duplicate the string and store it
    (*words)[*count] = strdup(buffer);
    to_upper_string((*words)[*count]);
    (*count)++;
  }

  fclose(f);
  return true;
}

/**
 * Frees the memory allocated for the word list.
 * @param words The array of strings.
 * @param count The number of words.
 */
void free_word_list(char **words, int count) {
  for (int i = 0; i < count; i++) {
    free(words[i]);
  }
  free(words);
}

/**
 * Checks if a word exists in the word list.
 * @param word The word to check.
 * @param word_list The list of valid words.
 * @param word_count The number of words in the list.
 * @return true if the word is found, false otherwise.
 */
bool is_valid_word(const char *word, char **word_list, int word_count) {
  if (strlen(word) != WORD_LENGTH)
    return false;

  for (int i = 0; i < word_count; i++) {
    if (strcmp(word, word_list[i]) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * Returns a random word from the word list.
 * @param word_list The list of valid words.
 * @param word_count The number of words in the list.
 * @return A pointer to a random word string.
 */
char *get_random_word(char **word_list, int word_count) {
  if (word_count == 0)
    return NULL;
  return word_list[rand() % word_count];
}
