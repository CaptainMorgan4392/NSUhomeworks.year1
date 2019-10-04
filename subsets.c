#include <stdio.h>
#include <mm_malloc.h>

void printCurrentSubset(char* array, int* arrayOfIndexes, int index, int n, int k);
void getNextChoose(char* array, int* arrayOfIndexes, int index, int n, int k);

void printCurrentSubset(char* array, int* arrayOfIndexes, int index, int n, int k) {
    printf("%c ", array[index]);
    for (int i = 0; i < k; i++) {
        if (arrayOfIndexes[i] >= index + 1) {
            arrayOfIndexes[i]++;
        }
    }
    for (int i = 0; i < k; i++) {
        printf("%c ", array[arrayOfIndexes[i] - 1]);
    }
    printf("\n");
    for (int i = 0; i < k; i++) {
        if (arrayOfIndexes[i] > index) {
            arrayOfIndexes[i]--;
        }
    }
    getNextChoose(array, arrayOfIndexes, index, n, k);
}

void getNextChoose(char* array, int* arrayOfIndexes, int index, int n, int k) {
    for (int i = k - 1; i >= 0; i--) {
        if (arrayOfIndexes[i] <= n - k + i) {
            arrayOfIndexes[i]++;
            for (int j = i + 1; j < k; j++) {
                arrayOfIndexes[j] = arrayOfIndexes[j - 1] + 1;
            }
            printCurrentSubset(array, arrayOfIndexes, index, n, k);
        } else if (i == 0) {
            return;
        }
    }
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    char useless;
    scanf("%c", &useless);
    char* sets = (char*)calloc((size_t)n, sizeof(char));
    for (int i = 0; i < n; i++) {
        scanf("%c", sets + i);
        if (sets[i] == ' ') {
            sets[i] = '\0';
            i--;
        }
    }
    char chosenSet;
    scanf("%c", &useless);
    scanf("%c", &chosenSet);
    int indexOfChosen = -1;
    for (int i = 0; i < n; i++) {
        if (sets[i] == chosenSet) {
            indexOfChosen = i;
            break;
        }
    }
    if (indexOfChosen == -1){
        printf("Slaviky ploho:(...");
        return 0;
    }

    int* arrayOfIndexesInChose = (int*)calloc((size_t)m, sizeof(int));
    for (int i = 0; i < m; i++) {
        arrayOfIndexesInChose[i] = i + 1;
    }
    printCurrentSubset(sets, arrayOfIndexesInChose, indexOfChosen, n - 1, m - 1);
    return 0;
}
