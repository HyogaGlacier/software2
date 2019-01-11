import sympy
sympy.init_printing()

def taylor(f, x, a, n):
  ret = f.subs([(x, a)])
  div = 1 # n!
  pol = 1 # (x - a)^n
  for i in range(n):
    div *= i + 1
    f = sympy.diff(f, x)
    pol *= (x - a)
    ret += f.subs([(x, a)]) / div * pol
  return ret

x = sympy.Symbol('x')
print(taylor(sympy.sin(x), x, 0, 8))
