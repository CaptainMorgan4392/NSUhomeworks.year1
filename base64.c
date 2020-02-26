#define boolean int
#define false 0
#define true 1

#include <stdio.h>
#include <mm_malloc.h>
#include <string.h>
#include <ctype.h>

// Exit codes
typedef enum {
    SUCCESS,
    OUT_OF_MEMORY,
    WRONG_INPUT,
    WRONG_OPTIONS,
    UNKNOWN_SYMBOL
} ExitCodes;

const char* exitMessages[] = {
        "success",
        "out of memory",
        "wrong input",
        "wrong options",
        "unknown symbol"
};

//Options
typedef enum {
    ENCODING,
    DECODING,
    IGNORE,
    ENTERS
} Options;

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

FILE* fileIn;
FILE* fileOut;

ExitCodes getOptions(int argc, char* argv[], boolean* options, int* enters) {
    for (int i = 1; i < argc; i++) {
        if ((argv[i][0] != '-' || strlen(argv[i]) != 2) && i < argc - 2) {
            return WRONG_INPUT;
        }

        if (i == argc - 2) {
            fileIn = fopen(argv[argc - 2], "r+");
            fileOut = fopen(argv[argc - 1], "w+");

            if (!fileIn || !fileOut) {
                return WRONG_INPUT;
            }

            break;
        }

        switch (argv[i][1]) {
            case 'd':
                if (options[DECODING]) {
                    return WRONG_INPUT;
                }
                options[DECODING] = true;
                break;
            case 'e':
                if (options[ENCODING]) {
                    return WRONG_INPUT;
                }
                options[ENCODING] = true;
                break;
            case 'f':
                if (options[ENTERS]) {
                    return WRONG_INPUT;
                }
                options[ENTERS] = true;
                i++;
                *enters = 0;
                for (int j = 0; j < strlen(argv[i]); j++) {
                    char currentSymbol = argv[i][j];
                    if (!isdigit(currentSymbol)) {
                        return WRONG_INPUT;
                    }
                    *enters = *enters * 10 + currentSymbol - '0';
                }
                break;
            case 'i':
                if (options[IGNORE]) {
                    return WRONG_INPUT;
                }
                options[IGNORE] = true;
                break;
            default:
                return WRONG_INPUT;
        }
    }

    return SUCCESS;
}

boolean checkOptions(const boolean* array) {
    if (!(array[DECODING] ^ array[ENCODING])) {
        return false;
    }
    if (array[ENCODING] && array[IGNORE]) {
        return false;
    }
    if (array[DECODING] && array[ENTERS]) {
        return false;
    }

    return true;
}

ExitCodes startEncoding(boolean enters, int charsInSingleLine) {
    size_t currentIndex = 0,
           currentB64Index = 0;
    unsigned char currentB64Symbol = 0,
                  currentRemainder = 0;
    unsigned char currentSymbol;


    while (!feof(fileIn) && (currentSymbol = (unsigned char)fgetc(fileIn)) != 255) {
        switch (currentIndex % 3) {
            case 0:
                currentB64Symbol = currentSymbol >> 2;
                currentRemainder = (unsigned char)(currentSymbol % 4);

                fprintf(fileOut, "%c", base64chars[currentB64Symbol]);
                currentB64Index++;
                break;
            case 1:
                currentB64Symbol = currentRemainder << 4;
                currentB64Symbol += currentSymbol >> 4;

                fprintf(fileOut, "%c", base64chars[currentB64Symbol]);
                currentB64Index++;

                currentRemainder = (unsigned char)(currentSymbol % 16);
                break;
            case 2:
                currentB64Symbol = currentRemainder << 2;
                currentRemainder = currentSymbol >> 6;
                currentB64Symbol += currentRemainder;
                currentRemainder = (unsigned char)(currentSymbol % 64);

                fprintf(fileOut, "%c", base64chars[currentB64Symbol]);
                currentB64Index++;
                if (currentB64Index % charsInSingleLine == 0) {
                    fprintf(fileOut, "\n");
                }

                fprintf(fileOut, "%c", base64chars[currentRemainder]);
                currentB64Index++;
                break;
            default:
                continue;
        }

        currentIndex++;
        if (currentB64Index % charsInSingleLine == 0) {
            fprintf(fileOut, "\n");
        }
    }

    if (currentIndex % 3 != 0) {
        int moveByRemainder = currentIndex % 3 == 1 ? 4 : 2;
        fprintf(fileOut, "%c", base64chars[currentRemainder << moveByRemainder]);
        if (currentB64Index % charsInSingleLine == 0) {
            fprintf(fileOut, "\n");
        }
        for (int i = 3 - ((int)currentIndex % 3); i > 0; i--) {
            fprintf(fileOut, "=");
        }
    }

    return SUCCESS;
}

