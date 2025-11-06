% Exercitiul 1 [rezolvat]
% Fie un predicat employee_salary/2 care configureaza un nume de angajat si salariul acestuia.
% De exemplu, employee_salary(peter, 1000)
% Primind o lista de elemente employee_salary, determinati numele tuturor angajatilor care castiga o suma minima configurata.
% veti scrie un predicat employee_with_good_salary(InputList, MinimumSalary, ResultList).
% de exemplu
% ?- employee_with_good_salary([employee_salary(peter, 1000), employee_salary(oliver, 1200), employee_salary(sam, 700), employee_salary(ben, 15000), employee_salary(sandra, 4500)], 1200, Res).
% Res = [oliver, ben, sandra]

employee_with_good_salary(InputList, MinSalary, Result) :-
    findall(Name, (member(employee_salary(Name, Salary), InputList), Salary >= MinSalary), Result).

% Exercitiul 2 [rezolvat]
% Primind o lista identica celei de la exercitiul 1, dar cu aparitii multiple ale unui angajat, 
% 	(considerand ca fiecare intrare reprezinta salariul intr-o anumita luna)
% 	determinati salariul mediu al fiecarui angajat. 
% de exemplu, daca peter apare employee_salary(peter, 1000), respectiv employee_salary(peter, 1200), in lista de rezultat va fi doar employee_salary(peter, 1100)
% ?- employee_average_salary([employee_salary(peter, 1000), employee_salary(oliver, 1200), employee_salary(sam, 700), employee_salary(oliver, 800), employee_salary(sam, 900), employee_salary(sandra, 4500), employee_salary(peter, 1200), employee_salary(oliver, 400)], Result).
% Result = [(peter, 1100), (oliver, 800), (sam, 800), (sandra, 4500)]

sum_and_count_by_name(_, [], 0, 0) :- !.
sum_and_count_by_name(Name, [employee_salary(Name, CurrentSalary) | Tail], Sum, Count) :-
    sum_and_count_by_name(Name, Tail, SumTail, CountTail),
    Sum is SumTail + CurrentSalary,
    Count is CountTail + 1, !.
sum_and_count_by_name(Name, [_ | Tail], Sum, Count) :-
    sum_and_count_by_name(Name, Tail, Sum, Count).
	
remove_by_name(_, [], []).
remove_by_name(Name, [employee_salary(Name, _) | Tail], TR) :-
    remove_by_name(Name, Tail, TR), !.
remove_by_name(Name, [H | T], [H | TR]) :-
    remove_by_name(Name, T, TR).

employee_average_salary([], []) :- !.
employee_average_salary([employee_salary(Name, Salary) | Tail], [(Name, Average) | TR]) :-
    sum_and_count_by_name(Name, [employee_salary(Name, Salary) | Tail], Sum, Count),
    Average is Sum / Count,
    remove_by_name(Name, [employee_salary(Name, Salary) | Tail], NewList),
    employee_average_salary(NewList, TR).

% Exercitiul 3 [rezolvat]
% Primind o lista identica celei de la exercitiul 2, in lista de rezultat ordonati angajatii dupa salariul mediu castigat, in ordine descrescatoare. 
% ?- sort_employee_average_salary([employee_salary(peter, 1000), employee_salary(oliver, 1200), employee_salary(sam, 700), employee_salary(oliver, 800), employee_salary(sam, 900), employee_salary(sandra, 4500), employee_salary(peter, 1200), employee_salary(oliver, 400)], Result).
% Result = [(sandra, 4500), (peter, 1100), (oliver, 800), (sam, 800)]

% vreau sa fie descrescator
my_cmp(X, (_, Salary1), (_, Salary2)) :-
    compare(X, Salary2, Salary1).

sort_employee_average_salary(InputList, SortedList) :-
    employee_average_salary(InputList, TempList),
    predsort(my_cmp, TempList, SortedList).
    


% Exercitiul 4 - Dutch Royal Family [rezolvat]

mother(wilhelmina,juliana).
mother(juliana,beatrix).
mother(juliana,christina).
mother(juliana,irene).
mother(juliana,margriet).
mother(beatrix,friso).
mother(beatrix,alexander).
mother(beatrix,constantijn).
mother(emma,wilhelmina).
father(hendrik,juliana).
father(bernard,beatrix).
father(bernard,christina).
father(bernard,irene).
father(bernard,margriet).
father(claus,friso).
father(claus,alexander).
father(claus,constantijn).
father(willem,wilhelmina).

% citim mother(beatrix, alexander) ca "beatrix este mama lui alexander". aceeasi interpretare la father 

queen(beatrix).
queen(juliana).
queen(wilhelmina).
queen(emma).
king(willem).

% 4a) definiti urmatoarele predicate
% parent/2 - parent(X, Y). sa fie true cand X este parinte al lui Y 
% ruler(X) - sa fie true cand X este conducator 

parent(X, Y) :- mother(X, Y).
parent(X, Y) :- father(X, Y).

ruler(X) :- king(X).
ruler(X) :- queen(X).

