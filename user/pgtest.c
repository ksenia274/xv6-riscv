#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PAGE_N 3
#define PGSIZE 4096
int stackArray[PAGE_N * (PGSIZE  / sizeof(int))];


void test_stack() {
    fprintf(1, "Test stack\n");
    //без изменений
    int stackAccessResult = 0;

    if (pgaccess(stackArray + (PGSIZE / sizeof(int)), PGSIZE, &stackAccessResult) < 0) {
        fprintf(2, "Failed to check stack array access\n");
        exit(1);
    }
    if (stackAccessResult != 0){
        fprintf(2, "Value error before changing: %d \n", stackAccessResult);
        exit(1);
    }
    //внесли изменения
    stackArray[(PGSIZE / sizeof(int))] = 0;
//    stackArray[10] = 5;
    if (pgaccess(stackArray + (PGSIZE / sizeof(int)), PGSIZE, &stackAccessResult) < 0) {
        fprintf(2, "Failed to check heap array access\n");
        exit(1);
    }
    if (stackAccessResult != 1){
        fprintf(2, "Value error after changing: %d \n", stackAccessResult);
        exit(1);
    }
    fprintf(1, "Passed\n");

}

void test_heap() {
    fprintf(1, "Test heap\n");

    int heapAccessResult;
    // Массив в куче
    int* heapArray = malloc(PAGE_N * (PGSIZE / sizeof(int)));
    if (heapArray == 0) {
        fprintf(2, "Failed to allocate memory\n");
        exit(1);
    }

    if (pgaccess(heapArray + (PGSIZE / sizeof(int)), PGSIZE, &heapAccessResult) < 0) {
        fprintf(2, "Failed to check heap array access\n");
        exit(1);
    }

    if (heapAccessResult != 0){
        fprintf(2, "Value error after changing: %d \n", heapAccessResult);
        exit(1);
    }

    heapArray[2] = 0;

    // Проверка доступа к массиву в куче
    if (pgaccess(heapArray + 2, PGSIZE, &heapAccessResult) < 0) {
        fprintf(2, "Failed to check heap array access\n");
        exit(1);
    }
    if (heapAccessResult != 1){
        fprintf(2, "Value error after changing: %d \n", heapAccessResult);
        exit(1);
    }

    if (pgaccess(heapArray + 2, PGSIZE, &heapAccessResult) < 0) {
        fprintf(2, "Failed to check heap array access\n");
        exit(1);
    }
    if (heapAccessResult != 0){
        fprintf(2, "Value error after not changing2: %d \n", heapAccessResult);
        exit(1);
    }

    fprintf(1, "Passed\n");

    free(heapArray);
}

int main() {
    test_stack();
    test_heap();
    exit(0);
}