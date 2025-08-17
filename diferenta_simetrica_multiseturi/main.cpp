#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

struct NodArbore {
    vector<char> cheie;
    NodArbore* left;
    NodArbore* right;
};

void deleteTree(NodArbore* nod) {
    if (nod == nullptr) return;

    deleteTree(nod->left);
    deleteTree(nod->right);
    
    delete nod;
}

NodArbore* newNode(const vector<char>& item) {
    NodArbore* temp = new NodArbore;
    temp->cheie = item;
    temp->left = temp->right = nullptr;
    return temp;
}

NodArbore* insert(NodArbore* nod, const vector<char>& cheie) {
    if (nod == nullptr) return newNode(cheie);

    if (cheie < nod->cheie)
        nod->left = insert(nod->left, cheie);
    else if (cheie > nod->cheie)
        nod->right = insert(nod->right, cheie);

    return nod;
}

void findint(NodArbore* nod, char find, int& cnt) {
    if (nod == nullptr) return ;
    cnt = 0 ;
    vector<char> c = nod->cheie;
    if(c[0] == find)
        while(c.size())
        {
            cnt++;
            c.pop_back();
        }

    else { findint(nod->left, find, cnt);
        findint(nod->right, find, cnt);}

}

void dif_simetrica(NodArbore* rad1, NodArbore* rad2) {
    if (rad1 == nullptr) return;
    int count = 0;
    int cnt;
    char found;
    for (char c : rad1->cheie) {
        count++;
        found = c;
    }
    findint(rad2, found, cnt);
    cout << found << " " << abs(count - cnt) << endl;
    dif_simetrica(rad1->left, rad2);
    dif_simetrica(rad1->right, rad2);

}

int main() {

    NodArbore* rad1 = nullptr;
    rad1 = insert(rad1, {'a', 'a'});
    insert(rad1, {'b', 'b', 'b', 'b'});
    insert(rad1, {'c', 'c', 'c', 'c', 'c', 'c', 'c'});

    NodArbore* rad2 = nullptr;
    rad2 = insert(rad2, {'b', 'b'});
    insert(rad2, {'a', 'a', 'a', 'a'});
    insert(rad2, {'c', 'c', 'c'});

    cout << "Differenta simetrica:" << endl;
    dif_simetrica(rad1, rad2);

    deleteTree(rad1);
    deleteTree(rad2);

    return 0;
}
