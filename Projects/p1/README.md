README — Question 1: Order Priority Sorting (Quick Sort)
Files
order_priority.c — main program
orders.txt — sample input data (used for testing; replace with the actual daily export as needed)
priority_orders.txt — generated output (created when the program runs)
analysis.md — full complexity derivation and Quick Sort vs. Insertion Sort discussion
What it does
Reads orders from orders.txt (format: OrderID CustomerName ProductCategory OrderValue, one order per line) into a dynamic array that grows automatically, so it handles an unknown number of orders up to n.
Sorts the orders in place with a hand-written Quick Sort (Lomuto partitioning), using the priority rule:
OrderValue — descending
CustomerName — ascending (tie-breaker)
OrderID — ascending (final tie-breaker)
Writes the sorted list to priority_orders.txt in the same format.
Prints the number of orders processed and their total value.

No qsort() or other library sort is used — the partitioning, recursion, and comparisons are all implemented from scratch in compare_orders, partition, and quick_sort.

How to build and run
bash
gcc -Wall -Wextra -O2 -o order_priority order_priority.c
./order_priority

Make sure orders.txt is present in the same directory before running. The program will create/overwrite priority_orders.txt in that same directory.

Sample input/output

Input line format:

ORD1023 Alice Electronics 450000

Given the bundled orders.txt (which includes tied order values on purpose to exercise the tie-breaking rules), the program correctly resolves ties by customer name and then by OrderID — see analysis.md for the full sample run.

Complexity summary (see analysis.md for full derivation)
Case	Time complexity
Best	O(n log n)
Average	O(n log n)
Worst	O(n²)

Auxiliary space: O(log n) average / O(n) worst case (recursion stack).

Why Quick Sort over Insertion Sort here: as the daily order volume grows, Insertion Sort's O(n²) average/worst-case behavior scales far worse than Quick Sort's O(n log n) average case, and daily order data has no natural "already sorted" structure to exploit — see analysis.md section 4 for the full discussion.
