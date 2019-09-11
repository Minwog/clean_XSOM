import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import math
file1 = "../experiences/2SOM_retro_2_phases/map1_w_2019-8-30_9-52.data"
file2 = "../experiences/2SOM_retro_2_phases/map2_w_2019-8-30_9-52.data"


w1 = pd.read_csv(file1, sep = ' ',header = None)
w2 = pd.read_csv(file2 ,sep = ' ', header = None)

tw1 = w1[w1.columns[0:500]].values[-1]
cw1 = w1[w1.columns[500:1000]].values[-1]

tw2 = w2[w2.columns[0:500]].values[-1]
cw2 = w2[w2.columns[500:1000]].values[-1]

#plot weights
plt.figure()
plt.subplot(2,1,1)
plt.plot(tw1,'r-')
plt.plot(cw1,'b-')
plt.subplot(2,1,2)
plt.plot(tw2,'r-')
plt.plot(cw2,'b-')


#2D plots !

"""
Couleurs :
L'idée est de représenter nos 4 cartes 1D sous une forme 2D.
En l'occurence, les points de même couleur représentent la meme position dans la carte 2. A re-explorer,pas clair
"""
x = np.zeros((500,2))
x2 = tw1
y = tw2
y2 = np.zeros((500,2))

print(len(x2))
for idx,elt in enumerate(tw2):
    pos1 = int(500*cw2[idx])
    x[idx,0] = tw1[pos1]
    x[idx,1] = idx

for idx,elt in enumerate(tw1):
    pos1 = int(500*cw1[idx])
    y2[idx,0] = tw2[pos1]
    y2[idx,1] = pos1

colortab = list(range(0,500))

plt.figure()
plt.subplot(1,2,1)
plt.scatter(x[:,0],y,c=x[:,1])
plt.subplot(1,2,2)
plt.scatter(x2,y2[:,0],c=y2[:,1])


activity = lambda val1, val2: np.exp(-(val1-val2)*(val1-val2)/(2*sigma2))

#testing !


plt.show()
