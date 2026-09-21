README — Question 4: Pharmacy Inventory BST
Files
pharmacy_bst.c — main program
medicine.txt — sample inventory data (as given in the assignment)
analysis_q4.md — search complexity, BST vs. Binary Tree, BST vs. linear-search-on-file discussion
What it does

Loads a pipe-delimited medicine inventory file (MedicineCode|MedicineName|Quantity|UnitPrice, one record per line) into a Binary Search Tree keyed on MedicineCode, then offers a menu-driven search interface without needing to rebuild the tree between searches.

During loading:

Malformed lines (missing fields) are skipped with a warning.
Invalid quantities/prices (non-numeric or negative) are skipped with a warning.
Duplicate medicine codes update the existing node's quantity instead of creating a second node, exactly as required.
Empty files are handled gracefully (reports zero records loaded rather than crashing).

Menu:

Search for a medicine by code — reports the full record if found, or a clear "not found" message otherwise. Can be repeated as many times as needed without rebuilding the tree.
Display full inventory — an in-order traversal, which naturally prints every medicine in ascending code order.
Exit — frees every node and terminates.
How to build and run
bash
gcc -Wall -Wextra -O2 -o pharmacy_bst pharmacy_bst.c
./pharmacy_bst

Make sure medicine.txt is present in the same directory before running.

Complexity summary (see analysis_q4.md for full justification)
Case	Search time complexity	Why
Best	O(1)–O(log n)	target near the root / tree is balanced
Average (random insertion order)	O(log n)	expected tree height is O(log n)
Worst (sorted/near-sorted insertion order — as in the sample file, M1001…M1040)	O(n)	tree degenerates into a linked-list shape

Binary Tree vs. BST: a Binary Tree only constrains each node to at most two children; a BST additionally requires the left/right subtree ordering invariant that makes searching by comparison possible.

BST vs. linear search on the file: linear search costs O(n) per search with repeated file I/O; the BST pays an upfront O(n log n) (average case) construction cost once, after which each subsequent search costs only O(log n) — this pays off as soon as more than a couple of searches are performed against the same loaded inventory.
