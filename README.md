# Wordle Game & Solver in C

**Author:** Wassim selama
**Course:** Algorithms & Data Structures in C
**Date:** 01/12/2025

## Overview
This project implements a clone of the popular game **Wordle** in C, along with an automated **Solver** that can guess the word efficiently.

The project consists of two main parts:
1.  **The Game**: A CLI-based Wordle game where you can play manually.
2.  **The Solver**: An algorithm that plays the game automatically and attempts to find the word in the minimum number of guesses.

## Features
-   **Dictionary Loading**: Reads valid 5-letter words from `words.txt`.
-   **Feedback System**: Correctly handles Green (correct pos), Yellow (wrong pos), and Gray (not in word) logic, including duplicate letters.
-   **Solver Strategy**: Uses a consistency-based filtering approach to narrow down the search space after every guess.
-   **Cross-Platform**: Includes Windows API support for ANSI colors in the terminal.

## How to Build and Run

### Prerequisites
-   GCC Compiler (MinGW on Windows, or standard GCC on Linux/Mac).

### Compilation
Open a terminal in the project directory and run:

```bash
gcc main.c game.c utils.c solver.c -o wordle.exe
```

### Usage

**1. Play Manually**
Run the executable and select option `1`.
```bash
.\wordle.exe
```
Type your 5-letter guesses.

**2. Run Solver**
Run the executable and select option `2`.
You can enter a specific target word (e.g., `APPLE`) or type `RANDOM` to let the game pick one.
```bash
.\wordle.exe
# Select 2, then type RANDOM
```

## Project Structure
-   `main.c`: Entry point, menu, and UI loop.
-   `game.c`: Core game logic (state management, feedback generation).
-   `solver.c`: Solver algorithm (filtering, guess selection).
-   `utils.c`: Helper functions (file I/O, string manipulation).
-   `wordle.h`: Shared header file with definitions and structs.
-   `words.txt`: Dictionary of valid 5-letter words.

## Example Output
```text
Target: ARISE
Solver started...
Guess 1: CRANE
Feedback: - G Y - G
Remaining possibilities: 1
Guess 2: ARISE
Feedback: G G G G G
Solver won in 2 guesses!
```
