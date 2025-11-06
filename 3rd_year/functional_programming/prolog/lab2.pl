% Laboratorul 2 

% Exercitiul 1 

% Varianta ineficienta

fib(0, 1) :- !.
fib(1, 1) :- !.
fib(N, R) :-
    N1 is N - 1,
    N2 is N - 2,
    fib(N1, R1),
    fib(N2, R2),
    R is R1 + R2.

% Varianta cu memoizare

fib(F, _, N, N, F) :- !.
fib(F1, F0, Index, N, F) :-
    F2 is F1 + F0,
    NewIndex is Index + 1,
    fib(F2, F1, NewIndex, N, F).

efficient_fib(N, F) :-
    fib(1, 1, 1, N, F).

% Exercitiul 2 

list_length([], 0).
list_length([_ | T], Length) :-
    list_length(T, LengthTail),
    Length is LengthTail + 1.

% Exercitiul 3 

list_sum([], 0).
list_sum([H | T], Sum) :-
    list_sum(T, SumTail),
    Sum is SumTail + H.

% Exercitiul 4 

elements_of(X, [X | _]) :- !.
elements_of(X, [_ | T]) :-
    elements_of(X, T).

% Exercitiul 5 

all_a([a]) :- !.
all_a([a | T]) :- all_a(T).

all_symb([S], S) :- !.
all_symb([H | T], S) :-
    H = S,
    all_symb(T, S).

% Exercitiul 6 

trans_a_b([a], [b]) :- !.
trans_a_b([a | T], [b | TR]) :-
    trans_a_b(T, TR).

% Exercitiul 7 

scalarMult(_, [], []) :- !.
scalarMult(X, [H | T], [HR | TR]) :-
    HR is X * H,
    scalarMult(X, T, TR).

% Exercitiul 8 

dot([], [], 0) :- !.
dot([H1 | T1], [H2 | T2], Result) :-
    dot(T1, T2, ResultTail),
    Result is ResultTail + H1 * H2.

% Exercitiul 9

max([], 0) :- !.
max([H | T], MaxTail) :-
    max(T, MaxTail),
    MaxTail >= H, !.
max([H | T], H) :-
    max(T, MaxTail),
    H >= MaxTail.

% Exercitiul 10

concat_lists([], L, L) :- !.
concat_lists([H | T], L, [H | TR]) :-
    concat_lists(T, L, TR).

% Exercitiul 11

remove_duplicates([], []) :- !.
remove_duplicates([H | T], [H | TR]) :-
    not(elements_of(H, T)), 
    remove_duplicates(T, TR), !.
remove_duplicates([_ | T], TR) :-
    remove_duplicates(T, TR).

% Exercitiul 12 
% e doar un exemplu in document, vezi laboratorul 5 pentru findall			
