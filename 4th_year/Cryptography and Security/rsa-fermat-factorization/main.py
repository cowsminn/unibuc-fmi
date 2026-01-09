import math
from gmpy2 import isqrt, is_square, mpz

N = mpz(493961897235469870292307704656493415997433138962510694052352354056015160611157146290217693123626837384152139449490467857038999255446215235069536873657874471897877142239811781135880733176634421346574981019498955208470548795522331860014033633929358420424900956205877093806353128067289210479254110552965542173649398809136807973185513780035880423)

def fermat_factorization(n):
    """
    Factorizează n = p * q când |p - q| este mic
    folosind atacul Fermat
    """
    A = isqrt(n)
    if A * A < n:
        A += 1
    
    print(f"√N ≈ {A}")
    print(f"Căutăm A începând de la {A}...\n")
    
    iterations = 0
    max_iterations = 2**20  # Din problemă: A - √N < 2^20
    
    while iterations < max_iterations:
        A_squared = A * A
        diff = A_squared - n
        
        if is_square(diff):
            x = isqrt(diff)
            p = A - x
            q = A + x
            
            if p * q == n:
                print(f"✓ Factorizare găsită după {iterations + 1} iterații!")
                print(f"\nA = {A}")
                print(f"x = {x}")
                print(f"\n{'='*70}")
                print(f"p = {p}")
                print(f"\n{'='*70}")
                print(f"q = {q}")
                print(f"\n{'='*70}")
                print(f"\nVerificare: p × q = N? {p * q == n}")
                print(f"|p - q| = {abs(p - q)}")
                print(f"2N^(1/4) = {2 * isqrt(isqrt(n))}")
                print(f"Condiția |p - q| < 2N^(1/4) este satisfăcută? {abs(p - q) < 2 * isqrt(isqrt(n))}")
                
                return p, q
        
        A += 1
        iterations += 1
        
        if iterations % 100000 == 0:
            print(f"Iterația {iterations}... A = {A}")
    
    print("Nu s-a găsit factorizarea în limita de iterații!")
    return None, None

# Rulăm atacul
print("Atacul Fermat pentru factorizarea RSA")
print("=" * 70)
print(f"N = {N}\n")

p, q = fermat_factorization(N)

if p and q:
    print("\n" + "=" * 70)
    print("REZULTAT FINAL:")
    print("=" * 70)
    print(f"p = {p}")
    print(f"q = {q}")