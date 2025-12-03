#include "wordle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ANSI Color Codes
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_BG_GREEN "\x1b[42m"
#define ANSI_BG_YELLOW "\x1b[43m"
#define ANSI_BG_GRAY "\x1b[100m"

void print_feedback(const char *guess, const FeedbackColor *feedback) {
  for (int i = 0; i < WORD_LENGTH; i++) {
    switch (feedback[i]) {
    case COLOR_GREEN:
      printf(ANSI_BG_GREEN " %c " ANSI_COLOR_RESET, guess[i]);
      break;
    case COLOR_YELLOW:
      printf(ANSI_BG_YELLOW " %c " ANSI_COLOR_RESET, guess[i]);
      break;
    case COLOR_GRAY:
      printf(ANSI_BG_GRAY " %c " ANSI_COLOR_RESET, guess[i]);
      break;
    }
    printf(" ");
  }
  printf("\n");
}

void play_manual(char **word_list, int word_count) {
  char *target = get_random_word(word_list, word_count);
  GameState game;
  init_game(&game, target);

  printf("Welcome to Wordle! Guess the %d-letter word.\n", WORD_LENGTH);
  printf("Green = Correct, Yellow = Wrong Position, Gray = Not in word.\n");

  char guess_input[100];

  while (!game.game_over) {
    printf("Guess %d/%d: ", game.guess_count + 1, MAX_GUESSES);
    if (scanf("%99s", guess_input) != 1)
      break;

    to_upper_string(guess_input);

    if (!is_valid_word(guess_input, word_list, word_count)) {
      printf("Invalid word. Try again.\n");
      continue;
    }

    process_guess(&game, guess_input);
    print_feedback(game.guesses[game.guess_count - 1].word,
                   game.guesses[game.guess_count - 1].feedback);
  }

  if (game.won) {
    printf("Congratulations! You guessed the word: %s\n", target);
  } else {
    printf("Game over! The word was: %s\n", target);
  }
}

void run_solver_mode(char **word_list, int word_count) {
  printf("Enter target word for solver (or 'RANDOM'): ");
  char input[100];
  if (scanf("%99s", input) != 1)
    return;
  to_upper_string(input);

  char *target;
  if (strcmp(input, "RANDOM") == 0) {
    target = get_random_word(word_list, word_count);
  } else {
    if (!is_valid_word(input, word_list, word_count)) {
      printf("Invalid word not in dictionary. Proceeding anyway, but solver "
             "might fail if logic depends on dictionary.\n");
    }
    target = input; // Note: if input is local stack, be careful. Here it's fine
                    // for this scope.
  }

  solve_game(target, word_list, word_count);
}

int main() {
  srand(time(NULL));

  char **word_list;
  int word_count;

  if (!load_word_list("words.txt", &word_list, &word_count)) {
    fprintf(stderr, "Failed to load words.txt\n");
    return 1;
  }
  printf("Loaded %d words.\n", word_count);

  printf("1. Play Manual\n");
  printf("2. Run Solver\n");
  printf("Choice: ");
  int choice;
  if (scanf("%d", &choice) != 1)
    return 1;

  if (choice == 1) {
    play_manual(word_list, word_count);
  } else if (choice == 2) {
    run_solver_mode(word_list, word_count);
  } else {
    printf("Invalid choice.\n");
  }

  free_word_list(word_list, word_count);
  return 0;
}