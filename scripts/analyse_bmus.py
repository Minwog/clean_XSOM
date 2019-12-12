import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import math
import sys

file3 = '../experiences/2SOM_retro/res_2019-11-14_16-46.data'
file1 = '../experiences/2SOM_retro/map1_w_2019-11-14_16-46.data'
file2 = '../experiences/2SOM_retro/map2_w_2019-11-14_16-46.data'

file1 = "../experiences/2SOM_retro/" +"map1_w_"+sys.argv[1]+".data"
file2 = "../experiences/2SOM_retro/" +"map2_w_"+sys.argv[1]+ ".data"
file3 = "../experiences/2SOM_retro/res_"+sys.argv[1]+".data"

w1 = pd.read_csv(file1, sep = ' ',header = None)
w2 = pd.read_csv(file2 ,sep = ' ', header = None)


tw1 = w1[w1.columns[0:500]].values[0]
cw1 = w1[w1.columns[500:1000]].values[0]

tw2 = w2[w2.columns[0:500]].values[0]
cw2 = w2[w2.columns[500:1000]].values[0]

res  = open(file3,'r')
data = res.read().split('\n')
res.close()

bmu_1 = data[8].split(';')
bmu_2 = data[9].split(';')

print(bmu_1)
print(bmu_2)

compteur_1 = [0 for i in range(500)]
compteur_2 = [0 for i in range(500)]
for i in bmu_1[:-1]:
    indice = math.floor(500*float(i))
    compteur_1[indice] +=1
for i in bmu_2[:-1]:
    indice = math.floor(500*float(i))
    compteur_2[indice] +=1

print(compteur_1)
print(compteur_2)



non_zero_1 = [i for c,i in zip(compteur_1,tw1) if c > 0]
non_zero_abs1 = [idx/500 for idx, (c,i) in enumerate(zip(compteur_1,tw1)) if c > 0]

non_zero_2 = [i for c,i in zip(compteur_2,tw2) if c > 0]
non_zero_abs2 = [idx/500 for idx, (c,i) in enumerate(zip(compteur_2,tw2)) if c > 0]

compteur_1_nz = [i/10000 for i in filter(lambda x: x > 0, compteur_1)]
compteur_2_nz = [i/10000 for i in filter(lambda x: x > 0, compteur_2)]

plt.figure()
plt.subplot(211)
plt.scatter(non_zero_abs1, non_zero_1, c = compteur_1_nz, s = 10, cmap='viridis_r' )
plt.plot(np.arange(0,1,0.002),cw1, '-g' )
plt.subplot(212)
plt.scatter(non_zero_abs2, non_zero_2, c = compteur_2_nz, s = 10, cmap='viridis_r')
plt.plot(np.arange(0,1,0.002),cw2, '-g' )


x = np.zeros((500,1))
x2 = tw1.copy()
y = tw2.copy()
y2 = np.zeros((500,1))

for idx,elt in enumerate(tw2):
    print(idx)
    print(elt)
    pos1 = int(500*cw2[idx])
    x[idx] = tw1[pos1]
    print(tw1[pos1])
    print('-----')

for idx,elt in enumerate(tw1):
    pos1 = int(500*cw1[idx])
    y2[idx] = tw2[pos1]

non_zero_x2 =  [elt for idx,elt in enumerate(x2) if compteur_1[idx] > 0]
non_zero_y2 =  [elt for idx,elt in enumerate(y2) if compteur_1[idx] > 0]

non_zero_x =  [elt for idx,elt in enumerate(x) if compteur_2[idx] > 0]
non_zero_y =  [elt for idx,elt in enumerate(y) if compteur_2[idx] > 0]

print(non_zero_x)
print(non_zero_y)


plt.figure()
plt.subplot(2,2,1)
plt.scatter(non_zero_x, non_zero_y, c = compteur_2_nz, cmap='viridis_r')
plt.subplot(2,2,2)
plt.scatter(non_zero_x2, non_zero_y2,c = compteur_1_nz,cmap='viridis_r' )
plt.subplot(2,2,3)
plt.scatter(x[0::10], y[0::10], c = np.linspace(0,1,50))
plt.subplot(2,2,4)
plt.scatter(x2[0::10],y2[0::10], c = np.linspace(0,1,50))
plt.show()
