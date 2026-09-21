# Question 1 — Sorting and Complexity Analysis

## 1. Program overview

`order_priority.c` reads orders from `orders.txt` into a dynamically
growing array (`OrderList`, capacity doubles when full, so handling an unknown
number of orders up to n is possible without a fixed-size limit), sorts them in place with a hand-written Quick Sort, writes the
result to `priority_orders.txt`, and prints the count and total value.

The multi-level ordering rule is implemented in one comparison
function, `compare_orders`:

1. `OrderValue` — descending
2. `CustomerName` — ascending (`strcmp`)
3. `OrderID` — ascending (`strcmp`), used only when the first two are tied

Quick Sort uses the standard Lomuto partition scheme with the last
element as pivot, and only this custom comparator is called — nowhere
does `qsort()` or other library sort appear.

## 2. Running-time derivation for Quick Sort

Quick Sort on an array of size n does O(n) work in `partition` (one
pass through the sub-array) and then recurses on the two resulting
partitions of sizes k and n − k − 1. This gives the general
recurrence:

T(n) = T(k) + T(n − k − 1) + Θ(n)

The value of k (how balanced the split is) depends on the pivot's
rank in the current sub-array, which is what separates the three
cases.

### Best case — pivot always splits the array evenly (k ≈ n/2)

T(n) = 2T(n/2) + Θ(n)

By the Master Theorem (a = 2, b = 2, f(n) = Θ(n) = Θ(n^log_b(a))):

T(n) = Θ(n log n) → O(n log n)

### Average case — pivot rank is uniformly random

Averaging over all possible pivot ranks k = 0 … n−1:

T(n) = Θ(n) + (1/n) · Σ_{k=0}^{n-1} [T(k) + T(n−k−1)]

Solving this recurrence (standard result, e.g. by the substitution
method or by bounding it against the balanced-split recurrence) gives:

T(n) = O(n log n), same order as the best case, only with a
larger constant factor — most splits are "good enough" even though
they are not exactly 50/50.

### Worst case — pivot is always the smallest or largest element
(e.g. the input is already sorted, or reverse-sorted, when using a
fixed last-element pivot, as in this implementation)

Each partition removes only one element from further consideration
(k = 0 or k = n − 1):

T(n) = T(n − 1) + Θ(n)

Expanding:

T(n) = Θ(n) + Θ(n−1) + Θ(n−2) + … + Θ(1) = Θ(n(n+1)/2)

T(n) = O(n²)

### Summary

| Case | Recurrence | Closed form | Big-O |
|---|---|---|---|
| Best | T(n) = 2T(n/2) + Θ(n) | Θ(n log n) | O(n log n) |
| Average | T(n) = Θ(n) + (1/n) Σ [T(k) + T(n−k−1)] | Θ(n log n) | O(n log n) |
| Worst | T(n) = T(n−1) + Θ(n) | Θ(n²) | O(n²) |

## 3. Auxiliary space complexity

The partitioning is done in place (only a constant number
of temporary variables per call, via `swap_orders`), so the extra
memory Quick Sort uses is the recursion call stack:

- Best/average case (balanced recursion tree of depth ~log₂ n):
O(log n) auxiliary space.
- Worst case (degenerate recursion, depth ~n, e.g. already-sorted
input with a fixed pivot choice): O(n) auxiliary space.

(This can be reduced to a guaranteed O(log n) worst-case stack depth by
always recursing first into the smaller partition and looping/tail-
recursing on the larger one, but the base algorithm as required here
is O(log n) average / O(n) worst case.)

The order data is stored once in a dynamic array — O(n) space
for the data, independent of the sort's own extra stack usage.

## 4. Why Quick Sort over Insertion Sort for this problem

Dataset size and expected running time. Insertion Sort runs in
O(n²) in the average and worst case — every new order is compared
against, and potentially shifted past, a large fraction of the
already-processed orders. Quick Sort's O(n log n) average case grows
much more slowly: for n = 1,000 orders, n² = 1,000,000 comparisons
versus roughly n log₂n ≈ 10,000 for Quick Sort; at n = 100,000 the gap
widens to about 10 billion versus ~1.7 million. As the business grows
and daily order counts move from hundreds into the thousands or tens
of thousands, this difference decides whether the warehouse's nightly
batch job finishes in a fraction of a second or becomes a serious
bottleneck.

Scalability. Because Insertion Sort's cost grows quadratically,
doubling the number of daily orders roughly quadruples its running
time, whereas Quick Sort's cost only slightly more than doubles
(n log n scales almost linearly). This matters directly for the
stated business context: "the number of daily orders is expected to
increase substantially," so the algorithm needs to scale sub-
quadratically to remain practical without constantly re-engineering
the pipeline.

Best/average/worst-case behavior. Insertion Sort's one advantage
is that it is efficient — close to O(n) — on data that is already
nearly sorted, and has low overhead for very small n. Freshly
exported daily orders, however, arrive in essentially arbitrary order
(whatever sequence customers placed them in), so Insertion Sort would
almost always hit its O(n²) average/worst case in practice. Quick
Sort's worst case (O(n²)) is also a real risk if the pivot is chosen
poorly on already-sorted or adversarial input, but this is mitigated
in practice by pivot strategies (random pivot, median-of-three), and
even the plain implementation's average behavior — the case that
matters for typical, arbitrarily-ordered daily order data — is
O(n log n), which Insertion Sort never achieves once n is large.

File-based input. The orders start out in a flat text file, so the
whole batch must be loaded into memory before any processing decision
can be made — no "streaming" or "mostly sorted" structure to exploit,
no benefit from Insertion Sort's efficient handling of incremental single-item insertions into an already-sorted list. Once
the full order set is in memory as an array, an array-based, cache-
friendly, in-place algorithm like Quick Sort is a natural fit; it also
avoids the extra data-movement cost that Insertion Sort incurs from
shifting large blocks of elements one position at a time as the array
grows.

Conclusion. For a batch job whose input size (n) is expected to
grow substantially and whose data has no natural pre-sorted structure,
Quick Sort's O(n log n) average-case performance and better
scalability make it the more appropriate choice over Insertion Sort's
O(n²) behavior, despite Quick Sort's theoretical O(n²) worst case,
which is both rare in practice and controllable via pivot-selection
strategies.
