#ifndef WORDLE_H
#define WORDLE_H

#include <stdbool.h>

// Game constants
#define WORD_LENGTH 5 // Length of the word to guess
#define MAX_GUESSES 6 // Maximum number of attempts allowed

// Enum for feedback colors representing the state of a letter in a guess
typedef enum {
  COLOR_GRAY = 0,   // Letter is not in the word
  COLOR_YELLOW = 1, // Letter is in the word but in the wrong position
  COLOR_GREEN = 2   // Letter is in the word and in the correct position
} FeedbackColor;

// Structure to represent a single guess
typedef struct {
  char word[WORD_LENGTH + 1];          // The guessed word (null-terminated)
  FeedbackColor feedback[WORD_LENGTH]; // Feedback for each letter in the guess
} Guess;

// Structure to represent the state of the game
typedef struct {
  char target_word[WORD_LENGTH + 1]; // The word to be guessed
  Guess guesses[MAX_GUESSES];        // Array of guesses made so far
  int guess_count;                   // Number of guesses made
  bool won;                          // True if the player has won
  bool game_over; // True if the game is over (won or max guesses reached)
} GameState;

// Function prototypes

// Utils
void to_upper_string(char *str);
bool load_word_list(const char *filename, char ***words, int *count);
void free_word_list(char **words, int count);
bool is_valid_word(const char *word, char **word_list, int word_count);
char *get_random_word(char **word_list, int word_count);

// Game Logic
void init_game(GameState *game, const char *target);
bool process_guess(GameState *game, const char *guess_word);
void get_feedback(const char *target, const char *guess,
                  FeedbackColor *feedback);

// Solver
void solve_game(const char *target, char **word_list, int word_count);

#endif