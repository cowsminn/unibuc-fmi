#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

struct intrare {
    char element;
    int aparitii;
};

struct nod {
    intrare info;
    nod* next;
};

struct lista {
    nod* primul_element;
    lista() {
        primul_element = NULL;
    }
    ~lista() {
        nod* current = primul_element;
        while (current != NULL) {
            nod* next = current->next;
            delete current;
            current = next;
        }
    }
    void insert_la_inceput(long long cheie, int valoare) {
        nod* nou = new nod;
        nou->info.element = cheie;
        nou->info.aparitii = valoare;
        nou->next = primul_element;
        primul_element = nou;
    }
    intrare* cauta_dupa_cheie(long long cheie) {
        nod* curent = primul_element;
        while (curent != NULL && curent->info.element != cheie)
            curent = curent->next;
        if (curent == NULL)
            return NULL;
        return &(curent->info);
    }
    void afisare() {
        nod* pointer;
        if (primul_element == NULL)
            cout << "prim=NULL";
        else
            for (pointer = primul_element; pointer != NULL; pointer = pointer->next)
                cout << "[" << pointer->info.element << ", " << pointer->info.aparitii << "] -> ";
        cout << endl;
    }
};

int hash_diviziune(long long cheie) {
    return cheie % 19;
}

struct hashtable_chaining {
    lista* T;
    int n;
    int (*hashfunc)(long long);
    hashtable_chaining(int N, int (*H)(long long)) {
        n = N;
        hashfunc = H;
        T = new lista[n];
    }

    hashtable_chaining(const hashtable_chaining& other) : n(other.n), hashfunc(other.hashfunc) {
        T = new lista[n];
        for (int i = 0; i < n; ++i) {
            T[i] = other.T[i];
        }
    }

    hashtable_chaining& operator=(const hashtable_chaining& other) {
        if (this != &other) {
            delete[] T;
            n = other.n;
            hashfunc = other.hashfunc;
            T = new lista[n];
            for (int i = 0; i < n; ++i) {
                T[i] = other.T[i];
            }
        }
        return *this;
    }
    ~hashtable_chaining() {
        delete[] T;
    }
    void put(long long cheie, int valoare) {
        int hash = hashfunc(cheie);
        int index = hash % n;
        intrare* gasit = T[index].cauta_dupa_cheie(cheie);
        if (gasit == NULL)
            T[index].insert_la_inceput(cheie, valoare);
        else
            gasit->aparitii = abs(gasit->aparitii - valoare);
    }

    void afisare() {
        for (int i = 0; i < n; i++)
            T[i].afisare();
    }
};

int main() {
    int n;
    cout << "Primul multiset" << endl;
    cout << "Numarul de perechi:";
    cin >> n;

    vector<intrare> multiset1;  
    for (int i = 0; i < n; i++) {
        cout << "Perechea " << i + 1 << endl;
        intrare entry;
        cin >> entry.element >> entry.aparitii;
        multiset1.push_back(entry);
    }

    int m;
    cout << "Al doilea multiset" << endl;
    cout << "Numarul de perechi:";
    cin >> m;

    vector<intrare> multiset2;  
    for (int i = 0; i < m; i++) {
        cout << "Perechea " << i + 1 << endl;
        intrare intrare;
        cin >> intrare.element >> intrare.aparitii;
        multiset2.push_back(intrare);
    }

    int max = (n > m) ? n : m;
    hashtable_chaining H(max, hash_diviziune);
    for (int i = 0; i < n; i++)
        H.put(multiset1[i].element, multiset1[i].aparitii);
    for (int i = 0; i < m; i++)
        H.put(multiset2[i].element, multiset2[i].aparitii);

    H.afisare();

    return 0;
}
