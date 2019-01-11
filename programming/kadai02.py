import sympy

def herminate(n, x = sympy.Symbol('x')):
  expr = [2 * x, 1]
  if n < 2:
    return expr[1 - n]
  for i in range(1, n):
    expr = [2 * x * expr[0] - 2 * i * expr[1], expr[0]]
  return expr[0].simplify()

def inner_prod(m, n):
  x = sympy.Symbol('x')
  return sympy.integrate(sympy.exp(-x ** 2) * herminate(n) * herminate(m), (x, -sympy.oo, sympy.oo))

for i in range(10):
  for j in range(10):
    print(i, j, inner_prod(i, j))
