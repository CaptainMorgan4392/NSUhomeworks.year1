#include <stdio.h>
#include <mm_malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <ctype.h>

//Declaration of struct "stack"
//INV: FILO - "First in - last out"
typedef struct {
    int* arrayOfIntegers;
    int size;
} stack;

//Creating instance of struct "stack"
stack* createStack(size_t size) {
    stack *newStack = calloc(1, sizeof(stack));
    newStack -> arrayOfIntegers = (int*)calloc(size, sizeof(int));
    newStack -> size = 0;
    return newStack;
}

void ensureCapacityOfStack(stack* stack) {
    int* newArray = (int*)calloc(2 * (size_t)stack -> size, sizeof(int));
    memcpy(newArray, stack -> arrayOfIntegers, stack -> size);
    stack -> arrayOfIntegers = newArray;
}

void ensureCapacity(char* expression, size_t size) {
    char* newLine = (char*)calloc(2 * size + 1, sizeof(char));
    memcpy(newLine, expression, size);
    expression = newLine;
}

//Add the element to stack                                                                  Time: O(1)
void push(stack* stack, int value) {
    stack -> arrayOfIntegers[stack -> size++] = value;
    if (stack -> size % 1000 == 0) {
        ensureCapacityOfStack(stack);
    }
}

//Returns the element on the peek of stack                                                  Time: O(1)
int peek(stack* stack) {
    return stack -> arrayOfIntegers[stack -> size - 1];
}

//Delete the peek element from stack                                                        Time: O(1)
void pop(stack* stack) {
    if (stack -> size == 0) {
        printf("bad input");
        exit(1);
    }
    stack -> arrayOfIntegers[stack -> size--] = 0;
}

// Method, which scanning input and returns string before the next 'enter' symbol          Time: O(length)
char* getLine() {
    char* line = (char*)calloc(1001, sizeof(char));
    size_t currentIndex = 0;
    char currentSymbol;
    while ((currentSymbol = (char)getchar()) != '\n') {
        line[currentIndex++] = currentSymbol;
        if (currentIndex % 1000 == 0) {
            ensureCapacity(line, currentIndex);
        }
    }
    if (strlen(line) == 0) {
        printf("bad input");
        exit(1);
    }
    return line;
}

//Splitting expression to "Tokens" which can be numbers or symbols of operations            Time: O(length)
char** splitBySpaces(char* expression, int *quantityOfTokens) {
    char** splitted = (char**)calloc(1001, sizeof(char*));
    int currentIndexOfLine = 0,
            currentIndex = 0;
    for (size_t i = 0; i < strlen(expression); i++) {
        if (expression[i] == ' ') {
            currentIndex = 0;
            continue;
        }
        if (currentIndex == 0) {
            splitted[currentIndexOfLine++] = (char*)calloc(1001, sizeof(char));
        }
        splitted[currentIndexOfLine - 1][currentIndex++] = expression[i];
    }
    (*quantityOfTokens) = currentIndexOfLine;
    return splitted;
}


//All of the 4 methods below are calculating an operation for two peek operands in stack     Time: O(1)
void operationAdd(stack* stack) {
    int secondOperand = peek(stack);
    pop(stack);
    int firstOperand = peek(stack);
    pop(stack);
    stack -> arrayOfIntegers[stack -> size++] = firstOperand + secondOperand;
}

void operationSubtract(stack* stack) {
    int secondOperand = peek(stack);
    pop(stack);
    int firstOperand = peek(stack);
    pop(stack);
    stack -> arrayOfIntegers[stack -> size++] = firstOperand - secondOperand;
}

void operationMultiply(stack* stack) {
    int secondOperand = peek(stack);
    pop(stack);
    int firstOperand = peek(stack);
    pop(stack);
    stack -> arrayOfIntegers[stack -> size++] = firstOperand * secondOperand;
}

void operationDivide(stack* stack) {
    int secondOperand = peek(stack);
    pop(stack);
    int firstOperand = peek(stack);
    pop(stack);
    if (secondOperand == 0) {
        printf("division by zero");
        exit(1);
    }
    stack -> arrayOfIntegers[stack -> size++] = firstOperand / secondOperand;
}

//Checking a token if it's a number                                                           Time: O(length)
bool isNumber(char* token) {
    for (size_t i = 0; i < strlen(token); i++) {
        if (!isdigit(token[i])) {
            return false;
        }
    }
    return true;
}

//My own "itoa"                                                                                Time: O(length)
int fromStringToInt(char* line) {
    int answer = 0;
    for (size_t i = 0; i < strlen(line); i++) {
        answer *= 10;
        answer += line[i] - '0';
    }
    return answer;
}

int main() {
    //First of all, we should create stack
    stack* stackOfNumbers = createStack(1000);

    //Allocating and initialising of input string
    char* expression = getLine();

    //Split the input string to tokens
    int quantityOfTokens = 0;
    char** splittedString = splitBySpaces(expression, &quantityOfTokens);

    //If the token is number - push to stack, else we should get a result of operation with two peek elements of stack
    for (int i = 0; i < quantityOfTokens; i++) {
        if (strcmp(splittedString[i], "+") == 0) {
            operationAdd(stackOfNumbers);
        } else if (strcmp(splittedString[i], "-") == 0) {
            operationSubtract(stackOfNumbers);
        } else if (strcmp(splittedString[i], "*") == 0) {
            operationMultiply(stackOfNumbers);
        } else if (strcmp(splittedString[i], "/") == 0) {
            operationDivide(stackOfNumbers);
        } else if (isNumber(splittedString[i])){
            push(stackOfNumbers, fromStringToInt(splittedString[i]));
        } else {
            printf("bad input");
            exit(1);
        }
    }

    //If the size of stack isn't a 1 - it's exception!

    if (stackOfNumbers -> size != 1) {
        printf("bad input");
        exit(1);
    }

    //At the end, in stack we have only one element - answer
    printf("%d", stackOfNumbers -> arrayOfIntegers[0]);
    return 0;
}
