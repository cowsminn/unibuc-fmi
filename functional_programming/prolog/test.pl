% Exercitiul 8
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

% b) determinati toti angajatii dintr-un anume departament 

% c) selectati name si scale al angajatilor din departamentul 1, si scale > 3


func1(List) :-
	findall(Name,
		(employee_info(Name, D, Sc), D = 1, Sc > 2),
		List).
		

func2(D, List) :-
    findall(Name,
        employee_info(Name, D, _),
        List).
		
func3(List) :-
	findall((Name, S),
		(employee_info(Name, D, S), D = 1, S > 3),
		List).	

palindrome(Atom) :-	
    atom_chars(Atom, ListaCaractere),    
    reverse(ListaCaractere, ListaInversata),  
    ListaCaractere = ListaInversata.   
    
palindromee(Atom) :-
    atom_chars(Atom, ListaCaractere),
    inverseaza(ListaCaractere, ListaInversata),
    ListaCaractere = ListaInversata.

inverseaza([], []).
inverseaza([H|T], ListaInversata) :-
    inverseaza(T, TInversata),
    append(TInversata, [H], ListaInversata).      
    
my_cmp(X, (_, Salary1), (_, Salary2)) :-
    compare(X, Salary2, Salary1).
    
list_sum([], 0).
list_sum([H | T], Sum) :-
    write('Processing head: '), write(H),
    list_sum(T, SumTail),
    Sum is SumTail + H,
    write('Current sum: '), write(Sum).
 
