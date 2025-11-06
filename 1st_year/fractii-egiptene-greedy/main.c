#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void egipt(int a, int b)
{
    assert(b != 0 && "nedifinit - numitorul == 0"); /// nu am stiut exact cum as putea sa folosesc
                                                    /// assert pt outputul de la void() recursiv
    if(a == 0){
        printf("0");
        return;
    }
    if (b % a == 0) {
        printf("1/%d", b / a);
        return;
    }
    if (a % b == 0) {
        printf("%d", a / b);
        return;
    }
    if (a > b) {
        printf("%d + ", a / b);
        egipt(a % b, b);
        return;
    }
    int x = b / a + 1;
    printf("1/%d + ", x);
    egipt(a * x - b, b * x);
}
int main()
{
    int a, b; /// a - numaratorul b - numitorul

    printf("Numaratorul: ");
    scanf("%d", &a);
    printf("Numitorul: ");
    scanf("%d", &b);
    printf("Fractia %d/%d se scrie in fractii egiptene astfel:\n", a, b);

    egipt(a, b);

    return 0;
}
