#include <stdio.h>
#include <stdlib.h>

/* ---------- Tree node -------------------------------------------- */
typedef struct TNode {
    int id;
    struct TNode *parent;
    struct TNode *left;
    struct TNode *right;
} TNode;

static TNode *create_node(int id, TNode *parent) {
    TNode *n = (TNode *) malloc(sizeof(TNode));
    if (!n) {
        fprintf(stderr, "Fatal: out of memory while creating a tree node.\n");
        exit(EXIT_FAILURE);
    }
    n->id = id;
    n->parent = parent;
    n->left = n->right = NULL;
    return n;
}

/* ---------- A tiny FIFO queue of TNode* --------------------------
 * Used to remember, in order, which nodes still have room for a
 * child. This is what lets every insertion (initial build AND any
 * later addition) run in O(1) amortised time instead of re-searching
 * the whole tree for the next open slot. */
typedef struct QNode {
    TNode *tree_node;
    struct QNode *next;
} QNode;

typedef struct {
    QNode *front;
    QNode *rear;
} Queue;

static void queue_init(Queue *q) { q->front = q->rear = NULL; }

static void enqueue(Queue *q, TNode *t) {
    QNode *qn = (QNode *) malloc(sizeof(QNode));
    if (!qn) { fprintf(stderr, "Fatal: out of memory (queue).\n"); exit(EXIT_FAILURE); }
    qn->tree_node = t;
    qn->next = NULL;
    if (q->rear == NULL) q->front = q->rear = qn;
    else { q->rear->next = qn; q->rear = qn; }
}

static TNode *queue_front(Queue *q) { return q->front ? q->front->tree_node : NULL; }

static void dequeue(Queue *q) {
    if (q->front == NULL) return;
    QNode *toFree = q->front;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(toFree);
}

static void queue_destroy(Queue *q) {
    while (q->front != NULL) dequeue(q);
}

/* ---------- Tournament tree wrapper -------------------------------- */
typedef struct {
    TNode *root;
    Queue openSlots;   /* nodes that can still accept a left and/or right child */
    int size;
} Tournament;

static void tournament_init(Tournament *t) {
    t->root = NULL;
    queue_init(&t->openSlots);
    t->size = 0;
}

/* Places one new participant into the tree using the level-order
 * ("first available slot") rule.
 *
 *   - If the tree is empty, the participant becomes the root.
 *   - Otherwise, look at the node at the front of openSlots:
 *       * if it has no left child, attach here as the left child
 *       * else attach as the right child, and this node is now
 *         full, so remove it from openSlots
 *   - The newly created node is always pushed onto openSlots,
 *     since it starts with no children of its own.
 *
 * Every step above does O(1) work, so inserting one new node is
 * O(1) AMORTISED regardless of how many nodes (n) already exist. */
static void tournament_insert(Tournament *t, int id) {
    TNode *node;

    if (t->root == NULL) {
        node = create_node(id, NULL);
        t->root = node;
    } else {
        TNode *parent = queue_front(&t->openSlots);
        node = create_node(id, parent);
        if (parent->left == NULL) {
            parent->left = node;
        } else {
            parent->right = node;
            dequeue(&t->openSlots);   /* parent is now full */
        }
    }
    enqueue(&t->openSlots, node);
    t->size++;
}

static void tournament_build(Tournament *t, const int *ids, int n) {
    for (int i = 0; i < n; i++)
        tournament_insert(t, ids[i]);
}

/* ---------- Search: find the node holding a given id -------------- */
static TNode *find_node(TNode *root, int id) {
    if (root == NULL) return NULL;
    if (root->id == id) return root;
    TNode *found = find_node(root->left, id);
    if (found) return found;
    return find_node(root->right, id);
}

/* ---------- Display helpers ---------------------------------------- */
static void display_root(const Tournament *t) {
    if (t->root == NULL) { printf("The tournament tree is empty.\n"); return; }
    printf("Root participant/match identifier: %d\n", t->root->id);
}

static void display_leaves(TNode *node, int *anyFound) {
    if (node == NULL) return;
    if (node->left == NULL && node->right == NULL) {
        printf("  %d\n", node->id);
        *anyFound = 1;
        return;
    }
    display_leaves(node->left, anyFound);
    display_leaves(node->right, anyFound);
}

