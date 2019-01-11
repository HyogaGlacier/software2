import sympy

def herminate(n):
  x = sympy.Symbol('x')
  expr = [2 * x, 1]
  if n < 2:
    return expr[1 - n]
  for i in range(1, n):
    expr = [2 * x * expr[0] - 2 * i * expr[1], expr[0]]
  return expr[0].simplify()

for i in range(11):
  print(herminate(i))
