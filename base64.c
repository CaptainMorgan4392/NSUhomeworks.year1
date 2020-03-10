#define us unsigned short
#define ll long long

#define boolean us
#define false 0
#define true 1

#include <stdio.h>
#include <mm_malloc.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    WRONG_OPTIONS,
    WRONG_SYMBOL
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "wrong options",
        "wrong symbols in input file"
};

const char* base64Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int hashTable[256];

typedef enum {
    ENCODE,
    DECODE,
    ENTERS,
    IGNORE
} Options;


int enters = -1;
FILE* fileIn;
FILE* fileOut;


void getHashTable() {
    for (int i = 0; i < 256; i++) {
        hashTable[i] = -1;
    }

    for (int i = 0; i < 64; i++) {
        hashTable[base64Alphabet[i]] = i;
    }

    hashTable['='] = 0;
}

ll power(ll number, int degree) {
    ll current = 1LL;
    for (int i = 1; i <= degree; i++) {
        current *= number;
    }

    return current;
}

ExitCodes getOptions(boolean* array, int argc, char* argv[]) {
    if (argc < 4) {
        return WRONG_OPTIONS;
    }

    for (int i = 1; i < argc; i++) {
        char firstSymbol = argv[i][0];
        if (firstSymbol == '-') {
            if (strlen(argv[i]) != 2 || i >= argc - 2) {
                return WRONG_OPTIONS;
            }

            char functionalSymbol = argv[i][1];
            switch (functionalSymbol) {
                case 'd':
                    if (array[DECODE]) {
                        return WRONG_OPTIONS;
                    }
                    array[DECODE] = true;
                    break;
                case 'e':
                    if (array[ENCODE]) {
                        return WRONG_OPTIONS;
                    }
                    array[ENCODE] = true;
                    break;
                case 'i':
                    if (array[IGNORE]) {
                        return WRONG_OPTIONS;
                    }
                    array[IGNORE] = true;
                    break;
                case 'f':
                    if (array[ENTERS] || i == argc - 1) {
                        return WRONG_OPTIONS;
                    }
                    i++;
                    for (size_t j = 0; j < strlen(argv[i]); j++) {
                        if (j == 0) {
                            enters = 0;
                        }
                        char curDigit = argv[i][j];
                        if (!isdigit(curDigit)) {
                            return WRONG_OPTIONS;
                        }
                        enters = 10 * enters + curDigit - '0';
                    }
                    if (enters < 0) {
                        return WRONG_OPTIONS;
                    }
                    array[ENTERS] = true;
                    break;
                default:
                    return WRONG_OPTIONS;
            }
        } else {
            if (i < argc - 2) {
                return WRONG_OPTIONS;
            }

            if (i == argc - 2) {
                 fileIn = fopen(argv[i], "r+");
                 if (!fileIn) {
                     return WRONG_OPTIONS;
                 }
            } else {
                fileOut = fopen(argv[i], "w+");
                if (!fileOut) {
                    return WRONG_OPTIONS;
                }
            }
        }
    }

    if (!(array[DECODE] ^ array[ENCODE])) {
        return WRONG_OPTIONS;
    }

    if (array[DECODE] && array[ENTERS]) {
        return WRONG_OPTIONS;
    }

    if (array[ENCODE] && array[IGNORE]) {
        return WRONG_OPTIONS;
    }

    return SUCCESS;
}

ExitCodes startDecoding(boolean ignore) {
    unsigned char curSymbol;
    if (!ignore) {
        while ((curSymbol = (unsigned char)fgetc(fileIn))) {
            if (feof(fileIn)) {
                break;
            }

            if (strchr(base64Alphabet, curSymbol) == NULL && curSymbol != '=') {
                return WRONG_SYMBOL;
            }
        }
    }

    fseek(fileIn, 0, SEEK_SET);

    unsigned char curBuffer[4];
    size_t currentRead = 0;

    getHashTable();

    while (((currentRead = fread(curBuffer, sizeof(unsigned char), 4, fileIn)) == 4)) {
        if (curBuffer[3] == '=') {
            break;
        }
        ll current = 0;
        for (int i = 0; i < 4; i++) {
            current *= 64;
            current += hashTable[curBuffer[i]];
        }

        for (int i = 2; i >= 0; i--) {
            fprintf(fileOut, "%c", (unsigned char)(current >> (8 * i)));

            if (i != 0) {
                current %= power(256, i);
            }
        }
    }

    if (currentRead > 0) {
        ll current = 0;
        for (int i = 0; i < 4; i++) {
            current *= 64;
            current += hashTable[curBuffer[i]];
        }

        for (int i = 2; i >= 0; i--) {
            if (current >> (8 * i) != 0) {
                fprintf(fileOut, "%c", (unsigned char)(current >> (8 * i)));
            } else {
                break;
            }


            if (i != 0) {
                current %= power(256, i);
            }
        }
    }
    
    return SUCCESS;
}

ExitCodes startEncoding(boolean isEnters) {
    unsigned char curBuffer[3];
    size_t currentRead = 0,
           currentIndex = 0;
    while ((currentRead = fread(curBuffer, sizeof(unsigned char), 3, fileIn)) == 3) {
        ll current = 0;
        for (int i = 0; i < 3; i++) {
            current *= 256;
            current += (ll)curBuffer[i];
        }

        for (int i = 3; i >= 0; i--) {
            fprintf(fileOut, "%c", base64Alphabet[current >> 6 * i]);
            currentIndex++;
            if (isEnters && (currentIndex % enters == 0)) {
                fprintf(fileOut, "\n");
            }

            if (i != 0) {
                current %= power(64, i);
            }
        }


        if (feof(fileIn)) {
            break;
        }
    }

    if (currentRead != 0) {
        ll finalCounting = 0;
        for (int i = 0; i < 3; i++) {
            finalCounting *= 256;

            if (i == currentRead) {
                continue;
            }

            finalCounting += (ll)curBuffer[i];
        }

        for (int i = 3; i >= 0; i--) {
            if ((currentRead == 1 && i >= 2) || (currentRead == 2 && i >= 1)) {
                fprintf(fileOut, "%c", base64Alphabet[finalCounting >> 6 * i]);
            } else {
                fprintf(fileOut, "=");
            }
            currentIndex++;
            if (isEnters && (currentIndex % enters == 0)) {
                fprintf(fileOut, "\n");
            }

            if (i != 0) {
                finalCounting %= power(64, i);
            }
        }
    }

    return SUCCESS;
}

ExitCodes mainMethod(int argc, char* argv[]) {
    boolean* options = (boolean*)calloc(4, sizeof(boolean));
    if (!options) {
        return OUT_OF_MEMORY;
    }
    for (int i = 0; i < 4; i++) {
        options[i] = false;
    }

    ExitCodes correctOptions;
    if ((correctOptions = getOptions(options, argc, argv)) != SUCCESS) {
        return correctOptions;
    }

    if (options[DECODE]) {
        ExitCodes decoding;
        if ((decoding = startDecoding(options[IGNORE])) != SUCCESS) {
            return decoding;
        }
    } else {
        ExitCodes encoding;
        if ((encoding = startEncoding(options[ENTERS])) != SUCCESS) {
            return encoding;
        }
    }

    return SUCCESS;
}

int main(int argc, char* argv[]) {
    ExitCodes exec;
    if ((exec = mainMethod(argc, argv)) != SUCCESS) {
        printf("%s", exitMessages[exec]);
    }
    return exec;
}