static void show_all_leaves(const Tournament *t) {
    if (t->root == NULL) { printf("The tournament tree is empty.\n"); return; }
    printf("Leaf participants (no further matches beneath them):\n");
    int any = 0;
    display_leaves(t->root, &any);
    if (!any) printf("  (none)\n");
}

static void show_node_relationships(const Tournament *t, int id) {
    TNode *node = find_node(t->root, id);
    if (node == NULL) {
        printf("No participant/match with identifier %d exists in the tournament tree.\n", id);
        return;
    }

    printf("--- Details for %d ---\n", id);

    /* Parent */
    if (node->parent == NULL)
        printf("Parent: none (this is the root of the tournament).\n");
    else
        printf("Parent: %d\n", node->parent->id);

    /* Sibling */
    if (node->parent == NULL) {
        printf("Sibling: none (the root has no sibling).\n");
    } else {
        TNode *sibling = (node->parent->left == node) ? node->parent->right
                                                        : node->parent->left;
        if (sibling == NULL)
            printf("Sibling: none.\n");
        else
            printf("Sibling: %d\n", sibling->id);
    }

    /* Grandchildren */
    printf("Grandchildren: ");
    int found = 0;
    TNode *candidates[4] = { NULL, NULL, NULL, NULL };
    if (node->left)  { candidates[0] = node->left->left;  candidates[1] = node->left->right; }
    if (node->right) { candidates[2] = node->right->left; candidates[3] = node->right->right; }
    for (int i = 0; i < 4; i++) {
        if (candidates[i] != NULL) {
            if (found) printf(", ");
            printf("%d", candidates[i]->id);
            found = 1;
        }
    }
    if (!found) printf("none");
    printf("\n");
}

/* ---------- Cleanup -------------------------------------------------- */
static void free_tree(TNode *node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

static void tournament_destroy(Tournament *t) {
    free_tree(t->root);
    queue_destroy(&t->openSlots);
    t->root = NULL;
    t->size = 0;
}

/* ---------- Menu ------------------------------------------------------ */
static void print_menu(void) {
    printf("\n===== Tournament Tree Menu =====\n");
    printf("1. Display root participant/match identifier\n");
    printf("2. Display all leaf participants\n");
    printf("3. Enter a participant ID -> view parent, sibling, grandchildren\n");
    printf("4. Insert a new participant (level-order placement)\n");
    printf("5. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    int ids[] = {
        42, 17, 68, 9, 23, 55, 81, 4, 13, 20, 31, 49, 61, 75, 90, 2, 7, 11, 15,
        19, 21, 27, 35, 45, 52, 58, 64, 72, 78, 85, 95, 1, 3, 5, 6, 8, 10, 12,
        14, 16, 18, 22, 24, 26, 29, 33, 37, 41, 44, 47, 50, 54, 57, 60, 63, 66,
        70, 74, 77, 80, 83, 87, 92, 97, 25, 28, 30, 34, 39
    };
    int n = (int) (sizeof(ids) / sizeof(ids[0]));

    Tournament tournament;
    tournament_init(&tournament);
    tournament_build(&tournament, ids, n);
    printf("Tournament tree constructed with %d participants.\n", tournament.size);

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:
                display_root(&tournament);
                break;
            case 2:
                show_all_leaves(&tournament);
                break;
            case 3: {
                int id;
                printf("Enter participant identifier: ");
                if (scanf("%d", &id) == 1)
                    show_node_relationships(&tournament, id);
                else
                    while (getchar() != '\n');
                break;
            }
            case 4: {
                int id;
                printf("Enter new participant identifier: ");
                if (scanf("%d", &id) == 1) {
                    tournament_insert(&tournament, id);
                    printf("Participant %d inserted. Tree now has %d nodes.\n", id, tournament.size);
                } else {
                    while (getchar() != '\n');
                }
                break;
            }
            case 5:
                printf("Exiting. Releasing all allocated memory...\n");
                break;
            default:
                printf("Invalid option. Please choose 1-5.\n");
        }
    } while (choice != 5);

    tournament_destroy(&tournament);
    printf("Tree freed. Goodbye.\n");
    return 0;
}
