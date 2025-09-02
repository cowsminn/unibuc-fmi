#include <iostream>
#include <stack>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <functional>


//a|b
//a.b 
//a*
//(a|b)*
//(a|b)*.a
//(a|b)*.a.b.#
using namespace std;

/*
 * Structura pentru un nod din arborele sintactic.
 * Fiecare nod poate fi:
 *  - Frunză (când este un simbol din alfabet sau #)
 *  - Operator ('*', '.', '|')
 */
struct SyntaxNode {
    char token;
    int position;  // Poziția frunzei (doar pentru simbolurile din alfabet și #)
    SyntaxNode* leftChild;
    SyntaxNode* rightChild;
    set<int> initialPositions;
    set<int> finalPositions;
    bool isNullable;

    SyntaxNode(char tok)
        : token(tok), position(-1),
          leftChild(nullptr), rightChild(nullptr),
          isNullable(false) {}
};

// ------------------------------------------------------------------------------------
// 1) Funcție de ajutor: Testăm dacă un caracter este un simbol din alfabet sau '#'.
// ------------------------------------------------------------------------------------
bool isSymbol(char c, const string& alphabet) {
    return (alphabet.find(c) != string::npos) || (c == '#');
}

// ------------------------------------------------------------------------------------
// 2) Inserăm explicit operatorul de concatenare '.' în expresie
//    De ex. (a|b)a#  ->  (a|b).a.#
// ------------------------------------------------------------------------------------
string insertConcatOperator(const string& expr, const string& alphabet) {
    string result;
    for (int i = 0; i < (int)expr.size(); i++) {
        char c = expr[i];
        result.push_back(c);

        // Dacă nu suntem la final, vedem ce urmează
        if (i + 1 < (int)expr.size()) {
            char next = expr[i+1];

            // Condiții pentru a pune '.' între c și next:
            // 1) c e simbol (din alfabet/#) sau '*' ori e ')'
            // 2) next e simbol sau '('
            bool cIsSymbol = isSymbol(c, alphabet);
            bool cIsStar   = (c == '*');
            bool cIsRightParen = (c == ')');
            
            bool nIsSymbol = isSymbol(next, alphabet);
            bool nIsLeftParen = (next == '(');

            // Dacă e final de sub-expresie și începe alta
            // introducem '.' dacă nu există alt operator între ele
            if ((cIsSymbol || cIsStar || cIsRightParen) && 
                (nIsSymbol || nIsLeftParen)) 
            {
                result.push_back('.');
            }
        }
    }
    return result;
}

// ------------------------------------------------------------------------------------
// 3) Funcții pentru conversia infix -> postfix (Shunting Yard simplificat)
// ------------------------------------------------------------------------------------
int getPrecedence(char op) {
    if (op == '*') return 3;
    if (op == '.') return 2;
    if (op == '|') return 1;
    return 0;
}

bool isOperatorToken(char c) {
    return (c == '*' || c == '.' || c == '|');
}

string convertInfixToPostfix(const string& infix) {
    stack<char> operatorStack;
    string postfix;

    for (char c : infix) {
        // Ignorăm spații (dacă apar)
        if (c == ' ') continue;

        if (!isOperatorToken(c) && c != '(' && c != ')') {
            // c este un simbol (alfabet sau '#')
            postfix += c;
        } 
        else if (c == '(') {
            operatorStack.push(c);
        } 
        else if (c == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            if (!operatorStack.empty()) {
                operatorStack.pop(); // scoatem '('
            }
        } 
        else if (isOperatorToken(c)) {
            // Scoatem din stivă operatorii cu precedență >= c
            while (!operatorStack.empty() && 
                   getPrecedence(operatorStack.top()) >= getPrecedence(c) &&
                   operatorStack.top() != '(') 
            {
                postfix += operatorStack.top();
                operatorStack.pop();
            }
            operatorStack.push(c);
        }
    }

    // Golește stiva
    while (!operatorStack.empty()) {
        postfix += operatorStack.top();
        operatorStack.pop();
    }

    return postfix;
}

// ------------------------------------------------------------------------------------
// 4) Construirea arborelui sintactic din postfix
// ------------------------------------------------------------------------------------
SyntaxNode* createSyntaxTree(const string& postfix) {
    stack<SyntaxNode*> syntaxNodes;

    for (char c : postfix) {
        if (!isOperatorToken(c) && c != '(' && c != ')') {
            // c este simbol => frunză
            syntaxNodes.push(new SyntaxNode(c));
        } 
        else if (isOperatorToken(c)) {
            SyntaxNode* newNode = new SyntaxNode(c);
            if (c == '*') {
                // Operator unar
                newNode->leftChild = syntaxNodes.top();
                syntaxNodes.pop();
            } else {
                // Operator binar ('.' sau '|')
                newNode->rightChild = syntaxNodes.top();
                syntaxNodes.pop();
                newNode->leftChild = syntaxNodes.top();
                syntaxNodes.pop();
            }
            syntaxNodes.push(newNode);
        }
    }

    return syntaxNodes.top();
}

