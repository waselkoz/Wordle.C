#include "wordle.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Checks if a candidate word is consistent with the feedback received
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

// Simple solver: Filters words based on consistency and picks the first
// available one.
int solve_game_simple(const char *target, char **word_list, int word_count,
                      bool silent) {
  bool *possible = malloc(word_count * sizeof(bool));
  for (int i = 0; i < word_count; i++)
    possible[i] = true;

  int possible_count = word_count;
  int guesses = 0;
  char current_guess[WORD_LENGTH + 1];

  // Start with "CRANE" or first word
  strcpy(current_guess, "CRANE");
  if (!is_valid_word(current_guess, word_list, word_count)) {
    strcpy(current_guess, word_list[0]);
  }

  if (!silent) {
    printf("Target: %s\n", target);
    printf("Simple Solver started...\n");
  }

  while (guesses < MAX_GUESSES) {
    if (!silent)
      printf("Guess %d: %s\n", guesses + 1, current_guess);

    FeedbackColor feedback[WORD_LENGTH];
    get_feedback(target, current_guess, feedback);

    if (!silent) {
      printf("Feedback: ");
      for (int i = 0; i < WORD_LENGTH; i++) {
        char c = '-';
        if (feedback[i] == COLOR_GREEN)
          c = 'G';
        else if (feedback[i] == COLOR_YELLOW)
          c = 'Y';
        printf("%c", c);
      }
      printf("\n");
    }

    if (strcmp(current_guess, target) == 0) {
      if (!silent)
        printf("Solver won in %d guesses!\n", guesses + 1);
      free(possible);
      return guesses + 1;
    }

    // Filter impossible words
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
    if (!silent)
      printf("Remaining possibilities: %d\n", possible_count);

    if (possible_count == 0) {
      if (!silent)
        printf("Error: No words left consistent with feedback!\n");
      break;
    }

    // Pick next valid word
    for (int i = 0; i < word_count; i++) {
      if (possible[i]) {
        strcpy(current_guess, word_list[i]);
        break;
      }
    }
    guesses++;
  }

  if (!silent)
    printf("Solver failed to find the word within %d guesses.\n", MAX_GUESSES);
  free(possible);
  return guesses;
}

// --- Minimax Solver Helpers ---

// Encodes feedback array into a single integer index (Base 3)
int get_feedback_index(const FeedbackColor *feedback) {
  int index = 0;
  int multiplier = 1;
  for (int i = 0; i < WORD_LENGTH; i++) {
    index += feedback[i] * multiplier;
    multiplier *= 3;
  }
  return index;
}

// Calculates the size of the largest group of remaining words for a given guess
int get_worst_case_split(const char *guess, char **word_list,
                         const bool *possible, int word_count) {
  int counts[243] = {0}; // 3^5 possible feedback patterns

  FeedbackColor feedback[WORD_LENGTH];

  for (int i = 0; i < word_count; i++) {
    if (possible[i]) {
      // Assume word_list[i] is the target
      get_feedback(word_list[i], guess, feedback);
      int idx = get_feedback_index(feedback);
      counts[idx]++;
    }
  }

  int max_count = 0;
  for (int i = 0; i < 243; i++) {
    if (counts[i] > max_count) {
      max_count = counts[i];
    }
  }
  return max_count;
}

int solve_game_minimax(const char *target, char **word_list, int word_count,
                       bool silent) {
  bool *possible = malloc(word_count * sizeof(bool));
  for (int i = 0; i < word_count; i++)
    possible[i] = true;

  int possible_count = word_count;
  int guesses = 0;
  char current_guess[WORD_LENGTH + 1];

  // Hardcoded best opener
  strcpy(current_guess, "RAISE");
  if (!is_valid_word(current_guess, word_list, word_count)) {
    strcpy(current_guess, word_list[0]);
  }

  if (!silent) {
    printf("Target: %s\n", target);
    printf("Minimax Solver started...\n");
  }

  while (guesses < MAX_GUESSES) {
    if (!silent)
      printf("Guess %d: %s\n", guesses + 1, current_guess);

    FeedbackColor feedback[WORD_LENGTH];
    get_feedback(target, current_guess, feedback);

    if (!silent) {
      printf("Feedback: ");
      for (int i = 0; i < WORD_LENGTH; i++) {
        char c = '-';
        if (feedback[i] == COLOR_GREEN)
          c = 'G';
        else if (feedback[i] == COLOR_YELLOW)
          c = 'Y';
        printf("%c", c);
      }
      printf("\n");
    }

    if (strcmp(current_guess, target) == 0) {
      if (!silent)
        printf("Solver won in %d guesses!\n", guesses + 1);
      free(possible);
      return guesses + 1;
    }

    // Filter possibilities
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
    if (!silent)
      printf("Remaining possibilities: %d\n", possible_count);

    if (possible_count == 0) {
      if (!silent)
        printf("Error: No words left consistent with feedback!\n");
      break;
    }

    if (possible_count == 1) {
      for (int i = 0; i < word_count; i++) {
        if (possible[i]) {
          strcpy(current_guess, word_list[i]);
          break;
        }
      }
      guesses++;
      continue;
    }

    // Minimax: Minimize worst-case split size
    // Tie-breaker: Prefer words that are possible answers
    int min_worst_case = word_count + 1;
    int best_word_index = -1;

    for (int i = 0; i < word_count; i++) {
      int worst_case =
          get_worst_case_split(word_list[i], word_list, possible, word_count);

      if (worst_case < min_worst_case) {
        min_worst_case = worst_case;
        best_word_index = i;
      } else if (worst_case == min_worst_case) {
        // Refinement: If worst cases are equal, prefer a word that is a
        // possible answer! This is a crucial heuristic. If I guess a word that
        // *can't* be the answer, I get info but 0% chance of winning now. If I
        // guess a word that *can* be the answer, I get info AND chance to win.
        if (best_word_index != -1 && !possible[best_word_index] &&
            possible[i]) {
          best_word_index = i;
        }
      }

      if (min_worst_case == 1 && possible[best_word_index])
        break;
    }

    if (best_word_index != -1) {
      strcpy(current_guess, word_list[best_word_index]);
    } else {
      for (int i = 0; i < word_count; i++) {
        if (possible[i]) {
          strcpy(current_guess, word_list[i]);
          break;
        }
      }
    }

    guesses++;
  }

  if (!silent)
    printf("Solver failed to find the word within %d guesses.\n", MAX_GUESSES);
  free(possible);
  return guesses;
}
