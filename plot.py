import numpy as np
import matplotlib.pyplot as plt
from sys import exit
# ,rho_mu_minus1,rho_mu_minus1_25,rho_mu_minus1_5,rho_mu_minus1_75,rho_mu_minus2,rho_mu_minus2_25,rho_mu_minus2_5,z

data = np.loadtxt("data.dat", delimiter=',')
n = data[:,0]
z = data[:,-1]
rho = data[:,1]


data = np.loadtxt("rho.dat")
simz = data[:,0]
simrho = data[:,1]

plt.plot(simz,simrho, label="sim")

plt.plot(z,rho)

plt.xlim([0,10])
plt.legend()
plt.show()




