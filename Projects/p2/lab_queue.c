#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELD 64

/* ---------- Node definition ------------------------------------ */
typedef struct Sample {
    char id[MAX_FIELD];        /* patient/sample identifier */
    char type[MAX_FIELD];      /* sample type                */
    int  priority;             /* 1 = urgent, 2 = normal, 3 = routine */
    struct Sample *prev;
    struct Sample *next;
} Sample;

/* ---------- Queue definition (head + tail + a "current" cursor) */
typedef struct {
    Sample *head;
    Sample *tail;      /* kept up to date -> O(1) append at the end */
    Sample *current;   /* the node the technician is currently viewing */
    int count;
} SampleQueue;

/* ---------- Basic helpers --------------------------------------- */
static void queue_init(SampleQueue *q) {
    q->head = q->tail = q->current = NULL;
    q->count = 0;
}

/* Creates a new node. Isolated so insertion logic can reuse it. */
static Sample *create_sample(const char *id, const char *type, int priority) {
    Sample *node = (Sample *) malloc(sizeof(Sample));
    if (!node) {
        fprintf(stderr, "Fatal: out of memory while creating a sample node.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(node->id, id, MAX_FIELD - 1);   node->id[MAX_FIELD - 1] = '\0';
    strncpy(node->type, type, MAX_FIELD - 1); node->type[MAX_FIELD - 1] = '\0';
    node->priority = priority;
    node->prev = node->next = NULL;
    return node;
}

/* Insert at the end of the queue. Because we keep a tail pointer,
 * this does NOT need to walk the list -> O(1). */
static void queue_append(SampleQueue *q, const char *id, const char *type, int priority) {
    Sample *node = create_sample(id, type, priority);

    if (q->head == NULL) {                 /* empty queue */
        q->head = q->tail = node;
        q->current = node;                 /* start cursor at the first sample */
    } else {
        node->prev = q->tail;
        q->tail->next = node;
        q->tail = node;
    }
    q->count++;
}

static void print_sample(const Sample *s) {
    if (!s) return;
    const char *priorityLabel =
        (s->priority == 1) ? "Urgent" : (s->priority == 2) ? "Normal" : "Routine";
    printf("  ID: %-10s | Type: %-15s | Priority: %d (%s)\n",
           s->id, s->type, s->priority, priorityLabel);
}

/* ---------- Menu operations -------------------------------------- */
static void op_display_current(const SampleQueue *q) {
    if (q->head == NULL) {
        printf("The queue is empty. Nothing to display.\n");
        return;
    }
    if (q->current == NULL) {
        printf("No sample is currently selected.\n");
        return;
    }
    printf("Current sample:\n");
    print_sample(q->current);
}

static void op_move_next(SampleQueue *q) {
    if (q->head == NULL) {
        printf("The queue is empty. Nothing to move to.\n");
        return;
    }
    if (q->current == NULL) q->current = q->head;
    if (q->current->next == NULL) {
        printf("Already at the last sample in the queue.\n");
        return;
    }
    q->current = q->current->next;
    printf("Moved to next sample:\n");
    print_sample(q->current);
}

static void op_move_prev(SampleQueue *q) {
    if (q->head == NULL) {
        printf("The queue is empty. Nothing to move to.\n");
        return;
    }
    if (q->current == NULL) q->current = q->tail;
    if (q->current->prev == NULL) {
        printf("Already at the first sample in the queue.\n");
        return;
    }
    q->current = q->current->prev;
    printf("Moved to previous sample:\n");
    print_sample(q->current);
}

/* Reviews from the current position to the end, printing every node,
 * and stops automatically when the end of the queue is reached. */
static void op_review_forward(SampleQueue *q) {
    if (q->head == NULL) {
        printf("The queue is empty. Nothing to review.\n");
        return;
    }
    Sample *cursor = (q->current != NULL) ? q->current : q->head;
    printf("Reviewing queue FORWARD from current position:\n");
    while (cursor != NULL) {
        print_sample(cursor);
        q->current = cursor;      /* leave technician positioned at the last node shown */
        cursor = cursor->next;
    }
    printf("End of queue reached.\n");
}

/* Reviews from the current position back to the start, printing every
 * node, and stops automatically when the beginning is reached. */
static void op_review_backward(SampleQueue *q) {
    if (q->head == NULL) {
        printf("The queue is empty. Nothing to review.\n");
        return;
    }
    Sample *cursor = (q->current != NULL) ? q->current : q->tail;
    printf("Reviewing queue BACKWARD from current position:\n");
    while (cursor != NULL) {
        print_sample(cursor);
        q->current = cursor;      /* leave technician positioned at the last node shown */
        cursor = cursor->prev;
    }
    printf("Beginning of queue reached.\n");
}

static void op_add_sample(SampleQueue *q) {
    char id[MAX_FIELD], type[MAX_FIELD];
    int priority;

    printf("Enter new sample ID: ");
    (void)scanf("%63s", id);
    printf("Enter sample type: ");
    (void)scanf("%63s", type);
    printf("Enter priority (1=Urgent, 2=Normal, 3=Routine): ");
    while (scanf("%d", &priority) != 1 || priority < 1 || priority > 3) {
        printf("Invalid priority. Enter 1, 2, or 3: ");
        while (getchar() != '\n');   /* clear bad input */
    }

    queue_append(q, id, type, priority);
    printf("Sample '%s' added to the end of the queue.\n", id);
}

/* Frees every node so the program terminates without memory leaks. */
static void queue_destroy(SampleQueue *q) {
    Sample *cursor = q->head;
    while (cursor != NULL) {
        Sample *toFree = cursor;
        cursor = cursor->next;
        free(toFree);
    }
    q->head = q->tail = q->current = NULL;
    q->count = 0;
}

/* ---------- Initial population from the user --------------------- */
static void load_initial_samples(SampleQueue *q) {
    int n = 0;
    printf("How many initial samples will you enter? ");
    while (scanf("%d", &n) != 1 || n < 0) {
        printf("Please enter a non-negative integer: ");
        while (getchar() != '\n');
    }

    for (int i = 0; i < n; i++) {
        char id[MAX_FIELD], type[MAX_FIELD];
        int priority;
        printf("\nSample %d:\n", i + 1);
        printf("  ID: ");
        (void)scanf("%63s", id);
        printf("  Type: ");
        (void)scanf("%63s", type);
        printf("  Priority (1=Urgent, 2=Normal, 3=Routine): ");
        while (scanf("%d", &priority) != 1 || priority < 1 || priority > 3) {
            printf("  Invalid priority. Enter 1, 2, or 3: ");
            while (getchar() != '\n');
        }
        queue_append(q, id, type, priority);
    }
}

/* ---------- Menu loop --------------------------------------------- */
static void print_menu(void) {
    printf("\n===== Laboratory Sample Queue Menu =====\n");
    printf("1. Move to next sample\n");
    printf("2. Move to previous sample\n");
    printf("3. Display current sample\n");
    printf("4. Add a new sample to the end of the queue\n");
    printf("5. Review queue FORWARD from current position to the end\n");
    printf("6. Review queue BACKWARD from current position to the start\n");
    printf("7. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    SampleQueue queue;
    queue_init(&queue);

    load_initial_samples(&queue);

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');   /* discard invalid input */
            choice = -1;
        }

        switch (choice) {
            case 1: op_move_next(&queue);       break;
            case 2: op_move_prev(&queue);       break;
            case 3: op_display_current(&queue); break;
            case 4: op_add_sample(&queue);      break;
            case 5: op_review_forward(&queue);  break;
            case 6: op_review_backward(&queue); break;
            case 7: printf("Exiting. Releasing all allocated memory...\n"); break;
            default: printf("Invalid option. Please choose 1-7.\n");
        }
    } while (choice != 7);

    queue_destroy(&queue);
    printf("All samples freed. Goodbye.\n");
    return 0;
}
