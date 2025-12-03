#include "wordle.h"
#include <string.h>


void init_game(GameState *game, const char *target) {
  strcpy(game->target_word, target);
  game->guess_count = 0;
  game->won = false;
  game->game_over = false;
}

// Generate feedback for a guess
void get_feedback(const char *target, const char *guess,
                  FeedbackColor *feedback) {
  int target_counts[26] = {0};
  int guess_counts[26] = {0};

  // Initialize feedback to Gray
  for (int i = 0; i < WORD_LENGTH; i++) {
    feedback[i] = COLOR_GRAY;
  }

  // First pass: Green
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (guess[i] == target[i]) {
      feedback[i] = COLOR_GREEN;
    } else {
      target_counts[target[i] - 'A']++;
    }
  }

  // Second pass: Yellow
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (feedback[i] == COLOR_GREEN)
      continue;

    if (target_counts[guess[i] - 'A'] > 0) {
      feedback[i] = COLOR_YELLOW;
      target_counts[guess[i] - 'A']--;
    }
  }
}

bool process_guess(GameState *game, const char *guess_word) {
  if (game->game_over)
    return false;
  if (game->guess_count >= MAX_GUESSES)
    return false;

  strcpy(game->guesses[game->guess_count].word, guess_word);
  get_feedback(game->target_word, guess_word,
               game->guesses[game->guess_count].feedback);

  if (strcmp(guess_word, game->target_word) == 0) {
    game->won = true;
    game->game_over = true;
  } else if (game->guess_count + 1 >= MAX_GUESSES) {
    game->game_over = true;
  }

  game->guess_count++;
  return true;
}
