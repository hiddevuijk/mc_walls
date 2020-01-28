import numpy as np
import matplotlib.pyplot as plt
from sys import exit

# data colomns
#,rho_model1,rho_model2_2,z

data = np.loadtxt("data.dat", delimiter=',')
n = data[:,0]
rho1 = data[:,1]
rho2 = data[:,2]
z = data[:,3]


sim = np.loadtxt("sim.dat")
zsim = sim[:,0]
rhosim = sim[:,1]

plt.plot(z,rho1, label="rho1")
plt.plot(z,rho2, label="rho2")
plt.plot(zsim,rhosim, label="sim")

sim = np.loadtxt("sim_new.dat")
zsim = sim[:,0]
rhosim = sim[:,1]

plt.plot(zsim,rhosim, label="sim_new")

plt.xlim([0,10])
plt.legend()
plt.show()

