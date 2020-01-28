import numpy as np
import matplotlib.pyplot as plt
from sys import exit

data = np.loadtxt("rho.dat")
x = data[:,0]
rho = data[:,1]

plt.plot(x,rho)
plt.show()

