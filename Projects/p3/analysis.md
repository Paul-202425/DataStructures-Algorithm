# Question 3 — Binary Tournament Tree: Analysis

## 1. Tree construction rule (documented)

We build the tree in level-order (complete binary tree) fashion:
the participants from the array are inserted one at a time,
always attaching the new participant as the left child (if free)
or otherwise the right child of the earliest-inserted node that
still has a free child slot. This is the standard approach for growing
a tournament bracket / complete binary tree from a flat list,
and it results in the tree being as balanced as possible (height
= ⌈log₂(n+1)⌉) at every stage, not just once at the end.

To make this efficient instead of searching the tree each time, we
keep a FIFO queue of "nodes that may still accept a child" alive
for the whole program (`Tournament.openSlots`), not just during
the initial build:

- The tree is initially empty, and the first participant becomes the root.
- For every following participant: check the node at the front of the queue.
If it has no left child, attach the new node there; otherwise attach
it as the right child and remove that now-full parent from the queue.
- The newly created node is always added to the back of the queue, since it has
no children to begin with.

(The array given in the assignment actually contains 69 identifiers
once counted programmatically — the program uses `sizeof(ids)/sizeof(ids[0])`
so it adapts automatically to however many values are supplied.)

## 2. Time complexity of inserting a new node when the tree has n nodes

This is where the choice of insertion strategy changes things, and there are
actually two cases worth distinguishing here:

(a) With the persistent "open slots" queue (this implementation):
each insertion only ever looks at the front of the queue and does a constant
number of pointer operations (attach child, possibly dequeue the now-full
parent, enqueue the new node).
Time complexity: O(1) amortised, independent of n. This holds no matter how
big the tree gets, because the queue always tells the algorithm exactly where
the next open slot is without any searching.

(b) Without any auxiliary structure (naively re-scanning the tree for the first
available slot on every insertion, e.g. via a fresh breadth-first search from
the root each time):
in the worst case (a tree whose last level is almost completely filled), the
search would have to traverse through a large fraction of the existing nodes
before finding an open slot.
Time complexity: O(n) per insertion in this case, since the BFS would touch
up to all n existing nodes to find a slot.

## 3. Why the shape/height doesn't change this story

Because we always maintain a complete binary tree during construction, the
tree's height is still O(log n) at every insertion step — this is
different to, say, a Binary Search Tree, where the height (and therefore search
or insertion cost) depends on the order values are inserted in and can degrade
to O(n) for a skewed tree. Here, the shape is always as balanced as it can be
by construction, so the only factor that varies the complexity of inserting
nodes is whether your algorithm remembers where the next open slot is (the
queue → O(1) amortised) or has to rediscover it from scratch every time (BFS/DFS
scan → O(n)). Just walking down from the root using "which child is missing" is
not sufficient in general in a linked-node (non-array) representation, since the
next open slot is not necessarily directly below the root — hence the need for
either the queue technique or a full scan.

(For completeness: if the tree was represented as a plain array with element i's
children at indices 2i+1 and 2i+2 — the classic heap-style representation — then
a new node could also be added in O(1) by simply writing to index n, because its
position is known purely from arithmetic on the count of existing nodes. That is
an alternative implementation strategy to the linked queue-based one used here,
with the same O(1) result.)
