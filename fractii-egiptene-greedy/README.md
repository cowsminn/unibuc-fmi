# Tema Greedy UniBuc

### Problema 10 - Fractii egiptene
- Sa se aplice algoritmul Greedy pentru a transforma o fractie obisnuit intr-o suma de fractii cu numaratorul 1. Se va aplica succesiv formula urmatoare:
    ## $`\frac{x}{y} = \frac{1}{[\frac{y}{x}]} + \frac{(-y) * mod(x)}{y * [\frac{y}{x}]}`$
- Date de intrare : o fractie initiala (cu numarator mai mare de 1).
- Date de iesire : suma de fractii cu numarator 1.
- Exemplu :
    ## $`\frac{7}{15} = \frac{1}{3} + \frac{2}{15} = \frac{1}{3} + \frac{1}{8} + \frac{1}{120}`$
