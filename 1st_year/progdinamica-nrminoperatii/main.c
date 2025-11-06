#include <stdio.h>
#include <stdlib.h>

void secventa(int x, int anterior[]) {
    if (x > 1) {
        if (anterior[x] == 1)
            {secventa(x - 1, anterior);
            printf("(+1) -> ");
            }
        else if (anterior[x] == 2)
            {secventa(x / 2, anterior);
            printf("(*2) -> ");
            }
        else
            {secventa(x / 3, anterior);
            printf("(*3) -> ");
            }
    }
    printf("%d", x);
}

void func(int x) {
    int nr_operatii[x+1];
    int anterior[x+1];
    nr_operatii[1] = 0;

    for (int i = 2; i <= x; i++) {
        nr_operatii[i] = nr_operatii[i-1] + 1;
        anterior[i] = 1;

        if (i % 2 == 0 && nr_operatii[i/2] + 1 < nr_operatii[i]) {
            nr_operatii[i] = nr_operatii[i/2] + 1;
            anterior[i] = 2;
        }

        if (i % 3 == 0 && nr_operatii[i/3] + 1 < nr_operatii[i]) {
            nr_operatii[i] = nr_operatii[i/3] + 1;
            anterior[i] = 3;
        }
    }

    printf("numarul de operatii minime pentru obtinerea numarului %d este: %d\n", x, nr_operatii[x]);

    printf("valori intermediare: ");
    secventa(x, anterior);
    printf("\n");
}

int main() {
    int x;
    FILE* f = fopen("date.in","r");
    if(f == NULL)
    {
        printf("eroare fisier");
    }
    else
    {
        fscanf(f, "%d", &x);
        func(x);
        fclose(f);
    }
    
    return 0;
}
