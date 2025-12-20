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

Let $N$ be the total number of words in the dictionary ($N \approx 2315$ for solutions, but our list is $\approx 2800$). Let $L$ be the word length ($5$). Let $|S|$ be the size of the set of *remaining possible words* at step $k$.

**1. Simple Solver**:
*   **Time Complexity**: $O(|S| \cdot N)$ per guess.
    *   In the worst case, we scan the entire dictionary ($N$) to find the first word consistent with history. Checking consistency takes $O(L)$.
    *   Since we only check if words are consistent with the *current* feedback, it behaves like $O(N)$.
*   **Space Complexity**: $O(N)$ to maintain the `possible` boolean array.

**2. Minimax Solver (Entropy Maximizer)**:
*   **Time Complexity**: $O(N \cdot |S|)$ per guess.
    *   For **every candidate guess** ($N$), we simulate feedback against **every possible target** inside the remaining set $|S|$.
    *   Inside the inner loop, feedback calculation is $O(L)$.
    *   **Worst Case**: At the first guess, $|S| = N$. Thus, the first move is $O(N^2)$. With $N \approx 2800$, this is $\approx 7.8 \times 10^6$ operations, which takes a few seconds on standard hardware.
    *   **Amortized**: As $|S|$ shrinks rapidly (typically $< 100$ after guess 1), subsequent moves are extremely fast ($O(100N)$).
*   **Space Complexity**:
    *   $O(N)$ for the `possible` array.
    *   $O(3^L)$ for the frequency array (feedback buckets). Since $L=5$, $3^5 = 243$, which is constant space $O(1)$.

### d. Optimizations & Heuristics

To ensure the Minimax solver runs efficiently and logically, we implemented several "extra" features:

**1. Hardcoded First Guess ($O(1)$ Start)**
*   **Problem**: The first move is the most expensive ($O(N^2)$).
*   **Solution**: We pre-calculated the best opener. The solver checks if "RAISE" (or "ROATE"/"TRACE" depending on the version) is valid and uses it immediately.
*   **Benefit**: This skips the 3-5 second initial generic computation, making the solver feel instant.

**2. Base-3 Feedback Encoding**
*   **Logic**: Instead of using a hash map to store feedback counts (e.g., "Green-Gray-Yellow..."), we map each pattern to a unique integer where Gray=0, Yellow=1, Green=2.
*   **Formula**: $Index = \sum_{i=0}^{4} Color[i] \times 3^i$.
*   **Benefit**: Allows us to use a simple, fast static array `int counts[243]` instead of dynamic data structures, significantly speeding up the inner loop.

**3. "Soft-Win" Tie-Breaker**
*   **Scenario**: often, multiple words provide the exact same "worst-case entropy reduction."
*   **Heuristic**: If `WorstCase(Word A) == WorstCase(Word B)`, but `Word A` is a *possible answer* (in $|S|$) and `Word B` is not, we choose `Word A`.
*   **Impact**: This allows us to potentially win on the current turn by luck, without sacrificing information gain guarantees.

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

