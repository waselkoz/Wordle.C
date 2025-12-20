# ALGO3 Mini Project Report: Wordle Solver

**Author:** Wassim selama/ Khelifa Mohamed Bachir/ Ouarezki Hichem
**Course:** Algorithms & Data Structures in C  
**Date:** 01/12/2025

---

## Part 3: Analysis, Justification, and Code Documentation

### a. Strategy Description

**1. Simple Strategy: Consistency Filtering**
The "Simple Solver" uses a basic but effective greedy approach:
*   **Logic**: Always pick the **first word** in the dictionary that is consistent with all previous feedback.
*   **Search Space Reduction**: By nature of the game, every guess eliminates inconsistent words. We strictly guess from the remaining pool (Hard Mode).
*   **Weakness**: It does not look ahead. It can fall into "traps" (e.g., _OUND -> BOUND, FOUND, HOUND, MOUND, ROUND, SOUND...). It might guess BOUND, then FOUND, then HOUND, wasting guesses locally without distinguishing the group globally.

**2. Advanced Strategy: Minimax (Information Maximization)**
To handle "traps" and ambiguity, we implemented a **Minimax Solver**.
*   **Concept (Entropy)**: In Information Theory, the best guess is the one that reduces the uncertainty (or entropy) the most.
*   **Worst-Case Minimization**:
    *   For every possible guess $G$ (even those not in the remaining pool, though our implementation sticks to hard mode for simplicity/performance in $N^2$ checks), we partition the remaining words $S$ into groups based on the feedback they would return.
    *   We calculate the **size of the largest group** (worst-case scenario).
    *   We choose the guess that **minimizes this worst-case size**.
*   **Effect**: This ensures that even if we are unlucky (the target is in the largest group), the remaining pool is as small as possible. It avoids 1-by-1 elimination in favor of cutting the space in half (or better) every time.
*   **Tie-Breaking**: If multiple words offer the same worst-case reduction, we pick a word that is a **possible solution**. This gives us a chance to win immediately while still playing optimally for information.

### b. Benchmark Results & Statistics

We conducted a benchmark on **100 random words** to compare the performance.

| Metric | Simple Solver | Minimax Solver |
| :--- | :--- | :--- |
| **Average Guesses** | **3.82** | **3.51** |
| **Failures (X/6)** | 0% | 0% |
| **Guess Distribution** | Variable (Spikes at 3, 4, 5) | tighter (Spikes at 3, 4) |

**Detailed Distribution (Sample Run)**:
*   **Simple**: 1s: 1 | 2s: 6 | 3s: 32 | 4s: 39 | 5s: 19 | 6s: 3 | Fail: 0
*   **Minimax**: 1s: 1 | 2s: 6 | 3s: 43 | 4s: 41 | 5s: 9 | 6s: 0 | Fail: 0

**Analysis**:
*   The **Minimax solver** successfully shifts the distribution to the left. It solves 84% of puzzles in 3 or 4 guesses, whereas the Simple solver often spills into 5 or 6 guesses.
*   The **3.51** average is very competitive (human average is roughly 4).
*   The Minimax strategy eliminates the "long tail" of 6-guess games, making it much safer.

### c. Complexity Analysis

Let $N$ be the number of words in the dictionary ($N \approx 2800$). $L=5$.

**1. Simple Solver**:
*   **Time Complexity**: $O(N)$ per guess. We scan the list once.
    *   Total: $O(G \times N) \approx O(N)$.
*   **Space Complexity**: $O(N)$ for the boolean array.

**2. Minimax Solver**:
*   **Time Complexity**: $O(N^2)$ per guess.
    *   We iterate through every candidate guess ($N$).
    *   Inside, we simulate feedback against every possible target ($N$).
    *   Total operations $\approx N^2 \approx 2800^2 \approx 7.8 \times 10^6$ ops per turn. This is computationally expensive but manageable for $N < 10,000$.
*   **Space Complexity**: $O(N)$ for bookkeeping arrays.

### d. Data Structure Justification

**1. Array of Strings (`char **word_list`)**
*   **Usage**: Stores dictionary.
*   **Justification**: Linear memory layout, cache-friendly, O(1) random access.

**2. Boolean Array (`bool *possible`)**
*   **Usage**: Efficiently marks eliminated words.
*   **Justification**: 1 byte/word is minimal overhead compared to linked list pointers (8-16 bytes).

### e. Code Documentation

**Module: `solver.c`**
*   `solve_game_minimax(...)`: Implements the $O(N^2)$ logic.
    *   `get_worst_case_split(...)`: Helper function that builds a frequency array of feedbacks (base-3 encoded: $3^5 = 243$ slots) to find the max group size.

**Module: `game.c`**
*   `get_feedback(...)`: Stateless function that computes the color pattern between any two words. Used by both the real game loop and the solver's internal simulation.

