#include "wordle.h"
#include <string.h>

/**
 * Initializes the game state.
 * @param game Pointer to the GameState structure.
 * @param target The target word to be guessed.
 */
void init_game(GameState *game, const char *target) {
  strcpy(game->target_word, target);
  game->guess_count = 0;
  game->won = false;
  game->game_over = false;
}

/**
 * Calculates the feedback for a given guess against the target word.
 * @param target The target word.
 * @param guess The guessed word.
 * @param feedback Array to store the feedback colors.
 */
void get_feedback(const char *target, const char *guess,
                  FeedbackColor *feedback) {
  int target_counts[26] = {0};
  int guess_counts[26] = {0}; // Note: guess_counts is not strictly used but
                              // good for symmetry if needed later

  // Initialize feedback to GRAY (not in word)
  for (int i = 0; i < WORD_LENGTH; i++) {
    feedback[i] = COLOR_GRAY;
  }

  // First pass: Check for GREEN (correct letter, correct position)
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (guess[i] == target[i]) {
      feedback[i] = COLOR_GREEN;
    } else {
      // Count frequencies of letters in target that are NOT matched by GREEN
      target_counts[target[i] - 'A']++;
    }
  }

  // Second pass: Check for YELLOW (correct letter, wrong position)
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (feedback[i] == COLOR_GREEN)
      continue;

    // If the guessed letter exists in the remaining target letters
    if (target_counts[guess[i] - 'A'] > 0) {
      feedback[i] = COLOR_YELLOW;
      target_counts[guess[i] -
                    'A']--; // Decrement count to handle duplicates correctly
    }
  }
}

/**
 * Processes a guess made by the player.
 * @param game Pointer to the GameState structure.
 * @param guess_word The word guessed by the player.
 * @return true if the guess was processed, false if the game is over or max
 * guesses reached.
 */
bool process_guess(GameState *game, const char *guess_word) {
  if (game->game_over)
    return false;
  if (game->guess_count >= MAX_GUESSES)
    return false;

  // Store the guess
  strcpy(game->guesses[game->guess_count].word, guess_word);

  // Calculate feedback
  get_feedback(game->target_word, guess_word,
               game->guesses[game->guess_count].feedback);

  // Check win condition
  if (strcmp(guess_word, game->target_word) == 0) {
    game->won = true;
    game->game_over = true;
  } else if (game->guess_count + 1 >= MAX_GUESSES) {
    // Check loss condition (max guesses reached)
    game->game_over = true;
  }

  game->guess_count++;
  return true;
}