boolean isValidChar(unsigned char c) {
    if ('0' <= c && c <= '9')
        return true;
    if ('a' <= c && c <= 'z')
        return true;
    if ('A' <= c && c <= 'Z')
        return true;
    if (c == '+' || c == '/' || c == '=')
        return true;
    return false;
}

void getHashTable(int* table) {
    memset(table, -1, 256);
    for (int i = 0; i < 64; i++) {
        table[base64chars[i]] = i;
    }
}

ExitCodes startDecoding(boolean ignore) {
    size_t currentB64Index = 0;
    unsigned char currentB64Symbol,
                  currentSymbol = 0,
                  currentRemainder = 0;

    int* hashTable = (int*)calloc(256, sizeof(unsigned char));
    if (!hashTable) {
        return OUT_OF_MEMORY;
    }

    getHashTable(hashTable);

    if (!ignore) {
        size_t currentIndex = 0;
        while (!feof(fileIn) && (currentB64Symbol = (unsigned char)fgetc(fileIn)) != 255)  {
            if (!isValidChar(currentB64Symbol) && currentB64Symbol != '\n') {
                printf("%li", currentIndex);
                return UNKNOWN_SYMBOL;
            }
            currentIndex++;
        }
    }

    fseek(fileIn, 0, SEEK_SET);

    size_t countOfEndSymbols = 0;
    while (!feof(fileIn) && (currentB64Symbol = (unsigned char)fgetc(fileIn)) != 255) {
        if (currentB64Symbol == '=') {
            while (currentB64Symbol == '=') {
                countOfEndSymbols++;
                currentB64Symbol = (unsigned char)fgetc(fileIn);
            }
            break;
        }

        if (currentB64Symbol == '\n') {
            continue;
        }

        switch (currentB64Index % 4) {
            case 0:
                currentSymbol = (unsigned char)(hashTable[currentB64Symbol] << 2);
                break;
            case 1:
                currentSymbol += (unsigned char)(hashTable[currentB64Symbol] >> 4);
                currentRemainder = (unsigned char)(hashTable[currentB64Symbol] % 16);

                fprintf(fileOut, "%c", currentSymbol);
                break;
            case 2:
                currentSymbol = currentRemainder << 4;
                currentSymbol += (unsigned char)(hashTable[currentB64Symbol] >> 2);
                currentRemainder = (unsigned char)(hashTable[currentB64Symbol] % 4);

                fprintf(fileOut, "%c", currentSymbol);
                break;
            case 3:
                currentSymbol = currentRemainder << 6;
                currentSymbol += hashTable[currentB64Symbol];

                fprintf(fileOut, "%c", currentSymbol);
                break;
            default:
                continue;
        }

        currentB64Index++;
    }
    switch (countOfEndSymbols) {
        case 1:
            fprintf(fileOut, "%c", currentRemainder << 4);
            break;
        case 2:
            fprintf(fileOut, "%c", currentRemainder << 6);
            break;
        default:
            break;
    }

    return SUCCESS;
}

ExitCodes startAlgorithm(int argc, char* argv[]) {
    //Allocating array for options
    boolean* includedOptions = (boolean*)calloc(4, sizeof(boolean));
    if (!includedOptions) {
        return OUT_OF_MEMORY;
    }

    //Get options and check
    ExitCodes getArrayOfOptions;
    int enters = -1;
    if ((getArrayOfOptions = getOptions(argc, argv, includedOptions, &enters)) != SUCCESS) {
        return getArrayOfOptions;
    }

    if (!checkOptions(includedOptions)) {
        return WRONG_OPTIONS;
    }

    //Start process
    ExitCodes transforming;
    boolean encoding = includedOptions[ENCODING] ? true : false;
    if (encoding) {
        boolean includeEnters = includedOptions[ENTERS] ? true : false;
        if ((transforming = includeEnters ? startEncoding(true, enters) : startEncoding(false, -1)) != SUCCESS) {
            return transforming;
        }
    } else {
        boolean ignore = includedOptions[IGNORE] ? true : false;
        if ((transforming = ignore ? startDecoding(true) : startDecoding(false)) != SUCCESS) {
            return transforming;
        }
    }

    return SUCCESS;
}

int main(int argc, char* argv[]) {
    ExitCodes executing;
    if ((executing = startAlgorithm(argc, argv)) != SUCCESS) {
        fprintf(fileOut, "%s\n", exitMessages[executing]);
    }
}
