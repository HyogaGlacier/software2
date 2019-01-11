import subprocess
import matplotlib.pyplot as plt
import numpy as np

step = 1000
x = []
y = []
for i in range(10):
  q = i / 10
  proc = subprocess.run(["./random_walk", str(q), str(step)], stdout=subprocess.PIPE)
  x.append(q)
  y.append(int(proc.stdout.decode("utf8")) / step)

plt.plot(x, y)
x = np.linspace(0, 1, 1000)
y = x / (x + 1)
plt.plot(x, y)
plt.savefig("figure.png")
