#include <stdio.h>

int a = 6;
int b = 7;

int add(int a, int b) {
    int result = a + b;
    return result;
}

int main() {
    int result = add(a, b);
    printf("6 + 7 = %d\n", result);
    return 0;
} 