/* Caravan Cosmin 364
  4. Scrieti un program care sorteaza prin interclasare un fisier
 de caractere in maniera descrisa mai jos.
  Sortarea prin interclasare presupune impartirea sirului in doua jumatati,
 sortarea fiecarei parti prin aceeasi metoda (deci recursiv), apoi 
 interclasarea celor doua parti (care sunt acum sortate).
  Programul va lucra astfel: imparte sirul in doua, genereaza doua
 thread-uri, folosind biblioteca 'Pthreads', care sorteaza cele doua
 jumatati in paralel, apoi asteapta sa se termine thread-urile, apoi
 interclaseaza jumatatile; fiecare thread va proceda similar, generand
 (daca e cazul) alte doua thread-uri.
  Nu se vor folosi fisiere auxiliare iar memoria folosita va fi limitata de
 o constanta.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_CHARS 1000

typedef struct {
    char *array;
    int left;
    int right;
} ThreadArgs;

void merge(char *array, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    char *L = (char *)malloc(n1);
    char *R = (char *)malloc(n2);

    for (int i = 0; i < n1; i++)
        L[i] = array[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = array[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k] = L[i];
            i++;
        } else {
            array[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void *merge_sort_thread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    char *array = threadArgs->array;
    int left = threadArgs->left;
    int right = threadArgs->right;

    if (left < right) {
        int mid = left + (right - left) / 2;

        pthread_t tid1, tid2;
        ThreadArgs leftArgs = {array, left, mid};
        ThreadArgs rightArgs = {array, mid + 1, right};

        pthread_create(&tid1, NULL, merge_sort_thread, &leftArgs);
        pthread_create(&tid2, NULL, merge_sort_thread, &rightArgs);

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);

        merge(array, left, mid, right);
    }

    return NULL;
}

int main() {
    char array[MAX_CHARS];
   // printf("Introduceti sirul de caractere: ");
    fgets(array, MAX_CHARS, stdin);

    size_t len = strlen(array);
    if (len > 0 && array[len - 1] == '\n') {
        array[len - 1] = '\0';
        len--;
    }

    ThreadArgs args = {array, 0, (int)len - 1};

    pthread_t main_thread;
    pthread_create(&main_thread, NULL, merge_sort_thread, &args);
    pthread_join(main_thread, NULL);

   printf(array);

    return 0;
}

