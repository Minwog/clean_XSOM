import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import random
import sys
from os import listdir
from os.path import isfile, join
from matplotlib import rc

size = 500
sigma = 0.05

dir_path  = "../experiences/2SOM_retro/"

#filesmap1 = "map1_w_"+sys.argv[1]+".data"
#filesmap2 = "map2_w_"+sys.argv[1]+".data"

fileres = "res_"+sys.argv[1]+".data"

#file1 = join(dir_path,filesmap1)
#file2 = join(dir_path,filesmap2)
fileres = join(dir_path, fileres)

#w1 = pd.read_csv(file1, sep = ' ',header = None)
#w2 = pd.read_csv(file2 ,sep = ' ', header = None)


#s1,s2 = w1.shape

"""
tw1 = w1[w1.columns[0:500]].values
cw1 = w1[w1.columns[500:1000]].values
tw2 = w2[w2.columns[0:500]].values
cw2 = w2[w2.columns[500:1000]].values
"""
res = open(fileres)

lines = res.readlines()



"""
np.reshape(tw1,(500,s1))
np.reshape(cw1,(500,s1))
np.reshape(cw2,(500,s1))
np.reshape(tw2,(500,s1))


print(tw1.shape)
"""
bmus_x = [float(x) for x  in lines[-2].strip().split(';')[:-1]]
bmus_y = [float(x) for x  in lines[-1].strip().split(';')[:-1]]

plt.scatter(bmus_x, bmus_y)
rc('text', usetex=True)
plt.xlabel("$p\star^x$")
plt.ylabel("$p\star^y$")
plt.show()
