#include <iostream>

#define k 16

using namespace std;

struct graf {
    int adiacenta[k+1][k+1];
    int vizitat[k+1];

    graf() {
        for (int i = 1; i <= k; ++i) {
            for (int j = 1; j <= k; ++j) {
                adiacenta[i][j] = 0;
            }
            vizitat[i] = 0;
        }
    }

    void adauga_muchie(int nod_sursa, int nod_destinatie) {
        adiacenta[nod_sursa][nod_destinatie] = 1;
        adiacenta[nod_destinatie][nod_sursa] = 1;
    }

    void afisare() {
        for (int i = 1; i <= k; ++i) {
            cout << i << " : ";
            for (int j = 1; j <= k; ++j) {
                if (adiacenta[i][j] == 1) {
                    cout << j << " ";
                }
            }
            cout << endl;
        }
    }

    void gaseste_cicluri_6() {
        cout << "Ciclurile de lungime 6 sunt:\n";
        for (int nod = 1; nod <= k; ++nod) {
            int ciclu_curent[6];
            for (int i = 0; i < 6; ++i) {
                ciclu_curent[i] = 0;
            }
            ciclu_curent[0] = nod;
            dfs(nod, nod, 1, ciclu_curent);
        }
    }

    void dfs(int nod_start, int nod_curent, int lungime_curenta, int ciclu_curent[]) {
        if (lungime_curenta == 6) {
            afiseaza_ciclu(ciclu_curent);
            return;
        }

        for (int vecin = 1; vecin <= k; ++vecin) {
            if (adiacenta[nod_curent][vecin] == 1 && vecin > nod_start && !exista_in_ciclu(ciclu_curent, vecin)) {
                ciclu_curent[lungime_curenta] = vecin;
                dfs(nod_start, vecin, lungime_curenta + 1, ciclu_curent);
                ciclu_curent[lungime_curenta] = 0;
            }
        }
    }

    bool exista_in_ciclu(int const ciclu_curent[], int nod) {
        for (int i = 0; i < 6; ++i) {
            if (ciclu_curent[i] == nod) {
                return true;
            }
        }
        return false;
    }

    void afiseaza_ciclu(int const ciclu_curent[]) {
        for (int i = 0; i < 6; ++i) {
            cout << ciclu_curent[i] << " ";
        }
        cout << endl;
    }
};

int main() {
    graf G;

    int muchii[22][2] = {{1, 2}, {1, 5}, {2, 6}, {2, 3}, {3, 7}, {3, 4}, {4, 8},
                         {5, 6}, {5, 9}, {6, 10}, {7, 11}, {7, 8}, {8, 12}, {9, 10},
                         {9, 13}, {10, 14}, {11, 15}, {11, 12}, {12, 16}, {13, 14},
                         {14, 15}, {15, 16}
    };

    for (int i = 0; i < 22; ++i) {
        G.adauga_muchie(muchii[i][0], muchii[i][1]);
    }

    G.afisare();
    G.gaseste_cicluri_6();

    return 0;
}
