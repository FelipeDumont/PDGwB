# Call to the C .so file from python

import numpy as np
from ctypes import CDLL, POINTER
from ctypes import c_size_t, c_double
import os

# importing .SO
# libfile = glob.glob('/home/felipe/Desktop/PDGwB/test.so')[0]
cwd = os.getcwd()
mylib = CDLL(cwd+"/test.so")

# Ctypes
ND_pointer_warr = np.ctypeslib.ndpointer(dtype=np.float64,ndim=1,flags="C")
mylib.ga.argtypes = [ND_pointer_warr]

# Calling
testing = np.zeros(10)
mylib.ga(testing)
print("Finished ???")
print(testing[0])
