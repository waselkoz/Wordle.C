#include "wordle.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Check if word matches feedback
bool is_consistent(const char *candidate, const char *guess,
                   const FeedbackColor *feedback) {
  FeedbackColor simulated_feedback[WORD_LENGTH];
  get_feedback(candidate, guess, simulated_feedback);

  for (int i = 0; i < WORD_LENGTH; i++) {
    if (simulated_feedback[i] != feedback[i]) {
      return false;
    }
  }
  return true;
}

// Main solver loop
void solve_game(const char *target, char **word_list, int word_count) {
  // Create a boolean array to track possible words
  bool *possible = malloc(word_count * sizeof(bool));
  for (int i = 0; i < word_count; i++)
    possible[i] = true;

  int possible_count = word_count;
  int guesses = 0;
  char current_guess[WORD_LENGTH + 1];

  // First guess strategy: Pick a good starter word if available, else random
  strcpy(current_guess, "CRANE");
  if (!is_valid_word(current_guess, word_list, word_count)) {
    strcpy(current_guess, word_list[0]);
  }

  printf("Target: %s\n", target);
  printf("Solver started...\n");

  while (guesses < MAX_GUESSES) {
    printf("Guess %d: %s\n", guesses + 1, current_guess);

    FeedbackColor feedback[WORD_LENGTH];
    get_feedback(target, current_guess, feedback);

    // Print feedback
    printf("Feedback: ");
    for (int i = 0; i < WORD_LENGTH; i++) {
      switch (feedback[i]) {
      case COLOR_GREEN:
        printf("G");
        break;
      case COLOR_YELLOW:
        printf("Y");
        break;
      case COLOR_GRAY:
        printf("-");
        break;
      }
    }
    printf("\n");

    if (strcmp(current_guess, target) == 0) {
      printf("Solver won in %d guesses!\n", guesses + 1);
      free(possible);
      return;
    }

    // Filter
    int new_count = 0;
    for (int i = 0; i < word_count; i++) {
      if (possible[i]) {
        if (is_consistent(word_list[i], current_guess, feedback)) {
          new_count++;
        } else {
          possible[i] = false;
        }
      }
    }
    possible_count = new_count;
    printf("Remaining possibilities: %d\n", possible_count);

    if (possible_count == 0) {
      printf("Error: No words left consistent with feedback!\n");
      break;
    }

    // Pick next guess
    for (int i = 0; i < word_count; i++) {
      if (possible[i]) {
        strcpy(current_guess, word_list[i]);
        break;
      }
    }

    guesses++;
  }

  printf("Solver failed to find the word within %d guesses.\n", MAX_GUESSES);
  free(possible);
}
