README — Question 3: E-Sports Tournament Binary Tree
Files
tournament_tree.c — main program
analysis_q3.md — construction rule, insertion complexity discussion
What it does

Builds a binary tournament tree from the assignment's fixed array of participant identifiers (69 values, counted automatically via sizeof), using level-order (complete binary tree) placement: each new participant is attached as the left, then right, child of the earliest node that still has a free slot — exactly how a tournament bracket skeleton is normally built from a flat entrant list.

A FIFO queue of "nodes that may still accept a child" is kept alive for the lifetime of the program (not just during the initial build), so both the original construction and any later additions of new participants use the same efficient placement rule.

Menu:

Display root participant/match identifier
Display all leaf participants (nodes with no children)
Enter a participant ID → view its parent, sibling, and grandchildren in one go (each explicitly reported as "none" when they don't exist)
Insert a new participant — adds one more node using the same level-order placement rule, live while the program runs
Exit — frees every node and terminates
Special cases handled
The root correctly reports it has no parent and no sibling.
A leaf correctly reports it has no children (and no grandchildren).
A node whose sibling slot is empty correctly reports "no sibling."
A node whose children have no children of their own correctly reports "no grandchildren."
An identifier that doesn't exist in the tree is clearly reported rather than crashing.
How to build and run
bash
gcc -Wall -Wextra -O2 -o tournament_tree tournament_tree.c
./tournament_tree

The participant array is embedded directly in the source (as given in the assignment), so no input file is needed — just run the executable.

Complexity summary (see analysis_q3.md for full justification)
Insertion strategy	Time complexity for one new node (tree has n nodes)
With the persistent "open slots" queue (used in this program)	O(1) amortised
Without any auxiliary structure (re-scanning the tree each time to find the next open slot)	O(n)

Because the tree is always kept complete by construction, its height stays O(log n) regardless — unlike a BST, the tree's shape doesn't degrade with input order; only the bookkeeping strategy used to find the next open slot changes the insertion cost.