// ------------------------------------------------------------------------------------
// 5) Calculul recursiv al isNullable, initialPositions, finalPositions
//    + popularea mulțimilor followPositions
// ------------------------------------------------------------------------------------
void computeSyntaxNodeProperties(
    SyntaxNode* node, 
    map<int, set<int>>& followPositions, 
    int& posIndex
) {
    if (!node) return;

    // Dacă e frunză (simbol)
    if (!isOperatorToken(node->token)) {
        // Atribuim o poziție (pentru a fi recunoscută în followPositions)
        node->position = posIndex++;
        node->isNullable = false;
        node->initialPositions = { node->position };
        node->finalPositions   = { node->position };
        return;
    }

    // Altfel, parcurgem copii
    if (node->leftChild) {
        computeSyntaxNodeProperties(node->leftChild, followPositions, posIndex);
    }
    if (node->rightChild) {
        computeSyntaxNodeProperties(node->rightChild, followPositions, posIndex);
    }

    // Prelucrare în funcție de operator
    if (node->token == '*') {
        node->isNullable = true;  // Kleene star e mereu nullable
        node->initialPositions = node->leftChild->initialPositions;
        node->finalPositions   = node->leftChild->finalPositions;

        // followpos(p) ∪= firstpos(left) pentru tot p ∈ finalpos(left)
        for (int p : node->leftChild->finalPositions) {
            followPositions[p].insert(
                node->leftChild->initialPositions.begin(),
                node->leftChild->initialPositions.end()
            );
        }
    }
    else if (node->token == '.') {
        // concatenare
        node->isNullable = (node->leftChild->isNullable && node->rightChild->isNullable);

        // initialPositions
        if (node->leftChild->isNullable) {
            // firstpos(node) = firstpos(left) ∪ firstpos(right)
            node->initialPositions.insert(node->leftChild->initialPositions.begin(),
                                          node->leftChild->initialPositions.end());
            node->initialPositions.insert(node->rightChild->initialPositions.begin(),
                                          node->rightChild->initialPositions.end());
        } else {
            // firstpos(node) = firstpos(left)
            node->initialPositions = node->leftChild->initialPositions;
        }

        // finalPositions
        if (node->rightChild->isNullable) {
            // lastpos(node) = lastpos(left) ∪ lastpos(right)
            node->finalPositions.insert(node->leftChild->finalPositions.begin(),
                                        node->leftChild->finalPositions.end());
            node->finalPositions.insert(node->rightChild->finalPositions.begin(),
                                        node->rightChild->finalPositions.end());
        } else {
            // lastpos(node) = lastpos(right)
            node->finalPositions = node->rightChild->finalPositions;
        }

        // followpos(left.final) += right.initial
        for (int p : node->leftChild->finalPositions) {
            followPositions[p].insert(
                node->rightChild->initialPositions.begin(),
                node->rightChild->initialPositions.end()
            );
        }
    }
    else if (node->token == '|') {
        // reuniune
        node->isNullable = (node->leftChild->isNullable || node->rightChild->isNullable);

        // firstpos = left ∪ right
        node->initialPositions.insert(node->leftChild->initialPositions.begin(),
                                      node->leftChild->initialPositions.end());
        node->initialPositions.insert(node->rightChild->initialPositions.begin(),
                                      node->rightChild->initialPositions.end());

        // lastpos = left ∪ right
        node->finalPositions.insert(node->leftChild->finalPositions.begin(),
                                    node->leftChild->finalPositions.end());
        node->finalPositions.insert(node->rightChild->finalPositions.begin(),
                                    node->rightChild->finalPositions.end());
    }
}

