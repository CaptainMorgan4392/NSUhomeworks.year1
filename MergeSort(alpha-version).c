#define ll long long
#define LENGTH 10000000
 
#include <stdio.h>
#include <mm_malloc.h>
#include <memory.h>
#include <limits.h>
#include <stdbool.h>
 
FILE* names;
 
ll stringToInt(char* numberToConvert) {
    ll answer = 0;
 
    for (size_t i = 0; i < strlen(numberToConvert); i++) {
        answer = 10 * answer + numberToConvert[i] - '0';
    }
 
    return answer;
}
 
void merge(ll* array, size_t left, size_t mid, size_t right) {
    size_t  iterator1 = 0,
            iterator2 = 0;
    ll* forUpdate = (ll*)calloc(right - left, sizeof(ll));
 
 
    while (left + iterator1 < mid && mid + iterator2 < right) {
        if (array[left + iterator1] < array[mid + iterator2]) {
            forUpdate[iterator1 + iterator2] = array[left + iterator1];
            iterator1++;
        } else {
            forUpdate[iterator1 + iterator2] = array[mid + iterator2];
            iterator2++;
        }
    }
 
 
    while (left + iterator1 < mid) {
        forUpdate[iterator1 + iterator2] = array[left + iterator1];
        iterator1++;
    }
    while (mid + iterator2 < right) {
        forUpdate[iterator1 + iterator2] = array[mid + iterator2];
        iterator2++;
    }
 
 
    for (size_t i = 0; i < iterator1 + iterator2; i++) {
        array[left + i] = forUpdate[i];
    }
}
 
void mergeSort(ll* array, size_t left, size_t right) {
    if (left + 1 >= right) {
        return;
    }
    size_t mid = (left + right) / 2;
    mergeSort(array, left, mid);
    mergeSort(array, mid, right);
    merge(array, left, mid, right);
}
 
void mergeFiles(FILE* dst, FILE* src1, FILE* src2) {
    size_t currentFromFirst, currentFromSecond;
 
    fseek(src1, 0, SEEK_SET);
    fseek(src2, 0, SEEK_SET);
    fscanf(src1, "%zu", &currentFromFirst);
    fscanf(src2, "%zu", &currentFromSecond);
 
    while (!feof(src1) && !feof(src2)) {
        if (currentFromFirst < currentFromSecond) {
            fprintf(dst, "%zu ", currentFromFirst);
            fscanf(src1, "%zu", &currentFromFirst);
        } else {
            fprintf(dst, "%zu ", currentFromSecond);
            fscanf(src2, "%zu", &currentFromSecond);
        }
    }
 
    while (!feof(src1)) {
        fprintf(dst, "%zu ", currentFromFirst);
        fscanf(src1, "%zu", &currentFromFirst);
    }
 
    while (!feof(src2)) {
        fprintf(dst, "%zu ", currentFromSecond);
        fscanf(src2, "%zu", &currentFromSecond);
    }
}
 
void mergeFilesBySegmentTree(FILE **arrayOfFiles, const size_t* arrayOfQuantities, size_t quantity) {
    FILE** segmentTreeOfFiles = (FILE**)calloc(2 * quantity - 1, sizeof(FILE*));
    size_t* segmentTreeOfQuantities = (size_t*)calloc(2 * quantity - 1, sizeof(size_t));
 
    for (size_t i = 0; i < quantity; i++) {
        segmentTreeOfFiles[quantity - 1 + i] = arrayOfFiles[i];
        segmentTreeOfQuantities[quantity - 1 + i] = arrayOfQuantities[i];
    }
 
    for (size_t i = quantity - 2; i >= 0; i--) {
        char *currentFileName = (char *)calloc(1001, sizeof(char));
        fscanf(names, "%1000s", currentFileName);
        segmentTreeOfFiles[i] = fopen(currentFileName, "w+");
        segmentTreeOfQuantities[i] = segmentTreeOfQuantities[2 * i + 1] + segmentTreeOfQuantities[2 * i + 2];
        if (i == 0) break;
    }
 
    for (size_t i = 0; i < 2 * quantity - 1; i++) {
        printf("%zu ", segmentTreeOfQuantities[i]);
    }
 
    for (size_t i = quantity - 2; i >= 0; i--) {
        mergeFiles(segmentTreeOfFiles[i], segmentTreeOfFiles[2 * i + 1], segmentTreeOfFiles[2 * i + 2]);
        if (i == 0) break;
    }
}
 
size_t findTheClosestDegree(size_t number) {
    size_t currentDegree = 1;
    while (currentDegree < number) {
        currentDegree *= 2;
    }
    return currentDegree;
}
 
