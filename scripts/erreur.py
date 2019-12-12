import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import math
import sys

file3 = "../experiences/2SOM_forced/res_"+sys.argv[1]+".data"

res  = open(file3,'r')
data = res.read().split('\n')
res.close()


inpx = [float(elt) for elt in data[0].split(';') if len(elt)>0]
inpy = [float(elt) for elt in data[1].split(';') if len(elt)>0]

x = [float(elt) for elt in data[4].split(';')  if len(elt)>0]
y = [float(elt) for elt in data[5].split(';')  if len(elt)>0]



err_moyx = (1/len(inpx)) * np.sqrt(sum([(inp - th)**2 for inp,th in zip(inpx,x)]))
err_moyy = (1/len(inpy)) * np.sqrt(sum([(inp - th)**2 for inp,th in zip(inpy,y)]))

print(err_moyx)
print(err_moyy)
