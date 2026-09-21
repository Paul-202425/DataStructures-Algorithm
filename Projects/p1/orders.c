#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELD 64          /* max characters for each text field   */
#define INITIAL_CAPACITY 16   /* starting size of the dynamic array   */

/* ---------- Data structure for a single order --------------- */
typedef struct {
    char orderID[MAX_FIELD];
    char customerName[MAX_FIELD];
    char productCategory[MAX_FIELD];
    long long orderValue;
} Order;

/* ---------- Dynamic array (grows as needed, handles "up to n") */
typedef struct {
    Order *data;
    size_t size;
    size_t capacity;
} OrderList;

static void list_init(OrderList *list) {
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
    list->data = (Order *) malloc(list->capacity * sizeof(Order));
    if (!list->data) {
        fprintf(stderr, "Fatal: out of memory while allocating order list.\n");
        exit(EXIT_FAILURE);
    }
}

static void list_push(OrderList *list, Order o) {
    if (list->size == list->capacity) {
        list->capacity *= 2;                 /* amortised O(1) growth */
        Order *tmp = (Order *) realloc(list->data, list->capacity * sizeof(Order));
        if (!tmp) {
            fprintf(stderr, "Fatal: out of memory while growing order list.\n");
            free(list->data);
            exit(EXIT_FAILURE);
        }
        list->data = tmp;
    }
    list->data[list->size++] = o;
}

static void list_free(OrderList *list) {
    free(list->data);
    list->data = NULL;
    list->size = list->capacity = 0;
}

/* ---------- Reading orders.txt --------------------------------
 * Expected format per line (whitespace separated), e.g.:
 *   ORD1023 Alice Electronics 450000
 * ------------------------------------------------------------- */
static int read_orders(const char *filename, OrderList *list) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open '%s' for reading.\n", filename);
        return -1;
    }

    Order o;
    /* %63s reads a single whitespace-delimited token safely (no overflow) */
    while (fscanf(fp, "%63s %63s %63s %lld",
                   o.orderID, o.customerName, o.productCategory, &o.orderValue) == 4) {
        list_push(list, o);
    }

    fclose(fp);
    return 0;
}

/* ---------- Writing priority_orders.txt ------------------------ */
static int write_orders(const char *filename, const OrderList *list) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: could not open '%s' for writing.\n", filename);
        return -1;
    }

    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "%s %s %s %lld\n",
                list->data[i].orderID,
                list->data[i].customerName,
                list->data[i].productCategory,
                list->data[i].orderValue);
    }

    fclose(fp);
    return 0;
}

/* ---------- Multi-level comparison rule -------------------------
 * Returns:  negative  if a should come BEFORE b
 *           positive  if a should come AFTER  b
 *           0         if they are equal under every rule
 *
 * Priority: OrderValue desc -> CustomerName asc -> OrderID asc
 * ------------------------------------------------------------- */
static int compare_orders(const Order *a, const Order *b) {
    if (a->orderValue != b->orderValue)
        return (a->orderValue > b->orderValue) ? -1 : 1;   /* descending value */

    int nameCmp = strcmp(a->customerName, b->customerName); /* ascending name */
    if (nameCmp != 0)
        return nameCmp;

    return strcmp(a->orderID, b->orderID);                  /* ascending ID   */
}

/* ---------- Quick Sort (from scratch, in-place) ------------------ */
static void swap_orders(Order *a, Order *b) {
    Order tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Lomuto partition scheme using the last element as pivot */
static int partition(Order arr[], int low, int high) {
    Order pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compare_orders(&arr[j], &pivot) <= 0) {
            i++;
            swap_orders(&arr[i], &arr[j]);
        }
    }
    swap_orders(&arr[i + 1], &arr[high]);
    return i + 1;
}

static void quick_sort(Order arr[], int low, int high) {
    if (low < high) {
        int pivotIndex = partition(arr, low, high);
        quick_sort(arr, low, pivotIndex - 1);
        quick_sort(arr, pivotIndex + 1, high);
    }
}

/* ---------- Main ------------------------------------------------- */
int main(void) {
    const char *inputFile  = "orders.txt";
    const char *outputFile = "priority_orders.txt";

    OrderList orders;
    list_init(&orders);

    if (read_orders(inputFile, &orders) != 0) {
        list_free(&orders);
        return EXIT_FAILURE;
    }

    if (orders.size == 0) {
        printf("No orders found in '%s'.\n", inputFile);
        list_free(&orders);
        return EXIT_SUCCESS;
    }

    /* Sort using our own Quick Sort implementation */
    quick_sort(orders.data, 0, (int) orders.size - 1);

    if (write_orders(outputFile, &orders) != 0) {
        list_free(&orders);
        return EXIT_FAILURE;
    }

    /* Summary statistics */
    long long totalValue = 0;
    for (size_t i = 0; i < orders.size; i++)
        totalValue += orders.data[i].orderValue;

    printf("Orders processed : %zu\n", orders.size);
    printf("Total order value: %lld RWF\n", totalValue);
    printf("Sorted results written to '%s'\n", outputFile);

    list_free(&orders);
    return EXIT_SUCCESS;
}
