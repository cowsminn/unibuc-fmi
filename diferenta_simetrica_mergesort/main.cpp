#include <iostream>
#include <vector>
using namespace std;

void merge(vector<int>& arr, int left, int middle, int right) {
    int n1 = middle - left + 1;
    int n2 = right - middle;

    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;

        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);

        merge(arr, left, middle, right);
    }
}

void diferentaSimetrica(const vector<int>& A, int m, const vector<int>& B, int n, vector<int>& rezultat) {
    int i = 0, j = 0, k = 0;

    while (i < m && j < n) {
        if (A[i] < B[j]) {
            if (k == 0 || A[i] != rezultat[k - 1]) {
                rezultat[k++] = A[i++];
            } else {
                i++;
            }
        } else if (A[i] > B[j]) {
            if (k == 0 || B[j] != rezultat[k - 1]) {
                rezultat[k++] = B[j++];
            } else {
                j++;
            }
        } else {
            i++;
            j++;
        }
    }

    while (i < m) {
        if (k == 0 || A[i] != rezultat[k - 1]) {
            rezultat[k++] = A[i++];
        } else {
            i++;
        }
    }

    while (j < n) {
        if (k == 0 || B[j] != rezultat[k - 1]) {
            rezultat[k++] = B[j++];
        } else {
            j++;
        }
    }

    rezultat.resize(k);
}

int main() {
    vector<int> A = {1, 2, 3, 4, 5, 6};
    int m = A.size();

    vector<int> B = {2, 3, 4, 7};
    int n = B.size();

    vector<int> rezultat(m + n);

    diferentaSimetrica(A, m, B, n, rezultat);

    cout << "Diferenta Simetrica: ";
    for (int i : rezultat) {
        cout << i << " ";
    }
    cout << endl;

    return 0;
}
