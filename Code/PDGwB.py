import numpy as np
from ctypes import CDLL, POINTER
from ctypes import c_size_t, c_double
import os

# importing .SO
cwd = os.getcwd()
mylib = CDLL(cwd+"/test")

# Ctypes
ND_pointer_warr = np.ctypeslib.ndpointer(dtype=np.float64,ndim=1,flags="C")
mylib.ga.argtypes = [ND_pointer_warr]

# Calling
testing = np.zeros(10)
mylib.ga(testing)
print("Finished ???")
print(testing[0], testing[1])
