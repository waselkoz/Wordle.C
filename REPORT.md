# ALGO3 Mini Project Report: Wordle Solver

**Author:** Wassim selama
**Course:** Algorithms & Data Structures in C  
**Date:** 01/12/2025

---

## Part 3: Analysis, Justification, and Code Documentation

### a. Strategy Description

**Word Selection Strategy**
The solver uses a **Consistency-Based Filtering Strategy**.
1.  **Initial State**: The solver considers all words in the dictionary as possible candidates.
2.  **Guessing**:
    *   For the first guess, we use a heuristic starting word (e.g., "CRANE" or "SLATE") which is known to eliminate a large number of possibilities on average due to its distribution of common vowels and consonants.
    *   For subsequent guesses, the solver picks the **first available word** from the remaining list of possible candidates. This is a "Hard Mode" compatible strategy, meaning every guess must be a valid solution to the puzzle so far.
3.  **Feedback Processing**:
    *   After making a guess, the game provides feedback (Green/Yellow/Gray).
    *   The solver iterates through the list of currently possible words.
    *   For each candidate, it simulates: "If this candidate were the target, would it produce the *exact same feedback* for the guess I just made?"
    *   If **Yes**: The candidate remains possible.
    *   If **No**: The candidate is eliminated.

**Why is this effective?**
This approach guarantees that the search space strictly decreases (or stays same in worst case, but practically decreases) with every guess. By filtering out inconsistent words, we rapidly converge on the single correct answer. While not theoretically optimal (Minimax or Entropy-based approaches are better but more complex), this strategy is highly efficient for the scope of 5-letter words, typically solving within 3-5 guesses.

### b. Data Structure Justification

**1. Array of Strings (`char **word_list`)**
*   **Usage**: Stores the entire dictionary of valid 5-letter words.
*   **Justification**: A simple array is sufficient because the dictionary size is fixed and relatively small (a few thousand words). It allows for O(1) access by index, which is useful for random selection.

**2. Boolean Array (`bool *possible`)**
*   **Usage**: Tracks which words in `word_list` are still valid candidates. `possible[i] == true` means `word_list[i]` is still a candidate.
*   **Justification**:
    *   **Space Efficiency**: Requires only 1 byte per word.
    *   **Time Efficiency**: Allows O(1) checking if a word is valid.
    *   **Alternative Considered**: A Linked List of possible words.
    *   **Why Rejected**: Linked lists have higher memory overhead (pointers) and worse cache locality. Iterating over a boolean array is very fast on modern CPUs.

**3. Structs (`GameState`, `Guess`)**
*   **Usage**: Encapsulates the game state, including history of guesses and feedback.
*   **Justification**: Grouping related data makes the code cleaner and easier to pass between functions.

### c. Complexity Analysis

Let $N$ be the number of words in the dictionary (e.g., $N \approx 2500$).
Let $L$ be the word length ($L=5$).

**Time Complexity**
*   **Filtering Step**: For each guess, we iterate through all $N$ words. For each word, we call `is_consistent`, which takes $O(L)$ time.
    *   Complexity per guess: $O(N \times L)$.
    *   Since $L$ is constant (5), this is effectively **$O(N)$**.
*   **Next Guess Selection**: We iterate through the `possible` array to find the next true value.
    *   Complexity: **$O(N)$**.
*   **Total Complexity**: If the game takes $G$ guesses (where $G \le 6$), the total complexity is $O(G \times N)$. Since $G$ is small, the solver runs in linear time relative to the dictionary size.

**Space Complexity**
*   **Word List**: $O(N \times L)$ characters.
*   **Possible Array**: $O(N)$ booleans.
*   **Total Space**: **$O(N \times L)$**.
*   For 3000 words, this is roughly $3000 \times 6$ bytes $\approx 18$ KB, which is negligible.

### d. Code Documentation

**Module: `solver.c`**

*   `bool is_consistent(const char *candidate, const char *guess, const FeedbackColor *feedback)`
    *   **Purpose**: Core logic of the solver. Determines if a `candidate` word could be the secret target, given the `feedback` received for a specific `guess`.
    *   **Logic**: It calls `get_feedback(candidate, guess)` to simulate the game. If the simulated feedback matches the actual feedback, the word is consistent.

*   `void solve_game(const char *target, char **word_list, int word_count)`
    *   **Purpose**: Orchestrates the solving process.
    *   **Logic**: Loops up to 6 times. Picks a guess, gets feedback, filters the `possible` list, and repeats until the list size is 1 or the word is found.

**Module: `game.c`**

*   `void get_feedback(const char *target, const char *guess, FeedbackColor *feedback)`
    *   **Purpose**: The "Rule Engine" of Wordle.
    *   **Logic**:
        1.  **Green Pass**: Check `guess[i] == target[i]`.
        2.  **Yellow Pass**: Check if `guess[i]` exists elsewhere in `target`, respecting character counts (e.g., if target has one 'E' and guess has two, only one gets Yellow).

---

## Example Code Snippet

```c
// From solver.c
// Filters the list of possible words based on the feedback received.
for (int i = 0; i < word_count; i++) {
    if (possible[i]) {
        // If the word 'word_list[i]' would NOT produce the same feedback
        // for the given guess, it cannot be the target.
        if (is_consistent(word_list[i], current_guess, feedback)) {
            new_count++;
        } else {
            possible[i] = false; // Eliminate this word
        }
    }
}
```
