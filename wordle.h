#ifndef WORDLE_H
#define WORDLE_H

#include <stdbool.h>

#define WORD_LENGTH 5
#define MAX_GUESSES 6

typedef enum {
  COLOR_GRAY = 0,
  COLOR_YELLOW = 1,
  COLOR_GREEN = 2
} FeedbackColor;

typedef struct {
  char word[WORD_LENGTH + 1];
  FeedbackColor feedback[WORD_LENGTH];
} Guess;

typedef struct {
  char target_word[WORD_LENGTH + 1];
  Guess guesses[MAX_GUESSES];
  int guess_count;
  bool won;
  bool game_over;
} GameState;

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

#endif // WORDLE_H