% 4b) definiti un predicat predecessor/2, astfel incat predecessor(X, Y) sa ne spuna ierarhia de predecesori pentru un conducator X
% (vrem sa raspundem, de fapt, la cine precede pe cine)

predecessor(X, Y) :- ruler(X), ruler(Y), parent(Y, X).
predecessor(X, Y) :- ruler(X), parent(Z, X), ruler(Z), predecessor(Z, Y).

% Exercitiul 5 [rezolvat]
% Fie L = (R, F, C, ari) o structura de ordinul I, unde F = {f, g, h}, C = {u, v, w} iar ari(f) = 2, ari(g) = 1, ari(h) = 3
% Considerand a, b, c, d variabile, care dintre urmatorii termeni se pot unifica?
% a) f(g(a), h(b, c, v)) cu f(g(u), h(g(w), w, g(a)))
% b) f(g(a), h(b, c, d)) cu f(g(u), h(g(w), w, g(a)))

% se va scrie in terminal
% ?- unify_with_occurs_check(f(g(A), h(B, C, v)), f(g(u), h(g(w), w, g(A)))).
% raspunsul este false, pentru ca termenii nu unifica. v/0 iar g/1, se va incerca unificarea v = g(A) => esec 

% ?- unify_with_occurs_check(f(g(A), h(B, C, D)), f(g(u), h(g(w), w, g(A)))).
% raspunde A = u, B = g(w), C = w, D = g(u)

% Exercitiul 6 [rezolvat]
% Fie predicatele succ/2 si obj/1
% Adaptati algoritmul BFS astfel incat sa pastram drumurile cele mai scurte care ajung de la un nod de start la un nod obiectiv. 

% Vom pastra doar cele mai scurte drumuri din lista rezultat, plecand de la un nod de start catre obiectivul deja setat.
% Daca rulati solve(1, R), veti gasi R = [[5, 3, 4, 2, 1], [5, 2, 1], [5, 3, 1], [5, 3, 4, 1]] 
% vrem sa pastram doar drumurile de lungime minima, adica de lungime 3 

% scriem o functie de minim
min([], 0) :- !.
min([X], X) :- !.
min([H | T], MinTail) :-
    min(T, MinTail),
    MinTail =< H, !.
min([H | T], H) :-
    min(T, MinTail),
    H =< MinTail.

minimiumPaths(Start, Result) :-
    solve(Start, AllPaths),				% gasim toate drumurile posibile
    									% vrem acum sa mapam fiecare lista din lista de drumuri la lungimea ei
    findall(Length, (member(Path, AllPaths), length(Path, Length)), Lengths),
    									% acum, Lengths este o lista de lungimi de forma [5, 3, 3, 4]
    									% adica primul drum are lungime 5, al doilea are lungime 3, al treilea lungime 3, ultimul lungime 4
   	min(Lengths, MinLength),			% am aflat ca drumul de lungime minima are lungime 3 (minim 3)
    findall(Path, (member(Path, AllPaths), length(Path, Length), Length =:= MinLength), Result).
										% am pus in Result toate Path care au lungimea minima calculata


% Exercitiul 7 [rezolvat]
% verificati daca un sir de caractere primit ca intrare, si reprezentat ca un atom, este palindrom sa nu
% palindrome(prolog). atunci false
% palindrome(ele). atunci true 

palindrome(Str) :-
    string_chars(Str, ListChars),
    reverse(ListChars, ReversedList),
    ListChars == ReversedList.

% Exercitiul 8 [rezolvat]
% Fie urmatoarea baza de cunostinte, definita de predicatele 
% 	employee_info(name, department_number, scale)
%	department(department_number, department_name)
%	salary(scale, amount)

employee_info(mcardon,1,5).
employee_info(treeman,2,3).
employee_info(chapman,1,2).
employee_info(claessen,4,1).
employee_info(petersen,5,8).
employee_info(cohn,1,7).
employee_info(duffy,1,9).
department(1,board).
department(2,human_resources).
department(3,production).
department(4,technical_services).
department(5,administration).
salary(1,1000).
salary(2,1500).
salary(3,2000).
salary(4,2500).
salary(5,3000).
salary(6,3500).
salary(7,4000).
salary(8,4500).
salary(9,5000).

% a) determinati toti angajatii din departamentul 1 si care au scale > 2 
% Exemplu pentru rezolvare
% ?- employee_info(Name, Department_Number, Scale), Department_Number = 1, Scale > 2 
% acum, puneti toate aceste rezultate intr-o singura lista 

employees_from_department_eq_1_with_scale_gt_2(List) :-
    findall(Name, (employee_info(Name, Department_Number, Scale), Department_Number = 1, Scale > 2), List).

% b) determinati toti angajatii dintr-un anume departament 

employees_from_department(DepNumber, List) :-
    findall(Name, (employee_info(Name, Department_Number, _), Department_Number = DepNumber), List).

% c) selectati name si scale al angajatilor din departamentul 1, si scale > 3 

employees_from_department_eq_1_with_scale_gt_3(List) :-
    findall((Name, Scale), (employee_info(Name, Department_Number, Scale), Department_Number = 1, Scale > 3), List).



