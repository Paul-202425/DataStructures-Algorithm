#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE 32
#define MAX_NAME 128

/* ---------- BST node ------------------------------------------- */
typedef struct MedNode {
    char code[MAX_CODE];
    char name[MAX_NAME];
    int  quantity;
    int  unitPrice;
    struct MedNode *left;
    struct MedNode *right;
} MedNode;

static MedNode *create_node(const char *code, const char *name, int qty, int price) {
    MedNode *n = (MedNode *) malloc(sizeof(MedNode));
    if (!n) {
        fprintf(stderr, "Fatal: out of memory while creating a BST node.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(n->code, code, MAX_CODE - 1); n->code[MAX_CODE - 1] = '\0';
    strncpy(n->name, name, MAX_NAME - 1); n->name[MAX_NAME - 1] = '\0';
    n->quantity = qty;
    n->unitPrice = price;
    n->left = n->right = NULL;
    return n;
}

/* Inserts a record, or if the code already exists, UPDATES the
 * existing node's quantity (per the assignment's duplicate rule),
 * instead of creating a second node. Returns the (possibly new) root. */
static MedNode *bst_insert(MedNode *root, const char *code, const char *name,
                            int qty, int price, int *wasDuplicate) {
    if (root == NULL) {
        return create_node(code, name, qty, price);
    }

    int cmp = strcmp(code, root->code);
    if (cmp == 0) {
        root->quantity = qty;          /* update existing record's quantity */
        if (wasDuplicate) *wasDuplicate = 1;
    } else if (cmp < 0) {
        root->left = bst_insert(root->left, code, name, qty, price, wasDuplicate);
    } else {
        root->right = bst_insert(root->right, code, name, qty, price, wasDuplicate);
    }
    return root;
}

/* Standard BST search by code. */
static MedNode *bst_search(MedNode *root, const char *code) {
    if (root == NULL) return NULL;
    int cmp = strcmp(code, root->code);
    if (cmp == 0) return root;
    return (cmp < 0) ? bst_search(root->left, code) : bst_search(root->right, code);
}

static void print_record(const MedNode *n) {
    printf("  %-8s | %-32s | Qty: %-5d | Unit Price: %d RWF\n",
           n->code, n->name, n->quantity, n->unitPrice);
}

static void inorder_display(const MedNode *root, int *count) {
    if (root == NULL) return;
    inorder_display(root->left, count);
    print_record(root);
    (*count)++;
    inorder_display(root->right, count);
}

/* ---------- File loading with validation ------------------------
 * Expected line format:  MedicineCode|MedicineName|Quantity|UnitPrice
 * Malformed lines, and lines with a non-positive quantity or price,
 * are skipped with a warning rather than crashing the program. */
static MedNode *load_inventory(const char *filename, int *loaded, int *skipped) {
    MedNode *root = NULL;
    *loaded = 0;
    *skipped = 0;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: could not open '%s'.\n", filename);
        return NULL;
    }

    char line[256];
    int lineNo = 0;
    while (fgets(line, sizeof(line), fp)) {
        lineNo++;
        line[strcspn(line, "\r\n")] = '\0';   /* strip trailing newline */
        if (line[0] == '\0') continue;         /* skip blank lines */

        char code[MAX_CODE], name[MAX_NAME];
        int qty, price;

        char *field1 = strtok(line, "|");
        char *field2 = strtok(NULL, "|");
        char *field3 = strtok(NULL, "|");
        char *field4 = strtok(NULL, "|");

        if (!field1 || !field2 || !field3 || !field4) {
            fprintf(stderr, "Warning: line %d is malformed and was skipped.\n", lineNo);
            (*skipped)++;
            continue;
        }

        strncpy(code, field1, MAX_CODE - 1); code[MAX_CODE - 1] = '\0';
        strncpy(name, field2, MAX_NAME - 1); name[MAX_NAME - 1] = '\0';

        char *endQty, *endPrice;
        qty = (int) strtol(field3, &endQty, 10);
        price = (int) strtol(field4, &endPrice, 10);

        if (*endQty != '\0' || *endPrice != '\0' || qty < 0 || price < 0) {
            fprintf(stderr, "Warning: line %d has an invalid quantity/price and was skipped.\n", lineNo);
            (*skipped)++;
            continue;
        }

        int wasDuplicate = 0;
        root = bst_insert(root, code, name, qty, price, &wasDuplicate);
        if (wasDuplicate)
            printf("Note: duplicate code '%s' on line %d -> quantity updated to %d.\n", code, lineNo, qty);
        (*loaded)++;
    }

    fclose(fp);
    return root;
}

/* ---------- Cleanup ------------------------------------------------ */
static void free_tree(MedNode *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ---------- Menu ---------------------------------------------------- */
static void print_menu(void) {
    printf("\n===== Pharmacy Inventory (BST) Menu =====\n");
    printf("1. Search for a medicine by code\n");
    printf("2. Display full inventory (ascending code order)\n");
    printf("3. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    const char *filename = "medicine.txt";

    int loaded = 0, skipped = 0;
    MedNode *root = load_inventory(filename, &loaded, &skipped);

    if (loaded == 0) {
        printf("No valid medicine records were loaded from '%s'. ", filename);
        printf("(%d line(s) skipped.) Exiting.\n", skipped);
        return EXIT_SUCCESS;
    }

    printf("Inventory loaded: %d record(s) into the BST", loaded);
    if (skipped > 0) printf(" (%d invalid line(s) skipped)", skipped);
    printf(".\n");

    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: {
                char code[MAX_CODE];
                printf("Enter medicine code to search: ");
                (void)scanf("%31s", code);
                MedNode *found = bst_search(root, code);
                if (found) {
                    printf("Medicine found:\n");
                    print_record(found);
                } else {
                    printf("No medicine with code '%s' was found.\n", code);
                }
                break;
            }
            case 2: {
                printf("Full inventory (ascending medicine code):\n");
                int count = 0;
                inorder_display(root, &count);
                printf("Total records displayed: %d\n", count);
                break;
            }
            case 3:
                printf("Exiting. Releasing all allocated memory...\n");
                break;
            default:
                printf("Invalid option. Please choose 1-3.\n");
        }
    } while (choice != 3);

    free_tree(root);
    printf("Tree freed. Goodbye.\n");
    return 0;
}