// ------------------------------------------------------------------------------------
// 6) Construirea și afișarea DFA (subset construction) pe baza mulțimilor followPositions
// ------------------------------------------------------------------------------------
void constructDFA(SyntaxNode* root, map<int, set<int>>& followPositions, const string& alphabet) {
    // Mapăm position -> caracter, pentru a ști ce frunză corespunde cărei litere
    map<int, char> positionToChar;

    // Funcție lambda pentru a parcurge arborele și a popula positionToChar
    function<void(SyntaxNode*)> mapPositionsToChars = [&](SyntaxNode* node) {
        if (!node) return;
        if (!isOperatorToken(node->token)) {
            // e frunză
            positionToChar[node->position] = node->token;
        }
        mapPositionsToChars(node->leftChild);
        mapPositionsToChars(node->rightChild);
    };
    mapPositionsToChars(root);

    // Set de stări (fiecare stare e un set<int>) + coadă BFS
    set<set<int>> stateSet;
    queue< set<int> > statesToProcess;

    // Map care reține pentru o stare, pentru fiecare simbol, starea (setul) următor
    map< set<int>, map<char, set<int>> > stateTransitions;

    // Starea inițială = root->initialPositions
    stateSet.insert(root->initialPositions);
    statesToProcess.push(root->initialPositions);

    while (!statesToProcess.empty()) {
        set<int> currentState = statesToProcess.front();
        statesToProcess.pop();

        // Pentru fiecare simbol din alfabet, determinăm starea următoare
        for (char symbol : alphabet) {
            set<int> nextState;

            // Căutăm în currentState pozițiile care corespund acestui symbol
            for (int pos : currentState) {
                // Dacă frunza de la "pos" are același simbol, adăugăm followpos(pos)
                if (positionToChar[pos] == symbol) {
                    if (followPositions.count(pos)) {
                        nextState.insert(followPositions[pos].begin(), followPositions[pos].end());
                    }
                }
            }

            if (!nextState.empty()) {
                // dacă încă nu avem nextState în setul de stări, îl adăugăm
                if (stateSet.find(nextState) == stateSet.end()) {
                    stateSet.insert(nextState);
                    statesToProcess.push(nextState);
                }
                // memorăm tranziția
                stateTransitions[currentState][symbol] = nextState;
            }
        }
    }

    // Afișăm stările ca Indici (0,1,2,...) și determinăm care e finală
    // Finală = starea care conține oricare dintre finalPositions(root)
    // (inclusiv poziția frunzei #)
    map< set<int>, int > stateToIndex;
    int idx = 0;
    for (auto& st : stateSet) {
        stateToIndex[st] = idx++;
    }

    // Afișăm
    cout << "\n=== DFA STATES & TRANSITIONS ===\n";
    for (auto& st : stateSet) {
        int stateIndex = stateToIndex[st];
        bool isAccepting = false;
        // Verificăm dacă intersectează finalPositions ale rădăcinii
        // (dacă conține poziția simbolului #, atunci clar e finală)
        for (int p : st) {
            if (root->finalPositions.find(p) != root->finalPositions.end()) {
                isAccepting = true;
                break;
            }
        }

        cout << "Stare " << stateIndex << (isAccepting ? " (FINAL)" : "") << " = { ";
        for (int p : st) cout << p << " ";
        cout << "}\n";

        // Afișăm tranzițiile
        if (stateTransitions.find(st) != stateTransitions.end()) {
            for (auto& [sym, nxt] : stateTransitions[st]) {
                cout << "   " << sym << " -> Stare "
                     << stateToIndex[nxt] << "\n";
            }
        }
    }
}

// ------------------------------------------------------------------------------------
// 7) MAIN modificat
// ------------------------------------------------------------------------------------
int main() {
    // Ex: Alfabet: ab
    //     Expresie infix: (a|b)*a#

    string inputAlphabet, infixExpression;
    cout << "Introduceti alfabetul (ex: ab): ";
    cin >> inputAlphabet;
    
    cout << "Introduceti expresia regulata in forma infixata (ex: (a|b)*a# ): ";
    cin >> infixExpression;

    // 1) Inserăm '.' pentru concatenare
    string exprWithConcat = insertConcatOperator(infixExpression, inputAlphabet);

    // 2) Convertim la postfix
    string postfixExpression = convertInfixToPostfix(exprWithConcat);

    cout << "\nExpresia cu '.' inserat: " << exprWithConcat << endl;
    cout << "Expresia postfixata:     " << postfixExpression << endl;

    // 3) Construim arborele sintactic
    SyntaxNode* syntaxTreeRoot = createSyntaxTree(postfixExpression);

    // 4) Calculăm proprietățile nodurilor + followPositions
    map<int, set<int>> followPositions;
    int positionIndex = 1;
    computeSyntaxNodeProperties(syntaxTreeRoot, followPositions, positionIndex);

    // 5) Construim și afișăm DFA
    constructDFA(syntaxTreeRoot, followPositions, inputAlphabet);

    return 0;
}
