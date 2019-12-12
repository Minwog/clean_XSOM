import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import axes3d
import pandas as pd
import pickle as pkl
from matplotlib import collections  as mc

file1 = "../experiences/3SOM_retro/res_2019-11-20_15-4.data"
#file2 = "../experiences/2SOM_retro/map2_w_2019-10-14_13-43.data"

tdf = {}

with open(file1) as file:
    for i,line in enumerate(file):
        tdf[i] = [float(x) for x in line.split(";")[:-1]]




x  = tdf[0]
y  = tdf[1]
z  = tdf[2]
xc  = tdf[3]
yc  = tdf[4]
zc  = tdf[5]

x_t = tdf[6]
y_t = tdf[7]
z_t = tdf[8]

x_tc = tdf[9]
y_tc= tdf[10]
z_tc = tdf[11]



u = np.linspace(0, np.pi,30)
v = np.linspace(0, 2 * np.pi,30)

xin = 0.5 + 0.5*np.outer(np.sin(u), np.sin(v))
yin = 0.5 +0.5*np.outer(np.sin(u), np.cos(v))
zin = 0.5 +0.5*np.outer(np.cos(u), np.ones_like(v))


fig1 = plt.figure(1)
ax = fig1.add_subplot(111, projection='3d')
ax.scatter(x, y, z)


fig2 = plt.figure(2)
ax = fig2.add_subplot(111, projection='3d')
ax.scatter(x, y, z, color='k', alpha=0.4)
ax.scatter(x_t, y_t, z_t,color='b')


fig3 = plt.figure(3)
ax = fig3.add_subplot(111, projection='3d')
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
ax.scatter(x_tc, y_tc, z_tc, color='b')
ax.scatter(xc, yc, zc, color='k' ,alpha=0.4)

plt.show()
