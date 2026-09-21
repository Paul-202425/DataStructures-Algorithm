README — Question 2: Laboratory Sample Queue (Doubly Linked List)
Files
lab_queue.c — main program
analysis_q2.md — complexity discussion (append + traversal)
What it does

Models the lab's sample processing queue as a doubly linked list, where each node stores a sample's ID, type, and priority (1 = urgent, 2 = normal, 3 = routine), plus prev/next pointers. The queue structure also keeps an explicit tail pointer and a current pointer (the technician's cursor).

On startup, the program asks how many initial samples to enter and reads each one (ID, type, priority) to build the initial queue. It then presents a menu:

Move to next sample — advances the cursor one step forward; reports "already at the last sample" if there's nowhere to go.
Move to previous sample — advances the cursor one step backward; reports "already at the first sample" if there's nowhere to go.
Display current sample — prints the sample at the cursor.
Add a new sample to the end of the queue — appends live, while the program is running.
Review queue FORWARD — walks from the current position to the end, printing every sample, and stops automatically at the end.
Review queue BACKWARD — walks from the current position to the start, printing every sample, and stops automatically at the start.
Exit — frees every node and terminates.
Edge cases handled
Empty queue: every operation reports a clear message instead of crashing or dereferencing a null pointer.
Single-sample queue: "next"/"previous" correctly report there is nowhere further to move.
Already at the first/last sample: reported explicitly rather than silently doing nothing.
Adding a sample while running: correctly relinks the tail pointer and, if the queue was empty, also initializes the cursor.
How to build and run
bash
gcc -Wall -Wextra -O2 -o lab_queue lab_queue.c
./lab_queue

The program is interactive: it will prompt for the number of initial samples, then their details, then present the numbered menu above.

Complexity summary (see analysis_q2.md for full justification)
Operation	Time complexity	Why
Add new sample to the end	O(1)	a tail pointer is maintained and updated on every append, so no traversal is needed
Traverse all n samples (either direction)	O(n)	every node must be visited exactly once
