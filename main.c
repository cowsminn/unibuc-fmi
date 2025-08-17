#include <rlutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int cmp(const void *a, const void *b)
{

  return (*(int*)a - *(int*)b);

}

void swap(int *x, int *y)
{
    int aux = *x;
    *x = *y;
    *y = aux;
}

int m_assert(int *v, int n, int k)
{
    qsort(v,n,sizeof(int),cmp);
    return v[k];
}

int divide(int *v, int stg, int dr)
{
    int pivot = v[dr];
    int i = stg - 1;
    for (int j = stg; j < dr; j++)
    {
        if (v[j] < pivot)
        {
            i++;
            swap(&v[i], &v[j]);
        }
    }
    swap(&v[i+1], &v[dr]);
    return i+1;
}

int min_k(int *v, int stg, int dr, int k)
{
    if (stg == dr)
        return v[stg];
    int pivot = divide(v, stg, dr);
    if (k == pivot)
        return v[pivot];
    else if (k < pivot)
        return min_k(v, stg, pivot-1, k);
    else
        return min_k(v, pivot+1, dr, k);

}

int main()
{
    FILE *f = fopen("date.in","r");
    FILE *g = fopen("date.out","w");
    if(f == NULL)
    {
        printf("eroare fisier");
        fclose(g);
        return 1;
    }

    int n,k;
    fscanf(f,"%d%d",&n,&k);

    int *v = (int*)malloc(sizeof(int) * n);
    for(int i = 0; i < n; i++)
        fscanf(f,"%d",&v[i]);

    assert(k > 0 && k <= n);
    assert(min_k(v,0,n-1,k-1) == m_assert(v,n,k-1));

    fprintf(g,"%d ",min_k(v,0,n-1,k-1)); // luam k-1 pentru ca incepe cnt de 0
    free(v);

    fclose(g);

    return 0;


}
