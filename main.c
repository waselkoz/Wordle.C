#include "wordle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ANSI Color Codes for terminal output
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_BG_GREEN "\x1b[42m"
#define ANSI_BG_YELLOW "\x1b[43m"
#define ANSI_BG_GRAY "\x1b[100m"

/**
 * Prints the feedback for a guess using colored backgrounds.
 * Green: Correct letter in correct position.
 * Yellow: Correct letter in wrong position.
 * Gray: Letter not in the word.
 */
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

/**
 * Runs the manual gameplay mode where the user guesses the word.
 */
void play_manual(char **word_list, int word_count) {
  // Select a random target word
  char *target = get_random_word(word_list, word_count);
  GameState game;
  init_game(&game, target);

  printf("Welcome to Wordle! Guess the %d-letter word.\n", WORD_LENGTH);
  printf("Green = Correct, Yellow = Wrong Position, Gray = Not in word.\n");

  char guess_input[100];

  // Main game loop
  while (!game.game_over) {
    printf("Guess %d/%d: ", game.guess_count + 1, MAX_GUESSES);
    if (scanf("%99s", guess_input) != 1)
      break;

    to_upper_string(guess_input);

    // Validate the guess
    if (!is_valid_word(guess_input, word_list, word_count)) {
      printf("Invalid word. Try again.\n");
      continue;
    }

    // Process the guess and update game state
    process_guess(&game, guess_input);

    // Display feedback
    print_feedback(game.guesses[game.guess_count - 1].word,
                   game.guesses[game.guess_count - 1].feedback);
  }

  // End of game message
  if (game.won) {
    printf("Congratulations! You guessed the word: %s\n", target);
  } else {
    printf("Game over! The word was: %s\n", target);
  }
}

/**
 * Runs the solver mode where the computer tries to solve the game.
 */
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

  printf("Choose Solver Strategy:\n");
  printf("1. Simple Solver (Consistency)\n");
  printf("2. Minimax Solver (Entropy/Worst-case)\n");
  int strategy = 1;
  printf("Choice: ");
  if (scanf("%d", &strategy) != 1)
    strategy = 1;

  if (strategy == 2) {
    solve_game_minimax(target, word_list, word_count, false);
  } else {
    solve_game_simple(target, word_list, word_count, false);
  }
}

/**
 * Runs the benchmark mode to compare solvers.
 */
void run_benchmark(char **word_list, int word_count) {
  printf("Starting Benchmark...\n");
  printf("1. Run on subset of random words\n");
  printf("2. Run on ALL words (Very slow!)\n");
  printf("Choice: ");
  int choice = 1;
  if (scanf("%d", &choice) != 1)
    choice = 1;

  int num_tests = (choice == 2) ? word_count : 100;

  // Arrays to store words to test
  char **test_words = malloc(num_tests * sizeof(char *));

  if (choice == 2) {
    for (int i = 0; i < word_count; i++)
      test_words[i] = word_list[i];
  } else {
    printf("Selecting %d random words...\n", num_tests);
    for (int i = 0; i < num_tests; i++) {
      test_words[i] = get_random_word(word_list, word_count);
    }
  }

  // Stats
  long long simple_guesses = 0;
  int simple_fails = 0;
  int simple_dist[MAX_GUESSES + 2] = {0}; // Index 1-6, 7 for fail

  long long minimax_guesses = 0;
  int minimax_fails = 0;
  int minimax_dist[MAX_GUESSES + 2] = {0};

  printf("\nRunning Simple Solver...\n[");
  for (int i = 0; i < num_tests; i++) {
    if (i % (num_tests / 10 + 1) == 0)
      printf("#");
    fflush(stdout);
    int g = solve_game_simple(test_words[i], word_list, word_count, true);
    if (g > MAX_GUESSES) {
      simple_fails++;
      simple_dist[MAX_GUESSES + 1]++;
    } else {
      simple_guesses += g;
      simple_dist[g]++;
    }
  }
  printf("] Done.\n");

  printf("\nRunning Minimax Solver...\n[");
  for (int i = 0; i < num_tests; i++) {
    if (i % (num_tests / 10 + 1) == 0)
      printf("#");
    fflush(stdout);
    int g = solve_game_minimax(test_words[i], word_list, word_count, true);
    if (g > MAX_GUESSES) {
      minimax_fails++;
      minimax_dist[MAX_GUESSES + 1]++;
    } else {
      minimax_guesses += g;
      minimax_dist[g]++;
    }
  }
  printf("] Done.\n\n");

  // Report
  printf("--- Benchmark Report ---\n");
  printf("Words Tested: %d\n\n", num_tests);

  printf("Strategy | Avg Guesses | Failures | Distribution (1..6+)\n");
  printf("---------|-------------|----------|----------------------\n");

  double simple_avg = (simple_fails == num_tests)
                          ? 0
                          : (double)simple_guesses / (num_tests - simple_fails);
  printf("Simple   | %11.2f | %8d | ", simple_avg, simple_fails);
  for (int i = 1; i <= MAX_GUESSES; i++)
    printf("%d ", simple_dist[i]);
  printf("(%d)\n", simple_dist[MAX_GUESSES + 1]);

  double minimax_avg =
      (minimax_fails == num_tests)
          ? 0
          : (double)minimax_guesses / (num_tests - minimax_fails);
  printf("Minimax  | %11.2f | %8d | ", minimax_avg, minimax_fails);
  for (int i = 1; i <= MAX_GUESSES; i++)
    printf("%d ", minimax_dist[i]);
  printf("(%d)\n", minimax_dist[MAX_GUESSES + 1]);

  if (choice != 2)
    free(test_words);
}

int main() {
  srand(time(NULL)); // Seed the random number generator

  char **word_list;
  int word_count;

  // Load the dictionary
  if (!load_word_list("words.txt", &word_list, &word_count)) {
    fprintf(stderr, "Failed to load words.txt\n");
    return 1;
  }
  printf("Loaded %d words.\n", word_count);

  // Menu
  printf("1. Play Manual\n");
  printf("2. Run Solver\n");
  printf("3. Benchmark Solvers\n");
  printf("Choice: ");
  int choice;
  if (scanf("%d", &choice) != 1)
    return 1;

  if (choice == 1) {
    play_manual(word_list, word_count);
  } else if (choice == 2) {
    run_solver_mode(word_list, word_count);
  } else if (choice == 3) {
    run_benchmark(word_list, word_count);
  } else {
    printf("Invalid choice.\n");
  }

  // Cleanup
  free_word_list(word_list, word_count);
  return 0;
}