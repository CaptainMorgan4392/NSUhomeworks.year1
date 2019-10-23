#include <mm_malloc.h>

typedef struct Node {
    int value;
    struct Node* left;
    struct Node* right;
} Node;

void calculateMaxHeight(Node* root, int currentHeight, int *maxHeight) {
    if (currentHeight > (*maxHeight)) {
        (*maxHeight) = currentHeight;
    }
    if (NULL != root -> left) {
        calculateMaxHeight(root -> left, currentHeight + 1, maxHeight);
    }
    if (NULL != root -> right) {
        calculateMaxHeight(root -> right, currentHeight + 1, maxHeight);
    }
}

int getWidthOfLevel(Node* root, int level) {
    if (NULL == root) {
        return 0;
    } else if (level == 1) {
        return 1;
    } else {
        return getWidthOfLevel(root -> left, level - 1) + getWidthOfLevel(root -> right, level - 1);
    }
}

void getAnElementFromCurrentLevel(Node* root, int level, FILE* fileOut) {
    if (NULL == root) {
        return;
    } else if (level == 0) {
        fprintf(fileOut, "%d ", root -> value);
    } else {
        getAnElementFromCurrentLevel(root -> left, level - 1, fileOut);
        getAnElementFromCurrentLevel(root -> right, level - 1, fileOut);
    }
}

int calculateMaxWidth(Node* root, int maxHeight) {
    int maxWidth = 0,
        width;
    for (int i = 1; i <= maxHeight; i++) {
        width = getWidthOfLevel(root, i);
        if (width > maxWidth) {
            maxWidth = width;
        }
    }
    return maxWidth;
}

void infixTraversal(Node *root, FILE *fileOut) {
    if (NULL != root -> left) {
        infixTraversal(root->left, fileOut);
    }
    fprintf(fileOut, "%d ", root -> value);
    if (NULL != root -> right) {
        infixTraversal(root -> right, fileOut);
    }
}

void prefixTraversal(Node *root, FILE *fileOut) {
    fprintf(fileOut, "%d ", root -> value);
    if (NULL != root -> left) {
        prefixTraversal(root -> left, fileOut);
    }
    if (NULL != root -> right) {
        prefixTraversal(root -> right, fileOut);
    }
}

void postfixTraversal(Node *root, FILE *fileOut) {
    if (NULL != root -> left) {
        postfixTraversal(root -> left, fileOut);
    }
    if (NULL != root -> right) {
        postfixTraversal(root -> right, fileOut);
    }
    fprintf(fileOut, "%d ", root -> value);
}

void widthTraversal(Node *root, int maxHeight, FILE *fileOut) {
    for (int i = 0; i <= maxHeight; i++) {
        getAnElementFromCurrentLevel(root, i, fileOut);
    }
}

int main() {
    FILE* fileIn = fopen("in.txt", "r");

    int currentValue = 0,
        size = 0;
    Node* root = calloc(1, sizeof(Node));

    while (fscanf(fileIn, "%d", &currentValue) && !feof(fileIn)) {
        if (size == 0) {
            root -> value = currentValue;
        } else {
            Node* current = root;
            while (1) {
                if (currentValue == current -> value) {
                    continue;
                } else if (currentValue < current -> value) {
                    if (NULL == current -> left) {
                        current -> left = calloc(1, sizeof(Node));
                        current -> left -> value = currentValue;
                        break;
                    } else {
                        current = current -> left;
                    }
                } else {
                    if (NULL == current -> right) {
                        current -> right = calloc(1, sizeof(Node));
                        current -> right -> value = currentValue;
                        break;
                    } else {
                        current = current -> right;
                    }
                }
            }
        }
        size++;
    }

    FILE* fileOut = fopen("out.txt", "w");

    int maxHeight = 0, maxWidth = 0;

    calculateMaxHeight(root, 0, &maxHeight);
    fprintf(fileOut, "%d\n", maxHeight);

    maxWidth = calculateMaxWidth(root, maxHeight);
    fprintf(fileOut, "%d\n", maxWidth);

    infixTraversal(root, fileOut);
    fprintf(fileOut, "\n");

    prefixTraversal(root, fileOut);
    fprintf(fileOut, "\n");

    postfixTraversal(root, fileOut);
    fprintf(fileOut, "\n");

    widthTraversal(root, maxHeight, fileOut);
    return 0;
}
