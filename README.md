# DataStructures-Algorithm
Data Structures & Algorithms Assignment — Overview

This submission contains four independent C programs, one per question, each with its own algorithm analysis write-up.

#	Program	Data structure	Task
1	order_priority.c	Dynamic array	Sort e-commerce orders by priority using a from-scratch Quick Sort
2	lab_queue.c	Doubly linked list	Menu-driven lab sample processing queue
3	tournament_tree.c	Binary tree (level-order)	E-sports tournament bracket built from a fixed participant array
4	pharmacy_bst.c	Binary Search Tree	Pharmacy inventory lookup by medicine code
Files included
order_priority.c    orders.txt          analysis.md      (Question 1)
lab_queue.c                             analysis_q2.md   (Question 2)
tournament_tree.c                       analysis_q3.md   (Question 3)
pharmacy_bst.c       medicine.txt       analysis_q4.md   (Question 4)

Each analysis_*.md contains the detailed time/space complexity derivations and written discussion required by that question; see the per-question READMEs below for build/run instructions and a summary of the results.

General build instructions

All four programs are standard C (no external libraries beyond the C standard library) and compile with:

bash
gcc -Wall -Wextra -O2 -o <program_name> <program_name>.c

e.g. gcc -Wall -Wextra -O2 -o order_priority order_priority.c

Programs that read from a file (Q1 and Q4) expect their input file (orders.txt / medicine.txt) to be in the same working directory as the compiled executable. Programs with a menu (Q2, Q3, Q4) are interactive — run the executable and follow the on-screen prompts.

Common design notes across all four programs
No library sorting/searching shortcuts are used anywhere Quick Sort, BST search, or tree traversal was explicitly required to be implemented from scratch (no qsort(), etc.).
Dynamic memory (malloc/realloc/free) is used wherever the amount of data isn't known in advance (orders, samples, tree nodes, medicine records), and every program frees all allocated memory before exiting.
Modular structure: each program separates node/record creation, insertion, the core algorithm, display/traversal, and cleanup into distinct functions.
Edge cases (empty input, single element, duplicate values, malformed lines, non-existent search keys, already at first/last position, etc.) are explicitly checked and reported with a clear message rather than crashing.

See analysis.md, analysis_q2.md, analysis_q3.md, and analysis_q4.md for the full Big-O derivations and conceptual discussion for each question.
