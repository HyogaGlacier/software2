import subprocess
import matplotlib.pyplot as plt
import numpy as np

step = 1000
x = []
y = []
for i in range(100):
  q = i / 100
  proc = subprocess.run(["./random_walk", str(q), str(step)], stdout=subprocess.PIPE)
  x.append(q)
  y.append(int(proc.stdout.decode("utf8")) / step)

plt.scatter(x, y, label=1)
x = np.linspace(0, 1, 1000)
y = x / (x + 1)
plt.plot(x, y)
plt.savefig("figure.png")
