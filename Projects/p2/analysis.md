# Question 2 — Doubly Linked List Sample Queue: Analysis

## 1. Design summary

`lab_queue.c` models the queue as a doubly linked list of `Sample`
nodes (`id`, `type`, `priority`, `prev`, `next`). The wrapper `SampleQueue`
keeps three pointers on the list: `head`, `tail`, and `current` (the
technician's cursor). Node allocation, insertion, traversal, and
deallocation are all encapsulated functions, as required.

## 2. Time complexity of adding a new sample to the end

The implementation has an explicit tail pointer (`q->tail`)
which is updated every time a new node is appended to the end of the
list (`queue_append`). This means that appending a new sample does
not require traversing the list to find the last node — it simply:

1. allocates the new node (O(1)),
2. inserts it after `q->tail` (O(1)),
3. updates `q->tail` to point to the new node (O(1)).

Time complexity: O(1), regardless of how many samples (n) are in the
queue at the time.

(If the list did not track a tail pointer, insertion to the end of
the list would require traversing the list from `head` to the last
node before inserting, which would take O(n) instead of O(1). Tracking
a tail pointer is what prevents this and is vital as the laboratory's
daily sample volume grows, since an O(n) insertion cost would increase
the total insertion cost up to O(n²) for n samples.)

## 3. Time complexity of traversing all n samples

Traversal in either direction (`next` or `prev`) entails visiting

each of the n nodes once and doing O(1) work for each (printing and
advancing the cursor). Doing this for all n samples requires:
Time complexity: O(n), in either direction.
This is the best complexity one can possibly achieve for a full
review of the contents of the queue, since all the n samples must be
inspected at least once.
