# Question 4 — Pharmacy Inventory BST: Analysis

## 1. Time complexity of searching for a medicine

`bst_search` proceeds from the root, at each node comparing the target
code to the current node's code (`strcmp`), and proceeding either to the
left or right — thus, only one branch of the tree is traversed at each
level, and comparisons made per level equal the number of levels descended.

- Best case: the medicine is stored at (or near) the root, or the
tree is otherwise perfectly balanced and the target is found
quickly. So, O(1) for the root itself; a balanced tree will
generally have a best achievable depth of O(log n).
- Average case: for a BST built from randomly ordered insertions,
the average height is O(log n), so an average search visits
O(log n) nodes.
- Worst case: if the medicine codes are inserted in already-sorted
order, which is a real risk here, because `M1001, M1002, M1003, ...`
are already in sorted order in the sample file — every node has only
a right (or left) child, and a BST built by such insertions becomes
a linked list of height n. Then, a search costs O(n).

## 2. How the shape of the BST affects search performance

Search cost is tied to the height of the tree, not the number of
nodes n by itself:

- A balanced BST has height O(log n), for a O(log n) search.
- A skewed/degenerate BST (e.g., built by inserting already-sorted
data, which the sample `medicine.txt` file does for codes M1001…M1040
in ascending order) has height O(n), and a O(n) search — no better
than a standard linked list scan.

That's why for data that can arrive in sorted order (as
inventory export files often do), a self-balancing BST (e.g., AVL or
red-black tree) would guarantee O(log n) search for any input, while
a plain BST only achieves that on average, for randomly
ordered input.

## 3. Binary Tree vs. Binary Search Tree

- A Binary Tree is simply a tree where every node has at most two
children (usually referred to as left and right). There is no ordering
rule in terms of a node's value relative to its children — the
tournament tree in Question 3 is an example: children are placed by
position (level order), not by comparing their values.
- A Binary Search Tree (BST) is a binary tree with an additional
ordering invariant — for every node, all values in its left
subtree are less than the node's value, and all values in its right
subtree are greater. This ordering is what lets searching by repeatedly discarding half the remaining candidates (in the
balanced case), not having to examine every node.

## 4. Linear search on the file vs. BST search in memory

- Linear search directly on the inventory file: every search
requires reading the file back in from the beginning and comparing
each record's code one by one until a match (or end of file) is found.
This costs O(n) per search, and the I/O overhead of repeatedly
reading from disk.
- BST search after loading into memory: the file is read once
to build the tree (an O(n) cost for n insertions in the balanced
case, since each insertion is O(log n): n × O(log n) = O(n log n)
overall to build; up to O(n²) in the worst case of a degenerate
tree built from sorted input). After that one-time cost, each
individual search costs only O(log n) on a reasonably balanced
tree, with no further file I/O.

When the BST approach becomes worthwhile: the BST pays the upfront
cost to construct (O(n log n) typically) that linear search never
does, but it earns that cost back as soon as more than one or two
searches are performed against the same loaded inventory — because
each subsequent BST search (O(log n)) is much cheaper than each linear
scan (O(n)). For a pharmacy inventory being searched repeatedly
throughout a shift without changing, the BST is clearly the better
approach — the fixed construction cost is paid once, while the savings from O(log n) instead of O(n) per search accumulate with every lookup that's performed afterward. If the inventory is only ever searched a single time before being discarded, the two approaches are closer in total cost, and the extra bookkeeping of building a tree may not be justified.
