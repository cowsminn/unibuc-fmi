#include <iostream>

#define k 20

struct subvector {
    int v[k] = {0};
    subvector* next = nullptr;
};

struct dynvector {

    subvector* primul = nullptr;

    void update(int poz, int x) {
        int nrnod = poz / k;
        int pozvector = poz % k;

        if (nrnod >= 7) {
            std::cerr << "noduri setate - 7" << std::endl;
            exit(1);
        }

        subvector* current = primul;
        subvector* prev = nullptr;

        for (int i = 0; i < nrnod; i++) {
            if (current == nullptr) {
                current = new subvector();
                if (prev != nullptr) {
                    prev->next = current;
                } else {
                    primul = current;
                }
                current->next = nullptr;
            }
            prev = current;
            current = current->next;
        }

        /// in caz ca poz < 20 => nrnod = 0 => nu intra pe for

        if (current == nullptr) {
            current = new subvector();
            if (prev != nullptr) {
                prev->next = current;
            } else {
                primul = current;
            }
            current->next = nullptr;
        }
        current->v[pozvector] = x;
    }

    int get(int poz) {
        int nrnod = poz / k;
        int pozvector = poz % k;

        if (nrnod >= 7) {
            std::cerr << "noduri setate - 7" << std::endl;
            exit(1);
        }

        subvector* current = primul;

        for (int i = 0; i < nrnod; i++) {
            if (current == nullptr) {
                return 0;
            }
            current = current->next;
        }

        if (current == nullptr) {
            return 0;
        }

        return current->v[pozvector];
    }

    dynvector operator+(const dynvector& other) const {
        dynvector rez;

        subvector* currentv = primul;
        subvector* currentother = other.primul;

        while (currentv != nullptr || currentother != nullptr) {
            subvector suma;

            if (currentv != nullptr) {
                for (int j = 0; j < k; j++) {
                    suma.v[j] += currentv->v[j];
                }
                currentv = currentv->next;
            }

            if (currentother != nullptr) {
                for (int j = 0; j < k; j++) {
                    suma.v[j] += currentother->v[j];
                }
                currentother = currentother->next;
            }

            /*
            if (rez.primul == nullptr) {
                rez.primul = new subvector(suma);
            } else {
                subvector* current = rez.primul;
                while (current->next != nullptr) {
                    current = current->next;
                }
                current->next = new subvector(suma);
            }
            */

            rez.lipirev(suma);
        }

        return rez;
    }


    void lipirev(const subvector& v) {
        subvector* nodnou = new subvector(v);
        nodnou->next = nullptr;

        if (primul == nullptr) {
            primul = nodnou;
        } else {
            subvector* current = primul;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = nodnou;
        }
    }

    ~dynvector() {
        subvector* current = primul;
        while (current != nullptr) {
            subvector* next = current->next;
            delete current;
            current = next;
        }
    }
};

int main() {
    dynvector v1, v2;

    v1.update(90, 15);
    v2.update(24, 10);
    v2.update(90,2);

    ///std::cout << v1.get(200); - er
    ///v1.update(200,200); - er

    dynvector suma = v1 + v2;

    for (int i = 0; i < 140; i++) {
        std::cout << suma.get(i) << " ";
        if ((i + 1) % 20 == 0) {
            std::cout << std::endl;
        }
    }

    return 0;
}