void mergeSortForLargeArray(ll quantity, FILE* fileIn, FILE* fileOut) {
    size_t quantityOfFiles = findTheClosestDegree((size_t)(quantity / LENGTH));
    FILE **arrayOfFiles = (FILE **) calloc(quantityOfFiles, sizeof(FILE *));
    for (size_t i = 0; i < quantityOfFiles; i++) {
        char *currentFileName = (char *)calloc(1001, sizeof(char));
        fscanf(names, "%1000s", currentFileName);
        arrayOfFiles[i] = fopen(currentFileName, "w+");
    }
 
    ll *array = (ll *)calloc(LENGTH, sizeof(ll));
    for (ll i = 0; i < quantity; i++) {
        fscanf(fileIn, "%lli", &array[i % LENGTH]);
        if (i % LENGTH == LENGTH - 1) {
            mergeSort(array, 0, LENGTH);
            for (size_t j = 0; j < LENGTH; j++) {
                fprintf(arrayOfFiles[(i - 1) / LENGTH], "%lli ", array[j]);
            }
        }
    }
 
    if (quantity % LENGTH != 0) {
        mergeSort(array, 0, (size_t) (quantity % LENGTH));
        for (size_t i = 0; i < (size_t) (quantity % LENGTH); i++) {
            fprintf(arrayOfFiles[quantity / LENGTH], "%lli ", array[i]);
        }
    }
 
    size_t* arrayOfQuantities = (size_t*)calloc(quantityOfFiles, sizeof(size_t));
    for (size_t i = 0; i < quantityOfFiles; i++) {
        if (i < quantity / LENGTH) {
            arrayOfQuantities[i] = LENGTH;
        } else if (i == quantity / LENGTH) {
            arrayOfQuantities[i] = (size_t)(quantity / LENGTH);
        } else {
            arrayOfQuantities[i] = 0;
        }
    }
 
    mergeFilesBySegmentTree(arrayOfFiles, arrayOfQuantities, quantityOfFiles);
}
 
void genArray(FILE* fileOut, int argc, char* argv[]) {
    if (argc < 4) {
        printf("Bad args of cmd");
        exit(1);
    }
 
    ll quantity = stringToInt(argv[2]);
 
    fileOut = fopen(argv[3], "w");
 
    for (ll i = 0; i < quantity; i++) {
        fprintf(fileOut, "%lli ", i + 1);
    }
}
 
void checkArray(FILE* fileIn, int argc, char* argv[]) {
    if (argc < 3) {
        printf("Bad args of cmd");
        exit(1);
    }
 
    fileIn = fopen(argv[2], "r");
 
    ll  currentIndex = 0,
        previous = 0,
        element = 0;
 
    if (fscanf(fileIn, "%lli", &previous) == 0) {
        printf("Array has the length = 0\nSorted");
    }
 
    while (fscanf(fileIn, "%lli", &element) != 0) {
        if (previous > element) {
            printf("Not sorted. Index = %lli\n", currentIndex + 1);
            return;
        }
        previous = element;
        currentIndex++;
        if (feof(fileIn)) break;
    }
 
    printf("Sorted\n");
}
 
void sortArray(FILE* fileIn, FILE* fileOut, int argc, char* argv[]) {
    if (argc < 4) {
        printf("Bad args of cmd");
        exit(1);
    }
 
    fileIn = fopen(argv[2], "r");
 
    ll quantity;
    fscanf(fileIn, "%lli", &quantity);
    if (quantity <= LENGTH) {
        ll* array = (ll*)calloc((size_t)quantity, sizeof(ll));
        for (ll i = 0; i < quantity; i++) {
            ll current;
            if (fscanf(fileIn, "%lli", &current) == 0) {
                fprintf(fileOut, "Check input file for correct data.");
                exit(1);
            } else {
                array[i] = current;
            }
        }
        mergeSort(array, 0, (size_t)quantity);
 
        fileOut = fopen(argv[3], "w");
 
        for (size_t i = 0; i < quantity; i++) {
            fprintf(fileOut, "%lli ", array[i]);
        }
    } else {
        mergeSortForLargeArray(quantity, fileIn, fileOut);
    }
}
 
int main(int argc, char* argv[]) {
    FILE* fileIn;
    FILE* fileOut;
 
    names = fopen("fileNames.txt", "r");
 
    short toDo = 0;
    if (strcmp(argv[1], "-g") == 0) {
        toDo = 1;
    } else if (strcmp(argv[1], "-c") == 0) {
        toDo = 2;
    } else if (strcmp(argv[1], "-s") == 0) {
        toDo = 3;
    }
    switch (toDo) {
        case 0:
            printf("Bad args of cmd");
            return 0;
        case 1:
            genArray(fileOut, argc, argv);
            break;
        case 2:
            checkArray(fileIn, argc, argv);
            break;
        case 3:
            sortArray(fileIn, fileOut, argc, argv);
            break;
    }
    return 0;
}
